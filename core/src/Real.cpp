#include "mitl/Real.hpp"
#include "mitl/Exceptions.hpp"
#include "mitl/BigInt.hpp"

#include <limits>
#include <iomanip>
#include <sstream>
#include <stdexcept>

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
    negative_(std::signbit(value))
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
    return length_ == 0 && !isInfinity() && !isNaN();
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

bool operator==(const Real& a, const Real& b) {
    if (a.isNaN() || b.isNaN()) {
        return false;
    }
    if (a.isInfinity() || b.isInfinity()) {
        return a.isPositiveInfinity() == b.isPositiveInfinity() &&
               a.isNegativeInfinity() == b.isNegativeInfinity();
    }
    if (a.isZero() && b.isZero()) {
        return true;
    }

    if (a.negative_ != b.negative_) {
        return false;
    }

    // Compare exact values by aligning binary exponents and matching scaled magnitudes.
    const std::int64_t commonExp = std::min(a.exponent_, b.exponent_);
    BigInt aMag = Real::getBigIntMagnitude(a);
    BigInt bMag = Real::getBigIntMagnitude(b);

    const std::int64_t aShift = a.exponent_ - commonExp;
    const std::int64_t bShift = b.exponent_ - commonExp;
    if (aShift > 0) {
        aMag.mulPow2(static_cast<std::uint64_t>(aShift));
    }
    if (bShift > 0) {
        bMag.mulPow2(static_cast<std::uint64_t>(bShift));
    }

    return aMag == bMag;
}

bool operator!=(const Real& a, const Real& b) {
    return !(a == b);
}

bool operator<(const Real& a, const Real& b) {
    if (a.isNaN() || b.isNaN()) {
        return false;
    }
    if (a.isNegativeInfinity() && !b.isNegativeInfinity()) {
        return true;
    }
    if (b.isPositiveInfinity() && !a.isPositiveInfinity()) {
        return true;
    }
    if (a.isPositiveInfinity() || b.isNegativeInfinity()) {
        return false;
    }
    return static_cast<double>(a) < static_cast<double>(b);
}

bool operator>(const Real& a, const Real& b) {
    return b < a;
}

bool operator<=(const Real& a, const Real& b) {
    return (a < b) || (a == b);
}

bool operator>=(const Real& a, const Real& b) {
    return (a > b) || (a == b);
}

/* ============================================================
   Arithmetic operations
   ============================================================ */

/* ============================================================
   Helper: convert Real magnitude to BigInt
   BigInt = (sum digits[i] * 2^(64*i)) * 2^max(0,exp)
   if exp < 0 the denominator is 2^(-exp)
   ============================================================ */

BigInt Real::getBigIntMagnitude(const Real& r)
{
    BigInt mag;
    for (std::size_t i = r.length_; i-- > 0;) {
        mag.mulPow2(64);
        std::uint64_t limb = r.digits_[i];
        if (limb != 0) {
            mag += BigInt(std::to_string(limb));
        }
    }
    return mag;
}

BigInt Real::getIntegerBigInt(const Real& r)
{
    BigInt mag;
    if (r.isZero()) return mag;
    std::int64_t e = r.exponent_;
    // Build magnitude from digits
    mag = getBigIntMagnitude(r);
    if (e > 0) {
        mag.mulPow2(static_cast<std::uint64_t>(e));
    } else if (e < 0) {
        std::uint64_t d = static_cast<std::uint64_t>(-r.exponent_);
        BigInt pow2(1);
        pow2.mulPow2(d);
        auto [q, rm] = mag.div(pow2);
        mag = q;
    }
    return mag;
}

Real Real::fromBigInt(const BigInt& value, std::int64_t exp)
{
    Real r;
    if (value.isZero()) {
        r.exponent_ = 0;
        r.negative_ = false;
        return r;
    }

    BigInt magnitude = value.isNegative() ? -value : value;
    r.negative_ = value.isNegative();
    r.exponent_ = exp;

    static const BigInt two64("18446744073709551616");

    while (!magnitude.isZero()) {
        auto [q, rem] = magnitude.div(two64);
        std::string remStr = rem.toString();
        std::uint64_t lv = std::stoull(remStr);

        if (r.capacity_ == 0 || r.length_ >= r.capacity_) {
            if (r.digits_ == nullptr) {
                r.allocate(4);
            } else {
                std::size_t newCap = r.capacity_ * 2;
                std::uint64_t* newDigits = static_cast<std::uint64_t*>(
                    std::malloc(newCap * sizeof(std::uint64_t)));
                if (!newDigits) {
                    throw AllocationError("Failed to allocate memory during BigInt conversion");
                }
                std::memcpy(newDigits, r.digits_, r.length_ * sizeof(std::uint64_t));
                std::memset(newDigits + r.length_, 0, (newCap - r.length_) * sizeof(std::uint64_t));
                std::free(r.digits_);
                r.digits_ = newDigits;
                r.capacity_ = newCap;
            }
        }
        r.digits_[r.length_++] = lv;
        magnitude = std::move(q);
    }

    r.normalize();
    return r;
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
    if (isNaN() || other.isNaN()) {
        Real nan; nan.exponent_ = EXP_NAN;
        return nan;
    }
    if (isInfinity() && other.isInfinity()) {
        if (negative_ != other.negative_) {
            Real nan; nan.exponent_ = EXP_NAN;
            return nan;
        }
        Real inf; inf.exponent_ = EXP_INF; inf.negative_ = negative_;
        return inf;
    }
    if (isInfinity()) {
        Real inf; inf.exponent_ = EXP_INF; inf.negative_ = negative_;
        return inf;
    }
    if (other.isInfinity()) {
        Real inf; inf.exponent_ = EXP_INF; inf.negative_ = other.negative_;
        return inf;
    }
    if (other.isZero()) return *this;
    if (isZero()) return other;

    std::int64_t commonExp = std::min(exponent_, other.exponent_);

    BigInt a = getBigIntMagnitude(*this);
    BigInt b = getBigIntMagnitude(other);

    std::int64_t aShift = exponent_ - commonExp;
    std::int64_t bShift = other.exponent_ - commonExp;
    if (aShift > 0) a.mulPow2(static_cast<std::uint64_t>(aShift));
    if (bShift > 0) b.mulPow2(static_cast<std::uint64_t>(bShift));

    if (negative_) a = -a;
    if (other.negative_) b = -b;

    BigInt sum = a + b;
    return fromBigInt(sum, commonExp);
}

Real Real::operator-(const Real& other) const {
    if (isNaN() || other.isNaN()) {
        Real nan; nan.exponent_ = EXP_NAN;
        return nan;
    }
    if (other.isInfinity()) {
        if (isInfinity() && negative_ == other.negative_) {
            Real nan; nan.exponent_ = EXP_NAN;
            return nan;
        }
        // inf - inf = NaN (handled above), inf - finite = inf, finite - inf = -inf
        if (isInfinity()) {
            // already handled same-sign NaN
            // Different sign: +inf - (-inf) = +inf + +inf = +inf
            // -inf - (+inf) = -inf + -inf = -inf
            Real inf; inf.exponent_ = EXP_INF; inf.negative_ = negative_;
            return inf;
        }
        // finite - inf = -inf_sign
        Real negInf; negInf.exponent_ = EXP_INF; negInf.negative_ = !other.negative_;
        return negInf;
    }
    if (isInfinity()) return *this;
    if (other.isZero()) return *this;
    if (isZero()) return -other;

    return *this + (-other);
}

Real Real::operator*(const Real& other) const {
    if (isNaN() || other.isNaN()) {
        Real nan; nan.exponent_ = EXP_NAN;
        return nan;
    }
    if (isZero() && other.isInfinity()) {
        Real nan; nan.exponent_ = EXP_NAN;
        return nan;
    }
    if (other.isZero() && isInfinity()) {
        Real nan; nan.exponent_ = EXP_NAN;
        return nan;
    }
    if (isInfinity()) {
        Real inf; inf.exponent_ = EXP_INF; inf.negative_ = negative_ != other.negative_;
        return inf;
    }
    if (other.isInfinity()) {
        Real inf; inf.exponent_ = EXP_INF; inf.negative_ = negative_ != other.negative_;
        return inf;
    }
    if (isZero() || other.isZero()) {
        return Real();
    }

    BigInt a = getBigIntMagnitude(*this);
    BigInt b = getBigIntMagnitude(other);
    std::int64_t resultExp = exponent_ + other.exponent_;

    if (negative_) a = -a;
    if (other.negative_) b = -b;

    BigInt product = a * b;
    return fromBigInt(product, resultExp);
}

Real Real::operator/(const Real& other) const {
    return divideBy(other, MAX_DIV_LENGTH);
}

Real Real::operator%(const Real& other) const {
    if (isNaN() || other.isNaN()) {
        Real nan; nan.exponent_ = EXP_NAN;
        return nan;
    }
    if (isInfinity()) {
        Real nan; nan.exponent_ = EXP_NAN;
        return nan;
    }
    if (other.isInfinity()) return *this;
    if (other.isZero()) {
        throw std::domain_error("Modulo by zero");
    }
    if (isZero()) return Real();

    BigInt a = getIntegerBigInt(*this);
    BigInt b = getIntegerBigInt(other);

    if (negative_) a = -a;
    if (other.negative_) b = -b;

    auto [q, rem] = a.div(b);
    return fromBigInt(rem, 0);
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
    if (divisor.isZero()) {
        throw std::domain_error("Division by zero");
    }

    if (isNaN() || divisor.isNaN()) {
        Real nan; nan.exponent_ = EXP_NAN;
        return nan;
    }

    if (isInfinity() && divisor.isInfinity()) {
        Real nan; nan.exponent_ = EXP_NAN;
        return nan;
    }

    if (isZero()) {
        return Real();
    }

    if (divisor.isInfinity()) {
        return Real();
    }

    if (isInfinity()) {
        Real inf; inf.exponent_ = EXP_INF;
        inf.negative_ = negative_ != divisor.negative_;
        return inf;
    }

    const long double lhs = static_cast<long double>(static_cast<double>(*this));
    const long double rhs = static_cast<long double>(static_cast<double>(divisor));
    const long double raw = lhs / rhs;

    if (precision == 0) {
        return Real(static_cast<double>(std::trunc(raw)));
    }

    const std::size_t clampedPrecision = std::min<std::size_t>(precision, 256);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(static_cast<int>(clampedPrecision)) << raw;

    long double quantized = 0.0L;
    try {
        quantized = std::stold(oss.str());
    } catch (const std::exception&) {
        quantized = raw;
    }

    return Real(static_cast<double>(quantized));
}

/* ============================================================
   Conversion operators
   ============================================================ */

Real::operator std::int64_t() const {
    if (isNaN() || isInfinity()) {
        throw ConversionError("Cannot convert NaN or Infinity to int64_t");
    }

    if (length_ == 0) {
        return 0;
    }

    double asDouble = static_cast<double>(*this);
    if (!std::isfinite(asDouble)) {
        throw ConversionError("Value is not finite");
    }

    if (asDouble < static_cast<double>(std::numeric_limits<std::int64_t>::min()) ||
        asDouble > static_cast<double>(std::numeric_limits<std::int64_t>::max())) {
        throw ConversionError("Value too large to convert to int64_t");
    }

    double truncated = std::trunc(asDouble);
    if (std::fabs(asDouble - truncated) > 0.0) {
        throw ConversionError("Cannot convert non-integer Real to int64_t");
    }

    return static_cast<std::int64_t>(truncated);
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

    long double accum = 0.0L;
    for (std::size_t i = length_; i-- > 0;) {
        accum = std::ldexp(accum, 64) + static_cast<long double>(digits_[i]);
    }

    accum = std::ldexp(accum, static_cast<int>(exponent_));
    if (!std::isfinite(static_cast<double>(accum)) && !std::isinf(static_cast<double>(accum))) {
        throw ConversionError("Value too large to convert to double");
    }

    double result = static_cast<double>(accum);
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
    const double value = static_cast<double>(*this);
    if (std::isfinite(value) && std::trunc(value) == value) {
        std::ostringstream intStream;
        intStream << std::fixed << std::setprecision(0) << value;
        return intStream.str();
    }

    std::ostringstream oss;
    oss << std::setprecision(std::numeric_limits<double>::max_digits10) << value;
    std::string out = oss.str();

    const std::size_t dot = out.find('.');
    if (dot != std::string::npos) {
        while (!out.empty() && out.back() == '0') {
            out.pop_back();
        }
        if (!out.empty() && out.back() == '.') {
            out.pop_back();
        }
    }

    return out;
}


    



} // namespace mitl