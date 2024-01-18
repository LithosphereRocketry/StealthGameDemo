#include "Collision.h"
#include <iostream>

constexpr size_t CollidingObject::MAX_ITER;

void CollidingObject::stepVelocity(float dt) {
    stepCollisions(dt, nullptr);
}

void CollidingObject::stepCollisions(float dt, Collider* stuck, size_t max_iter) {
    if(!environment) {
        PhysicsObject::stepVelocity(dt);
    } else {
        Vec2f move = (vel + pendingVel)*dt;
        FreePathResult f = environment->getFreePath(Ray2f(pos, move), radius, move.mag());
        if(move.mag() <= f.safeDist) {
            vel += pendingVel;
            pos += move;
        } else if(max_iter == 0) {
            if(pendingVel.magSq() > std::pow(Collidable::SKIM_EPSILON, 2)) {
                pendingVel = {0, 0};    
                stepCollisions(dt, stuck, 0);
            } else {
                std::cout << "Warning: iteration timeout\n";
            }
        } else {
            float mvfrac = f.safeDist/move.mag();
            // roll forward to collision
            pos += move.toMag(f.safeDist);
            vel += pendingVel*mvfrac;
            // Partition up the change in velocity this frame to before & after
            // the collision
            pendingVel *= 1.0f - mvfrac;
            if(f.type == STUCK) {
                if(!stuck) {
                    stuck = f.target;
                } else if(stuck != f.target) {
                    // if we're stuck in two things at once, give up
                    pendingVel = {0, 0};
                    return;
                }
                f.target->slide(this, dt*mvfrac);
            } else {
                f.target->collide(this);
            }
            // Continue with the rest of the motion
            stepCollisions(dt * (1.0f - mvfrac), stuck, max_iter-1);
        }
    }
}


FreePathResult CircleCollider::getFreePath(const Ray2f& path, float radius, float) {
    const Circle_f regGeom = addRadius(radius);
    const Circle_f safeGeom = addRadius(radius + SKIM_EPSILON);
    float dist = path.travelTo(regGeom);
    if(dist < 0 || dist == INFINITY) {
        return {FREE, INFINITY, INFINITY, this};
    }
    float safeDist = path.travelTo(safeGeom);
    if(safeDist <= 0) {
        return {STUCK, 0, 0, this};
    } else {
        return {COLLIDING, dist, safeDist, this};
    }
}

void CircleCollider::collide(CollidingObject* const obj) {
    obj->applyBounce(obj->pos - origin, elasticity);
}
void CircleCollider::slide(CollidingObject* const obj, float dt) {
    Vec2f normal = obj->pos - origin;
    obj->vel = obj->vel.rej(normal);
    obj->pendingVel = obj->pendingVel.rej(normal);
}

 // this feels like it should be able to be const but it sorta isn't because it 
 // gives other things permission to modify the object
FreePathResult EdgeCollider::getFreePath(const Ray2f& path, float radius, float) {
    Ray2f regGeom = offset_along(radius);
    Ray2f safeGeom = offset_along(radius + SKIM_EPSILON);
    float dist = path.travelTo(regGeom);
    if(dist <= 0) {
        return {FREE, INFINITY, INFINITY, this};
    }
    float safeDist = path.travelTo(safeGeom);
    if(safeDist <= 0) {
        return {STUCK, 0, safeDist, this};
    } else {
        return {COLLIDING, dist, safeDist, this};
    }
}

void EdgeCollider::collide(CollidingObject* const obj) {
    obj->applyBounce(dir, elasticity);
}

void EdgeCollider::slide(CollidingObject* const obj, float dt) {
    obj->vel = obj->vel.rej(dir);
    obj->pendingVel = obj->pendingVel.rej(dir);
}

FreePathResult SegmentCollider::getFreePath(const Ray2f& path, float radius, float) {
    Ray2f regGeom = offset_along(radius);
    Ray2f safeGeom = offset_along(radius + SKIM_EPSILON);
    float dist = path.travelTo(regGeom);
    // something has gone horribly wrong here
    float along = ((path.origin + path.dir*dist) - origin).dot(dir.orthogonal());
    // std::cout << along << "\n";
    if(dist <= 0 || along < 0 || along > length) {
        return {FREE, INFINITY, INFINITY, this};
    }
    float safeDist = path.travelTo(safeGeom);
    if(safeDist <= 0) {
        return {STUCK, 0, safeDist, this};
    } else {
        return {COLLIDING, dist, safeDist, this};
    }
}

void SegmentCollider::collide(CollidingObject* const obj) {
    obj->applyBounce(dir, elasticity);
}
void SegmentCollider::slide(CollidingObject* const obj, float dt) {
    obj->vel = obj->vel.rej(dir);
    obj->pendingVel = obj->pendingVel.rej(dir);
}