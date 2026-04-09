#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include "mitl/BigInt.hpp"
#include "mitl/Complex.hpp"
#include "mitl/Exceptions.hpp"
#include "mitl/Real.hpp"

namespace py = pybind11;

PYBIND11_MODULE(mitl, m) {
    m.doc() = "MITL arbitrary-precision math bindings";

    auto pyMitlError = py::register_exception<mitl::MitlError>(m, "MitlError");
    py::register_exception<mitl::AllocationError>(m, "AllocationError", pyMitlError.ptr());
    py::register_exception<mitl::ConversionError>(m, "ConversionError", pyMitlError.ptr());

    py::class_<mitl::BigInt>(m, "BigInt")
        .def(py::init<>())
        .def(py::init<std::int64_t>())
        .def(py::init<const std::string&>())
        .def_static("zero", &mitl::BigInt::zero)
        .def_static("one", &mitl::BigInt::one)
        .def("is_zero", &mitl::BigInt::isZero)
        .def("is_negative", &mitl::BigInt::isNegative)
        .def("to_string", &mitl::BigInt::toString)
        .def("mul_pow2", &mitl::BigInt::mulPow2, py::arg("k"))
        .def("mul_pow5", &mitl::BigInt::mulPow5, py::arg("k"))
        .def("divmod", [](const mitl::BigInt& a, const mitl::BigInt& b) {
            auto pair = a.div(b);
            return py::make_tuple(pair.first, pair.second);
        })
        .def("__str__", &mitl::BigInt::toString)
        .def("__repr__", [](const mitl::BigInt& v) { return std::string("BigInt('") + v.toString() + "')"; })
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self % py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self)
        .def(py::self <= py::self)
        .def(py::self > py::self)
        .def(py::self >= py::self)
        .def(-py::self)
        .def("__iadd__", [](mitl::BigInt& self, const mitl::BigInt& rhs) -> mitl::BigInt& { return self += rhs; })
        .def("__isub__", [](mitl::BigInt& self, const mitl::BigInt& rhs) -> mitl::BigInt& { return self -= rhs; })
        .def("__imul__", [](mitl::BigInt& self, const mitl::BigInt& rhs) -> mitl::BigInt& { return self *= rhs; })
        .def("__itruediv__", [](mitl::BigInt& self, const mitl::BigInt& rhs) -> mitl::BigInt& { return self /= rhs; })
        .def("__imod__", [](mitl::BigInt& self, const mitl::BigInt& rhs) -> mitl::BigInt& { return self %= rhs; });

    py::class_<mitl::Real>(m, "Real")
        .def(py::init<>())
        .def(py::init<double>())
        .def("is_zero", &mitl::Real::isZero)
        .def("is_negative", &mitl::Real::isNegative)
        .def("is_positive", &mitl::Real::isPositive)
        .def("is_infinity", &mitl::Real::isInfinity)
        .def("is_positive_infinity", &mitl::Real::isPositiveInfinity)
        .def("is_negative_infinity", &mitl::Real::isNegativeInfinity)
        .def("is_nan", &mitl::Real::isNaN)
        .def("divide_by", &mitl::Real::divideBy, py::arg("divisor"), py::arg("precision") = 64)
        .def("to_string", &mitl::Real::toString)
        .def("to_float", [](const mitl::Real& self) { return static_cast<double>(self); })
        .def("to_int", [](const mitl::Real& self) { return static_cast<std::int64_t>(self); })
        .def("__str__", &mitl::Real::toString)
        .def("__repr__", [](const mitl::Real& v) { return std::string("Real('") + v.toString() + "')"; })
        .def("is_integer", &mitl::Real::isInteger)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self % py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self)
        .def(py::self <= py::self)
        .def(py::self > py::self)
        .def(py::self >= py::self)
        .def(-py::self)
        .def("__iadd__", [](mitl::Real& self, const mitl::Real& rhs) -> mitl::Real& { return self += rhs; })
        .def("__isub__", [](mitl::Real& self, const mitl::Real& rhs) -> mitl::Real& { return self -= rhs; })
        .def("__imul__", [](mitl::Real& self, const mitl::Real& rhs) -> mitl::Real& { return self *= rhs; })
        .def("__itruediv__", [](mitl::Real& self, const mitl::Real& rhs) -> mitl::Real& { return self /= rhs; })
        .def("__imod__", [](mitl::Real& self, const mitl::Real& rhs) -> mitl::Real& { return self %= rhs; });

    py::class_<mitl::Complex>(m, "Complex")
        .def(py::init<>())
        .def(py::init<double, double>(), py::arg("real"), py::arg("imag") = 0.0)
        .def(py::init<const mitl::Real&, const mitl::Real&>(), py::arg("real"), py::arg("imag") = mitl::Real())
        .def_property("real",
            [](const mitl::Complex& self) { return self.real(); },
            [](mitl::Complex& self, const mitl::Real& value) { self.setReal(value); })
        .def_property("imag",
            [](const mitl::Complex& self) { return self.imag(); },
            [](mitl::Complex& self, const mitl::Real& value) { self.setImag(value); })
        .def("is_zero", &mitl::Complex::isZero)
        .def("is_nan", &mitl::Complex::isNaN)
        .def("is_infinity", &mitl::Complex::isInfinity)
        .def("conjugate", &mitl::Complex::conjugate)
        .def("norm_squared", &mitl::Complex::normSquared)
        .def("abs", &mitl::Complex::abs)
        .def("phase", &mitl::Complex::phase)
        .def("reciprocal", &mitl::Complex::reciprocal)
        .def_static("from_polar", &mitl::Complex::fromPolar, py::arg("magnitude"), py::arg("angle_radians"))
        .def_static("parse", &mitl::Complex::parse, py::arg("text"))
        .def("to_string", &mitl::Complex::toString)
        .def("__str__", &mitl::Complex::toString)
        .def("__repr__", [](const mitl::Complex& v) { return std::string("Complex('") + v.toString() + "')"; })
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self + mitl::Real())
        .def(mitl::Real() + py::self)
        .def(py::self - mitl::Real())
        .def(mitl::Real() - py::self)
        .def(py::self * mitl::Real())
        .def(mitl::Real() * py::self)
        .def(py::self / mitl::Real())
        .def(mitl::Real() / py::self)
        .def(py::self + double())
        .def(double() + py::self)
        .def(py::self - double())
        .def(double() - py::self)
        .def(py::self * double())
        .def(double() * py::self)
        .def(py::self / double())
        .def(double() / py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(-py::self)
        .def("__iadd__", [](mitl::Complex& self, const mitl::Complex& rhs) -> mitl::Complex& { return self += rhs; })
        .def("__isub__", [](mitl::Complex& self, const mitl::Complex& rhs) -> mitl::Complex& { return self -= rhs; })
        .def("__imul__", [](mitl::Complex& self, const mitl::Complex& rhs) -> mitl::Complex& { return self *= rhs; })
        .def("__itruediv__", [](mitl::Complex& self, const mitl::Complex& rhs) -> mitl::Complex& { return self /= rhs; });
}
