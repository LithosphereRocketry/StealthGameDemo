#include "Collision.h"
#include <iostream>

void CollidingObject::stepVelocity(float dt) {
    Vector<float, 2> move = (vel + pendingVel)*dt;
    FreePathResult f = environment->getFreePath(pos, move, radius);
    if(move.magSq() <= f.distSq) {
        vel += pendingVel;
        pos += move;
    } else {
        printf("Got to %f of %f\n", sqrtf(f.distSq), move.mag());
        // Roll forward to collision
        pos += move.toMagSq(f.distSq);
        f.target->collide(this);
        float mvfrac = sqrtf(f.distSq/move.magSq());
        // Partition up the change in velocity this frame to before & after the collision
        vel += pendingVel*mvfrac;
        pendingVel *= 1 - mvfrac;
        // Continue with the rest of the motion
        stepVelocity(dt * (1.0f - mvfrac));
    }
}
 // this feels like it should be able to be const but it sorta isn't because it gives other things permission to modify the object
FreePathResult EdgeCollider::getFreePath(const Vector<float, 2> objpos, const Vector<float, 2> dir, float radius) {
    Vector<float, 2> ctroffs = (objpos - position).proj(normal);
    if(dir.dot(ctroffs) > 0) {
        return {INFINITY, this};
    }
    Vector<float, 2> offs = ctroffs - ctroffs.toMag(radius); // offset the line 1 radius closer
    if(dir.dot(offs) > 0) {
        return {0, this};
    }
    if(offs.magSq() == 0 || dir.magSq() == 0) {
        return {0, this}; // this should almost never happen
    } else {
        return {dir.magSq()*powf(offs.magSq()/offs.dot(dir), 2), this};
    }
}

void EdgeCollider::collide(CollidingObject* const obj) {
    obj->applyBounce(normal, elasticity);
}

FreePathResult SegmentCollider::getFreePath(const Vector<float, 2> pos, const Vector<float, 2> dir, float radius) {
    Vector<float, 2> ctrdiff = pos - position;
    Vector<float, 2> ctroffs = ctrdiff.rej(offset);
    if(dir.dot(ctroffs) > 0) {
        return {INFINITY, this};
    }
    Vector<float, 2> radoffs = ctroffs.toMag(radius);
    Vector<float, 2> offs = ctroffs - radoffs;
    Vector<float, 2> diff = ctrdiff - radoffs; // offset the line 1 radius closer
    float distsq = dir.magSq()*powf(offs.magSq()/offs.dot(dir), 2);
    Vector<float, 2> along = diff + dir.toMagSq(distsq);
    float alongdist = along.dot(offset.normal());
    if(offs.magSq() == 0 || dir.magSq() == 0) {
        return {0, this}; // this should almost never happen
    } else if(alongdist < 0 || alongdist > radius) {
        return {INFINITY, this};
    } else {
        return {distsq, this};
    }
}

void SegmentCollider::collide(CollidingObject* const obj) {
    obj->applyBounce(offset.orthogonal(), elasticity);
}