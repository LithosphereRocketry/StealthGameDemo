#include "GameObject.h"

GameObject::GameObject(float m, float r, float x, float y,
            Elasticity elasticity, float sdrag,
            float ddrag, Collidable* env):
        PhysicsObject(m, x, y, elasticity),
        CollidingObject(x, y, m, r, elasticity, env),
        FrictionObject(m, x, y, elasticity, sdrag, ddrag) {}