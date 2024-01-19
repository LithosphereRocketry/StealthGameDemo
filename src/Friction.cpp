#include "Friction.h"

FrictionObject::FrictionObject(float m, float x, float y,
                Elasticity elasticity,
                float sdrag, float ddrag):
        PhysicsObject(m, x, y, elasticity),
        static_drag(sdrag), dynamic_drag(ddrag) {}

void FrictionObject::stepForces(float dt) {
    applyForce(-vel.toMag(static_drag)
              - vel * dynamic_drag);
    // it feels like this *dt calculation happens kinda redundantly
    // also this code is kinda bad but it's better
    if((vel + pendingAccel*dt).dot(vel) < 0) {
        vel = {0, 0};
        pendingAccel = {0, 0};
    }
}