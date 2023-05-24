#ifndef COLLISION_H
#define COLLISION_H

#include "Physics.h"
#include <iostream>
#include <vector>
#include <math.h>

/*
Abstract collision-related types

A Collider is a physical object that can be collided with. This cannot be an aggregate; it
must have its own material properties, etc.

A CollidingObject is a PhysicsObject that may interact with Colliders. As of right now,
moving objects must be circular. CollidingObjects have a specific "collision universe"
that is passed in at object creation and may be changed as desired; this is an aggregate
Collidable representing everything the object can collide with.

The process for detecting collisions goes roughly like this:

A CollidingObject wishes to move. It asks its Collidable what the nearest object on its
path is and what the distance to that object is. If the distance is less than the distance
it needs to travel this frame, it steps as normal. Otherwise, things get interesting.
*/
class CollidingObject;
class Collider;

struct FreePathResult {
    float distSq;
    Collider* target;
};
// make free paths printable
inline std::ostream& operator << (std::ostream& os, FreePathResult fpr) {
    os << "Path{" << sqrt(fpr.distSq) << ", " << fpr.target << "}";
    return os;
}

/**
 * A structure that a physics object may collide with. May be a single object or an aggregate.
*/
class Collidable {
    public:
        /**
         * Gets the available free path along a given vector.
         * pos: The position of the physics object
         * dir: The direction the physics object is traveling
         * radius: The radius of the physics object.
         * Returns: a FreePathResult containing the available distance and the actual Collider hit (or null if no collider is hit).
        */
        virtual FreePathResult getFreePath(const Vector<float, 2> pos,
                                           const Vector<float, 2> dir,
                                           float radius);
};

class Collider: public Collidable {
    public:
        /**
         * Handles collision with a physics object. Usually this will mean bouncing, but it could mean destroying it, damaging it, or otherwise.
         * obj: The object colliding with this collider. This object will usually be modified.
        */
        virtual void collide(CollidingObject* obj); // modifies obj
};

class CollidingObject: public PhysicsObject {
    public:
        float radius;
        Collidable* environment;
        CollidingObject(float x, float y, float m, float r, Elasticity e, Collidable* env): PhysicsObject(m, x, y, e), radius(r), environment(env) {}
        void stepVelocity(float dt);
};

/**
 * Collision Shapes
*/

/**
 * EdgeCollider
 * 
 * One-sided, infinite straight line collider
 * Defined by a center point and normal vector
*/
class EdgeCollider: public Collider {
    public:
        Vector<float, 2> position;
        Vector<float, 2> normal;
        Elasticity elasticity;
        EdgeCollider(Vector<float, 2> pos, Vector<float, 2> norm, Elasticity e = PhysicsObject::ELAS_DEFAULT):
            position(pos), normal(norm), elasticity(e) {}
        FreePathResult getFreePath(const Vector<float, 2> pos,
                                const Vector<float, 2> dir,
                                float radius);
        void collide(CollidingObject* const obj);
};

/**
 * SegmentCollider
 * 
 * One-sided, limited-length straight line collider
 * Defined by an endpoint and an offset
*/
class SegmentCollider: public Collider {
    public:
        Vector<float, 2> position;
        Vector<float, 2> offset;
        Elasticity elasticity;
        SegmentCollider(Vector<float, 2> pos, Vector<float, 2> offs, Elasticity e = PhysicsObject::ELAS_DEFAULT):
            position(pos), offset(offs), elasticity(e) {}
        FreePathResult getFreePath(const Vector<float, 2> pos,
                                const Vector<float, 2> dir,
                                float radius);
        void collide(CollidingObject* const obj);
};

/**
 * Collision Aggregates
*/

/**
 * CollisionGroup
 * 
 * Simple dynamic group of colliders
 * Checks collision iteratively, no geometric shortcuts
*/
class CollisionGroup: public Collidable {
    public:
        std::vector<Collidable*> colliders;
        FreePathResult getFreePath(const Vector<float, 2> pos,
                                           const Vector<float, 2> dir,
                                           float radius) {
            FreePathResult result = {INFINITY, nullptr};
            for(int i = 0; i < colliders.size(); i++) {
                FreePathResult newres = colliders[i]->getFreePath(pos, dir, radius);
                if(newres.distSq < result.distSq) {
                    result = newres;
                }
            }
            return result;
        }
};

#endif