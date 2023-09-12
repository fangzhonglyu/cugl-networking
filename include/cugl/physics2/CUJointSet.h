//
//  CUJointSet.h
//  cugl
//
//  Created by Barry Lyu on 9/11/23, based on the original Complex Obstacle class.
//  Copyright © 2023 Game Design Initiative at Cornell. All rights reserved.
//
//  Author: Barry Lyu, Walker White
//  Version: 11/9/23

#ifndef __CU_JOINT_SET_H__
#define __CU_JOINT_SET_H__

#include <box2d/b2_joint.h>
#include "CUObstacle.h"

#endif /* __CU_JOINT_SET_H__ */

namespace cugl {
    namespace physics2 {
    
#pragma mark -
#pragma mark Joint Set
    
class JointSet {
protected:
    /** A root body for this box 2d. */
    std::shared_ptr<Obstacle> _base;
    /** A complex physics object has multiple bodies */
    std::vector<std::shared_ptr<Obstacle>> _bodies;
    /** Potential joints for connecting the multiple bodies */
    std::vector<b2Joint*>  _joints;
    
#pragma mark -
#pragma mark Constructors
public:
    /**
     * Creates a new jointSet object at the origin.
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
     * the heap, use one of the static constructors instead (in this case, in
     * one of the subclasses).
     */
    JointSet() { }
    
    /**
     * Deletes this JointSet and all of its resources.
     *
     * We have to make the destructor public so that we can polymorphically
     * delete physics objects.
     *
     * The purpose of this destructor is to warn us if we delete an object
     * pre-maturely.
     */
    virtual ~JointSet();
    
#pragma mark -
#pragma mark Physics Methods
    /**
     * Returns the Box2D body for this object.
     *
     * This method only returrns the root body in this composite structure.  For more
     * fine-grain control, you should use the iterator methods.
     *
     * @return the Box2D body for this object.
     */
    virtual std::shared_ptr<Obstacle>& getBaseObstacle() { return _base; }
    
    /**
     * Returns the collection of component physics objects.
     *
     * While the vector does not allow you to modify the list, it is possible to
     * modify the individual objects.
     *
     * @return the collection of component physics objects.
     */
    const std::vector<std::shared_ptr<Obstacle>>& getBodies() { return _bodies; }
    
    /**
     * Returns the collection of joints for this object (may be empty).
     *
     * While the iterable does not allow you to modify the list, it is possible to
     * modify the individual joints.
     *
     * @return the collection of joints for this object.
     */
    const std::vector<b2Joint*>& getJoints() { return _joints; }
    
    /**
     * Creates the joints for this object.
     *
     * This method is executed as part of activePhysics. This is the primary method to
     * override for custom physics objects.
     *
     * @param world Box2D world to store joints
     *
     * @return true if object allocation succeeded
     */
    virtual bool createJoints(b2World& world) {  return false;  }
    
};

    }
}
