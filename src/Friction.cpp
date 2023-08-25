#include "Friction.h"

FrictionObject::FrictionObject(float m, float x, float y,
                Elasticity elasticity,
                float sdrag, float ddrag):
        PhysicsObject(m, x, y, elasticity),
        static_drag(sdrag), dynamic_drag(ddrag) {}

void FrictionObject::stepForces(float dt) {
    applyForce(-vel.toMag(static_drag)
              - vel * dynamic_drag);
    PhysicsObject::stepForces(dt);
}