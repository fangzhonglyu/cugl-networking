//
//  CUNetPhysicsController.h
//  Networked Physics Library
// 
//  The header for the networked physics controller
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
//  Created by Barry Lyu on 6/28/23.
//

#ifndef __CU_NET_PHYSICS_CONTROLLER_H__
#define __CU_NET_PHYSICS_CONTROLLER_H__

#include <queue>
#include <cugl/netphysics/cu_net_events.h>
#include <cugl/physics2/CUObstacleWorld.h>

namespace cugl {

    /**
     * The CUGL networked physics classes.
     *
     * This internal namespace is for optional networking physics management package.
     * This package provides automatic synchronization of physics objects across devices.
     */
    namespace netphysics {

/** 
 * Struct for storing target parameters for interpolation
 * 
 * This struct is used to store the target parameters for interpolation.
 */
typedef struct{
    int curStep; // current step of interpolation
    int numSteps; // total steps designated for interpolation
    Vec2 P0; // source position
    Vec2 P1; // (For spline interpolation) control point 1
    Vec2 P2; // (For spline interpolation) control point 2
    Vec2 P3; // target position
    Vec2 targetVel; // target velocity
    float targetAngle; 
    float targetAngV;
    Vec2 I; // (For PID interpolation) Integral term sum
    Uint64 numI; // (For PID interpolation) Number of Integral terms summed
} targetParam;


/**
 * This class is the physics controller for the networked physics library.
 * 
 * This class holds a reference to an {@link ObstacleWorld} instance, and
 * respondible for all networked physics synchronization and management.
 * 
 * 
 */
class NetPhysicsController {
    
protected:

#pragma mark -
#pragma mark PhysicsController Stats

    /** Total number of interpolations done */
    long _itprCount;
    /** Total number of overriden interpolations */
    long _ovrdCount;
    /** Total number of steps interpolated */
    long _stepSum;

    /** The next available obstacle ID */
    Uint64 _objRotation;
    /** The physics world instance */
    std::shared_ptr<physics2::ObstacleWorld> _world;
    /** Cache of all on-ogoing interpolations */
    std::unordered_map<std::shared_ptr<physics2::Obstacle>,std::shared_ptr<netphysics::targetParam>> _cache;
    /** Temporary cache for removal after traversal */
    std::vector<std::shared_ptr<physics2::Obstacle>> _deleteCache;

    /** Vector of attached obstacle factories for obstacle creation */
    std::vector<std::shared_ptr<ObstacleFactory>> _obstacleFacts;
    /** Function for linking newly added obstacle to a scene node */
    std::function<void(const std::shared_ptr<physics2::Obstacle>&, const std::shared_ptr<scene2::SceneNode>&)> _linkSceneToObsFunc;
    /** Local map from added obstacles to scene nodes */
    std::unordered_map<std::shared_ptr<physics2::Obstacle>,std::shared_ptr<scene2::SceneNode>> _sharedObsToNodeMap;
    
    /** 
     * Helper function for linear object interpolation 
     * 
     * Formula: (target-source)/stepsLeft + source
     */
    float interpolate(int stepsLeft, float target, float source);

    /** Vector of generated events to be sent */
    std::vector<std::shared_ptr<NetEvent>> _outEvents;
    
public:
    /** 
     * Constructor for the controller without initialization. 
     */
    NetPhysicsController():
        _itprCount(0),_ovrdCount(0),_stepSum(0),_objRotation(0) {};

    /**
     * Allocates a new physics controller with the default values.
     */
    static std::shared_ptr<NetPhysicsController> alloc() {
		return std::make_shared<NetPhysicsController>();
	};

    /**
	 * Initializes a new physics controller with the given values.
     *
     * @param world The physics world instance to manage
     * @param shortUID shortUID assigned by NetEventController
     * @param linkSceneToObsFunc Function for linking obstacle to scene node
     * 
     * If the function is provided, the controller will automatically link 
     * newly added obstacles to their corresponding scene nodes by calling it.
     * The controller will also handle removal of scenes nodes when removing
     * obstacles.
     * 
     * Otherwise, if linkSceneToObsFunc is nullptr, the user will be 
     * responsible for linking obstacles to scene nodes. In that case, the
     * user is recommended to use custom NetEvent types to handle obstacle
     * creation without use of the physics controller.
     */
    void init(std::shared_ptr<physics2::ObstacleWorld>& world, Uint32 shortUID, std::function<void(const std::shared_ptr<physics2::Obstacle>&, const std::shared_ptr<scene2::SceneNode>&)> linkSceneToObsFunc) {
        _world = world;
        _world->setShortUID(shortUID);
        _linkSceneToObsFunc = linkSceneToObsFunc;
    }

    /**
     * Disposes the physics controller, releasing all resources.
     */
    void dispose() {
    	_world = nullptr;
		_linkSceneToObsFunc = nullptr;
    }

    /**
     * Destructor for the physics controller.
     */
    ~NetPhysicsController() {
		dispose();
	}

    /**
     * Add a custom obstacle factory to the controller.
     * 
     * This method allows users to leverage automatic object synchronization
     * to add obstacles to the physics world. See {@link ObstacleFactory} for
     * how to implement a custom obstacle factory.
     * 
     * @return The ID of the added obstacle factory
     */
    uint32 attachFactory(std::shared_ptr<ObstacleFactory> fact) {
        _obstacleFacts.push_back(fact);
        return (uint32)_obstacleFacts.size() - 1;
    }

    /**
     * Processes a physics object synchronization event.
     */
    void processPhysObjEvent(const std::shared_ptr<PhysObjEvent>& event);

    /**
     * Adds a shared obstacle to the physics world.
     * 
     * This method is used to add a shared obstacle across all clients.
     * 
     * @param factoryID The ID of the obstacle factory to use
     * @param bytes The serialized parameters taken by the obstacle factory
     * 
     * @return A pair of the added obstacle and its corresponding scene node
     * 
     * Users can uses the returned references to manually link the obstacle,
     * or for custom obstacle setups.
     */
    std::pair<std::shared_ptr<physics2::Obstacle>,std::shared_ptr<scene2::SceneNode>> addSharedObstacle(Uint32 factoryID, std::shared_ptr<std::vector<std::byte>> bytes);

    /**
     * Removes a shared obstacle from the physics world.
     *
     * If linkSceneToObsFunc was provided, the scene node will also be removed.
     */
    void removeSharedObstacle(std::shared_ptr<physics2::Obstacle> obj);

    /**
     * Resets the physics controller.
     */
    void reset(){
        _itprCount = 0;
        _ovrdCount = 0;
        _stepSum = 0;
        _cache.clear();
        _deleteCache.clear();
        _outEvents.clear();
        _sharedObsToNodeMap.clear();
    }
    
    /**
     * Returns true if the given obstacle is being interpolated.
     */
    bool isInSync(std::shared_ptr<physics2::Obstacle> obj);

    /**
     * Adds an object to interpolate with the given target parameters.
     * 
     * @param obj The obstacle to interpolate
     * @param param The target parameters for interpolation
     */
    void addSyncObject(std::shared_ptr<physics2::Obstacle> obj, std::shared_ptr<netphysics::targetParam> param);

    /**
     * Packs syncs for the physics info and adds it to _outEvents.
     */
    void packPhysSync();

    /**
     * Packs syncs for obstacle changes and adds it to _outEvents.
     */
    void packPhysObj();
    
    /**
     * Processes a physics synchronization event.
     */
    void processPhysSyncEvent(const std::shared_ptr<PhysSyncEvent>& event);
    
    /**
     * Updates the physics controller.
     */
    void fixedUpdate();

    /**
     * Returns the vector of generated events to be sent.
     */
    std::vector<std::shared_ptr<NetEvent>>& getOutEvents() {
		return _outEvents;
	}
};

    }
}
#endif /* __CU_NET_PHYSICS_CONTROLLER_H__ */
