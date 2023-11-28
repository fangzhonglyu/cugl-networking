//
//  CUNetEventController.cpp
//  Networked Physics Library
// 
//  The class for a general network controller for multiplayer physics based game.
// 
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Created by Barry Lyu on 9/5/23.
//

#include <cugl/netphysics/CUNetEventController.h>
#include <cugl/netphysics/CULWSerializer.h>

#define MIN_MSG_LENGTH sizeof(std::byte)+sizeof(Uint64)

using namespace cugl::netphysics;

/**
 * Initializes the controller with the given asset manager.
 *
 * Requires asset to contain key "server" for a json value of the form:
 * {
 *     "lobby" : {
 *         "address" : "xxx.xxx.xxx.xxx",
 *         "port": xxxx
 *     },
 *     "ice servers" : [
 *         {
 *             "turn" : false,
 *             "address" : "xxx.xxx.xxx.xxx",
 *             "port" : xxxx
 *         },
 *         {
 *             "turn" : true,
 *             "address" : "xxx.xxx.xxx.xxx",
 *             "port" : xxxx,
 *             "username": "xxxxxx",
 *             "password": "xxxxxx"
 *         }
 *     ],
 *     "max players" : <INT>,
 *     "API version" : <INT>
 * }

 */
bool NetEventController::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Attach the primitive event types for deserialization
    attachEventType<GameStateEvent>();

    // Configure the NetcodeConnection
    _assets = assets;
    auto json = _assets->get<JsonValue>("server");
    _config.set(json);
    _status = Status::IDLE;
    _appRef = Application::get();
    _physController = NetPhysicsController::alloc();
    return true;
}

/**
 * Starts handshake process for starting game.
 
 * Once the handshake is finished, the controller changes status to
 * INGAME, starts sending synchronization events if physics is enabled.
 */
void NetEventController::startGame() {
    CUAssertLog(_isHost, "Only host should call startGame()");
    if (_status == Status::CONNECTED) {
        _network->startSession();
    }
}

/**
 * Marks the client as ready for game start.
 
 * Returns true if the mark was successful. And false otherwise. Only
 * valid after receiving shortUID from host.
 */
bool NetEventController::markReady() {
    if (_status == Status::HANDSHAKE && _shortUID) {
		_status = Status::READY;
		pushOutEvent(GameStateEvent::allocReady());
        return true;
	}
    return false;
}

/**
 * Connect to a new lobby as host.
 *
 * If successful, the controller status changes to CONNECTED, and the
 * {@link _roomid} is set to the lobby id.
 */
bool NetEventController::connectAsHost() {
    if (_status == Status::NETERROR) {
        disconnect();
    }

    _isHost = true;
    if (_status == Status::IDLE) {
        _status = Status::CONNECTING;
        _network = net::NetcodeConnection::alloc(_config);
        _network->open();
    }
    return checkConnection();
}

/**
 * Connect to an existing lobby as client.
 *
 * If successful, the controller status changes to CONNECTED.
 */
bool NetEventController::connectAsClient(std::string roomid) {
    if (_status == Status::NETERROR) {
        disconnect();
    }

    _isHost = false;
    if (_status == Status::IDLE) {
        _status = Status::CONNECTING;
        _network = net::NetcodeConnection::alloc(_config, roomid);
        _network->open();
    }
    _roomid = roomid;
    return checkConnection();
}

/**
 * Disconnect from the current lobby.
 */
void NetEventController::disconnect() {
    _network->close();
    _network = nullptr;
    _shortUID = 0;
    _status = Status::IDLE;
    _physEnabled = false;
    _isHost = false;
    _startGameTimeStamp = 0;
    _numReady = 0;
    _outEventQueue.clear();
    while (!_inEventQueue.empty()) {
        _inEventQueue.pop();
    }
    _physController = nullptr;
}

/**
 * This method checks the connection status and updates the controller status.
 */
bool NetEventController::checkConnection() {
    auto state = _network->getState();
    if (state == net::NetcodeConnection::State::CONNECTED) {
        if(_status == Status::CONNECTING || _status == Status::IDLE)
            _status = Status::CONNECTED;
        if (_isHost) {
            _roomid = _network->getRoom();
        }
        return true;
    }
    else if (_status == CONNECTED && state == net::NetcodeConnection::State::INSESSION) {
		_status = Status::HANDSHAKE;
        if (_isHost) {
            auto players = _network->getPlayers();
            Uint32 shortUID = 1;
            CULog("NUM Players: %zu", players.size());
            for (auto it = players.begin(); it != players.end(); it++) {
                CULog("Player Name: %s", (*it).c_str());
                _network->sendTo((*it), wrap(GameStateEvent::allocUIDAssign(shortUID++)));
            }
        }
		return true;
	}
    else if (_status == READY && _numReady == _network->getNumPlayers() && _isHost) {
        CULog("GAME START MESSAGE SENT");
        pushOutEvent(GameStateEvent::allocGameStart());
    }
    else if (state == net::NetcodeConnection::State::NEGOTIATING) {
        _status = Status::CONNECTING;
        return true;
    }
    else if (state == net::NetcodeConnection::State::DENIED ||
        state == net::NetcodeConnection::State::DISCONNECTED || state == net::NetcodeConnection::State::FAILED ||
        state == net::NetcodeConnection::State::INVALID || state == net::NetcodeConnection::State::MISMATCHED) {
        _status = Status::NETERROR;
        return false;
    }
    return true;
}

/**
 * Processes all events received during the last update.
 *
 * This method either processes events internally if it is a built-in event
 * and adds them to the inbound event queue otherwise.
 */
void NetEventController::processReceivedEvent(const std::shared_ptr<NetEvent>& e) {
    if (auto game = std::dynamic_pointer_cast<GameStateEvent>(e)) {
        processGameStateEvent(game);
    } else if (_status == INGAME){
        if (auto phys = std::dynamic_pointer_cast<PhysSyncEvent>(e)) {
            if(_physEnabled)
			    _physController->processPhysSyncEvent(phys);
		}
        else if (auto phys = std::dynamic_pointer_cast<PhysObjEvent>(e)) {
            if (_physEnabled) {
                _physController->processPhysObjEvent(phys);
            }
        }
        else {
            _inEventQueue.push(e);
        } 
    }
}

/**
 * Processes a GameStateEvent.
 *
 * This method updates the controller status based on the event received.
 */
void NetEventController::processGameStateEvent(const std::shared_ptr<GameStateEvent>& e) {
    CULog("GAME STATE %d, CUR STATE %d", e->getType(), _status);
    if (_status == HANDSHAKE && e->getType() == GameStateEvent::UID_ASSIGN) {
        _shortUID = e->getShortUID();
        CULog("THE UID ASSIGNED IS %x", _shortUID);
    }
    if (_status == READY && e->getType() == GameStateEvent::GAME_START) {
        _status = INGAME;
        _startGameTimeStamp = _appRef->getUpdateCount();
    }
    if (_isHost) {
        if (e->getType() == GameStateEvent::CLIENT_RDY) {
            _numReady++;
            CULog("RECEIVED RDY FROM %s", e->getSourceId().c_str());
        }
    }
    CULog("FINISHED STATE %d", _status);
}

/**
 * Processes all received packets received during the last update.
 *
 * This method unwraps byte vectors into NetEvents and calls
 * {@link processReceivedEvent()}.
 */
void NetEventController::processReceivedData(){
    _network->receive([this](const std::string source,
        const std::vector<std::byte>& data) {
        //CULog("DATA %d, CUR STATE %d, SOURCE %s", data[0], _status, source.c_str());
        processReceivedEvent(unwrap(data, source)); 
    });
}

/**
 * Broadcasts all queued outbound events.
 */
void NetEventController::sendQueuedOutData(){
    int msgCount = 0;
    int byteCount = 0;
    for(auto it = _outEventQueue.begin(); it != _outEventQueue.end(); it++){
        auto e = *(it);
        auto wrapped = wrap(e);
        msgCount++;
        byteCount += wrapped.size();
        //CULog("flag: %x", (std::byte)getType(*e))
        _network->broadcast(wrap(e));
    }
    _outEventQueue.clear();
}

/**
 * Updates the network controller.
 */
void NetEventController::updateNet() {
    if(_network){
        checkConnection();
        

        if (_status == INGAME && _physEnabled) {
            if (_isHost) {
                _physController->packPhysSync(NetPhysicsController::FULL_SYNC);
            }
            _physController->packPhysObj();
			_physController->fixedUpdate();
            for (auto it = _physController->getOutEvents().begin(); it != _physController->getOutEvents().end(); it++) {
                pushOutEvent(*it);
		    }
            _physController->getOutEvents().clear();
                
		}
        
        processReceivedData();
        sendQueuedOutData();
    }
}

/**
 * Returns if there are remaining custom inbound events.
 *
 * Thhe events in this queue is to be polled and processed by outside
 * classes. Inbound events are preserved acrossupdates, and only cleared
 * by {@link popInEvent()}.
 */
bool NetEventController::isInAvailable() {
    if ( _inEventQueue.empty() )
        return false;
    std::shared_ptr<NetEvent> top = _inEventQueue.front();
    return top->_eventTimeStamp <= _appRef->getUpdateCount()-_startGameTimeStamp;
}

/**
 * Returns the next custom inbound event and removes it from the queue.
 *
 * Requires there to be remaining inbound events.
 */
std::shared_ptr<NetEvent> NetEventController::popInEvent() {
	auto e = _inEventQueue.front();
	_inEventQueue.pop();
	return e;
}

/**
 * Queues an outbound event to be sent to peers.
 *
 * Queued events are sent when {@link updateNet()} is called. and cleared
 * after sending.
 */
void NetEventController::pushOutEvent(const std::shared_ptr<NetEvent>& e) {
	_outEventQueue.push_back(e);
}

/**
 * Unwraps the a byte vector data into a NetEvent.
 *
 * The controller automatically detects the type of event, spawns a new
 * empty instance of that event, and calls the event's
 * {@link NetEvent#deserialize()} method. This method is only called on
 * outbound events.
 */
std::shared_ptr<NetEvent> NetEventController::unwrap(const std::vector<std::byte>& data, std::string source) {
    CUAssertLog(data.size() >= MIN_MSG_LENGTH && (Uint8)data[0] < _newEventVector.size(), "Unwrapping invalid event");
    LWDeserializer deserializer;
    deserializer.receive(data);
    Uint8 eventType = (Uint8)deserializer.readByte();
    std::shared_ptr<NetEvent> e = _newEventVector[eventType]->newEvent();
    Uint64 eventTimeStamp = deserializer.readUint64();
    Uint64 receiveTimeStamp =  _appRef->getUpdateCount()-_startGameTimeStamp;
	e->setMetaData(eventTimeStamp, receiveTimeStamp, source);
    e->deserialize(std::vector(data.begin()+MIN_MSG_LENGTH,data.end()));
    return e;
}

/**
 * Wraps a NetEvent into a byte vector.
 *
 * The controller calls the event's {@link NetEvent#serialize()} method
 * and packs the event into byte data. This method is only on inbound
 * events.
 */
const std::vector<std::byte> NetEventController::wrap(const std::shared_ptr<NetEvent>& e) {
    LWSerializer serializer;
    serializer.writeByte((std::byte)getType(*e));
    //CULog("flag: %x",(std::byte)getType(*e));
    serializer.writeUint64(_appRef->getUpdateCount()-_startGameTimeStamp);
    serializer.writeByteVector(e->serialize());
	return serializer.serialize();
}
