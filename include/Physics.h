#ifndef PHYSICS_H
#define PHYSICS_H

#include "VectorMath.h"

// units roughly m/kg/s
class PhysicsObject {
    public:
        float mass;
        Vector<float, 2> pos;
        Vector<float, 2> vel;
        PhysicsObject(): pos({0, 0}), vel({0, 0}) {}
        PhysicsObject(float x, float y): pos({x, y}), vel({0, 0}) {}
        inline void step(float dt) { pos += vel*dt; }
        inline void applyDV(Vector<float, 2> dv) { vel += dv; }
        inline void applyImpulse(Vector<float, 2> dp) { applyDV(dp/mass); }
        inline void applyBounce(Vector<float, 2> dir) {
            vel -= vel.proj(dir)*2; // for now, things lose energy in both axes equally
            vel *= vel.mag(); // this is a quasi unnecessary sqrt but I sorta don't care
        }
};

#endif