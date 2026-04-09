#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <iosfwd>
#include <utility>

namespace mitl {

/**
 * @brief Signed arbitrary-precision integer.
 *
 * Representation:
 * - sign_ is false for non-negative and true for negative values.
 * - limbs_ stores little-endian limbs in base 10^9.
 */
class BigInt
{
public:
	static constexpr std::uint32_t BASE      = 1000000000u; // 10^9
	static constexpr unsigned      BASE_DIGS = 9;           // digits per limb

	/** @brief Constructs zero. */
	BigInt() = default;

	/** @brief Constructs from a signed 64-bit integer. */
	BigInt(std::int64_t value);

	/**
	 * @brief Constructs from a decimal string.
	 * @param decimal Strict decimal text in the form `[+|-]?[0-9]+` (for example "-123456").
	 * @throws std::invalid_argument If the text is empty or contains invalid characters.
	 */
	explicit BigInt(const std::string& decimal);

	/** @brief Returns zero. */
	static BigInt zero();
	/** @brief Returns one. */
	static BigInt one();

	/** @brief Returns true if the value is zero. */
	bool isZero() const noexcept { return limbs_.empty(); }
	/** @brief Returns true if the value is strictly negative. */
	bool isNegative() const noexcept { return sign_ && !isZero(); }

	/** @brief Compares two values and returns -1, 0, or 1. */
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

	/** @brief Returns arithmetic negation. */
	BigInt operator-() const;

	/** @brief Adds another BigInt to this value. */
	BigInt& operator+=(const BigInt& other);
	/** @brief Subtracts another BigInt from this value. */
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

	/**
	 * @brief Multiplies by a small non-negative integer.
	 * @param k Factor where k < BASE.
	 */
	BigInt& mulSmall(std::uint32_t k);

	/**
	 * @brief Divides by a small positive integer.
	 * @param k Divisor where 0 < k < BASE.
	 * @return Remainder of the division.
	 * @throws std::domain_error If k is zero.
	 */
	std::uint32_t divSmall(std::uint32_t k);

	/** @brief Multiplies by another BigInt. */
	BigInt& operator*=(const BigInt& other);

	friend BigInt operator*(BigInt lhs, const BigInt& rhs)
	{
		lhs *= rhs;
		return lhs;
	}

	/** @brief Divides this value by another BigInt and stores the quotient. */
	BigInt& operator/=(const BigInt& other);
	/** @brief Replaces this value with the remainder of division by another BigInt. */
	BigInt& operator%=(const BigInt& other);

	friend BigInt operator/(BigInt lhs, const BigInt& rhs)
	{
		lhs /= rhs;
		return lhs;
	}

	friend BigInt operator%(BigInt lhs, const BigInt& rhs)
	{
		lhs %= rhs;
		return lhs;
	}

	/** @brief Removes leading zero limbs and canonicalizes the sign for zero. */
	void normalize() noexcept;

	/** @brief Multiplies by 2^k. */
	BigInt& mulPow2(std::uint64_t k);

	/** @brief Multiplies by 5^k. */
	BigInt& mulPow5(std::uint64_t k);

	/**
	 * @brief Divides by another BigInt.
	 * @param divisor Non-zero divisor.
	 * @return Pair of {quotient, remainder} using truncated division semantics.
	 * @throws std::domain_error If divisor is zero.
	 */
	std::pair<BigInt, BigInt> div(const BigInt& divisor) const;

	/** @brief Returns canonical decimal text (for example "-123", "0", "456789"). */
	std::string toString() const;

	/** @brief Streams canonical decimal representation. */
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

