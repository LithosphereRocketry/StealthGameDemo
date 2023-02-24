#ifndef PHYSICS_H
#define PHYSICS_H

#include "VectorMath.h"

// units roughly m/kg/s
class PhysicsObject {
    public:
        float mass;
        Vector<float, 2> pos;
        Vector<float, 2> vel;
        PhysicsObject() {};
        PhysicsObject(float x, float y) {
            pos[0] = x;
            pos[1] = y;
            vel[0] = 0;
            vel[1] = 0;
        }
        inline void step(float dt) { pos += vel*dt; }
        inline void applyDV(Vector<float, 2> dv) { vel += dv; }
        inline void applyImpulse(Vector<float, 2> dp) { applyDV(dp/mass); }
};

#endif