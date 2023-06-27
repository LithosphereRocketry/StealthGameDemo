#include "Collision.h"
#include <iostream>

void CollidingObject::stepVelocity(float dt) {
    stepCollisions(dt, NULL);
}

void CollidingObject::stepCollisions(float dt, Collider* stuck) {
    Vector<float, 2> move = (vel + pendingVel)*dt;
    FreePathResult f = environment->getFreePath(pos, move, radius);
    if(move.magSq() <= f.distSq) {
        vel += pendingVel;
        pos += move;
    } else {
        float mvfrac = sqrtf(f.distSq/move.magSq());
        // roll forward to collision
        pos += move.toMagSq(f.distSq);
        vel += pendingVel*mvfrac;
        // Partition up the change in velocity this frame to before & after the
        // collision
        pendingVel *= 1.0f - mvfrac;
        if(f.type == STUCK) {
            if(!stuck) {
                stuck = f.target;
            } else if(stuck != f.target) {
                // if we're stuck in two things at once, give up
                pendingVel = {0, 0};
                return;
            }
            f.target->slide(this, dt*mvfrac);
        } else {
            f.target->collide(this);
        }
        // Continue with the rest of the motion
        stepCollisions(dt * (1.0f - mvfrac), stuck);
    }
}


FreePathResult CircleCollider::getFreePath(const Vector<float, 2> objpos,
                                           const Vector<float, 2> dir,
                                           float rad) {
    // see if we hit
    float colrad = radius + rad;
    Vector<float, 2> offs = position - objpos;
    Vector<float, 2> tangent = offs.rej(dir);
    if(tangent.magSq() >= powf(colrad - SKIM_EPSILON, 2)
        || dir.dot(offs) <= SKIM_EPSILON) {
        return {FREE, INFINITY, this};
    }
    // See if the object is inside us
    if(offs.magSq() <= powf(colrad, 2)) {
        return {STUCK, 0, this};
    }
    // if not, use Pythagorean Theorem to figure out how soon we hit
    Vector<float, 2> toHitPoint = tangent + dir.toMagSq(powf(colrad, 2)
                                                         - tangent.magSq());
    Vector<float, 2> travel = offs - toHitPoint;
    return {COLLIDING, travel.magSq(), this};
}

void CircleCollider::collide(CollidingObject* const obj) {
    obj->applyBounce(obj->pos-position, elasticity);
}
void CircleCollider::slide(CollidingObject* const obj, float dt) {
    Vector<float, 2> normal = obj->pos-position;
    obj->vel = obj->vel.rej(normal);
    obj->pendingVel = obj->pendingVel.rej(normal);
}

 // this feels like it should be able to be const but it sorta isn't because it 
 // gives other things permission to modify the object
FreePathResult EdgeCollider::getFreePath(const Vector<float, 2> objpos,
                                    const Vector<float, 2> dir, float radius) {
    Vector<float, 2> ctroffs = (objpos - position).proj(normal);
    if(dir.dot(ctroffs) >= -SKIM_EPSILON) {
        return {FREE, INFINITY, this};
    }
    Vector<float, 2> offs = ctroffs - ctroffs.toMag(radius);
    // offset the line 1 radius closer
    // if the line is now past the point, we're stuck and should slide
    if(dir.dot(offs) >= 0) {
        return {STUCK, 0, this};
    }
    return {COLLIDING, dir.magSq()*powf(offs.magSq()/offs.dot(dir), 2), this};
}

void EdgeCollider::collide(CollidingObject* const obj) {
    obj->applyBounce(normal, elasticity);
}

void EdgeCollider::slide(CollidingObject* const obj, float dt) {
    obj->vel = obj->vel.rej(normal);
    obj->pendingVel = obj->pendingVel.rej(normal);
}

FreePathResult SegmentCollider::getFreePath(const Vector<float, 2> pos,
                                    const Vector<float, 2> dir, float radius) {
    Vector<float, 2> ctrdiff = pos - position;
    Vector<float, 2> ctroffs = ctrdiff.rej(offset);
    if(dir.dot(ctroffs) >= -SKIM_EPSILON) {
        return {FREE, INFINITY, this};
    }
    Vector<float, 2> radoffs = ctroffs.toMag(radius);
    Vector<float, 2> offs = ctroffs - radoffs;
    Vector<float, 2> diff = ctrdiff - radoffs; // offset 1 radius closer
    if(offs.magSq() == 0 || dir.magSq() == 0) {
        return {FALLBACK, 0, this}; // this should almost never happen
    }
    float distsq = dir.magSq()*powf(offs.magSq()/offs.dot(dir), 2);
    Vector<float, 2> along = diff + dir.toMagSq(distsq);
    float alongdist = along.dot(offset.normal());
    if(alongdist < 0 || alongdist > offset.mag()) {
        return {FREE, INFINITY, this};
    } else {
        if(dir.dot(offs) >= 0) {
            return {STUCK, 0, this};
        } else {
            return {COLLIDING, distsq, this};
        }
    }
}

void SegmentCollider::collide(CollidingObject* const obj) {
    obj->applyBounce(offset.orthogonal(), elasticity);
}
void SegmentCollider::slide(CollidingObject* const obj, float dt) {
    obj->vel = obj->vel.proj(offset);
    obj->pendingVel = obj->pendingVel.proj(offset);
}