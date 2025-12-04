#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <iosfwd>

namespace mitl {

// A simple signed arbitrary-precision integer.
// Representation:
//   - sign_  : false => non-negative, true => negative
//   - limbs_ : little-endian base BASE = 10^9
//     value = (sign_ ? -1 : +1) * sum(limbs_[i] * BASE^i)
class BigInt
{
public:
	static constexpr std::uint32_t BASE      = 1000000000u; // 10^9
	static constexpr unsigned      BASE_DIGS = 9;           // digits per limb

	BigInt() = default;

	// Construct from 64-bit signed integer
	BigInt(std::int64_t value);

	// Construct from decimal string (e.g. "-123456")
	explicit BigInt(const std::string& decimal);

	// Factory for zero / one
	static BigInt zero();
	static BigInt one();

	// Observers
	bool isZero() const noexcept { return limbs_.empty(); }
	bool isNegative() const noexcept { return sign_ && !isZero(); }

	// Comparison
	friend int compare(const BigInt& a, const BigInt& b) noexcept;

	friend bool operator==(const BigInt& a, const BigInt& b) noexcept
	{
		return a.sign_ == b.sign_ && a.limbs_ == b.limbs_;
	}

	friend bool operator!=(const BigInt& a, const BigInt& b) noexcept
	{
		return !(a == b);
	}

	friend bool operator<(const BigInt& a, const BigInt& b) noexcept
	{
		return compare(a, b) < 0;
	}

	friend bool operator>(const BigInt& a, const BigInt& b) noexcept
	{
		return compare(a, b) > 0;
	}

	friend bool operator<=(const BigInt& a, const BigInt& b) noexcept
	{
		return compare(a, b) <= 0;
	}

	friend bool operator>=(const BigInt& a, const BigInt& b) noexcept
	{
		return compare(a, b) >= 0;
	}

	// Unary minus
	BigInt operator-() const;

	// Addition / subtraction
	BigInt& operator+=(const BigInt& other);
	BigInt& operator-=(const BigInt& other);

	friend BigInt operator+(BigInt lhs, const BigInt& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	friend BigInt operator-(BigInt lhs, const BigInt& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	// Multiply by small non-negative integer (k < BASE)
	BigInt& mulSmall(std::uint32_t k);

	// Divide by small positive integer (k < BASE).
	// Returns remainder. Undefined for k == 0.
	std::uint32_t divSmall(std::uint32_t k);

	// Full big-int multiplication
	BigInt& operator*=(const BigInt& other);

	friend BigInt operator*(BigInt lhs, const BigInt& rhs)
	{
		lhs *= rhs;
		return lhs;
	}

	// Remove leading zero limbs and normalize sign for zero.
	void normalize() noexcept;

	// Multiply by 2^k (k >= 0).
	BigInt& mulPow2(std::uint64_t k);

	// Multiply by 5^k (k >= 0).
	BigInt& mulPow5(std::uint64_t k);

	// Convert to decimal string.
	// Always returns canonical form (e.g. "-123", "0", "456789").
	std::string toString() const;

	// Stream output helper
	friend std::ostream& operator<<(std::ostream& os, const BigInt& value);

private:
	bool                        sign_  = false; // false = non-negative, true = negative
	std::vector<std::uint32_t>  limbs_;         // little-endian limbs (least significant first)

	// Helpers for absolute-value operations (ignores sign_)
	static int   cmpAbs(const BigInt& a, const BigInt& b) noexcept;
	static BigInt addAbs(const BigInt& a, const BigInt& b);
	static BigInt subAbs(const BigInt& a, const BigInt& b); // assumes |a| >= |b|
};

} // namespace mitl

