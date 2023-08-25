#ifndef FRICTION_H
#define FRICTION_H

#include "Physics.h"

class FrictionObject: public virtual PhysicsObject {
    public:
        FrictionObject(float m, float x = 0, float y = 0,
                        Elasticity elasticity = ELAS_DEFAULT,
                        float sdrag = 0, float ddrag = 0);
    protected:
        void stepForces(float dt);
    private:
        float static_drag;
        float dynamic_drag;
};

#endif