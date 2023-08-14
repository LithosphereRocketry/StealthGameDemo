#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <cmath>
#include "VectorMath.h"

template <class TYPE>
class BoundingBox {
    public:
        Vector<TYPE, 2> c1;
        Vector<TYPE, 2> c2;

        inline Vector<TYPE, 2> bl() { return c1; }
        inline Vector<TYPE, 2> tr() { return c2; }
        inline Vector<TYPE, 2> br() { return {c2[0], c1[1]}; }
        inline Vector<TYPE, 2> tl() { return {c1[0], c2[1]}; }
};

template <class TYPE>
BoundingBox<TYPE> operator | (BoundingBox<TYPE> a, BoundingBox<TYPE> b) {
    return {
        { std::min(a.c1[0], b.c1[0]), std::min(a.c1[1], b.c1[1]) },
        { std::max(a.c2[0], b.c2[0]), std::max(a.c2[1], b.c2[1]) }
    };
}

// RVO should make this not slow (I think; it doesn't make much difference)
template <class TYPE> 
inline BoundingBox<TYPE> operator |= (BoundingBox<TYPE>& a, BoundingBox<TYPE> b) {
    return (a = a|b);
}

template <class TYPE> // TODO: this doesn't work since the & operator can be empty
BoundingBox<TYPE> operator & (BoundingBox<TYPE> a, BoundingBox<TYPE> b) {
    return {
        { std::max(a.c1[0], b.c1[0]), std::max(a.c1[1], b.c1[1]) },
        { std::min(a.c2[0], b.c2[0]), std::min(a.c2[1], b.c2[1]) }
    };
}
template <class TYPE>
inline BoundingBox<TYPE> operator &= (BoundingBox<TYPE>& a, BoundingBox<TYPE> b) {
    return (a = a&b);
}

// It's debatable whether these should really be operator overloads, but I think
// they work well enough
template <class TYPE>
BoundingBox<TYPE> operator + (BoundingBox<TYPE> a, Vector<TYPE, 2> offs) {
    return {
        a.c1 + offs,
        a.c2 + offs
    };
}

template <class TYPE>
BoundingBox<TYPE> operator + (Vector<TYPE, 2> offs, BoundingBox<TYPE> a) {
    return a + offs;
}

template <class TYPE>
BoundingBox<TYPE> operator - (BoundingBox<TYPE> a, Vector<TYPE, 2> offs) {
    return a + (-offs);
}

// make boxes printable
template <class TYPE>
inline std::ostream& operator << (std::ostream& os, BoundingBox<TYPE> a) {
    os << "[" << a.c1 << "..." << a.c2 << "]";
    return os;
}

#endif