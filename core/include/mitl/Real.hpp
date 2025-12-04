#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <string>
#include <cmath>


namespace mitl {

class Real {
public:
    Real();
    explicit Real(double value);

    Real(const Real& other);
    Real(Real&& other) noexcept;

    Real& operator=(const Real& other);
    Real& operator=(Real&& other) noexcept;

    ~Real();

private:
    std::uint64_t* digits_;   // pointer to limbs
    std::size_t    length_;   // number of limbs used
    std::size_t    capacity_; // allocated limbs

    std::int64_t   exponent_;
    bool           negative_;

private:
    static constexpr std::int64_t EXP_INF = std::numeric_limits<std::int64_t>::max();
    static constexpr std::int64_t EXP_NAN = std::numeric_limits<std::int64_t>::min();
    static constexpr std::size_t MAX_DIV_LENGTH = 1024; // arbitrary limit for this implementation

private:
    void allocate(std::size_t capacity);
    void grow();              // doubles capacity
    void freeMemory();
    void normalize();
    void shrinkToFit();

public:
    bool isZero() const;
    bool isNegative() const;
    bool isPositive() const;
    bool isInfinity() const;
    bool isPositiveInfinity() const;
    bool isNegativeInfinity() const;
    bool isNaN() const;

private:
    bool isInteger() const;
    
    

public:
    // Basic arithmetic operations
    Real operator-() const;
    Real operator+(const Real& other) const;
    Real operator-(const Real& other) const;
    Real operator*(const Real& other) const;
    Real operator/(const Real& other) const;
    Real operator%(const Real& other) const;
    Real& operator+=(const Real& other);
    Real& operator-=(const Real& other);
    Real& operator*=(const Real& other);
    Real& operator/=(const Real& other);
    Real& operator%=(const Real& other);

    // Division with arbitrary precision
    Real divideBy(const Real& divisor, std::size_t precision = MAX_DIV_LENGTH) const;

    // Conversion operators overloads
    explicit operator std::int64_t() const;
    explicit operator double() const;

    // String representation
    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const Real& value);

};



} // namespace mitl
