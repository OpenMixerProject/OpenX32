#include "test_framework.h"
#include "fastApproxMath.h"

#include <cmath>
#include <cstdint>
#include <limits>

TEST_CASE(fastApproxMath_powf_basic_and_asymmetry) {
    // Independent reference: standard math powf for baseline check
    // fastApproxMath_powf is Leitner-Ankerl approximation:
    // u.i = (int)(b * (u.i - 1064866816) + 1064866816);
    // 2^3 = 8
    float r1 = fastApproxMath_powf(2.0f, 3.0f);
    ASSERT_NEAR(r1, 8.0f, 1.5f); // Approximation error margin

    // 4^0.5 = 2
    float r2 = fastApproxMath_powf(4.0f, 0.5f);
    ASSERT_NEAR(r2, 2.0f, 0.3f);

    // 10^2 = 100
    float r3 = fastApproxMath_powf(10.0f, 2.0f);
    ASSERT_NEAR(r3, 100.0f, 15.0f);

    // a^0: (int)(0 * (u.i - 1064866816) + 1064866816) = 1064866816 -> float ~0.95367f
    float r4 = fastApproxMath_powf(5.0f, 0.0f);
    union { float f; int32_t i; } u_zero_exp;
    u_zero_exp.i = 1064866816;
    ASSERT_EQ(r4, u_zero_exp.f);

    // 1^5: u.i for 1.0f is 1065353216; offset is 486400; 5 * 486400 + 1064866816 = 1067298816 -> ~1.2319f
    float r5 = fastApproxMath_powf(1.0f, 5.0f);
    union { float f; int32_t i; } u_one_base;
    u_one_base.i = (int32_t)(5.0f * (1065353216 - 1064866816) + 1064866816);
    ASSERT_EQ(r5, u_one_base.f);

    // Asymmetric tests: negative exponents (decay)
    float r_neg_exp = fastApproxMath_powf(2.0f, -2.0f); // ~0.25
    ASSERT_NEAR(r_neg_exp, 0.25f, 0.08f);

    // Negative base input: floating point bit interpretation treats sign bit as integer
    // Verifying it does not crash or throw, but produces finite / predictable bit representation
    float r_neg_base = fastApproxMath_powf(-2.0f, 2.0f);
    ASSERT_TRUE(std::isfinite(r_neg_base) || std::isnan(r_neg_base));

    // Independent calculation directly from the algorithm's integer arithmetic:
    union { float f; int32_t i; } u_indep = { 3.0f };
    u_indep.i = (int32_t)(2.0f * (u_indep.i - 1064866816) + 1064866816);
    float actual_3_2 = fastApproxMath_powf(3.0f, 2.0f);
    ASSERT_EQ(actual_3_2, u_indep.f);
}

TEST_CASE(fastApproxMath_invsqrtf_domains_and_boundaries) {
    const float tol_percent = 0.03f; // Carmack invsqrt is within ~1.75%

    // Typical positive values
    float tests[] = { 1.0f, 2.0f, 4.0f, 9.0f, 16.0f, 100.0f, 0.25f, 0.01f };
    for (float v : tests) {
        float expected = 1.0f / std::sqrt(v);
        float actual = fastApproxMath_invsqrtf(v);
        float err = std::fabs(actual - expected) / expected;
        ASSERT_LE(err, tol_percent);
    }

    // Boundary: 0.0f
    // Carmack bit manipulation on 0.0f:
    // i = 0x5f3759df - 0 = 0x5f3759df
    // produces a huge finite float, then 1 iteration of Newton-Raphson
    float r_zero = fastApproxMath_invsqrtf(0.0f);
    ASSERT_TRUE(std::isfinite(r_zero));
    ASSERT_GT(r_zero, 1e18f); // Very large number

    // Boundary: Negative input
    // With negative input, x2 is negative, leading to overflow to +inf in Newton-Raphson step
    float r_neg = fastApproxMath_invsqrtf(-4.0f);
    ASSERT_TRUE(std::isinf(r_neg) || std::isnan(r_neg));

    // Boundary: Infinity -> Newton-Raphson with +inf produces -inf
    float r_inf = fastApproxMath_invsqrtf(std::numeric_limits<float>::infinity());
    ASSERT_TRUE(std::isinf(r_inf) || std::isnan(r_inf));

    // Boundary: NaN
    float r_nan = fastApproxMath_invsqrtf(std::numeric_limits<float>::quiet_NaN());
    ASSERT_NAN(r_nan);

    // Denormal / Subnormal boundary
    float denorm = 1.0e-38f;
    float r_denorm = fastApproxMath_invsqrtf(denorm);
    ASSERT_TRUE(std::isfinite(r_denorm));
    ASSERT_GT(r_denorm, 0.0f);
}

TEST_CASE(fastApproxMath_expf_and_lnf_inversion) {
    // Verify expf across positive and negative inputs
    float vals[] = { -3.0f, -1.0f, -0.5f, 0.0f, 0.5f, 1.0f, 2.0f, 4.0f };
    for (float v : vals) {
        float expected = std::exp(v);
        float actual = fastApproxMath_expf(v);
        float rel_err = std::fabs(actual - expected) / expected;
        ASSERT_LE(rel_err, 0.08f); // expf approximation within 8%
    }

    // lnf for positive inputs
    float ln_inputs[] = { 0.1f, 0.5f, 1.0f, 2.71828f, 10.0f, 50.0f };
    for (float v : ln_inputs) {
        float expected = std::log(v);
        float actual = fastApproxMath_lnf(v);
        ASSERT_NEAR(actual, expected, 0.15f);
    }

    // Roundtrip / Inverse test: lnf(expf(x)) ~ x
    for (float v : { -2.0f, -0.5f, 0.0f, 0.7f, 2.0f, 3.5f }) {
        float roundtrip = fastApproxMath_lnf(fastApproxMath_expf(v));
        ASSERT_NEAR(roundtrip, v, 0.2f);
    }

    // Boundaries for lnf: zero and negative
    // For 0.0f: converter.i = 0 -> (0 - 1064866805.0) / 12102203.0 ~ -87.9894f
    float ln_zero = fastApproxMath_lnf(0.0f);
    ASSERT_NEAR(ln_zero, -87.9894f, 0.01f);

    // For negative input: converter.i has sign bit set -> returns positive number
    // Document this asymmetric behavior of the fast approximation!
    float ln_neg = fastApproxMath_lnf(-1.0f);
    ASSERT_TRUE(std::isfinite(ln_neg));
}

TEST_CASE(fastApproxMath_sinf_bhaskara) {
    const float PI = 3.14159265f;

    // In [0, PI], Bhaskara I approximation is accurate within 0.0016 (~0.16%)
    float angles[] = { 0.0f, PI/6.0f, PI/4.0f, PI/3.0f, PI/2.0f, 2.0f*PI/3.0f, 3.0f*PI/4.0f, 5.0f*PI/6.0f, PI };
    for (float a : angles) {
        float expected = std::sin(a);
        float actual = fastApproxMath_sinf(a);
        ASSERT_NEAR(actual, expected, 0.005f);
    }

    // Asymmetric behavior outside [0, PI]:
    // The Bhaskara formula: 16*x*(PI-x) / (5*PI^2 - 4*x*(PI-x))
    // For x = -PI/2: x*(PI-x) = -PI/2 * 3PI/2 = -3/4 PI^2
    // denominator = 5*PI^2 - 4*(-3/4 PI^2) = 8*PI^2
    // numerator = 16*(-3/4 PI^2) = -12*PI^2
    // actual = -12 / 8 = -1.5f (distorted outside domain!)
    float actual_neg = fastApproxMath_sinf(-PI / 2.0f);
    ASSERT_NEAR(actual_neg, -1.5f, 0.01f);
}

TEST_CASE(fastApproxMath_log2f_powers_and_boundaries) {
    // Exact powers of 2
    ASSERT_NEAR(fastApproxMath_log2f(1.0f), 0.0f, 0.01f);
    ASSERT_NEAR(fastApproxMath_log2f(2.0f), 1.0f, 0.01f);
    ASSERT_NEAR(fastApproxMath_log2f(4.0f), 2.0f, 0.01f);
    ASSERT_NEAR(fastApproxMath_log2f(8.0f), 3.0f, 0.01f);
    ASSERT_NEAR(fastApproxMath_log2f(1024.0f), 10.0f, 0.01f);
    ASSERT_NEAR(fastApproxMath_log2f(0.5f), -1.0f, 0.01f);
    ASSERT_NEAR(fastApproxMath_log2f(0.25f), -2.0f, 0.01f);
    ASSERT_NEAR(fastApproxMath_log2f(0.125f), -3.0f, 0.01f);

    // Non-power of 2: step-wise floor nature of this exponent-only extraction
    // For 3.0f (between 2 and 4), floor is 1
    ASSERT_NEAR(fastApproxMath_log2f(3.0f), 1.0f, 0.01f);
    ASSERT_NEAR(fastApproxMath_log2f(7.99f), 2.0f, 0.01f);

    // Boundary: 0.0f -> exponent bits 0 -> 0 - 127 = -127
    ASSERT_NEAR(fastApproxMath_log2f(0.0f), -127.0f, 0.01f);
}

TEST_CASE(fastApproxMath_absf_sign_clearing) {
    // Normal positive and negative
    ASSERT_EQ(fastApproxMath_absf(42.5f), 42.5f);
    ASSERT_EQ(fastApproxMath_absf(-42.5f), 42.5f);

    // Signed zeros: -0.0f must become +0.0f
    float neg_zero = -0.0f;
    float pos_zero = fastApproxMath_absf(neg_zero);
    ASSERT_EQ(pos_zero, 0.0f);
    ASSERT_FALSE(std::signbit(pos_zero)); // Sign bit cleared

    // Infinities
    float neg_inf = -std::numeric_limits<float>::infinity();
    float pos_inf = fastApproxMath_absf(neg_inf);
    ASSERT_TRUE(std::isinf(pos_inf));
    ASSERT_GT(pos_inf, 0.0f);

    // Denormals
    float neg_denorm = -1.0e-40f;
    float pos_denorm = fastApproxMath_absf(neg_denorm);
    ASSERT_GT(pos_denorm, 0.0f);
}

TEST_CASE(fastApproxMath_tanh_symmetry_and_growth) {
    // Odd symmetry: tanh(-x) == -tanh(x)
    float test_x[] = { 0.1f, 0.5f, 1.0f, 1.5f, 2.0f, 3.0f };
    for (float x : test_x) {
        float pos = fastApproxMath_tanh(x);
        float neg = fastApproxMath_tanh(-x);
        ASSERT_NEAR(pos, -neg, 1e-6f);

        // Near origin, compare with std::tanh
        float expected = std::tanh(x);
        ASSERT_NEAR(pos, expected, 0.05f);
    }

    // Zero
    ASSERT_EQ(fastApproxMath_tanh(0.0f), 0.0f);

    // Limitation check for large x:
    // Production formula is x * (27 + x^2) / (27 + 9*x^2).
    // As x -> infinity, this approaches x / 9, NOT 1.0!
    float large_x = 18.0f;
    float res_large = fastApproxMath_tanh(large_x);
    // Independent formula check: 18 * (27 + 324) / (27 + 9*324) = 18 * 351 / 2943 = 6318 / 2943 ~ 2.1467
    ASSERT_NEAR(res_large, 6318.0f / 2943.0f, 0.001f);
    ASSERT_GT(res_large, 1.0f); // Demonstrates the non-saturating rational limitation!
}

TEST_CASE(fastApproxMath_double_functions) {
    // Double precision pow
    double p = fastApproxMath_pow(2.0, 3.0);
    ASSERT_NEAR(p, 8.0, 1.5);

    // Double precision exp
    double e1 = fastApproxMath_exp(1.0);
    ASSERT_NEAR(e1, 2.71828, 0.2);

    double e0 = fastApproxMath_exp(0.0);
    ASSERT_NEAR(e0, 1.0, 0.1);

    // Double precision ln
    double l_e = fastApproxMath_ln(2.71828);
    ASSERT_NEAR(l_e, 1.0, 0.15);

    double l_1 = fastApproxMath_ln(1.0);
    ASSERT_NEAR(l_1, 0.0, 0.15);
}

int main() {
    return ::HostTest::TestRegistry::instance().runAll("fastApproxMath");
}
