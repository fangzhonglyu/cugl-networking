//
//  CUPhysObjEvent.h
//  Networked Physics Library
//
//  This class represents an event of obstacle state change.
//  This includes setting the position and velocity manually, or other
//  changes to the obstacle state.
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

#ifndef __CU_PHYS_OBJ_EVENT_H__
#define __CU_PHYS_OBJ_EVENT_H__

#include <cugl/netphysics/CUNetEvent.h>
#include <cugl/netphysics/CUObstacleFactory.h>
#include <cugl/physics2/CUObstacle.h>
#include <SDL_stdinc.h>

namespace cugl {

    /**
     * The CUGL networked physics classes.
     *
     * This internal namespace is for optional networking physics management package.
     * This package provides automatic synchronization of physics objects across devices.
     */
    namespace netphysics {

/** 
 * This class represents an event of obstacle state change.
 * Instances of this class will be created automatically by the physics world and gathered
 * by the network controller and gathered by the network controller.
 * 
 * This includes setting the position and velocity manually, or other changes to the obstacle state.
 * As wee
 * 
 */
class PhysObjEvent : public NetEvent {
public:
    /** Enum for the type of the event. */
    enum Type
    {
        OBJ_CREATION,
        OBJ_DELETION,
        OBJ_BODY_TYPE,
        OBJ_POSITION,
        OBJ_VELOCITY,
        OBJ_ANGLE,
        OBJ_ANGULAR_VEL,
        OBJ_BOOL_CONSTS,
        OBJ_FLOAT_CONSTS,
        OBJ_OWNER_ACQUIRE,
        OBJ_OWNER_RELEASE
    };

protected:
    /** The type of the event. */
    Type _type;
    /** The obstacle global id. */
    Uint64 _objId;

    /** 
     * The obstacle factory id. 
     * Obtained by calling {@link NetPhysicsController::attachFactory()}
     */
    Uint32 _obstacleFactId;

    
    /** The packed parameter for obstacle creation. */
    std::shared_ptr<std::vector<std::byte>> _packedParam;

public:
    /** field for OBJ_POSITION */
    Vec2 _pos;
    /** field for OBJ_VELOCITY */
    Vec2 _vel;
    /** field for OBJ_ANGLE */
    float _angle;
    /** field for OBJ_ANGULAR_VEL */
    float _angularVel;

    /** fields for OBJ_BOOL_CONSTS */
    bool _isStatic;
    bool _isEnabled;
    bool _isAwake;
    bool _isSleepingAllowed;
    bool _isFixedRotation;
    bool _isBullet;
    bool _isSensor;

    /** fields for OBJ_FLOAT_CONSTS */
    float _density;
    float _friction;
    float _restitution;
    float _linearDamping;
    float _angularDamping;
    float _gravityScale;
    float _mass;
    float _inertia;
    Vec2 _centroid;

    /** field for OBJ_BODY_TYPE */
    b2BodyType _bodyType;
    
    /** field for OBJ_OWNER_ACQUIRE */
    Uint64 _duration;

    /** Serializer for packing data */
    LWSerializer _serializer;
    /** Deserializer for unpacking data */
    LWDeserializer _deserializer;

public:

   

    /**
     * Returns the obstacle global id of this event.
     */
    Uint64 getObjId() const { return _objId; }

    /**
	 * Returns the type of this event.
	 */
    Type getType() const { return _type; }

    /**
    * Returns the obstacle factory id of this event.
    *
    * Only valid for OBJ_CREATION events.
    */
    Uint32 getObstacleFactId() const { return _obstacleFactId; }

    /**
     * Returns the packed parameters for creating the obstacle.
     * 
     * Only valid for OBJ_CREATION events.
     */  
    const std::shared_ptr<std::vector<std::byte>> getPackedParam() const { return _packedParam; }

    /**
	 * Initializes an empty event to OBJ_CREATION.
     * 
     * This event symbolizes the creation of an obstacle.
	 * 
     * @param obstacleFactId The obstacle factory id.
     * @param objId The obstacle global id.
     * @param packedParam The packed parameters for creating the obstacle.
	 */
    void initCreation(Uint32 obstacleFactId, Uint64 objId, std::shared_ptr<std::vector<std::byte>> packedParam) {
        _type = OBJ_CREATION;
        _obstacleFactId = obstacleFactId;
        _objId = objId;
        _packedParam = packedParam;
    }

    /**
     * Initializes an empty event to OBJ_DELETION.
     * 
     * This event symbolizes the deletion of an obstacle.
     *
     * @param objId The obstacle global id.
     */
    void initDeletion(Uint64 objId) {
        _type = OBJ_DELETION;
        _objId = objId;
    }

    /**
     * Initializes an empty event to OBJ_POSITION.
     * 
     * This event symbolizes setting the position of an obstacle.
     *
     * @param objId The obstacle global id.
     */
    void initPos(Uint64 objId, Vec2 pos) {
        _type = OBJ_POSITION;
        _objId = objId;
        _pos = pos;
    }

    /**
	 * Initializes an empty event to OBJ_VELOCITY.
	 * 
	 * This event symbolizes setting the velocity of an obstacle.
	 *
	 * @param objId The obstacle global id.
	 */
    void initVel(Uint64 objId, Vec2 vel) {
        _type = OBJ_VELOCITY;
        _objId = objId;
        _vel = vel;
    }

    /**
     * Initializes an empty event to OBJ_ANGLE.
     * 
     * This event symbolizes setting the angle of an obstacle.
     *
     * @param objId The obstacle global id.
     */
    void initAngle(Uint64 objId, float angle) {
        _type = OBJ_ANGLE;
        _objId = objId;
        _angle = angle;
    }

    /**
     * Initializes an empty event to OBJ_ANGULAR_VEL.
     *
     * This event symbolizes setting the anglular velocity of an obstacle.
     *
     * @param objId The obstacle global id.
     */
    void initAngularVel(Uint64 objId, float angularVel) {
        _type = OBJ_ANGULAR_VEL;
        _objId = objId;
        _angularVel = angularVel;
    }

    /**
	 * Initializes an empty event to OBJ_BODY_TYPE.
	 *
	 * This event symbolizes setting the body type of an obstacle.
	 *
	 * @param objId The obstacle global id.
	 */
    void initBodyType(Uint64 objId, b2BodyType bodyType) {
        _type = OBJ_BODY_TYPE;
        _objId = objId;
        _bodyType = bodyType;
    }

    /**
	 * Initializes an empty event to OBJ_BOOL_CONSTS.
	 *
	 * This event symbolizes setting the boolean constants of an obstacle.
     * Due to the relatively rarer use of these constants, they are packed 
     * into a single event.
     * 
     * This includes the following properties in the Obstacle class:
     * - isEnabled
     * - isAwake
     * - isSleepingAllowed
     * - isFixedRotation
     * - isBullet
     * - isSensor
	 *
	 * @param objId The obstacle global id.
	 */
    void initBoolConsts(Uint64 objId, bool isEnabled, bool isAwake, bool isSleepingAllowed, bool isFixedRotation, bool isBullet, bool isSensor) {
        _type = OBJ_BOOL_CONSTS;
        _objId = objId;
        _isEnabled = isEnabled;
        _isAwake = isAwake;
        _isSleepingAllowed = isSleepingAllowed;
        _isFixedRotation = isFixedRotation;
        _isBullet = isBullet;
        _isSensor = isSensor;
    }

    /**
    * Initializes an empty event to OBJ_FLOAT_CONSTS.
    *  
    * This event symbolizes setting the float constants of an obstacle.
    * Due to the relatively rarer use of these constants, they are packed
    * into a single event.
    * 
    * This includes the following properties in the Obstacle class:
    * - density
    * - friction
    * - restitution
    * - linearDamping
    * - angularDamping
    * - gravityScale
    * - mass
    * - inertia
    * - centroid
    * 
    * @param objId The obstacle global id.
    */
    void initFloatConsts(Uint64 objId, float density, float friction, float restitution, float linearDamping, float angularDamping, float gravityScale, float mass, float inertia, Vec2 centroid) {
        _type = OBJ_FLOAT_CONSTS;
        _objId = objId;
        _density = density;
        _friction = friction;
        _restitution = restitution;
        _linearDamping = linearDamping;
        _angularDamping = angularDamping;
        _gravityScale = gravityScale;
        _mass = mass;
        _inertia = inertia;
        _centroid = centroid;
    }
    
    void initOwnerAcquire(Uint64 objId, Uint64 duration){
        _type = OBJ_OWNER_ACQUIRE;
        _objId = objId;
        _duration = duration;
    }
    
    void initOwnerRelease(Uint64 objId){
        _type = OBJ_OWNER_RELEASE;
        _objId = objId;
    }

    /** 
     * Allocates a new OBJ_CREATION event.
     * 
     * Shortcut for creating a shared object and calling {@link initCreation()}.
	 */
    static std::shared_ptr<PhysObjEvent> allocCreation(Uint32 obstacleFactId, Uint64 objId, std::shared_ptr<std::vector<std::byte>> packedParam) {
        auto e = std::make_shared<PhysObjEvent>();
        e->initCreation(obstacleFactId, objId, packedParam);
        return e;
    }

    /** 
	 * Allocates a new OBJ_DELETION event.
	 * 
	 * Shortcut for creating a shared object and calling {@link initDeletion()}.
     */
    static std::shared_ptr<PhysObjEvent> allocDeletion(Uint64 objId) {
        auto e = std::make_shared<PhysObjEvent>();
        e->initDeletion(objId);
        return e;
    }

    /**
     * Allocates a new OBJ_POSITION event.
     * 
     * Shortcut for creating a shared object and calling {@link initPos()}.
     */
    static std::shared_ptr<PhysObjEvent> allocPos(Uint64 objId, Vec2 pos) {
        auto e = std::make_shared<PhysObjEvent>();
        e->initPos(objId, pos);
        return e;
    }

    /**
     * Allocates a new OBJ_VELOCITY event.
     * 
     * Shortcut for creating a shared object and calling {@link initVel()}.
     */
    static std::shared_ptr<PhysObjEvent> allocVel(Uint64 objId, Vec2 vel) {
        auto e = std::make_shared<PhysObjEvent>();
        e->initVel(objId, vel);
        return e;
    }

    /**
     * Allocates a new OBJ_ANGLE event.
     *
     * Shortcut for creating a shared object and calling {@link initAngle()}.
     */
    static std::shared_ptr<PhysObjEvent> allocAngle(Uint64 objId, float angle) {
        auto e = std::make_shared<PhysObjEvent>();
        e->initAngle(objId, angle);
        return e;
    }

    /** 
     * Allocates a new OBJ_ANGULAR_VEL event.
     * 
     * Shortcut for creating a shared object and calling {@link initAngularVel()}.
     */
    static std::shared_ptr<PhysObjEvent> allocAngularVel(Uint64 objId, float angularVel) {
        auto e = std::make_shared<PhysObjEvent>();
        e->initAngularVel(objId, angularVel);
        return e;
    }

    /** 
	 * Allocates a new OBJ_BODY_TYPE event.
	 * 
	 * Shortcut for creating a shared object and calling {@link initBodyType()}.
	 */
    static std::shared_ptr<PhysObjEvent> allocBodyType(Uint64 objId, b2BodyType bodyType) {
        auto e = std::make_shared<PhysObjEvent>();
        e->initBodyType(objId, bodyType);
        return e;
    }

    /**
     * Allocates a new OBJ_BOOL_CONSTS event.
	 *
	 * Shortcut for creating a shared object and calling {@link initBoolConsts()}.
	 */
    static std::shared_ptr<PhysObjEvent> allocBoolConsts(Uint64 objId, bool isEnabled, bool isAwake, bool isSleepingAllowed, bool isFixedRotation, bool isBullet, bool isSensor) {
        auto e = std::make_shared<PhysObjEvent>();
        e->initBoolConsts(objId, isEnabled, isAwake, isSleepingAllowed, isFixedRotation, isBullet, isSensor);
        return e;
    }

    /**
	 * Allocates a new OBJ_FLOAT_CONSTS event.
	 *
	 * Shortcut for creating a shared object and calling {@link initFloatConsts()}.
	 */
    static std::shared_ptr<PhysObjEvent> allocFloatConsts(Uint64 objId, float density, float friction, float restitution, float linearDamping, float angularDamping, float gravityScale, float mass, float inertia, Vec2 centroid) {
        auto e = std::make_shared<PhysObjEvent>();
        e->initFloatConsts(objId, density, friction, restitution, linearDamping, angularDamping, gravityScale, mass, inertia, centroid);
        return e;
    }
    
    
    static std::shared_ptr<PhysObjEvent> allocOwnerAcquire(Uint64 objId, Uint64 duration){
        auto e = std::make_shared<PhysObjEvent>();
        e->initOwnerAcquire(objId,duration);
        return e;
    }
    
    static std::shared_ptr<PhysObjEvent> allocOwnerRelease(Uint64 objId){
        auto e = std::make_shared<PhysObjEvent>();
        e->initOwnerRelease(objId);
        return e;
    }
    

    /**
	 * Returns a newly allocated copy of this event.
	 */
    std::shared_ptr<NetEvent> newEvent() override {
        return std::make_shared<PhysObjEvent>();
    }

    /**
     * Serializes this event into a byte vector.
     */
    std::vector<std::byte> serialize() override {
        _serializer.reset();
        _serializer.writeUint32((uint32)_type);
        _serializer.writeUint64(_objId);
        switch (_type) {
        case PhysObjEvent::OBJ_CREATION:
            _serializer.writeUint32(_obstacleFactId);
            _serializer.writeByteVector(*_packedParam);
            break;
        case PhysObjEvent::OBJ_DELETION:
            break;
        case PhysObjEvent::OBJ_BODY_TYPE:
            _serializer.writeUint32(_bodyType);
            break;
        case PhysObjEvent::OBJ_POSITION:
            _serializer.writeFloat(_pos.x);
            _serializer.writeFloat(_pos.y);
            break;
        case PhysObjEvent::OBJ_VELOCITY:
            _serializer.writeFloat(_vel.x);
            _serializer.writeFloat(_vel.y);
            break;
        case PhysObjEvent::OBJ_ANGLE:
            _serializer.writeFloat(_angle);
            break;
        case PhysObjEvent::OBJ_ANGULAR_VEL:
            _serializer.writeFloat(_angularVel);
            break;
        case PhysObjEvent::OBJ_BOOL_CONSTS:
            _serializer.writeBool(_isEnabled);
            _serializer.writeBool(_isAwake);
            _serializer.writeBool(_isSleepingAllowed);
            _serializer.writeBool(_isFixedRotation);
            _serializer.writeBool(_isBullet);
            _serializer.writeBool(_isSensor);
            break;
        case PhysObjEvent::OBJ_FLOAT_CONSTS:
            _serializer.writeFloat(_density);
            _serializer.writeFloat(_friction);
            _serializer.writeFloat(_restitution);
            _serializer.writeFloat(_linearDamping);
            _serializer.writeFloat(_angularDamping);
            _serializer.writeFloat(_gravityScale);
            _serializer.writeFloat(_mass);
            _serializer.writeFloat(_inertia);
            _serializer.writeFloat(_centroid.x);
            _serializer.writeFloat(_centroid.y);
            break;
        case PhysObjEvent::OBJ_OWNER_ACQUIRE:
            _serializer.writeUint64(_duration);
            break;
        case PhysObjEvent::OBJ_OWNER_RELEASE:
            break;
        default:
            CUAssertLog(false, "Serializing invalid obstacle event type");
        }
        return _serializer.serialize();
    }

    /**
	 * Deserializes this event from a byte vector.
     * 
     * This method will set the type of the event and all relevant fields.
	 */
    void deserialize(const std::vector<std::byte>& data) override {
        if (data.size() < sizeof(Uint32) + sizeof(Uint64))
            return;
        _deserializer.reset();
        _deserializer.receive(data);
        _type = (Type)_deserializer.readUint32();
        _objId = _deserializer.readUint64();
        switch (_type) {
        case PhysObjEvent::OBJ_CREATION:
            _obstacleFactId = _deserializer.readUint32();
            _packedParam = std::make_shared<std::vector<std::byte>>(data.begin() + 2 * sizeof(Uint32) + sizeof(Uint64), data.end());
            break;
        case PhysObjEvent::OBJ_DELETION:
            break;
        case PhysObjEvent::OBJ_BODY_TYPE:
            _bodyType = (b2BodyType)_deserializer.readUint32();
            break;
        case PhysObjEvent::OBJ_POSITION:
            _pos.x = _deserializer.readFloat();
            _pos.y = _deserializer.readFloat();
            break;
        case PhysObjEvent::OBJ_VELOCITY:
            _vel.x = _deserializer.readFloat();
            _vel.y = _deserializer.readFloat();
            break;
        case PhysObjEvent::OBJ_ANGLE:
            _angle = _deserializer.readFloat();
            break;
        case PhysObjEvent::OBJ_ANGULAR_VEL:
            _angularVel = _deserializer.readFloat();
            break;
        case PhysObjEvent::OBJ_BOOL_CONSTS:
            _isEnabled = _deserializer.readBool();
            _isAwake = _deserializer.readBool();
            _isSleepingAllowed = _deserializer.readBool();
            _isFixedRotation = _deserializer.readBool();
            _isBullet = _deserializer.readBool();
            _isSensor = _deserializer.readBool();
            break;
        case PhysObjEvent::OBJ_FLOAT_CONSTS:
            _density = _deserializer.readFloat();
            _friction = _deserializer.readFloat();
            _restitution = _deserializer.readFloat();
            _linearDamping = _deserializer.readFloat();
            _angularDamping = _deserializer.readFloat();
            _gravityScale = _deserializer.readFloat();
            _mass = _deserializer.readFloat();
            _inertia = _deserializer.readFloat();
            _centroid.x = _deserializer.readFloat();
            _centroid.y = _deserializer.readFloat();
            break;
        case PhysObjEvent::OBJ_OWNER_ACQUIRE:
            _duration = _deserializer.readUint64();
            break;
        case PhysObjEvent::OBJ_OWNER_RELEASE:
            break;
        default:
            CUAssertLog(false, "Deserializing invalid obstacle event type");
        }
    }

};

    }
}

#endif /* __CU_PHYS_OBJ_EVENT_H__ */
