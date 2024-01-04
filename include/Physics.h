#ifndef PHYSICS_H
#define PHYSICS_H

#include <SDL2/SDL.h>
#include "VectorMath.h"

#include <iostream>

struct Elasticity {
    float normal;
    float parallel;
    float slide;
};

// units roughly m/kg/s
class PhysicsObject {
    public:
        virtual ~PhysicsObject() {};
        static constexpr Elasticity ELAS_DEFAULT = {1, 1, 0};
        float mass;
        Vec2f pos;
        Vec2f vel;
        Vec2f pendingVel;
        Elasticity elas;
        PhysicsObject(float m, float x = 0, float y = 0,
                                        Elasticity elasticity = ELAS_DEFAULT):
            mass(m),
            pos({x, y}),
            vel({0, 0}),
            pendingVel({0, 0}),
            elas(elasticity),
            pendingAccel({0, 0}) {}
        inline void step(float dt) {
            stepForces(dt);
            stepVelocity(dt);
        }
        inline void applyDV(Vec2f dv) { vel += dv; }
        inline void applyImpulse(Vec2f dp) { applyDV(dp/mass); }
        inline void applyBounce(Vec2f dir, Elasticity surfelas) {
            vel = -vel.proj(dir)*elas.normal*surfelas.normal
                    + vel.rej(dir)*elas.parallel*surfelas.parallel;
        }
        inline void applyForce(Vec2f f) {
            pendingAccel += f/mass;
        }
        float getTE(float g) {
            return 0.5f*mass*(vel.magSq()) + mass*g*pos[1];
        }
    protected:
        virtual void stepForces(float dt) {
            pendingVel = pendingAccel * dt;
            pendingAccel = {0, 0};
        }
        virtual void stepVelocity(float dt) {
            vel += pendingVel;
            pos += vel * dt; 
        }
    protected:
        Vec2f pendingAccel;
};

#endif