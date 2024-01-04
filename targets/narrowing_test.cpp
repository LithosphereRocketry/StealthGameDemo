#include <iostream>
#include <cmath>

int main(int, char**) {
    // what the hell is this
    std::cout << "ceil(inf) which is " << std::ceil(INFINITY) << ": " << int(std::ceil(INFINITY)) << "\n";
    float f = std::ceil(INFINITY);
    std::cout << "ceil(inf)2 which is " << f << ": " << int(f) << "\n";
    std::cout << "inf:" << int(INFINITY) << "\n";
    std::cout << "big:" << int(1e12f) << "\n";
    std::cout << "normal:" << int(1e9f) << "\n";
}