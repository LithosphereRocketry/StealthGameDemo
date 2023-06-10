#ifndef COLLISION_H
#define COLLISION_H

#include "Physics.h"
#include <iostream>
#include <vector>
#include <memory>
#include <math.h>

/*
Abstract collision-related types

A Collider is a physical object that can be collided with. This cannot be an
aggregate; it must have its own material properties, etc.

A CollidingObject is a PhysicsObject that may interact with Colliders. As of
right now, moving objects must be circular. CollidingObjects have a specific
"collision universe" that is passed in at object creation and may be changed as 
desired; this is an aggregate Collidable representing everything the object can 
collide with.

The process for detecting collisions goes roughly like this:

A CollidingObject wishes to move. It asks its Collidable what the nearest object
on its path is and what the distance to that object is. If the distance is less 
than the distance it needs to travel this frame, it steps as normal. Otherwise, 
it advances up to the point it would touch the object, its velocity is changed
to account for colliding with the object, and then it continues to move,
starting a new collision search.

When an object is accelerating while colliding, the change in velocity for that
frame is not applied instantly, because this can lead to gravity 'spiking' the
object against the ground and causing it to bounce unnaturally while at rest.
Instead, velocity is transferred from the 'new speed' pool to the 'old speed'
pool on each step proportional to the distance travelled. The object always
steps and advances as if it had the full speed granted by both speed pools,
because the object moving in a straight line makes math a lot easier.

An issue with this system occurs when an object is physically touching the
ground while wanting to step downward. In this case (usually occurring while the
object is stationary or sliding) the object is declared 'stuck' in that object
for that frame, and its old and new speeds are forced parallel to the surface.
If an object becomes stuck in two surfaces at once, it is assumed to be wedged
at an inside corner, and its velocity is set to 0.
*/
class CollidingObject;
class Collider;

enum FreePathType {
    FREE,
    COLLIDING,
    STUCK,
    FALLBACK
};

struct FreePathResult {
    FreePathType type;
    float distSq;
    Collider* target;
};
// make free paths printable
inline std::ostream& operator << (std::ostream& os, FreePathResult fpr) {
    os << "Path{" << sqrt(fpr.distSq) << ", " << fpr.target << "}";
    return os;
}

/**
 * A structure that a physics object may collide with. May be a single object or
 *  an aggregate.
*/
class Collidable {
    public:
        /**
         * Gets the available free path along a given vector.
         * pos: The position of the physics object
         * dir: The direction the physics object is traveling
         * radius: The radius of the physics object.
         * Returns: a FreePathResult containing the available distance and the
         *          actual Collider hit (or null if no collider is hit).
        */
        virtual FreePathResult getFreePath(const Vector<float, 2> pos,
                                           const Vector<float, 2> dir,
                                           float radius);
};

class Collider: public Collidable {
    public:
        /**
         * Handles collision with a physics object. Usually this will mean
         * bouncing, but it could mean destroying it, damaging it, or otherwise.
         * 
         * obj: The object colliding with this collider. This object will
         *      usually be modified.
        */
        virtual void collide(CollidingObject* obj); // modifies obj
        virtual void slide(CollidingObject* obj);
    protected:
        static constexpr float SKIM_EPSILON = 1e-6;
        // the minimum distance an object may intrude into a surface and still
        // be "free to travel"
};

class CollidingObject: public PhysicsObject {
    public:
        float radius;
        Collidable* environment;
        CollidingObject(float x, float y, float m, float r, Elasticity e,
                                    Collidable* env): PhysicsObject(m, x, y, e),
                                                      radius(r),
                                                      environment(env) {}
        void stepVelocity(float dt);
        void stepCollisions(float dt, Collider* stuck);
};

/**
 * Collision Shapes
*/

/**
 * CircleCollider
 * 
 * Circular collider
 * Defined by a center point and radius
*/

class CircleCollider: public Collider {
    public:
        Vector<float, 2> position;
        float radius;
        Elasticity elasticity;
        CircleCollider(Vector<float, 2> pos, float rad,
                                    Elasticity e = PhysicsObject::ELAS_DEFAULT):
            position(pos), radius(rad), elasticity(e) {}
        FreePathResult getFreePath(const Vector<float, 2> pos,
                                const Vector<float, 2> dir,
                                float radius);
        void collide(CollidingObject* const obj);
        void slide(CollidingObject* const obj);
};

/**
 * EdgeCollider
 * 
 * Infinite straight line collider
 * Defined by a center point and normal vector
*/
class EdgeCollider: public Collider {
    public:
        Vector<float, 2> position;
        Vector<float, 2> normal;
        Elasticity elasticity;
        EdgeCollider(Vector<float, 2> pos, Vector<float, 2> norm,
                                    Elasticity e = PhysicsObject::ELAS_DEFAULT):
            position(pos), normal(norm), elasticity(e) {}
        FreePathResult getFreePath(const Vector<float, 2> pos,
                                const Vector<float, 2> dir,
                                float radius);
        void collide(CollidingObject* const obj);
        void slide(CollidingObject* const obj);
};

/**
 * SegmentCollider
 * 
 * Limited-length straight line collider
 * Defined by an endpoint and an offset
 * NOTE: Does not include endpoints; CircleColliders with radius 0 should be
 * placed there
*/
class SegmentCollider: public Collider {
    public:
        Vector<float, 2> position;
        Vector<float, 2> offset;
        Elasticity elasticity;
        SegmentCollider(Vector<float, 2> pos, Vector<float, 2> offs,
                                    Elasticity e = PhysicsObject::ELAS_DEFAULT):
            position(pos), offset(offs), elasticity(e) {}
        FreePathResult getFreePath(const Vector<float, 2> pos,
                                const Vector<float, 2> dir,
                                float radius);
        void collide(CollidingObject* const obj);
        void slide(CollidingObject* const obj);
};

/**
 * Collision Aggregates
*/

/**
 * CollisionGroup
 * 
 * Simple dynamic group of colliders
 * Checks collision iteratively, no geometric shortcuts
 * This object does not own its pointers.
*/
class CollisionGroup: public Collidable {
    public:
        FreePathResult getFreePath(const Vector<float, 2> pos,
                                const Vector<float, 2> dir,
                                float radius) {
            FreePathResult result = {FREE, INFINITY, nullptr};
            for(int i = 0; i < colliders.size(); i++) {
                FreePathResult newres =
                                    colliders[i]->getFreePath(pos, dir, radius);
                if(newres.distSq < result.distSq) {
                    result = newres;
                }
            }
            return result;
        }
/**
 * TYPE* add(TYPE obj)
 * 
 * Adds a collidable to the group. Makes a copy of the object to maintain
 * ownership; returns a persistent pointer to the object. The instantiated
 * object must inherit from Collidable.
*/
        template<class TYPE>
        TYPE* add(TYPE obj) {
            static_assert(std::is_base_of<Collidable, TYPE>::value,
                    "Collision group members must inherit from Collidable");
            std::unique_ptr<Collidable> owned = 
                    std::make_unique<TYPE>(std::move(obj));
            TYPE* ptr = (TYPE*) owned.get();
            colliders.push_back(std::move(owned));
            return ptr;
        }
    protected:
        std::vector<std::unique_ptr<Collidable>> colliders;
};

/**
 * CollisionPoly
 * 
 * Polygon collision group. For now it's not particularly easy to modify after
 * creation, but will maybe fix that later?
*/
class CollisionPoly: public CollisionGroup {
    public:
        CollisionPoly(std::vector<Vector<float, 2>> vertices,
                            Elasticity elas = PhysicsObject::ELAS_DEFAULT) {
            for(int i = 0; i < vertices.size(); i++) {
                add(SegmentCollider(vertices[i],
                    vertices[(i+1)%vertices.size()]-vertices[i], elas));
                add(CircleCollider(vertices[i], 0, elas));
            }
        }
};

#endif