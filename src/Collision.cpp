#include "Collision.h"
#include <iostream>

void CollidingObject::stepVelocity(float dt) {
    stepCollisions(dt, NULL);
}

void CollidingObject::stepCollisions(float dt, Collider* stuck) {
    Vector<float, 2> move = (vel + pendingVel)*dt;
    FreePathResult f = environment->getFreePath(pos, move, radius);
    if(move.magSq() <= f.distSq) {
        vel += pendingVel;
        pos += move;
    } else {
        float mvfrac = sqrtf(f.distSq/move.magSq());
        // roll forward to collision
        pos += move.toMagSq(f.distSq);
        vel += pendingVel*mvfrac;
        // Partition up the change in velocity this frame to before & after the collision
        pendingVel *= 1.0f - mvfrac;
        if(f.type == STUCK) {
            if(stuck == NULL) {
                stuck = f.target;
            } else if(stuck != f.target) {
                // if we're stuck in two things at once, give up
                pendingVel = {0, 0};
                return;
            }
            f.target->slide(this);
        } else {
            f.target->collide(this);
        }
        // Continue with the rest of the motion
        stepCollisions(dt * (1.0f - mvfrac), stuck);
    }
}
 // this feels like it should be able to be const but it sorta isn't because it gives other things permission to modify the object
FreePathResult EdgeCollider::getFreePath(const Vector<float, 2> objpos, const Vector<float, 2> dir, float radius) {
    Vector<float, 2> ctroffs = (objpos - position).proj(normal);
    if(dir.dot(ctroffs) >= -SKIM_EPSILON) {
        return {FREE, INFINITY, this};
    }
    Vector<float, 2> offs = ctroffs - ctroffs.toMag(radius); // offset the line 1 radius closer
    // if the line is now past the point, we're stuck and should slide
    if(dir.dot(offs) >= 0) {
        return {STUCK, 0, this};
    }
    return {COLLIDING, dir.magSq()*powf(offs.magSq()/offs.dot(dir), 2), this};
}

void EdgeCollider::collide(CollidingObject* const obj) {
    obj->applyBounce(normal, elasticity);
}

void EdgeCollider::slide(CollidingObject* const obj) {
    obj->vel = obj->vel.rej(normal)*elasticity.parallel*obj->elas.parallel;
    obj->pendingVel = obj->pendingVel.rej(normal);
}

FreePathResult SegmentCollider::getFreePath(const Vector<float, 2> pos, const Vector<float, 2> dir, float radius) {
    Vector<float, 2> ctrdiff = pos - position;
    Vector<float, 2> ctroffs = ctrdiff.rej(offset);
    if(dir.dot(ctroffs) > 0) {
        return {FREE, INFINITY, this};
    }
    Vector<float, 2> radoffs = ctroffs.toMag(radius);
    Vector<float, 2> offs = ctroffs - radoffs;
    Vector<float, 2> diff = ctrdiff - radoffs; // offset the line 1 radius closer
    float distsq = dir.magSq()*powf(offs.magSq()/offs.dot(dir), 2);
    Vector<float, 2> along = diff + dir.toMagSq(distsq);
    float alongdist = along.dot(offset.normal());
    if(offs.magSq() == 0 || dir.magSq() == 0) {
        return {FALLBACK, 0, this}; // this should almost never happen
    } else if(alongdist < 0 || alongdist > radius) {
        return {FREE, INFINITY, this};
    } else {
        return {COLLIDING, distsq, this};
    }
}

void SegmentCollider::collide(CollidingObject* const obj) {
    obj->applyBounce(offset.orthogonal(), elasticity);
}
void SegmentCollider::slide(CollidingObject* const obj) {
    collide(obj);
}