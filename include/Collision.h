#ifndef COLLISION_H
#define COLLISION_H

#include "Physics.h"
#include <iostream>
#include <math.h>

/*
Abstract collision-related types

A Collidiable is any static structure that a physics object may collide with. This includes
aggregate objects such as tiles, grids, etc.

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

class Collidable {
    public:
        virtual FreePathResult getFreePath(const Vector<float, 2> pos,
                                           const Vector<float, 2> dir,
                                           float radius);
};

class Collider: public Collidable {
    public:
        virtual void collide(CollidingObject* obj); // modifies obj
};

class EdgeCollider: public Collider {
    public:
        Vector<float, 2> position;
        Vector<float, 2> normal;
        EdgeCollider(Vector<float, 2> pos, Vector<float, 2> norm): position(pos), normal(norm) {}
        FreePathResult getFreePath(const Vector<float, 2> pos,
                                const Vector<float, 2> dir,
                                float radius);
        void collide(CollidingObject* const obj);
};

class CollidingObject: public PhysicsObject {
    public:
        float radius;
        float elasticity;
        Collider* environment;
        CollidingObject() {};
        CollidingObject(float x, float y, float r, float e, Collider* env): PhysicsObject(x, y) {
            radius = r;
            elasticity = e;
            environment = env;
        }
        void step(float dt);
};

// make vectors printable
inline std::ostream& operator << (std::ostream& os, FreePathResult fpr) {
    os << "Path{" << sqrt(fpr.distSq) << ", " << fpr.target << "}";
    return os;
}

#endif