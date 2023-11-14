//
//  CUPhysSyncEvent.h
//  Networked Physics Library
//
//  This class provides a standard template for an shareable creation of 
//  obstacles. Users can create their own factory and subclass this class
//  to create their custom obstacles.
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

#ifndef __CU_OBSTACLE_FACTORY_H__
#define __CU_OBSTACLE_FACTORY_H__

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
 * This class provides a standard template for an shareable creation of obstacles.
 * These factories can be attached to the provided net physics controller so that 
 * creation of obstacles can be synchronized across devices without the need to
 * send texture and body information.
 * 
 * In order for every client to be updated when a new obstacle is created, they must 
 * have a uniform way to share info about the obstacle to be created. However, syncing
 * the texture and body data is very costly, so this class is a template for creating
 * an obstacle and (optionally) a scene node from serialized parameters.
 */
class ObstacleFactory {
public:

    /**
	 * Returns a newly allocated obstacle factory.
	 */
    static std::shared_ptr<ObstacleFactory> alloc() {
        return std::make_shared<ObstacleFactory>();
    };

    /**
     * Takes a serialized parameters and creates an obstacle and (optionally) a scene node.
     * 
     * @param params    The serialized parameters for the obstacle
     * 
     * If you want the obstacle to be accompanied by a scene node, you must return a pair
     * of the obstacle and the scene node, as well as provide call the 
     * {@link NetEventController#enablePhysics() } method and provide it a function for linking
     * the obstacle and scene node.
     * 
     * Otherwise, you can return a nullptr for the scene node and the obstacle will be created
     * without a scene node.
     * 
     * Some typically parameters to be serialized are:
     * - The texture name
     * - Size of the obstacle
     * - Position/Velocity of the obstacle
     * - Any metadata about the obstacle
     */
    virtual std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(const std::vector<std::byte>& params);
};

    }
}

#endif /* __CU_OBSTACLE_FACTORY_H__ */