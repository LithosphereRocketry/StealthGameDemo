#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H
#include <initializer_list>

// TYPE here should really be something like float or double
template <class TYPE, int SIZE>
class Vector {
    public:
        TYPE data[SIZE];
        Vector() {}
        Vector(TYPE dataIn[SIZE]) {
            for(int i = 0; i < SIZE; i++) {
                data[i] = dataIn[i];
            }
        }
        inline TYPE operator [] (int index) const {
            return data[index];
        }
        inline TYPE& operator [] (int index) {
            return data[index];
        }
        Vector<TYPE, SIZE> operator * (const TYPE& scalar) const {
            Vector<TYPE, SIZE> v;
            for(int i = 0; i < SIZE; i++) {
                v[i] = data[i]*scalar;
            }
            return v;
        }
        Vector<TYPE, SIZE> operator + (const Vector<TYPE, SIZE>& other) const {
            Vector<TYPE, SIZE> v;
            for(int i = 0; i < SIZE; i++) {
                v[i] = data[i] + other[i];
            }
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
        TYPE magSq() { return dot(*this); }
        TYPE mag() { return sqrt(magSq()); }
};

// make vectors printable
template <class TYPE, int SIZE>
inline std::ostream& operator << (std::ostream& os, Vector<TYPE, SIZE> v) {
    for(int i = 0; i < SIZE; i++) {
        if(i == 0) { os << "<"; }
        os << v[i];
        if(i < SIZE-1) {
            os << ", ";
        } else {
            os << ">";
        }
    }
    return os;
}

#endif