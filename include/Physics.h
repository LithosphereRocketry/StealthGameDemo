#ifndef PHYSICS_H
#define PHYSICS_H

#include <stack>

#include <SDL2/SDL.h>
#include "VectorMath.h"

// units roughly m/kg/s
class PhysicsObject {
    public:
        float mass;
        Vector<float, 2> pos;
        Vector<float, 2> vel;
        PhysicsObject(float m): pos({0, 0}), vel({0, 0}), mass(m) {}
        PhysicsObject(float x, float y, float m): pos({x, y}), vel({0, 0}), mass(m) {}
        inline void step(float dt) {
            stepForces(dt);
            stepVelocity(dt);
        }
        inline void applyDV(Vector<float, 2> dv) { vel += dv; }
        inline void applyImpulse(Vector<float, 2> dp) { applyDV(dp/mass); }
        inline void applyBounce(Vector<float, 2> dir) {
            applyDV(-vel.proj(dir)*2); // for now, things lose energy in both axes equally
            vel *= dir.mag(); // this is a quasi unnecessary sqrt but I sorta don't care
        }
        inline void applyForce(Vector<float, 2> f) {
            pendingAccel.push(f/mass);
        }
    protected:
        inline void stepForces(float dt) {
            while(pendingAccel.size() > 0) {
                vel += pendingAccel.top() * dt;
                pendingAccel.pop();
            }
        }
        virtual void stepVelocity(float dt) {
            pos += vel*dt; 
        }
    private:
        std::stack<Vector<float, 2>> pendingAccel;
};

#endif