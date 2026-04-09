#include <iostream>

#include "mitl/Real.hpp"

int main() {
    mitl::Real x(22.0);
    mitl::Real y(7.0);

    auto q = x.divideBy(y, 64);
    auto qTrunc = x.divideBy(y, 0);
    auto r = x % y;

    std::cout << "x        = " << x << "\n";
    std::cout << "y        = " << y << "\n";
    std::cout << "x / y    = " << q << "\n";
    std::cout << "trunc(x/y)= " << qTrunc << "\n";
    std::cout << "x % y    = " << r << "\n";

    return 0;
}
