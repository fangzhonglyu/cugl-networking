//
//  CUPhysSyncEvent.h
//  Networked Physics Library
//
//  This class represents physics synchronization events. 
//  It is handled by the NetEventController internally.
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

#ifndef __CU_PHYS_SYNC_EVENT_H__
#define __CU_PHYS_SYNC_EVENT_H__

#include <cugl/netphysics/CUNetEvent.h>
#include <SDL_stdinc.h>
#include <unordered_set>
#include <cugl/net/CUNetcodeSerializer.h>
#include <cugl/physics2/CUObstacle.h>

namespace cugl {

    /**
     * The CUGL networked physics classes.
     *
     * This internal namespace is for optional networking physics management package.
     * This package provides automatic synchronization of physics objects across devices.
     */
    namespace netphysics {


/**
 * The struct for an object snapshot. Contains the object's global Id, position, and velocity.
 */
typedef struct {
    Uint64 objId;
    float x;
    float y;
    float vx;
    float vy;
    float angle;
    float vAngular;
} ObjParam;

/**
 * This class represents a message for the networked physics library to synchronize object 
 * positions. It should only be used by the networked physics library, not for custom game 
 * informations.
 */
class PhysSyncEvent : public NetEvent {
private:
    /** 
     * The set of objectIds of all objects added to be serialized. 
     * Used to prevent duplicate objects. 
     */
    std::unordered_set<Uint64> _objSet;
    /** The serializer for converting basic types to byte vectors. */
    net::NetcodeSerializer _serializer;
    /** The deserializer for converting byte vectors to basic types. */
    net::NetcodeDeserializer _deserializer;
protected:
    /** The vector of added object snapshots. */
    std::vector<ObjParam> _syncList;

public:
    /**
     * This method takes a snapshot of an obstacle's current position and velocity, and adds 
     * the snapshot to the list for serialization.
     *
     * @param obj the obstacle reference to add, duplicate obstacles would be ignored
     * @param id the global Id of the obstacle
     */
    void addObj(const std::shared_ptr<physics2::Obstacle>& obj, Uint64 id) {
        if (_objSet.count(id))
            return;

        _objSet.insert(id);
        ObjParam param;
        param.objId = id;
        param.x = obj->getX();
        param.y = obj->getY();
        param.vx = obj->getVX();
        param.vy = obj->getVY();
        param.angle = obj->getAngle();
        param.vAngular = obj->getAngularVelocity();
        _syncList.push_back(param);
    }

    /**
	 * This method returns a reference of the current vector of object snapshots added.
	 */
    const std::vector<ObjParam>& getSyncList() const {
        return _syncList;
    }

    /**
	 * This method allocates a new physic synchronization event.
	 */
    static std::shared_ptr<PhysSyncEvent> alloc() {
        return std::make_shared<PhysSyncEvent>();
    }

    /**
     * This method is used by the NetEventController to create a new event of using a
     * reference of the same type.
     *
     * Not that this method is not static, it differs from the static alloc() method
     * and all methods must implement this method.
     *
     * @return a new event of the same type
     */
    std::shared_ptr<NetEvent> newEvent() override {
        return std::make_shared<PhysSyncEvent>();
    }

    /**
     * This method takes the current list of snapshots and serializes them to a byte vector.
     */
    std::vector<std::byte> serialize() override {
        _serializer.reset();
        _serializer.writeUint64((Uint64)_syncList.size());
        for (auto it = _syncList.begin(); it != _syncList.end(); it++) {
            ObjParam& obj = (*it);
            _serializer.writeUint64(obj.objId);
            _serializer.writeFloat(obj.x);
            _serializer.writeFloat(obj.y);
            _serializer.writeFloat(obj.vx);
            _serializer.writeFloat(obj.vy);
            _serializer.writeFloat(obj.angle);
            _serializer.writeFloat(obj.vAngular);
        }
        return _serializer.serialize();
    }

    /**
     * This method unpacks a byte vector to a list of snapshots that can be read and used for 
     * physics synchronizations.
     * 
     * @param data the byte vector to deserialize
     */
    void deserialize(const std::vector<std::byte>& data) override {
        if (data.size() < 4)
            return;

        _deserializer.reset();
        _deserializer.receive(data);
        Uint64 numObjs = _deserializer.readUint64();
        for (size_t i = 0; i < numObjs; i++) {
            ObjParam param;
            param.objId = _deserializer.readUint64();
            param.x = _deserializer.readFloat();
            param.y = _deserializer.readFloat();
            param.vx = _deserializer.readFloat();
            param.vy = _deserializer.readFloat();
            param.angle = _deserializer.readFloat();
            param.vAngular = _deserializer.readFloat();
            _syncList.push_back(param);
        }
    }
};

    }
}

#endif /* __CU_PHYS_SYNC_EVENT_H__ */