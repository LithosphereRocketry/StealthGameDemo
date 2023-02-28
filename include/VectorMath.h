#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H
#include <initializer_list>
#include <iostream>
#include <math.h>

// TYPE here should really be something like float or double
template <class TYPE, size_t SIZE>
class Vector {
    public:
        TYPE data[SIZE];
        Vector(std::initializer_list<TYPE> lst) {
            size_t pos = 0;
            for(TYPE element : lst) {
                data[pos++] = element; // I kinda hate this
            }
        }
        inline TYPE operator [] (int index) const {
            return data[index];
        }
        inline TYPE& operator [] (int index) {
            return data[index];
        }
        void operator *= (const TYPE& scalar) {
            for(int i = 0; i < SIZE; i++) {
                data[i] *= scalar;
            }
        }
        inline Vector<TYPE, SIZE> operator * (const TYPE& scalar) const {
            Vector<TYPE, SIZE> v(*this);
            v *= scalar;
            return v;
        }
        void operator /= (const TYPE& scalar) {
            for(int i = 0; i < SIZE; i++) {
                data[i] /= scalar;
            }
        }
        inline Vector<TYPE, SIZE> operator / (const TYPE& scalar) const {
            Vector<TYPE, SIZE> v(*this);
            v /= scalar;
            return v;
        }
        void operator += (const Vector<TYPE, SIZE>& other) {
            for(int i = 0; i < SIZE; i++) {
                data[i] += other[i];
            }
        }
        inline Vector<TYPE, SIZE> operator + (const Vector<TYPE, SIZE>& other) const {
            Vector<TYPE, SIZE> v(*this);
            v += other;
            return v;
        }
        void operator -= (const Vector<TYPE, SIZE>& other) {
            for(int i = 0; i < SIZE; i++) {
                data[i] -= other[i];
            }
        }
        inline Vector<TYPE, SIZE> operator - (const Vector<TYPE, SIZE>& other) const {
            Vector<TYPE, SIZE> v(*this);
            v -= other;
            return v;
        }
        TYPE dot(const Vector<TYPE, SIZE>& other) const {
            TYPE total = 0;
            for(int i = 0; i < SIZE; i++) {
                total += data[i]*other[i];
            }
            return total;
        }
        Vector<TYPE, SIZE> cross(const Vector<TYPE, SIZE>& other) const {
            static_assert(SIZE==3, "Cross-product is only valid for 3D vectors");
            Vector<TYPE, SIZE> v;
            v[0] = data[1]*other[2] - data[2]*other[1];
            v[1] = data[2]*other[0] - data[0]*other[2];
            v[2] = data[0]*other[1] - data[1]*other[0];
            return v;
        }
        TYPE magSq() const { return dot(*this); }
        TYPE mag() const { return sqrt(magSq()); }

        Vector<TYPE, SIZE> proj(Vector<TYPE, SIZE> other) { // project self onto other
            return other * dot(other) / other.magSq();
        }
        Vector<TYPE, SIZE> rej(Vector<TYPE, SIZE> other) { // reject self onto other
            return operator-(proj(other));
        }
        inline Vector<TYPE, SIZE> normal() {
            return operator/(mag());
        }
        inline Vector<TYPE, SIZE> toMag(TYPE length) {
            return operator*(length/mag()); // I don't think there's any way to avoid the sqrt here
        }
};

// make vectors printable
template <class TYPE, size_t SIZE>
inline std::ostream& operator << (std::ostream& os, Vector<TYPE, SIZE> v) {
    os << "<";
    for(int i = 0; i < SIZE; i++) {
        if(i == 0) {}
        os << v[i];
        if(i < SIZE-1) {
            os << ", ";
        }
    }
    os << ">";
    return os;
}

#endif