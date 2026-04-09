#include <iostream>

#include "mitl/BigInt.hpp"

int main() {
    mitl::BigInt a("123456789012345678901234567890");
    mitl::BigInt b("9876543210");
    mitl::BigInt negDividend(-17);
    mitl::BigInt smallDivisor(5);

    auto sum = a + b;
    auto product = a * b;
    auto qr = a.div(b);
    auto negQr = negDividend.div(smallDivisor);

    std::cout << "a      = " << a << "\n";
    std::cout << "b      = " << b << "\n";
    std::cout << "a + b  = " << sum << "\n";
    std::cout << "a * b  = " << product << "\n";
    std::cout << "a / b  = " << qr.first << "\n";
    std::cout << "a % b  = " << qr.second << "\n";
    std::cout << "-17 / 5 = " << negQr.first << "\n";
    std::cout << "-17 % 5 = " << negQr.second << "\n";

    return 0;
}
