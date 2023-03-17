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
FreePathResult EdgeCollider::getFreePath(const Vector<float, 2> pos, const Vector<float, 2> dir, float radius) {
    if(dir.dot(normal) > 0) {
        return {INFINITY, this};
    }
    Vector<float, 2> ctroffs = (pos-position).proj(normal);
    Vector<float, 2> offs = ctroffs - ctroffs.toMag(radius); // offset the line 1 radius closer
    if(offs.magSq() == 0 || dir.magSq() == 0) {
        return {0, this}; // this should almost never happen
    } else {
        return {dir.magSq()*powf(offs.magSq()/offs.dot(dir), 2), this};
    }
}

void EdgeCollider::collide(CollidingObject* const obj) {
    // really I should do this in a more pixel-perfect way
    // e.g. move to extent of free path, recalculate collision with remaining step
    // but this is good enough for now
    
    obj->applyBounce(normal.toMag(elasticity*obj->elasticity));
}