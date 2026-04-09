#include <iostream>

#include "mitl/Complex.hpp"

int main() {
    mitl::Complex a(1.0, 2.0);
    mitl::Complex b(3.0, -4.0);
    mitl::Complex parsed = mitl::Complex::parse(" 1.5-2e0i ");

    auto sum = a + b;
    auto product = a * b;
    auto quotient = a / b;

    std::cout << "a         = " << a << "\n";
    std::cout << "b         = " << b << "\n";
    std::cout << "a + b     = " << sum << "\n";
    std::cout << "a * b     = " << product << "\n";
    std::cout << "a / b     = " << quotient << "\n";
    std::cout << "parsed    = " << parsed << "\n";
    std::cout << "conj(a)   = " << a.conjugate() << "\n";
    std::cout << "|a|       = " << a.abs() << "\n";
    std::cout << "arg(a)    = " << a.phase() << "\n";

    return 0;
}
