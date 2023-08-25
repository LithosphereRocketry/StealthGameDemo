#ifndef OBJECT_H
#define OBJECT_H

#include "Collision.h"
#include "Friction.h"

class GameObject: public CollidingObject, public FrictionObject {
    public:
        GameObject(float m, float r, float x = 0, float y = 0,
                   Elasticity elasticity = ELAS_DEFAULT, float sdrag = 0,
                   float ddrag = 0, Collidable* env = nullptr);
};
#endif