#include "Collision.h"

void CollidingObject::step(float dt) {
    FreePathResult f = environment->getFreePath(pos, vel, radius);
    Vector<float, 2> move = vel*dt;
    if(move.magSq() >= f.distSq) {
        pos += move;
    } else {
        f.target->collide(this);
    }
}