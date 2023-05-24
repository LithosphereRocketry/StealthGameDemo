#ifndef PHYSICS_H
#define PHYSICS_H

#include <SDL2/SDL.h>
#include "VectorMath.h"

struct Elasticity {
    float normal;
    float parallel;
};

// units roughly m/kg/s
class PhysicsObject {
    static float zeroThresh = 0.1;
    public:
        static constexpr Elasticity ELAS_DEFAULT = {1, 1};
        float mass;
        Vector<float, 2> pos;
        Vector<float, 2> vel;
        Elasticity elas;
        PhysicsObject(float m, float x = 0, float y = 0, Elasticity elasticity = ELAS_DEFAULT):
            mass(m),
            pos({x, y}),
            vel({0, 0}),
            elas(elasticity) {}
        inline void step(float dt) {
            stepForces(dt);
            stepVelocity(dt);
        }
        inline void applyDV(Vector<float, 2> dv) { vel += dv; }
        inline void applyImpulse(Vector<float, 2> dp) { applyDV(dp/mass); }
        inline void applyBounce(Vector<float, 2> dir, Elasticity surfelas) {
            vel = -vel.proj(dir)*elas.normal*surfelas.normal + vel.rej(dir)*elas.parallel*surfelas.parallel;
        }
        inline void applyForce(Vector<float, 2> f) {
            pendingAccel += f/mass;
        }
    protected:
        inline void stepForces(float dt) {
            vel += pendingAccel * dt;
        }
        virtual void stepVelocity(float dt) {
            if(vel.magSq() < zeroThresh*zeroThresh) {
                vel = {0, 0};
            }
            pos += vel * dt; 
        }
    private:
        Vector<float, 2> pendingAccel;
};

#endif