#pragma once

#include <iosfwd>
#include <string>

#include "mitl/Real.hpp"

namespace mitl {

/**
 * @brief Complex number type backed by mitl::Real components.
 */
class Complex {
public:
    /** @brief Constructs 0 + 0i. */
    Complex();

    /** @brief Constructs from real and imaginary components. */
    Complex(const Real& realPart, const Real& imagPart = Real());

    /** @brief Constructs from native doubles. */
    Complex(double realPart, double imagPart = 0.0);

    /** @brief Returns real component. */
    const Real& real() const noexcept;

    /** @brief Returns imaginary component. */
    const Real& imag() const noexcept;

    /** @brief Sets real component. */
    void setReal(const Real& value);

    /** @brief Sets imaginary component. */
    void setImag(const Real& value);

    /** @brief Returns true when both components are zero. */
    bool isZero() const;

    /** @brief Returns true when any component is NaN. */
    bool isNaN() const;

    /** @brief Returns true when any component is infinite and no component is NaN. */
    bool isInfinity() const;

    /** @brief Returns conjugate value (a - bi). */
    Complex conjugate() const;

    /** @brief Returns squared magnitude, a^2 + b^2. */
    Real normSquared() const;

    /** @brief Returns magnitude, sqrt(a^2 + b^2). */
    Real abs() const;

    /** @brief Returns principal phase angle in radians. */
    Real phase() const;

    /**
     * @brief Returns multiplicative inverse.
     * @throws std::domain_error if the value is zero.
     */
    Complex reciprocal() const;

    /**
     * @brief Creates a complex number from polar coordinates.
     * @param magnitude Radius in polar form.
     * @param angleRadians Angle in radians.
     */
    static Complex fromPolar(double magnitude, double angleRadians);

    /** @brief Returns unary negation. */
    Complex operator-() const;

    /** @brief Adds another complex value in place. */
    Complex& operator+=(const Complex& other);

    /** @brief Adds a real scalar in place. */
    Complex& operator+=(const Real& scalar);

    /** @brief Adds a double scalar in place. */
    Complex& operator+=(double scalar);

    /** @brief Subtracts another complex value in place. */
    Complex& operator-=(const Complex& other);

    /** @brief Subtracts a real scalar in place. */
    Complex& operator-=(const Real& scalar);

    /** @brief Subtracts a double scalar in place. */
    Complex& operator-=(double scalar);

    /** @brief Multiplies by another complex value in place. */
    Complex& operator*=(const Complex& other);

    /** @brief Multiplies by a real scalar in place. */
    Complex& operator*=(const Real& scalar);

    /** @brief Multiplies by a double scalar in place. */
    Complex& operator*=(double scalar);

    /**
     * @brief Divides by another complex value in place.
     * @throws std::domain_error if other is zero.
     */
    Complex& operator/=(const Complex& other);

    /** @brief Divides by a real scalar in place. */
    Complex& operator/=(const Real& scalar);

    /** @brief Divides by a double scalar in place. */
    Complex& operator/=(double scalar);

    friend Complex operator+(Complex lhs, const Complex& rhs) {
        lhs += rhs;
        return lhs;
    }

    friend Complex operator-(Complex lhs, const Complex& rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend Complex operator*(Complex lhs, const Complex& rhs) {
        lhs *= rhs;
        return lhs;
    }

    friend Complex operator/(Complex lhs, const Complex& rhs) {
        lhs /= rhs;
        return lhs;
    }

    friend Complex operator+(Complex lhs, const Real& rhs) {
        lhs += rhs;
        return lhs;
    }

    friend Complex operator+(const Real& lhs, Complex rhs) {
        rhs += lhs;
        return rhs;
    }

    friend Complex operator+(Complex lhs, double rhs) {
        lhs += rhs;
        return lhs;
    }

    friend Complex operator+(double lhs, Complex rhs) {
        rhs += lhs;
        return rhs;
    }

    friend Complex operator-(Complex lhs, const Real& rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend Complex operator-(const Real& lhs, const Complex& rhs) {
        return Complex(lhs, Real()) - rhs;
    }

    friend Complex operator-(Complex lhs, double rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend Complex operator-(double lhs, const Complex& rhs) {
        return Complex(lhs, 0.0) - rhs;
    }

    friend Complex operator*(Complex lhs, const Real& rhs) {
        lhs *= rhs;
        return lhs;
    }

    friend Complex operator*(const Real& lhs, Complex rhs) {
        rhs *= lhs;
        return rhs;
    }

    friend Complex operator*(Complex lhs, double rhs) {
        lhs *= rhs;
        return lhs;
    }

    friend Complex operator*(double lhs, Complex rhs) {
        rhs *= lhs;
        return rhs;
    }

    friend Complex operator/(Complex lhs, const Real& rhs) {
        lhs /= rhs;
        return lhs;
    }

    friend Complex operator/(const Real& lhs, const Complex& rhs) {
        return Complex(lhs, Real()) / rhs;
    }

    friend Complex operator/(Complex lhs, double rhs) {
        lhs /= rhs;
        return lhs;
    }

    friend Complex operator/(double lhs, const Complex& rhs) {
        return Complex(lhs, 0.0) / rhs;
    }

    /** @brief Returns true when both parts are exactly equal by mitl::Real equality. */
    friend bool operator==(const Complex& a, const Complex& b);

    /** @brief Returns true when values are not exactly equal. */
    friend bool operator!=(const Complex& a, const Complex& b) {
        return !(a == b);
    }

    /** @brief Returns canonical text representation like a+bi. */
    std::string toString() const;

    /**
     * @brief Parses text form a+bi or a-bi.
     *
     * Supports optional surrounding whitespace and scientific notation in each
     * numeric component.
     *
     * @throws std::invalid_argument If text does not match supported grammar.
     */
    static Complex parse(const std::string& text);

    /** @brief Streams canonical text representation. */
    friend std::ostream& operator<<(std::ostream& os, const Complex& value);

private:
    Real real_;
    Real imag_;
};

} // namespace mitl
