#include "Collision.h"
#include <iostream>

void CollidingObject::step(float dt) {
    FreePathResult f = environment->getFreePath(pos, vel, radius);
    Vector<float, 2> move = vel*dt;
    if(move.magSq() >= f.distSq) {
        pos += move;
    } else {
        f.target->collide(this);
    }
}
 // this feels like it should be able to be const but it sorta isn't because it gives other things permission to modify the object
FreePathResult EdgeCollider::getFreePath(const Vector<float, 2> pos, const Vector<float, 2> dir, float radius) {
    Vector<float, 2> ctroffs = (pos-position).proj(normal);
    Vector<float, 2> offs = ctroffs - ctroffs.toMag(radius); // offset the line 1 radius closer
    if(offs.magSq() == 0) {
        return {0, this}; // this should almost never happen
    } else {
        return {dir.magSq()*powf(offs.magSq()/offs.dot(dir), 2), this};
    }
}

void EdgeCollider::collide(CollidingObject* const obj) {
    
}