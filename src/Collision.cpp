#include "Collision.h"
#include <iostream>

void CollidingObject::stepVelocity(float dt) {
    FreePathResult f = environment->getFreePath(pos, vel, radius);
    Vector<float, 2> move = vel*dt;
    if(move.magSq() <= f.distSq) {
        pos += move;
    } else {
        pos += move.toMagSq(f.distSq);
        f.target->collide(this);
        float newdt = dt*(1 - sqrtf(f.distSq/move.magSq()));
        stepVelocity(newdt);
    }
}
 // this feels like it should be able to be const but it sorta isn't because it gives other things permission to modify the object
FreePathResult EdgeCollider::getFreePath(const Vector<float, 2> objpos, const Vector<float, 2> dir, float radius) {
    Vector<float, 2> ctroffs = (objpos - position).proj(normal);
    if(dir.dot(ctroffs) > 0) {
        return {INFINITY, this};
    }
    Vector<float, 2> offs = ctroffs - ctroffs.toMag(radius); // offset the line 1 radius closer
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