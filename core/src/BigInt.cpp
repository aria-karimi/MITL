#include "mitl/BigInt.hpp"

#include <algorithm>
#include <stdexcept>
#include <ostream>

namespace mitl {

BigInt::BigInt(std::int64_t value)
{
	if (value == 0) {
		sign_ = false;
		return;
	}
	if (value < 0) {
		sign_ = true;
		// careful with INT64_MIN
		std::uint64_t mag = static_cast<std::uint64_t>(-(value + 1));
		++mag;
		while (mag > 0) {
			limbs_.push_back(static_cast<std::uint32_t>(mag % BASE));
			mag /= BASE;
		}
	} else {
		sign_ = false;
		std::uint64_t mag = static_cast<std::uint64_t>(value);
		while (mag > 0) {
			limbs_.push_back(static_cast<std::uint32_t>(mag % BASE));
			mag /= BASE;
		}
	}
}

BigInt::BigInt(const std::string& decimal)
{
	if (decimal.empty()) {
		throw std::invalid_argument("BigInt: empty input");
	}

	std::size_t pos = 0;
	if (decimal[pos] == '+') ++pos;
	else if (decimal[pos] == '-') { sign_ = true; ++pos; }

	if (pos >= decimal.size()) {
		throw std::invalid_argument("BigInt: sign without digits");
	}

	if (decimal[pos] < '0' || decimal[pos] > '9') {
		throw std::invalid_argument("BigInt: invalid first digit");
	}

	bool isNeg = sign_;  // save before any operations

	// Process decimal digits left-to-right: result = result * 10 + digit
	for (; pos < decimal.size(); ++pos) {
		char c = decimal[pos];
		if (c < '0' || c > '9') {
			throw std::invalid_argument("BigInt: invalid character in input");
		}
		std::uint32_t d = static_cast<std::uint32_t>(c - '0');

		if (isZero()) {
			if (d != 0) limbs_.push_back(d);
			continue;
		}

		// Multiply by 10, preserving sign
		sign_ = isNeg;
		mulSmall(10u);
		sign_ = isNeg;

		// Add d to lowest limb with carry
		std::uint64_t sum = static_cast<std::uint64_t>(limbs_[0]) + d;
		if (sum >= BASE) {
			limbs_[0] = static_cast<std::uint32_t>(sum - BASE);
			std::size_t i = 1;
			while (i < limbs_.size()) {
				if (limbs_[i] < BASE - 1) {
					limbs_[i]++;
					break;
				}
				limbs_[i] = 0;
				++i;
			}
			if (i >= limbs_.size()) limbs_.push_back(1);
		} else {
			limbs_[0] = static_cast<std::uint32_t>(sum);
		}
	}

	normalize();
	if (isZero()) sign_ = false;
	else sign_ = isNeg;
}

BigInt BigInt::zero()
{
	return BigInt{};
}

BigInt BigInt::one()
{
	return BigInt{1};
}

int BigInt::cmpAbs(const BigInt& a, const BigInt& b) noexcept
{
	if (a.limbs_.size() != b.limbs_.size()) {
		return (a.limbs_.size() < b.limbs_.size()) ? -1 : 1;
	}
	for (std::size_t i = a.limbs_.size(); i-- > 0;) {
		if (a.limbs_[i] != b.limbs_[i]) {
			return (a.limbs_[i] < b.limbs_[i]) ? -1 : 1;
		}
	}
	return 0;
}

BigInt BigInt::addAbs(const BigInt& a, const BigInt& b)
{
	BigInt res;
	const std::size_t n = std::max(a.limbs_.size(), b.limbs_.size());
	res.limbs_.resize(n);
	std::uint64_t carry = 0;
	for (std::size_t i = 0; i < n; ++i) {
		std::uint64_t av = (i < a.limbs_.size() ? a.limbs_[i] : 0);
		std::uint64_t bv = (i < b.limbs_.size() ? b.limbs_[i] : 0);
		std::uint64_t sum = av + bv + carry;
		res.limbs_[i] = static_cast<std::uint32_t>(sum % BASE);
		carry = sum / BASE;
	}
	if (carry) {
		res.limbs_.push_back(static_cast<std::uint32_t>(carry));
	}
	res.sign_ = false;
	res.normalize();
	return res;
}

BigInt BigInt::subAbs(const BigInt& a, const BigInt& b)
{
	BigInt res;
	res.limbs_.resize(a.limbs_.size());
	std::int64_t carry = 0;
	for (std::size_t i = 0; i < a.limbs_.size(); ++i) {
		std::int64_t av = static_cast<std::int64_t>(a.limbs_[i]);
		std::int64_t bv = (i < b.limbs_.size() ? static_cast<std::int64_t>(b.limbs_[i]) : 0);
		std::int64_t diff = av - bv + carry;
		if (diff < 0) {
			diff += static_cast<std::int64_t>(BASE);
			carry = -1;
		} else {
			carry = 0;
		}
		res.limbs_[i] = static_cast<std::uint32_t>(diff);
	}
	res.sign_ = false;
	res.normalize();
	return res;
}

int compare(const BigInt& a, const BigInt& b) noexcept
{
	if (a.isZero() && b.isZero()) {
		return 0;
	}
	if (a.isNegative() != b.isNegative()) {
		return a.isNegative() ? -1 : 1;
	}
	int cmp = BigInt::cmpAbs(a, b);
	return a.isNegative() ? -cmp : cmp;
}

BigInt BigInt::operator-() const
{
	BigInt tmp(*this);
	if (!tmp.isZero()) {
		tmp.sign_ = !tmp.sign_;
	}
	return tmp;
}

BigInt& BigInt::operator+=(const BigInt& other)
{
	if (other.isZero()) {
		return *this;
	}
	if (isZero()) {
		*this = other;
		return *this;
	}

	if (sign_ == other.sign_) {
		*this = addAbs(*this, other);
		sign_ = other.sign_;
	} else {
		int cmp = cmpAbs(*this, other);
		if (cmp == 0) {
			limbs_.clear();
			sign_ = false;
		} else if (cmp > 0) {
			*this = subAbs(*this, other);
			// sign_ stays as this->sign_
		} else {
			*this = subAbs(other, *this);
			sign_ = other.sign_;
		}
	}
	normalize();
	return *this;
}

BigInt& BigInt::operator-=(const BigInt& other)
{
	*this += -other;
	return *this;
}

BigInt& BigInt::mulSmall(std::uint32_t k)
{
	if (k == 0 || isZero()) {
		limbs_.clear();
		sign_ = false;
		return *this;
	}
	std::uint64_t carry = 0;
	for (std::size_t i = 0; i < limbs_.size(); ++i) {
		std::uint64_t prod = static_cast<std::uint64_t>(limbs_[i]) * k + carry;
		limbs_[i] = static_cast<std::uint32_t>(prod % BASE);
		carry = prod / BASE;
	}
	if (carry) {
		limbs_.push_back(static_cast<std::uint32_t>(carry));
	}
	return *this;
}

std::uint32_t BigInt::divSmall(std::uint32_t k)
{
	if (k == 0) {
		throw std::domain_error("BigInt::divSmall division by zero");
	}

	std::uint64_t rem = 0;
	for (std::size_t i = limbs_.size(); i-- > 0;) {
		std::uint64_t cur = limbs_[i] + rem * BASE;
		limbs_[i] = static_cast<std::uint32_t>(cur / k);
		rem = cur % k;
	}
	normalize();
	return static_cast<std::uint32_t>(rem);
}

BigInt& BigInt::operator*=(const BigInt& other)
{
	if (isZero() || other.isZero()) {
		limbs_.clear();
		sign_ = false;
		return *this;
	}
	BigInt res;
	res.limbs_.assign(limbs_.size() + other.limbs_.size(), 0);
	for (std::size_t i = 0; i < limbs_.size(); ++i) {
		std::uint64_t carry = 0;
		std::uint64_t av = limbs_[i];
		for (std::size_t j = 0; j < other.limbs_.size() || carry; ++j) {
			std::uint64_t bv = (j < other.limbs_.size() ? other.limbs_[j] : 0);
			std::uint64_t cur = res.limbs_[i + j] + av * bv + carry;
			res.limbs_[i + j] = static_cast<std::uint32_t>(cur % BASE);
			carry = cur / BASE;
		}
	}
	res.sign_ = (sign_ != other.sign_);
	res.normalize();
	*this = res;
	return *this;
}

BigInt& BigInt::operator/=(const BigInt& other)
{
	*this = div(other).first;
	return *this;
}

BigInt& BigInt::operator%=(const BigInt& other)
{
	*this = div(other).second;
	return *this;
}

BigInt& BigInt::mulPow2(std::uint64_t k)
{
	if (isZero() || k == 0) {
		return *this;
	}
	BigInt base(2);
	BigInt acc(1);
	while (k > 0) {
		if (k & 1) {
			acc *= base;
		}
		k >>= 1;
		if (k) {
			base *= base;
		}
	}
	*this *= acc;
	return *this;
}

BigInt& BigInt::mulPow5(std::uint64_t k)
{
	if (isZero() || k == 0) {
		return *this;
	}
	BigInt base(5);
	BigInt acc(1);
	while (k > 0) {
		if (k & 1) {
			acc *= base;
		}
		k >>= 1;
		if (k) {
			base *= base;
		}
	}
	*this *= acc;
	return *this;
}

void BigInt::normalize() noexcept
{
	while (!limbs_.empty() && limbs_.back() == 0) {
		limbs_.pop_back();
	}
	if (limbs_.empty()) {
		sign_ = false;
	}
}

std::pair<BigInt, BigInt> BigInt::div(const BigInt& divisor) const
{
	if (divisor.isZero()) {
		throw std::domain_error("BigInt::div division by zero");
	}

	if (isZero()) {
		return { BigInt(0), BigInt(0) };
	}

	BigInt absDividend(*this);
	absDividend.sign_ = false;
	BigInt absDivisor(divisor);
	absDivisor.sign_ = false;

	if (cmpAbs(absDividend, absDivisor) < 0) {
		BigInt q(0);
		BigInt r(*this);
		r.sign_ = sign_ && !r.isZero();
		return { std::move(q), std::move(r) };
	}

	BigInt quotient(0);
	BigInt remainder(absDividend);

	while (cmpAbs(remainder, absDivisor) >= 0) {
		BigInt scaledDivisor(absDivisor);
		BigInt scaledQuotient(1);

		while (true) {
			BigInt doubledDivisor = scaledDivisor + scaledDivisor;
			if (cmpAbs(doubledDivisor, remainder) > 0) {
				break;
			}
			scaledDivisor = std::move(doubledDivisor);
			scaledQuotient = scaledQuotient + scaledQuotient;
		}

		remainder = subAbs(remainder, scaledDivisor);
		quotient += scaledQuotient;
	}

	quotient.sign_ = (sign_ != divisor.sign_) && !quotient.isZero();
	remainder.sign_ = sign_ && !remainder.isZero();
	return { std::move(quotient), std::move(remainder) };
}

std::string BigInt::toString() const
{
	if (isZero()) {
		return "0";
	}

	BigInt tmp(*this);
	tmp.sign_ = false; // work with magnitude only
	std::string out;
	out.reserve(tmp.limbs_.size() * BASE_DIGS);

	while (!tmp.isZero()) {
		std::uint32_t rem = tmp.divSmall(10u);
		out.push_back(static_cast<char>('0' + rem));
	}
	std::reverse(out.begin(), out.end());
	if (sign_) {
		out.insert(out.begin(), '-');
	}
	return out;
}

std::ostream& operator<<(std::ostream& os, const BigInt& value)
{
	return os << value.toString();
}

} // namespace mitl