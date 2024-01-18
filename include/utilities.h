#ifndef UTILITIES_H
#define UTILITIES_H

#include <type_traits>
#include <limits>

template <class TO, class FROM>
TO saturating_cast(FROM value) {
    static_assert(std::is_arithmetic<FROM>::value,
            "Saturating cast only for numbers");
    static_assert(std::is_arithmetic<TO>::value,
            "Saturating cast only for numbers");
    if(value > std::numeric_limits<TO>::max()) {
        return (std::numeric_limits<TO>::has_infinity ?
                std::numeric_limits<TO>::infinity()
            :   std::numeric_limits<TO>::max());
    } else if(value < std::numeric_limits<TO>::min()) {
        return (std::numeric_limits<TO>::has_infinity ?
                -std::numeric_limits<TO>::infinity()
            :   std::numeric_limits<TO>::min());
    } else {
        return TO(value);
    }
}

#endif