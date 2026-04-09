#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <string>
#include <cmath>
#include <utility>

namespace mitl {

class BigInt;

/**
 * @brief Arbitrary-precision floating-point number with sign-magnitude binary representation.
 */
class Real {
public:
    /** @brief Constructs zero. */
    Real();
    /** @brief Constructs from IEEE-754 double. */
    explicit Real(double value);

    /** @brief Copy constructor. */
    Real(const Real& other);
    /** @brief Move constructor. */
    Real(Real&& other) noexcept;

    /** @brief Copy assignment. */
    Real& operator=(const Real& other);
    /** @brief Move assignment. */
    Real& operator=(Real&& other) noexcept;

    /** @brief Destructor. */
    ~Real();

private:
    std::uint64_t* digits_;   // pointer to limbs
    std::size_t    length_;   // number of limbs used
    std::size_t    capacity_; // allocated limbs

    std::int64_t   exponent_;
    bool           negative_;

private:
    // Helpers
    static BigInt getBigIntMagnitude(const Real& r);
    static BigInt getIntegerBigInt(const Real& r);
    static Real fromBigInt(const BigInt& value, std::int64_t exp = 0);

    static constexpr std::int64_t EXP_INF = std::numeric_limits<std::int64_t>::max();
    static constexpr std::int64_t EXP_NAN = std::numeric_limits<std::int64_t>::min();
    static constexpr std::size_t MAX_DIV_LENGTH = 64;

private:
    void allocate(std::size_t capacity);
    void grow();              // doubles capacity
    void freeMemory();
    void normalize();
    void shrinkToFit();

public:
    /** @brief Returns true if value is exactly zero. */
    bool isZero() const;
    /** @brief Returns true if value is finite and strictly negative. */
    bool isNegative() const;
    /** @brief Returns true if value is finite and strictly positive. */
    bool isPositive() const;
    /** @brief Returns true if value is infinity. */
    bool isInfinity() const;
    /** @brief Returns true if value is positive infinity. */
    bool isPositiveInfinity() const;
    /** @brief Returns true if value is negative infinity. */
    bool isNegativeInfinity() const;
    /** @brief Returns true if value is NaN. */
    bool isNaN() const;

    /** @brief Returns true if the value is mathematically integral. */
    bool isInteger() const;

    /** @brief Equality comparison (NaN is never equal). */
    friend bool operator==(const Real& a, const Real& b);
    /** @brief Inequality comparison. */
    friend bool operator!=(const Real& a, const Real& b);
    /** @brief Strict less-than (false if either operand is NaN). */
    friend bool operator<(const Real& a, const Real& b);
    /** @brief Strict greater-than (false if either operand is NaN). */
    friend bool operator>(const Real& a, const Real& b);
    /** @brief Less-than-or-equal (false if either operand is NaN). */
    friend bool operator<=(const Real& a, const Real& b);
    /** @brief Greater-than-or-equal (false if either operand is NaN). */
    friend bool operator>=(const Real& a, const Real& b);

public:
    /** @brief Returns arithmetic negation. */
    Real operator-() const;
    /** @brief Returns sum with another Real. */
    Real operator+(const Real& other) const;
    /** @brief Returns difference with another Real. */
    Real operator-(const Real& other) const;
    /** @brief Returns product with another Real. */
    Real operator*(const Real& other) const;
    /** @brief Returns quotient with another Real. */
    Real operator/(const Real& other) const;
    /** @brief Returns remainder for integer-like operands. */
    Real operator%(const Real& other) const;
    /** @brief Adds another Real in place. */
    Real& operator+=(const Real& other);
    /** @brief Subtracts another Real in place. */
    Real& operator-=(const Real& other);
    /** @brief Multiplies by another Real in place. */
    Real& operator*=(const Real& other);
    /** @brief Divides by another Real in place. */
    Real& operator/=(const Real& other);
    /** @brief Computes remainder by another Real in place. */
    Real& operator%=(const Real& other);

     /**
      * @brief Divides with caller-selected precision budget.
      * @param divisor Divisor value.
      * @param precision Decimal digits kept after the radix point in fixed formatting.
      *        Value `0` returns truncation toward zero. Internal cap is 256.
      * @return Quotient.
      */
    Real divideBy(const Real& divisor, std::size_t precision = MAX_DIV_LENGTH) const;

    /** @brief Converts to signed 64-bit integer if representable and integral. */
    explicit operator std::int64_t() const;
    /** @brief Converts to IEEE-754 double (may lose precision). */
    explicit operator double() const;

    /** @brief Returns human-readable decimal string representation. */
    std::string toString() const;
    /** @brief Streams decimal string representation. */
    friend std::ostream& operator<<(std::ostream& os, const Real& value);

};



} // namespace mitl
