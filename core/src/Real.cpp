#include "mitl/Real.hpp"
#include "mitl/Exceptions.hpp"
#include "mitl/BigInt.hpp"

#include <sstream>

namespace mitl {

/* ============================================================
   Constructors
   ============================================================ */

Real::Real()
    : digits_(nullptr),
      length_(0),
      capacity_(0),
      exponent_(0),
      negative_(false)
{}


Real::Real(double value)
    : digits_(nullptr),
      length_(0),
      capacity_(0),
      exponent_(0),
      negative_(value < 0.0)
{
    std::uint64_t bits;
    std::memcpy(&bits, &value, sizeof(double));
    std::uint64_t mag = bits & 0x000FFFFFFFFFFFFF;
    std::int64_t exp = ((bits >> 52) & 0x7FF) - 1023;
    if (exp == 1024) { // Inf or NaN
        if (mag == 0) {
            exponent_ = EXP_INF;
        } else {
            exponent_ = EXP_NAN;
        }
        return;
    }
    if (exp == -1023) { // Denormalized
        if (mag == 0) {
            return; // zero
        }
        // Normalize denormalized number
        while ((mag & (1ULL << 52)) == 0) {
            mag <<= 1;
            --exp;
        }
    } else {
        mag |= (1ULL << 52); // Add implicit leading 1
    }
    exp -= 52; // Adjust exponent for mantissa bits

    exponent_ = exp;
    allocate(1);
    digits_[0] = mag;
    length_ = 1;
}


/* ============================================================
   Copy Constructor
   ============================================================ */

Real::Real(const Real& other)
    : digits_(nullptr),
      length_(other.length_),
      capacity_(other.length_),
      exponent_(other.exponent_),
      negative_(other.negative_)
{
    if (capacity_ > 0) {
        allocate(capacity_);
        std::memcpy(digits_, other.digits_, length_ * sizeof(std::uint64_t));
    }
}


/* ============================================================
   Move Constructor
   ============================================================ */

Real::Real(Real&& other) noexcept
    : digits_(other.digits_),
      length_(other.length_),
      capacity_(other.capacity_),
      exponent_(other.exponent_),
      negative_(other.negative_)
{
    other.digits_ = nullptr;
    other.length_ = 0;
    other.capacity_ = 0;
}


/* ============================================================
   Copy Assignment
   ============================================================ */

Real& Real::operator=(const Real& other) {
    if (this == &other)
        return *this;

    freeMemory();

    length_ = other.length_;
    capacity_ = other.length_;   // allocate tight
    exponent_ = other.exponent_;
    negative_ = other.negative_;

    if (capacity_ > 0) {
        allocate(capacity_);
        std::memcpy(digits_, other.digits_, length_ * sizeof(std::uint64_t));
    }

    return *this;
}


/* ============================================================
   Move Assignment
   ============================================================ */

Real& Real::operator=(Real&& other) noexcept {
    if (this == &other)
        return *this;

    freeMemory();

    digits_ = other.digits_;
    length_ = other.length_;
    capacity_ = other.capacity_;
    exponent_ = other.exponent_;
    negative_ = other.negative_;

    other.digits_ = nullptr;
    other.length_ = 0;
    other.capacity_ = 0;

    return *this;
}


/* ============================================================
   Destructor
   ============================================================ */

Real::~Real() {
    freeMemory();
}


/* ============================================================
   Internal helpers
   ============================================================ */

void Real::allocate(std::size_t capacity) {
    if (digits_ != nullptr) {
        throw AllocationError("Memory already allocated");
    }

    digits_ = static_cast<std::uint64_t*>(
        std::malloc(capacity * sizeof(std::uint64_t))
    );

    if (digits_ == nullptr) {
        throw AllocationError("Failed to allocate memory");
    }

    std::memset(digits_, 0, capacity * sizeof(std::uint64_t));
    capacity_ = capacity;
}

void Real::grow() {
    std::size_t newCapacity = (capacity_ == 0 ? 4 : capacity_ * 2);

    std::uint64_t* newDigits = static_cast<std::uint64_t*>(
        std::malloc(newCapacity * sizeof(std::uint64_t))
    );

    if (newDigits == nullptr) {
        throw AllocationError("Failed to allocate memory during growth");
    }

    if (digits_ && length_ > 0) {
        std::memcpy(newDigits, digits_, length_ * sizeof(std::uint64_t));
    }

    std::memset(newDigits + length_, 0, (newCapacity - length_) * sizeof(std::uint64_t));

    std::free(digits_);
    digits_ = newDigits;
    capacity_ = newCapacity;
}

void Real::freeMemory() {
    if (digits_ != nullptr) {
        std::free(digits_);
        digits_ = nullptr;
    }

    length_ = 0;
    capacity_ = 0;
}

void Real::normalize()
{
    if (isNaN() || isInfinity()) {
        return;
    }

    while (length_ > 0 && digits_[length_ - 1] == 0) {
        --length_;
    }

    if (length_ == 0) {
        negative_ = false;
        exponent_ = 0;
    }
}

void Real::shrinkToFit() {
    if (length_ < capacity_) {
        if (length_ == 0) {
            std::free(digits_);
            digits_ = nullptr;
            capacity_ = 0;
        } else {
            std::uint64_t* newDigits = static_cast<std::uint64_t*>(
                std::malloc(length_ * sizeof(std::uint64_t))
            );

            if (newDigits == nullptr) {
                throw AllocationError("Failed to allocate memory during shrinkToFit");
            }

            std::memcpy(newDigits, digits_, length_ * sizeof(std::uint64_t));
            std::free(digits_);
            digits_ = newDigits;
            capacity_ = length_;
        }
    }
}

/* ============================================================
   Public methods
   ============================================================ */

bool Real::isZero() const {
    return length_ == 0;
}

bool Real::isNegative() const {
    return !isZero() && negative_;
}

bool Real::isPositive() const {
    return !isZero() && !negative_;
}

bool Real::isInfinity() const {
    return exponent_ == EXP_INF && length_ == 0;
}

bool Real::isPositiveInfinity() const {
    return isInfinity() && !negative_;
}

bool Real::isNegativeInfinity() const {
    return isInfinity() && negative_;
}

bool Real::isNaN() const {
    return exponent_ == EXP_NAN;
}

bool Real::isInteger() const {
    if (isNaN() || isInfinity()) {
        return false;
    }
    if (isZero()) {
        return true;
    }
    if (exponent_ >= 0) {
        return true;
    }
    // exponent_ < 0: need to check divisibility by 2^{-exponent_}
    std::uint64_t neededZeros = static_cast<std::uint64_t>(-exponent_);
    std::uint64_t bitsPerLimb = 64;

    std::size_t limbIndex = 0;
    while (neededZeros >= bitsPerLimb && limbIndex < length_) {
        if (digits_[limbIndex] != 0) {
            return false;
        }
        neededZeros -= bitsPerLimb;
        ++limbIndex;
    }
    if (neededZeros == 0) {
        return true;
    }
    if (limbIndex >= length_) {
        return false;
    }
    std::uint64_t limb = digits_[limbIndex];
    // Check that the lowest neededZeros bits of this limb are zero
    std::uint64_t mask = (neededZeros >= 64) ? ~std::uint64_t(0) : ((std::uint64_t(1) << neededZeros) - 1);
    return (limb & mask) == 0;
}

/* ============================================================
   Friend functions
   ============================================================ */
std::ostream& operator<<(std::ostream& os, const Real& value) {
    os << value.toString();
    return os;
}

/* ============================================================
   Arithmetic operations
   ============================================================ */

Real Real::operator-() const {
    Real result(*this);
    if (!result.isZero()) {
        result.negative_ = !result.negative_;
    }
    return result;
}

Real Real::operator+(const Real& other) const {
    Real result;
    // Placeholder implementation
    return result;
}

Real Real::operator-(const Real& other) const {
    Real result;
    // Placeholder implementation
    return result;
}

Real Real::operator*(const Real& other) const {
    Real result;
    // Placeholder implementation
    return result;
}

Real Real::operator/(const Real& other) const {
    Real result;
    // Placeholder implementation
    return result;
}

Real Real::operator%(const Real& other) const {
    Real result;
    // Placeholder implementation
    return result;
}

Real& Real::operator+=(const Real& other) {
    *this = *this + other;
    return *this;
}

Real& Real::operator-=(const Real& other) {
    *this = *this - other;
    return *this;
}

Real& Real::operator*=(const Real& other) {
    *this = *this * other;
    return *this;
}

Real& Real::operator/=(const Real& other) {
    *this = *this / other;
    return *this;
}

Real& Real::operator%=(const Real& other) {
    *this = *this % other;
    return *this;
}

Real Real::divideBy(const Real& divisor, std::size_t precision) const {
    Real result;
    // Placeholder implementation
    return result;
}

/* ============================================================
   Conversion operators
   ============================================================ */

Real::operator std::int64_t() const {
    if (isNaN() || isInfinity() || length_ == 0) {
        throw ConversionError("Cannot convert NaN, Infinity, or Zero to int64_t");
    }

    std::int64_t result = 0;
    if (length_ == 1) {
        result = static_cast<std::int64_t>(digits_[0]);
    } else {
        throw ConversionError("Value too large to convert to int64_t");
    }
    return negative_ ? -result : result;
}

Real::operator double() const {
    if (isNaN()) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    if (isInfinity()) {
        return negative_ ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
    }
    if (length_ == 0) {
        return 0.0;
    }

    double result = 0.0;
    if (length_ == 1) {
        result = static_cast<double>(digits_[0]);
    } else {
        throw ConversionError("Value too large to convert to double");
    }
    result *= std::pow(2.0, static_cast<double>(exponent_));

    return negative_ ? -result : result;
}

/* ============================================================
   String representation
   ============================================================ */

std::string Real::toString() const {
    if (isNaN()) {
        return "NaN";
    }
    if (isInfinity()) {
        return negative_ ? "-Infinity" : "Infinity";
    }
    if (isZero()) {
        return "0";
    }
    // Build BigInt magnitude from base-2^64 limbs
    mitl::BigInt mag;
    // Import digits_ as big integer: mag = sum_i digits_[i] * 2^(64*i)
    for (std::size_t i = length_; i-- > 0;) {
        // multiply by 2^64 for each more-significant limb
        mag.mulPow2(64);
        std::uint64_t limb = digits_[i];
        if (limb != 0) {
            mitl::BigInt limbBig(static_cast<std::int64_t>(limb));
            mag += limbBig;
        }
    }

    bool neg = negative_;

    // Apply binary exponent: numerator and decimal exponent
    std::int64_t e = exponent_;
    std::uint64_t denomPow2 = 0;
    if (e > 0) {
        mag.mulPow2(static_cast<std::uint64_t>(e));
    } else if (e < 0) {
        denomPow2 = static_cast<std::uint64_t>(-e);
    }

    // Convert denominator 2^denomPow2 into decimal scaling: multiply by 5^denomPow2, track 10^{-denomPow2}
    long long decExp = 0;
    if (denomPow2 > 0) {
        mag.mulPow5(denomPow2);
        decExp = -static_cast<long long>(denomPow2);
    }

    // Convert magnitude to decimal string
    std::string magStr = mag.toString();
    int numDigits = static_cast<int>(magStr.size());

    std::string result;

    if (decExp >= 0) {
        // Pure integer (possibly with decimal trailing zeros from scaling)
        result = magStr;
        result.append(static_cast<std::size_t>(decExp), '0');
    } else {
        long long k = -decExp; // number of digits to shift left
        long long pointPos = static_cast<long long>(numDigits) - k;
        if (pointPos > 0) {
            // Some digits before decimal point
            result.assign(magStr.begin(), magStr.begin() + pointPos);
            result.push_back('.');
            result.append(magStr.begin() + pointPos, magStr.end());
        } else {
            // All digits after decimal point with leading zeros
            std::size_t leadZeros = static_cast<std::size_t>(-pointPos);
            result = "0.";
            result.append(leadZeros, '0');
            result += magStr;
        }

        // Trim trailing zeros in fractional part (but keep at least one digit)
        auto dotPos = result.find('.');
        if (dotPos != std::string::npos) {
            std::size_t last = result.size() - 1;
            while (last > dotPos && result[last] == '0') {
                --last;
            }
            if (last == dotPos) {
                // All fractional digits were zeros: drop decimal point entirely
                result.erase(dotPos);
            } else {
                result.erase(last + 1);
            }
        }
    }

    // Prepend sign
    if (neg && result != "0") {
        result.insert(result.begin(), '-');
    }

    return result;
}


    



} // namespace mitl