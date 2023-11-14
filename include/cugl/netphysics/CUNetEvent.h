//
//  CUGameEvent.h
//  Networked Physics Library
//
//  This class provides an object-oriented approach for representing shared data.
//  It is the base class for all events that are sent through the network. 
//  Users can use this class to encapsulate serialization and deserialization 
//  of data send through the network.
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
//  Author: Barry Lyu
//  Version: 11/13/23
//

#ifndef __CU_NET_EVENT_H__
#define __CU_NET_EVENT_H__

#include <SDL_stdinc.h>
#include <vector>
#include <string>
#include <memory>
#include <cugl/netphysics/CULWSerializer.h>
#include <cugl/netphysics/CULWDeserializer.h>

namespace cugl {

    /**
     * The CUGL networked physics classes.
     *
     * This internal namespace is for optional networking physics management package.
     * This package provides automatic synchronization of physics objects across devices.
     */
    namespace netphysics {

/**
 * This is a template class for all communication messages between machines.
 *
 * Any information that needs to be sent through the network during gameplay should 
 * be wrapped in a NetEvent object. Custom events types can be made by subclassing 
 * this class and adding parameters as necessary.
 */
class NetEvent {
private:
    //==============================META DATA================================
    /** 
     * These fields are set by the NetEventController when an event is sent or
     * received. Don't include them in the serialize() and deserialize() methods.
     */

    /** The time of the event from the sender. */
    Uint64 _eventTimeStamp;
    /** The time when the event was received by the recipient. */
    Uint64 _receiveTimeStamp;
    /** The ID of the sender. */
    std::string _sourceID;

    //==============================META DATA================================

    /** 
     * This method is used by the NetEventController to set the meta data of the event.
     * 
     * @param eventTimeStamp    the timestamp of the event from the sender
     * @param receiveTimeStamp  the timestamp when the event was received by the recipient
     * @param sourceID          the ID of the sender
     */
    void setMetaData(Uint64 eventTimeStamp, Uint64 receiveTimeStamp, const std::string sourceID) {
        _eventTimeStamp = eventTimeStamp;
        _receiveTimeStamp = receiveTimeStamp;
        _sourceID = sourceID;
    }

    friend class NetEventController;

public:
    /**
     * This method is used by the NetEventController to create a new event of using a
     * reference of the same type.
     *
     * Not that this method is not static, it differs from the static alloc() method
     * and all methods must implement this method.
     *
     * @return a new event of the same type
     */
    virtual std::shared_ptr<NetEvent> newEvent() {
        return std::make_shared<NetEvent>();
    }
    /**
     * Serialize any paramater that the event contains to a vector of bytes.
     */
    virtual std::vector<std::byte> serialize() {
        return std::vector<std::byte>();
    }
    /**
     * Deserialize a vector of bytes and set the corresponding parameters.
     *
     * @param data  a byte vector packed by serialize()
     *
     * This function should be the "reverse" of the serialize() function: it
     * should be able to recreate a serialized event entirely, setting all the 
     * useful parameters of this class.
     */
    virtual void deserialize(const std::vector<std::byte>& data) { }

    /**
     * This method returns the timestamp of the event from the sender.
     * 
     * Valid only if the event was received by this client.
     */
    Uint64 getEventTimeStamp() const { return _eventTimeStamp; }
    
    /**
	 * This method returns the timestamp when the event was received 
     * by this client.
     * 
     * Valid only if the event was received by this client.
	 */
    Uint64 getReceiveTimeStamp() const { return _receiveTimeStamp; }

    /**
	 * This method returns the ID of the sender.
	 * 
	 * Valid only if the event was received by this client.
	 */
    const std::string getSourceId() const { return _sourceID; }
};
    }
}
#endif /* __CU_NET_EVENT_H__ */
