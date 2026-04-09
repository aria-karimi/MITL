#include "mitl/Complex.hpp"

#include <cmath>
#include <limits>
#include <ostream>
#include <regex>
#include <stdexcept>

namespace mitl {

namespace {

bool approxEqual(double a, double b) {
    if (std::isnan(a) || std::isnan(b)) {
        return false;
    }
    if (std::isinf(a) || std::isinf(b)) {
        return a == b;
    }

    const double diff = std::fabs(a - b);
    const double scale = std::max(1.0, std::max(std::fabs(a), std::fabs(b)));
    return diff <= (1e-12 * scale);
}

} // namespace

Complex::Complex()
    : real_(0.0), imag_(0.0) {}

Complex::Complex(const Real& realPart, const Real& imagPart)
    : real_(realPart), imag_(imagPart) {}

Complex::Complex(double realPart, double imagPart)
    : real_(realPart), imag_(imagPart) {}

const Real& Complex::real() const noexcept {
    return real_;
}

const Real& Complex::imag() const noexcept {
    return imag_;
}

void Complex::setReal(const Real& value) {
    real_ = value;
}

void Complex::setImag(const Real& value) {
    imag_ = value;
}

bool Complex::isZero() const {
    return real_.isZero() && imag_.isZero();
}

bool Complex::isNaN() const {
    return real_.isNaN() || imag_.isNaN();
}

bool Complex::isInfinity() const {
    return !isNaN() && (real_.isInfinity() || imag_.isInfinity());
}

Complex Complex::conjugate() const {
    return Complex(real_, -imag_);
}

Real Complex::normSquared() const {
    return (real_ * real_) + (imag_ * imag_);
}

Real Complex::abs() const {
    const double a = static_cast<double>(real_);
    const double b = static_cast<double>(imag_);
    return Real(std::hypot(a, b));
}

Real Complex::phase() const {
    const double a = static_cast<double>(real_);
    const double b = static_cast<double>(imag_);
    return Real(std::atan2(b, a));
}

Complex Complex::reciprocal() const {
    if (isZero()) {
        throw std::domain_error("Complex reciprocal of zero");
    }

    const Real denom = normSquared();
    return Complex(real_ / denom, (-imag_) / denom);
}

Complex Complex::fromPolar(double magnitude, double angleRadians) {
    return Complex(magnitude * std::cos(angleRadians), magnitude * std::sin(angleRadians));
}

Complex Complex::operator-() const {
    return Complex(-real_, -imag_);
}

Complex& Complex::operator+=(const Complex& other) {
    real_ += other.real_;
    imag_ += other.imag_;
    return *this;
}

Complex& Complex::operator+=(const Real& scalar) {
    real_ += scalar;
    return *this;
}

Complex& Complex::operator+=(double scalar) {
    real_ += Real(scalar);
    return *this;
}

Complex& Complex::operator-=(const Complex& other) {
    real_ -= other.real_;
    imag_ -= other.imag_;
    return *this;
}

Complex& Complex::operator-=(const Real& scalar) {
    real_ -= scalar;
    return *this;
}

Complex& Complex::operator-=(double scalar) {
    real_ -= Real(scalar);
    return *this;
}

Complex& Complex::operator*=(const Complex& other) {
    const Real a = real_;
    const Real b = imag_;
    const Real c = other.real_;
    const Real d = other.imag_;

    real_ = (a * c) - (b * d);
    imag_ = (a * d) + (b * c);
    return *this;
}

Complex& Complex::operator*=(const Real& scalar) {
    real_ *= scalar;
    imag_ *= scalar;
    return *this;
}

Complex& Complex::operator*=(double scalar) {
    const Real s(scalar);
    real_ *= s;
    imag_ *= s;
    return *this;
}

Complex& Complex::operator/=(const Complex& other) {
    const Real c = other.real_;
    const Real d = other.imag_;
    const Real denom = (c * c) + (d * d);

    if (denom.isZero()) {
        throw std::domain_error("Complex division by zero");
    }

    const Real a = real_;
    const Real b = imag_;

    real_ = ((a * c) + (b * d)) / denom;
    imag_ = ((b * c) - (a * d)) / denom;
    return *this;
}

Complex& Complex::operator/=(const Real& scalar) {
    if (scalar.isZero()) {
        throw std::domain_error("Complex division by zero scalar");
    }
    real_ /= scalar;
    imag_ /= scalar;
    return *this;
}

Complex& Complex::operator/=(double scalar) {
    if (scalar == 0.0) {
        throw std::domain_error("Complex division by zero scalar");
    }
    const Real s(scalar);
    real_ /= s;
    imag_ /= s;
    return *this;
}

bool operator==(const Complex& a, const Complex& b) {
    return approxEqual(static_cast<double>(a.real_), static_cast<double>(b.real_)) &&
           approxEqual(static_cast<double>(a.imag_), static_cast<double>(b.imag_));
}

std::string Complex::toString() const {
    const std::string realStr = real_.toString();
    const std::string imagStr = imag_.toString();

    if (imag_.isNegative()) {
        return realStr + imagStr + "i";
    }

    return realStr + "+" + imagStr + "i";
}

Complex Complex::parse(const std::string& text) {
    static const std::regex pattern(
        R"(^\s*([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?)\s*([+-])\s*((?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?)\s*i\s*$)");

    std::smatch match;
    if (!std::regex_match(text, match, pattern)) {
        throw std::invalid_argument("Complex::parse expects format a+bi or a-bi");
    }

    double realValue = 0.0;
    double imagValue = 0.0;
    try {
        realValue = std::stod(match[1].str());
        imagValue = std::stod(match[3].str());
    } catch (const std::exception&) {
        throw std::invalid_argument("Complex::parse numeric component out of range");
    }
    if (match[2].str() == "-") {
        imagValue = -imagValue;
    }

    return Complex(realValue, imagValue);
}

std::ostream& operator<<(std::ostream& os, const Complex& value) {
    return os << value.toString();
}

} // namespace mitl
