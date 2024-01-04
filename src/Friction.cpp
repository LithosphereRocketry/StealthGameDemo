#include "Friction.h"

FrictionObject::FrictionObject(float m, float x, float y,
                Elasticity elasticity,
                float sdrag, float ddrag):
        PhysicsObject(m, x, y, elasticity),
        static_drag(sdrag), dynamic_drag(ddrag) {}

void FrictionObject::stepForces(float dt) {
    Vec2f ppvel = pendingVel;
    applyForce(-vel.toMag(static_drag)
              - vel * dynamic_drag);
    if(pendingVel.dot(ppvel) < 0) {
        pendingVel = {0, 0};
    }
    PhysicsObject::stepForces(dt);
}