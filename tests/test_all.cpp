/**
 * @file test_all.cpp
 * @brief Unit tests for mitl::BigInt and mitl::Real
 */

#include "mitl/BigInt.hpp"
#include "mitl/Complex.hpp"
#include "mitl/Real.hpp"
#include "mitl/Exceptions.hpp"

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <limits>

// ---------- Mini test framework ----------
static int g_passes = 0;
static int g_fails = 0;

static double pos_inf() {
    return std::numeric_limits<double>::infinity();
}

static double neg_inf() {
    return -std::numeric_limits<double>::infinity();
}

static double quiet_nan() {
    return std::numeric_limits<double>::quiet_NaN();
}

#define ASSERT_EQ(a, b) do { \
    if (!((a) == (b))) { \
        std::ostringstream oss_; \
        oss_ << "ASSERT " #a " == " #b " failed: got " << (a) << " vs " << (b); \
        throw std::runtime_error(oss_.str()); \
    } } while(0)

#define ASSERT_EQ_STR(a, b) do { \
    std::string sa_ = (a), sb_ = (b); \
    if (sa_ != sb_) { \
        std::ostringstream oss_; \
        oss_ << "ASSERT " #a " == " #b " failed: got \"" << sa_ << "\" vs \"" << sb_ << "\""; \
        throw std::runtime_error(oss_.str()); \
    } } while(0)

#define ASSERT_THROW(expr, exc) do { \
    bool caught = false; \
    try { expr; } catch (const exc&) { caught = true; } \
    if (!caught) throw std::runtime_error("Expected " #exc " not thrown"); \
} while(0)

static void run_test(const char* name, void (*fn)()) {
    try { fn(); ++g_passes; std::cout << "  [PASS] " << name << "\n"; }
    catch (const std::exception& e) {
        ++g_fails; std::cout << "  [FAIL] " << name << ": " << e.what() << "\n";
    }
}

#define TEST(name) static void name()
#define RUN(name) run_test(#name, name)

// =============================================================================
// BigInt Tests
// =============================================================================

TEST(bigint_zero) {
    mitl::BigInt b;
    ASSERT_EQ(b.isZero(), true);
    ASSERT_EQ_STR(b.toString(), "0");
}

TEST(bigint_small_positive) {
    mitl::BigInt b(42);
    ASSERT_EQ_STR(b.toString(), "42");
}

TEST(bigint_small_negative) {
    mitl::BigInt b(-99);
    ASSERT_EQ_STR(b.toString(), "-99");
}

TEST(bigint_large_positive) {
    mitl::BigInt b(123456789012345678LL);
    ASSERT_EQ_STR(b.toString(), "123456789012345678");
}

TEST(bigint_from_string_positive) {
    mitl::BigInt b("123456789012345678901234567890");
    ASSERT_EQ_STR(b.toString(), "123456789012345678901234567890");
}

TEST(bigint_from_string_negative) {
    mitl::BigInt b("-123456789012345678901234567890");
    ASSERT_EQ_STR(b.toString(), "-123456789012345678901234567890");
}

TEST(bigint_from_string_plus_sign) {
    mitl::BigInt b("+12345");
    ASSERT_EQ_STR(b.toString(), "12345");
}

TEST(bigint_from_string_invalid_throws) {
    ASSERT_THROW(mitl::BigInt(""), std::invalid_argument);
    ASSERT_THROW(mitl::BigInt("-"), std::invalid_argument);
    ASSERT_THROW(mitl::BigInt("12a3"), std::invalid_argument);
    ASSERT_THROW(mitl::BigInt("+ 123"), std::invalid_argument);
}

TEST(bigint_add_same_sign) {
    mitl::BigInt a(100);
    mitl::BigInt b(200);
    mitl::BigInt c = a + b;
    ASSERT_EQ_STR(c.toString(), "300");
}

TEST(bigint_add_opposite_signs) {
    mitl::BigInt a(100);
    mitl::BigInt b(-30);
    mitl::BigInt c = a + b;
    ASSERT_EQ_STR(c.toString(), "70");
}

TEST(bigint_subtract_same_sign) {
    mitl::BigInt a(200);
    mitl::BigInt b(100);
    mitl::BigInt c = a - b;
    ASSERT_EQ_STR(c.toString(), "100");
}

TEST(bigint_subtract_negative_result) {
    mitl::BigInt a(50);
    mitl::BigInt b(100);
    mitl::BigInt c = a - b;
    ASSERT_EQ_STR(c.toString(), "-50");
}

TEST(bigint_subtract_equal) {
    mitl::BigInt a(42);
    mitl::BigInt b(42);
    mitl::BigInt c = a - b;
    ASSERT_EQ(c.isZero(), true);
}

TEST(bigint_multiply_small) {
    mitl::BigInt a(12345);
    mitl::BigInt b(67890);
    mitl::BigInt c = a * b;
    ASSERT_EQ_STR(c.toString(), "838102050");
}

TEST(bigint_multiply_large) {
    mitl::BigInt a("12345678901234567890");
    mitl::BigInt b("98765432109876543210");
    mitl::BigInt c = a * b;
    ASSERT_EQ_STR(c.toString(), "1219326311370217952237463801111263526900");
}

TEST(bigint_multiply_by_zero) {
    mitl::BigInt a(12345);
    mitl::BigInt b(0);
    mitl::BigInt c = a * b;
    ASSERT_EQ(c.isZero(), true);
}

TEST(bigint_mulPow2) {
    mitl::BigInt a(1);
    a.mulPow2(10);
    ASSERT_EQ_STR(a.toString(), "1024");
}

TEST(bigint_mulPow5) {
    mitl::BigInt a(1);
    a.mulPow5(5);
    ASSERT_EQ_STR(a.toString(), "3125");
}

TEST(bigint_unary_negate) {
    mitl::BigInt a(42);
    mitl::BigInt b = -a;
    ASSERT_EQ_STR(b.toString(), "-42");
}

TEST(bigint_comparison) {
    mitl::BigInt a(10);
    mitl::BigInt b(20);
    ASSERT_EQ(a < b, true);
    ASSERT_EQ(a == a, true);
    ASSERT_EQ(a != b, true);
}

TEST(bigint_negative_comparison) {
    mitl::BigInt a(-100);
    mitl::BigInt b(-50);
    ASSERT_EQ(a < b, true);
}

TEST(bigint_div_small_even) {
    mitl::BigInt a(100);
    auto [q, r] = a.div(mitl::BigInt(10));
    ASSERT_EQ_STR(q.toString(), "10");
    ASSERT_EQ_STR(r.toString(), "0");
}

TEST(bigint_div_with_remainder) {
    mitl::BigInt a(17);
    auto [q, r] = a.div(mitl::BigInt(5));
    ASSERT_EQ_STR(q.toString(), "3");
    ASSERT_EQ_STR(r.toString(), "2");
}

TEST(bigint_div_negative) {
    mitl::BigInt a(-17);
    auto [q, r] = a.div(mitl::BigInt(5));
    ASSERT_EQ_STR(q.toString(), "-3");
    ASSERT_EQ_STR(r.toString(), "-2");
}

TEST(bigint_div_large) {
    mitl::BigInt a("10000000000000000000000");
    mitl::BigInt b("999999999999");
    auto [q, r] = a.div(b);
    mitl::BigInt check = b * q + r;
    ASSERT_EQ(check == a, true);
}

TEST(bigint_div_smaller) {
    mitl::BigInt a(3);
    auto [q, r] = a.div(mitl::BigInt(10));
    ASSERT_EQ(q.isZero(), true);
    ASSERT_EQ_STR(r.toString(), "3");
}

TEST(bigint_divide_by_zero_throws) {
    mitl::BigInt a(17);
    ASSERT_THROW(a.div(mitl::BigInt(0)), std::domain_error);
}

TEST(bigint_compound_div_mod) {
    mitl::BigInt a(17);
    mitl::BigInt b(5);
    mitl::BigInt q = a;
    mitl::BigInt r = a;
    q /= b;
    r %= b;
    ASSERT_EQ_STR(q.toString(), "3");
    ASSERT_EQ_STR(r.toString(), "2");
}

TEST(bigint_factory_zero_one) {
    ASSERT_EQ(mitl::BigInt::zero().isZero(), true);
    ASSERT_EQ_STR(mitl::BigInt::one().toString(), "1");
}

TEST(bigint_stream_output) {
    std::ostringstream oss;
    mitl::BigInt b(12345);
    oss << b;
    ASSERT_EQ(oss.str(), "12345");
}

// =============================================================================
// Real Tests
// =============================================================================

TEST(real_zero) {
    mitl::Real r(0.0);
    ASSERT_EQ(r.isZero(), true);
    ASSERT_EQ_STR(r.toString(), "0");
}

TEST(real_one) {
    mitl::Real r(1.0);
    ASSERT_EQ_STR(r.toString(), "1");
}

TEST(real_negative) {
    mitl::Real r(-1.0);
    ASSERT_EQ(r.isNegative(), true);
    ASSERT_EQ_STR(r.toString(), "-1");
}

TEST(real_half) {
    mitl::Real r(0.5);
    ASSERT_EQ_STR(r.toString(), "0.5");
}

TEST(real_one_eighth) {
    mitl::Real r(0.125);
    ASSERT_EQ_STR(r.toString(), "0.125");
}

TEST(real_large_number) {
    mitl::Real r(1e10);
    ASSERT_EQ_STR(r.toString(), "10000000000");
}

TEST(real_infinity) {
    mitl::Real r(pos_inf());
    ASSERT_EQ(r.isInfinity(), true);
    ASSERT_EQ(r.isPositiveInfinity(), true);
    ASSERT_EQ_STR(r.toString(), "Infinity");
}

TEST(real_negative_infinity) {
    mitl::Real r(neg_inf());
    ASSERT_EQ(r.isNegativeInfinity(), true);
    ASSERT_EQ_STR(r.toString(), "-Infinity");
}

TEST(real_nan) {
    mitl::Real r(quiet_nan());
    ASSERT_EQ(r.isNaN(), true);
    ASSERT_EQ_STR(r.toString(), "NaN");
}

TEST(real_copy_constructor) {
    mitl::Real a(42.5);
    mitl::Real b(a);
    ASSERT_EQ_STR(b.toString(), "42.5");
}

TEST(real_move_constructor) {
    mitl::Real a(42.5);
    mitl::Real b(std::move(a));
    ASSERT_EQ_STR(b.toString(), "42.5");
}

TEST(real_copy_assignment) {
    mitl::Real a(99.5);
    mitl::Real b;
    b = a;
    ASSERT_EQ_STR(b.toString(), "99.5");
}

TEST(real_unary_negate) {
    mitl::Real a(3.5);
    mitl::Real b = -a;
    ASSERT_EQ_STR(b.toString(), "-3.5");
}

TEST(real_add_integers) {
    mitl::Real a(3.0);
    mitl::Real b(4.0);
    mitl::Real c = a + b;
    ASSERT_EQ_STR(c.toString(), "7");
}

TEST(real_add_negative) {
    mitl::Real a(10.0);
    mitl::Real b(-3.0);
    mitl::Real c = a + b;
    ASSERT_EQ_STR(c.toString(), "7");
}

TEST(real_add_opposite_same_magnitude) {
    mitl::Real a(5.0);
    mitl::Real b(-5.0);
    mitl::Real c = a + b;
    ASSERT_EQ(c.isZero(), true);
}

TEST(real_add_fractions) {
    mitl::Real a(0.5);
    mitl::Real b(0.25);
    mitl::Real c = a + b;
    ASSERT_EQ_STR(c.toString(), "0.75");
}

TEST(real_add_zero) {
    mitl::Real a(42.0);
    mitl::Real b(0.0);
    mitl::Real c = a + b;
    ASSERT_EQ_STR(c.toString(), "42");
}

TEST(real_subtract_integers) {
    mitl::Real a(10.0);
    mitl::Real b(3.0);
    mitl::Real c = a - b;
    ASSERT_EQ_STR(c.toString(), "7");
}

TEST(real_subtract_negative_result) {
    mitl::Real a(3.0);
    mitl::Real b(10.0);
    mitl::Real c = a - b;
    ASSERT_EQ_STR(c.toString(), "-7");
}

TEST(real_subtract_neg_operand) {
    mitl::Real a(10.0);
    mitl::Real b(-3.0);
    mitl::Real c = a - b;
    ASSERT_EQ_STR(c.toString(), "13");
}

TEST(real_multiply_integers) {
    mitl::Real a(6.0);
    mitl::Real b(7.0);
    mitl::Real c = a * b;
    ASSERT_EQ_STR(c.toString(), "42");
}

TEST(real_multiply_by_zero) {
    mitl::Real a(42.0);
    mitl::Real b(0.0);
    mitl::Real c = a * b;
    ASSERT_EQ(c.isZero(), true);
}

TEST(real_multiply_negative) {
    mitl::Real a(5.0);
    mitl::Real b(-3.0);
    mitl::Real c = a * b;
    ASSERT_EQ_STR(c.toString(), "-15");
}

TEST(real_multiply_two_neg) {
    mitl::Real a(-4.0);
    mitl::Real b(-5.0);
    mitl::Real c = a * b;
    ASSERT_EQ_STR(c.toString(), "20");
}

TEST(real_multiply_fractions) {
    mitl::Real a(0.5);
    mitl::Real b(0.5);
    mitl::Real c = a * b;
    ASSERT_EQ_STR(c.toString(), "0.25");
}

TEST(real_multiply_large) {
    mitl::Real a(1000000.0);
    mitl::Real b(1000000.0);
    mitl::Real c = a * b;
    ASSERT_EQ_STR(c.toString(), "1000000000000");
}

TEST(real_division_exact) {
    mitl::Real a(10.0);
    mitl::Real b(2.0);
    mitl::Real c = a / b;
    ASSERT_EQ_STR(c.toString(), "5");
}

TEST(real_division_fractional) {
    mitl::Real a(1.0);
    mitl::Real b(8.0);
    mitl::Real c = a / b;
    ASSERT_EQ_STR(c.toString(), "0.125");
}

TEST(real_division_negative) {
    mitl::Real a(10.0);
    mitl::Real b(-2.0);
    mitl::Real c = a / b;
    ASSERT_EQ_STR(c.toString(), "-5");
}

TEST(real_division_by_one) {
    mitl::Real a(42.5);
    mitl::Real b(1.0);
    mitl::Real c = a / b;
    ASSERT_EQ_STR(c.toString(), "42.5");
}

TEST(real_mod_same_value) {
    mitl::Real a(10.0);
    mitl::Real b(10.0);
    mitl::Real c = a % b;
    ASSERT_EQ(c.isZero(), true);
}

TEST(real_mod_small) {
    mitl::Real a(17.0);
    mitl::Real b(5.0);
    mitl::Real c = a % b;
    ASSERT_EQ_STR(c.toString(), "2");
}

TEST(real_add_infinity) {
    mitl::Real inf_pos(pos_inf());
    mitl::Real a(42.0);
    mitl::Real c = a + inf_pos;
    ASSERT_EQ(c.isPositiveInfinity(), true);
}

TEST(real_inf_plus_inf_same) {
    mitl::Real inf1(pos_inf());
    mitl::Real inf2(pos_inf());
    mitl::Real c = inf1 + inf2;
    ASSERT_EQ(c.isPositiveInfinity(), true);
}

TEST(real_inf_plus_inf_opp_nan) {
    mitl::Real posInf(pos_inf());
    mitl::Real negInf(neg_inf());
    mitl::Real c = posInf + negInf;
    ASSERT_EQ(c.isNaN(), true);
}

TEST(real_nan_plus_anything) {
    mitl::Real nan(quiet_nan());
    mitl::Real a(42.0);
    mitl::Real c = nan + a;
    ASSERT_EQ(c.isNaN(), true);
}

TEST(real_mult_inf_nonzero) {
    mitl::Real inf(pos_inf());
    mitl::Real a(5.0);
    mitl::Real c = a * inf;
    ASSERT_EQ(c.isPositiveInfinity(), true);
}

TEST(real_mult_inf_negative) {
    mitl::Real inf(pos_inf());
    mitl::Real a(-5.0);
    mitl::Real c = a * inf;
    ASSERT_EQ(c.isNegativeInfinity(), true);
}

TEST(real_zero_times_inf_nan) {
    mitl::Real zero(0.0);
    mitl::Real inf(pos_inf());
    mitl::Real c = zero * inf;
    ASSERT_EQ(c.isNaN(), true);
}

TEST(real_inf_minus_inf_nan) {
    mitl::Real posInf(pos_inf());
    mitl::Real d = posInf - posInf;
    ASSERT_EQ(d.isNaN(), true);
}

TEST(real_mod_by_inf) {
    mitl::Real a(10.0);
    mitl::Real inf(pos_inf());
    mitl::Real c = a % inf;
    ASSERT_EQ_STR(c.toString(), "10");
}

TEST(real_mod_inf_nan) {
    mitl::Real inf(pos_inf());
    mitl::Real b(3.0);
    mitl::Real c = inf % b;
    ASSERT_EQ(c.isNaN(), true);
}

TEST(real_compound_add) {
    mitl::Real a(10.0);
    mitl::Real b(5.0);
    a += b;
    ASSERT_EQ_STR(a.toString(), "15");
}

TEST(real_compound_mul) {
    mitl::Real a(3.0);
    mitl::Real b(4.0);
    a *= b;
    ASSERT_EQ_STR(a.toString(), "12");
}

TEST(real_compound_div) {
    mitl::Real a(20.0);
    mitl::Real b(4.0);
    a /= b;
    ASSERT_EQ_STR(a.toString(), "5");
}

TEST(real_divideBy_precision) {
    mitl::Real a(1.0);
    mitl::Real b(3.0);
    mitl::Real c = a.divideBy(b, 64);
    ASSERT_EQ(c.isPositive(), true);
    std::string s = c.toString();
    ASSERT_EQ(s.substr(0, 5) == "0.333", true);
}

TEST(real_divideBy_decimal_precision_budget) {
    mitl::Real a(1.0);
    mitl::Real b(7.0);
    mitl::Real p2 = a.divideBy(b, 2);
    mitl::Real p6 = a.divideBy(b, 6);
    ASSERT_EQ(std::fabs(static_cast<double>(p2) - 0.14) < 1e-12, true);
    ASSERT_EQ(std::fabs(static_cast<double>(p6) - 0.142857) < 1e-12, true);
}

TEST(real_int64_conversion) {
    mitl::Real r(42.0);
    ASSERT_EQ(static_cast<std::int64_t>(r), 42);
}

TEST(real_double_conversion) {
    mitl::Real r(0.5);
    double d = static_cast<double>(r);
    ASSERT_EQ(d > 0.49 && d < 0.51, true);
}

TEST(real_is_integer_public_query) {
    mitl::Real i1(42.0);
    mitl::Real i2(-7.0);
    mitl::Real f(3.125);
    ASSERT_EQ(i1.isInteger(), true);
    ASSERT_EQ(i2.isInteger(), true);
    ASSERT_EQ(f.isInteger(), false);
}

TEST(real_comparison_operators) {
    mitl::Real a(2.0);
    mitl::Real b(3.0);
    mitl::Real c(2.0);
    ASSERT_EQ(a == c, true);
    ASSERT_EQ(a != b, true);
    ASSERT_EQ(a < b, true);
    ASSERT_EQ(b > a, true);
    ASSERT_EQ(a <= c, true);
    ASSERT_EQ(b >= a, true);
}

TEST(real_comparison_nan_ieee) {
    mitl::Real nan(quiet_nan());
    mitl::Real x(1.0);
    ASSERT_EQ(nan == nan, false);
    ASSERT_EQ(nan != nan, true);
    ASSERT_EQ(nan < x, false);
    ASSERT_EQ(x < nan, false);
    ASSERT_EQ(nan <= x, false);
    ASSERT_EQ(nan >= x, false);
}

TEST(real_subtract_from_zero) {
    mitl::Real a(0.0);
    mitl::Real b(5.0);
    mitl::Real c = a - b;
    ASSERT_EQ_STR(c.toString(), "-5");
}

TEST(real_division_throws_zero) {
    mitl::Real a(10.0);
    mitl::Real b(0.0);
    ASSERT_THROW(a / b, std::domain_error);
}

TEST(real_zero_int64_conversion) {
    mitl::Real z(0.0);
    ASSERT_EQ(static_cast<std::int64_t>(z), 0);
}

TEST(real_compound_chain) {
    mitl::Real r(1.0);
    r += mitl::Real(2.0);  // 3
    r *= mitl::Real(4.0);  // 12
    r -= mitl::Real(2.0);  // 10
    r /= mitl::Real(2.0);  // 5
    ASSERT_EQ_STR(r.toString(), "5");
}

TEST(real_stream_output) {
    std::ostringstream oss;
    mitl::Real r(3.125);
    oss << r;
    ASSERT_EQ(oss.str(), "3.125");
}

// =============================================================================
// Complex Tests
// =============================================================================

TEST(complex_default_zero) {
    mitl::Complex c;
    ASSERT_EQ(c.isZero(), true);
    ASSERT_EQ_STR(c.toString(), "0+0i");
}

TEST(complex_basic_construction) {
    mitl::Complex c(3.0, 4.0);
    ASSERT_EQ_STR(c.toString(), "3+4i");
}

TEST(complex_add_subtract) {
    mitl::Complex a(1.0, 2.0);
    mitl::Complex b(3.0, -4.0);
    mitl::Complex s = a + b;
    mitl::Complex d = a - b;
    ASSERT_EQ_STR(s.toString(), "4-2i");
    ASSERT_EQ_STR(d.toString(), "-2+6i");
}

TEST(complex_multiply) {
    mitl::Complex a(1.0, 2.0);
    mitl::Complex b(3.0, 4.0);
    mitl::Complex p = a * b;
    ASSERT_EQ(std::fabs(static_cast<double>(p.real()) + 5.0) < 1e-9, true);
    ASSERT_EQ(std::fabs(static_cast<double>(p.imag()) - 10.0) < 1e-9, true);
}

TEST(complex_divide) {
    mitl::Complex a(1.0, 2.0);
    mitl::Complex b(3.0, -4.0);
    mitl::Complex q = a / b;
    ASSERT_EQ(std::fabs(static_cast<double>(q.real()) + 0.2) < 1e-9, true);
    ASSERT_EQ(std::fabs(static_cast<double>(q.imag()) - 0.4) < 1e-9, true);
}

TEST(complex_conjugate) {
    mitl::Complex c(6.0, -7.0);
    mitl::Complex cc = c.conjugate();
    ASSERT_EQ_STR(cc.toString(), "6+7i");
}

TEST(complex_norm_abs_phase) {
    mitl::Complex c(3.0, 4.0);
    ASSERT_EQ(std::fabs(static_cast<double>(c.normSquared()) - 25.0) < 1e-9, true);
    ASSERT_EQ(std::fabs(static_cast<double>(c.abs()) - 5.0) < 1e-9, true);
    ASSERT_EQ(std::fabs(static_cast<double>(c.phase()) - std::atan2(4.0, 3.0)) < 1e-9, true);
}

TEST(complex_reciprocal) {
    mitl::Complex c(1.0, 1.0);
    mitl::Complex r = c.reciprocal();
    ASSERT_EQ(std::fabs(static_cast<double>(r.real()) - 0.5) < 1e-9, true);
    ASSERT_EQ(std::fabs(static_cast<double>(r.imag()) + 0.5) < 1e-9, true);
}

TEST(complex_from_polar) {
    mitl::Complex c = mitl::Complex::fromPolar(2.0, std::acos(-1.0) / 2.0);
    ASSERT_EQ(std::fabs(static_cast<double>(c.real())) < 1e-9, true);
    ASSERT_EQ(std::fabs(static_cast<double>(c.imag()) - 2.0) < 1e-9, true);
}

TEST(complex_divide_by_zero_throws) {
    mitl::Complex a(1.0, 2.0);
    mitl::Complex z(0.0, 0.0);
    ASSERT_THROW(a / z, std::domain_error);
}

TEST(complex_stream_output) {
    std::ostringstream oss;
    mitl::Complex c(1.5, -2.25);
    oss << c;
    ASSERT_EQ_STR(oss.str(), "1.5-2.25i");
}

TEST(complex_scalar_overloads_real_double) {
    mitl::Complex c(1.0, 2.0);
    mitl::Complex a = c + 2.0;
    mitl::Complex b = 2.0 + c;
    mitl::Complex m = c * mitl::Real(2.0);
    mitl::Complex d = c / 2.0;
    ASSERT_EQ_STR(a.toString(), "3+2i");
    ASSERT_EQ_STR(b.toString(), "3+2i");
    ASSERT_EQ_STR(m.toString(), "2+4i");
    ASSERT_EQ_STR(d.toString(), "0.5+1i");
}

TEST(complex_parse_valid) {
    mitl::Complex c1 = mitl::Complex::parse("3+4i");
    mitl::Complex c2 = mitl::Complex::parse("-2.5-7.25i");
    ASSERT_EQ_STR(c1.toString(), "3+4i");
    ASSERT_EQ_STR(c2.toString(), "-2.5-7.25i");
}

TEST(complex_parse_invalid_throws) {
    ASSERT_THROW(mitl::Complex::parse("3"), std::invalid_argument);
    ASSERT_THROW(mitl::Complex::parse("4i"), std::invalid_argument);
    ASSERT_THROW(mitl::Complex::parse("(3,4)"), std::invalid_argument);
}

// =============================================================================
// Main
// =============================================================================

int main() {
    std::cout << "\n=== BigInt Tests ===\n";
    RUN(bigint_zero);
    RUN(bigint_small_positive);
    RUN(bigint_small_negative);
    RUN(bigint_large_positive);
    RUN(bigint_from_string_positive);
    RUN(bigint_from_string_negative);
    RUN(bigint_from_string_plus_sign);
    RUN(bigint_from_string_invalid_throws);
    RUN(bigint_add_same_sign);
    RUN(bigint_add_opposite_signs);
    RUN(bigint_subtract_same_sign);
    RUN(bigint_subtract_negative_result);
    RUN(bigint_subtract_equal);
    RUN(bigint_multiply_small);
    RUN(bigint_multiply_large);
    RUN(bigint_multiply_by_zero);
    RUN(bigint_mulPow2);
    RUN(bigint_mulPow5);
    RUN(bigint_unary_negate);
    RUN(bigint_comparison);
    RUN(bigint_negative_comparison);
    RUN(bigint_div_small_even);
    RUN(bigint_div_with_remainder);
    RUN(bigint_div_negative);
    RUN(bigint_div_large);
    RUN(bigint_div_smaller);
    RUN(bigint_divide_by_zero_throws);
    RUN(bigint_compound_div_mod);
    RUN(bigint_factory_zero_one);
    RUN(bigint_stream_output);

    std::cout << "\n=== Real Tests ===\n";
    RUN(real_zero);
    RUN(real_one);
    RUN(real_negative);
    RUN(real_half);
    RUN(real_one_eighth);
    RUN(real_large_number);
    RUN(real_infinity);
    RUN(real_negative_infinity);
    RUN(real_nan);
    RUN(real_copy_constructor);
    RUN(real_move_constructor);
    RUN(real_copy_assignment);
    RUN(real_unary_negate);
    RUN(real_add_integers);
    RUN(real_add_negative);
    RUN(real_add_opposite_same_magnitude);
    RUN(real_add_fractions);
    RUN(real_add_zero);
    RUN(real_subtract_integers);
    RUN(real_subtract_negative_result);
    RUN(real_subtract_neg_operand);
    RUN(real_multiply_integers);
    RUN(real_multiply_by_zero);
    RUN(real_multiply_negative);
    RUN(real_multiply_two_neg);
    RUN(real_multiply_fractions);
    RUN(real_multiply_large);
    RUN(real_division_exact);
    RUN(real_division_fractional);
    RUN(real_division_negative);
    RUN(real_division_by_one);
    RUN(real_mod_same_value);
    RUN(real_mod_small);
    RUN(real_add_infinity);
    RUN(real_inf_plus_inf_same);
    RUN(real_inf_plus_inf_opp_nan);
    RUN(real_nan_plus_anything);
    RUN(real_mult_inf_nonzero);
    RUN(real_mult_inf_negative);
    RUN(real_zero_times_inf_nan);
    RUN(real_inf_minus_inf_nan);
    RUN(real_mod_by_inf);
    RUN(real_mod_inf_nan);
    RUN(real_compound_add);
    RUN(real_compound_mul);
    RUN(real_compound_div);
    RUN(real_divideBy_precision);
    RUN(real_divideBy_decimal_precision_budget);
    RUN(real_int64_conversion);
    RUN(real_double_conversion);
    RUN(real_is_integer_public_query);
    RUN(real_comparison_operators);
    RUN(real_comparison_nan_ieee);
    RUN(real_subtract_from_zero);
    RUN(real_division_throws_zero);
    RUN(real_zero_int64_conversion);
    RUN(real_compound_chain);
    RUN(real_stream_output);

    std::cout << "\n=== Complex Tests ===\n";
    RUN(complex_default_zero);
    RUN(complex_basic_construction);
    RUN(complex_add_subtract);
    RUN(complex_multiply);
    RUN(complex_divide);
    RUN(complex_conjugate);
    RUN(complex_norm_abs_phase);
    RUN(complex_reciprocal);
    RUN(complex_from_polar);
    RUN(complex_divide_by_zero_throws);
    RUN(complex_stream_output);
    RUN(complex_scalar_overloads_real_double);
    RUN(complex_parse_valid);
    RUN(complex_parse_invalid_throws);

    std::cout << "\n========================================\n";
    std::cout << "Results: " << g_passes << " passed, " << g_fails << " failed\n";
    std::cout << "========================================\n";
    return (g_fails == 0) ? 0 : 1;
}
