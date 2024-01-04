#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

/**
 * General 2D math header-only library
 * 
 * Originally written for just vectors, but ended up encompassing spheres and
 * rays as well, plus some interactions between them
*/

#include <initializer_list>
#include <iostream>
#include <typeinfo>
#include <cmath>

// TYPE here should really be something like float or double
template <class TYPE, size_t SIZE>
class Vector {
    static_assert(std::is_arithmetic<TYPE>::value,
        "Geometric vector must have arithmetic type");
    public:
        TYPE data[SIZE];
        Vector(std::initializer_list<TYPE> lst): Vector() {
//          For some reason this assert thinks it's non-static even if lst is 
//          declared constexpr,  why :(
//          static_assert(lst.size() == SIZE,
//      "Geometric vector initializer must have the correct number of elements")
//          HOWEVER, this call seems to be inlined in such a way that strict
//          mode at least issues a warning
            size_t pos = 0;
            for(TYPE element : lst) {
                data[pos++] = element; // I kinda hate this
            }
        }
        Vector() {};
        inline TYPE operator [] (int index) const {
            return data[index];
        }
        inline TYPE& operator [] (int index) {
            return data[index];
        }
        void operator *= (const TYPE& scalar) {
            for(size_t i = 0; i < SIZE; i++) {
                data[i] *= scalar;
            }
        }
        inline Vector<TYPE, SIZE> operator * (const TYPE& scalar) const {
            Vector<TYPE, SIZE> v(*this);
            v *= scalar;
            return v;
        }
        void operator /= (const TYPE& scalar) {
            for(size_t i = 0; i < SIZE; i++) {
                data[i] /= scalar;
            }
        }
        inline Vector<TYPE, SIZE> operator / (const TYPE& scalar) const {
            Vector<TYPE, SIZE> v(*this);
            v /= scalar;
            return v;
        }
        void operator += (const Vector<TYPE, SIZE>& other) {
            for(size_t i = 0; i < SIZE; i++) {
                data[i] += other[i];
            }
        }
        inline Vector<TYPE, SIZE> operator + (const Vector<TYPE, SIZE>& other)
                    const {
            Vector<TYPE, SIZE> v(*this);
            v += other;
            return v;
        }
        void operator -= (const Vector<TYPE, SIZE>& other) {
            for(size_t i =  0; i < SIZE; i++) {
                data[i] -= other[i];
            }
        }
        inline Vector<TYPE, SIZE> operator - (const Vector<TYPE, SIZE>& other)
                    const {
            Vector<TYPE, SIZE> v(*this);
            v -= other;
            return v;
        }
        inline Vector<TYPE, SIZE> operator - () const {
            Vector<TYPE, SIZE> v(*this);
            v *= -1;
            return v;
        }
        TYPE dot(const Vector<TYPE, SIZE>& other) const {
            TYPE total = 0;
            for(size_t i = 0; i < SIZE; i++) {
                total += data[i]*other[i];
            }
            return total;
        }
        Vector<TYPE, SIZE> cross(const Vector<TYPE, SIZE>& other) const {
            static_assert(SIZE==3,
                    "Cross-product is only valid for 3D vectors");
            Vector<TYPE, SIZE> v;
            v[0] = data[1]*other[2] - data[2]*other[1];
            v[1] = data[2]*other[0] - data[0]*other[2];
            v[2] = data[0]*other[1] - data[1]*other[0];
            return v;
        }
        TYPE magSq() const { return dot(*this); }
        TYPE mag() const { return std::sqrt(magSq()); }

        // project self onto other
        TYPE scalar_proj(const Vector<TYPE, SIZE>& other) const {
            return dot(other) / other.mag();
        }
        Vector<TYPE, SIZE> proj(const Vector<TYPE, SIZE>& other) const {
            return other * dot(other) / other.magSq();
        }
        // reject self onto other
        TYPE scalar_rej(const Vector<TYPE, SIZE>& other) const {
            // There is probably a cycle-saving way to do this with clever
            // vector math but it's late and I'm tired
            return rej(other).mag();
        }
        Vector<TYPE, SIZE> rej(const Vector<TYPE, SIZE>& other) const {
            return operator-(proj(other));
        }
        inline Vector<TYPE, SIZE> normal() const {
            return operator/(mag());
        }
        // counterclockwise orthogonal
        // TODO: this is ugly
        inline Vector<TYPE, SIZE> orthogonal() const {
            static_assert(SIZE==2, "Orthogonal is only valid for 2D vectors");
            return Vector<TYPE, SIZE>({-data[1], data[0]});
        }
        inline Vector<TYPE, SIZE> toMag(TYPE length) const {
            float m = mag();
            if(m <= 0) {
                return *this;
            } else {
                return operator*(length/m);
            }
        }
        inline Vector<TYPE, SIZE> toMagSq(TYPE lengthSq) const {
            float m = magSq();
            if(m <= 0) {
                return *this;
            } else {
                return operator*(std::sqrt(lengthSq/m));
            }
        }
};

// make vectors printable
template <class TYPE, size_t SIZE>
inline std::ostream& operator << (std::ostream& os, const Vector<TYPE, SIZE>& v) {
    os << "<";
    for(size_t i = 0; i < SIZE; i++) {
        if(i == 0) {}
        os << v[i];
        if(i < SIZE-1) {
            os << ", ";
        }
    }
    os << ">";
    return os;
}

// Spheres (very barebones for now, mostly exist to support other classes)
template <class TYPE, size_t SIZE>
class NSphere {
    public:
        const Vector<TYPE, SIZE> origin;
        float radius;
        NSphere(Vector<TYPE, SIZE> ori, TYPE r): origin(ori), radius(r) {}

        NSphere<TYPE, SIZE> addRadius(TYPE dr) {
            return NSphere(origin, radius + dr);
        }
        NSphere<TYPE, SIZE> offset(Vector<TYPE, SIZE> offs) {
            return NSphere(origin + offs, radius);
        }
        
};

// Rays
template <class TYPE, size_t SIZE>
class Ray {
    public:
        const Vector<TYPE, SIZE> origin, dir;
        Ray(Vector<TYPE, SIZE> ori, Vector<TYPE, SIZE> d): origin(ori),
                dir(d.toMag(1)) {}

        Ray<TYPE, SIZE> offset(Vector<TYPE, SIZE> offs) {
            return Ray(origin + offs, dir);
        }
        Ray<TYPE, SIZE> offset_along(TYPE dist) {
            return offset(dir * dist);
        }

        TYPE travelTo(const Ray<TYPE, SIZE>& surfNorm) const {
            Vector<TYPE, SIZE> offset = surfNorm.origin - origin;
            TYPE orthdist = offset.scalar_proj(surfNorm.dir);
            // Note this assumes ray directions are always normalized
            return orthdist / dir.dot(surfNorm.dir);
        }
        // Always returns the closer point of intersection
        // TODO: what if you're inside it - do you want the backwards one or the
        // forwards one? sorta unclear
        TYPE travelTo(const NSphere<TYPE, SIZE>& sphere) const {
            // thanks wikipedia for this algorithm
            Vector<TYPE, SIZE> offset = origin - sphere.origin;
            TYPE realpart = -(dir.dot(offset));
            TYPE discriminant = std::pow(realpart, 2)
                    - (offset.magSq() - std::pow(sphere.radius, 2));
            if(discriminant < 0) return INFINITY;
            else return realpart - std::sqrt(discriminant);
        }
};

// Give some nice-to-have shorthands
template <class TYPE>
using Vec2 = Vector<TYPE, 2>;

template <class TYPE>
using Vec3 = Vector<TYPE, 3>;

using Vec2d = Vec2<double>;
using Vec2f = Vec2<float>;

using Vec3d = Vec3<double>;
using Vec3f = Vec3<float>;


template <class TYPE>
using Circle = NSphere<TYPE, 2>;

template <class TYPE>
using Sphere = NSphere<TYPE, 3>;

using Circle_d = Circle<double>;
using Circle_f = Circle<float>;

using Sphere_d = Sphere<double>;
using Sphere_f = Sphere<float>;

template <class TYPE>
using Ray2 = Ray<TYPE, 2>;

template <class TYPE>
using Ray3 = Ray<TYPE, 3>;

using Ray2d = Ray2<double>;
using Ray2f = Ray2<float>;

using Ray3d = Ray3<double>;
using Ray3f = Ray3<float>;

#endif