#include "mitl/Real.hpp"

#include <iostream>

using mitl::Real;

int main()
{
    auto test = [](const char* label, const Real& r) {
        std::cout << label << ": " << r.toString() << '\n';
    };

    test("zero", Real(0));
    test("one", Real(1));
    test("minus_one", Real(-1));
    test("half", Real(0.5));
    test("one_eighth", Real(0.125));
    test("big_pos", Real(1e10));
    test("big_neg", Real(-1e10));

    return 0;
}
