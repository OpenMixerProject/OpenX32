#include "test_framework.h"
#undef M_PI
#include "helperFcn.h"

#include <cmath>
#include <limits>

TEST_CASE(helperFcn_db2lin_and_lin2db) {
    // Exact independent mathematical checks for db2lin
    // lin = 10^(dB / 20)
    struct TestCase {
        float db;
        float expected_lin;
        float tol;
    } db_tests[] = {
        { 0.0f, 1.0f, 1e-6f },
        { 6.0205999f, 2.0f, 1e-4f },
        { -6.0205999f, 0.5f, 1e-4f },
        { 20.0f, 10.0f, 1e-5f },
        { -20.0f, 0.1f, 1e-5f },
        { 40.0f, 100.0f, 1e-4f },
        { -60.0f, 0.001f, 1e-6f },
        { 12.0f, 3.9810717f, 1e-4f },
        { -12.0f, 0.25118864f, 1e-4f },
        { -100.0f, 1.0e-5f, 1e-7f }
    };

    for (const auto& tc : db_tests) {
        float actual = helperFcn_db2lin(tc.db);
        float expected = std::pow(10.0f, tc.db * 0.05f);
        ASSERT_NEAR(actual, expected, tc.tol);
    }

    // Exact independent mathematical checks for lin2db
    // db = 20 * log10(lin + 1e-9)
    struct LinTestCase {
        float lin;
        float expected_db;
        float tol;
    } lin_tests[] = {
        { 1.0f, 0.0f, 1e-4f },
        { 2.0f, 6.0205999f, 1e-4f },
        { 0.5f, -6.0205999f, 1e-4f },
        { 10.0f, 20.0f, 1e-4f },
        { 0.1f, -20.0f, 1e-4f },
        { 100.0f, 40.0f, 1e-4f },
        { 0.001f, -60.0f, 1e-3f }
    };

    for (const auto& tc : lin_tests) {
        float actual = helperFcn_lin2db(tc.lin);
        float expected = 20.0f * std::log10(tc.lin + 1e-9f);
        ASSERT_NEAR(actual, expected, tc.tol);
    }

    // Zero boundary condition for lin2db:
    // With lin = 0.0f, lin2db computes 20 * log10(1e-9) = 20 * (-9) = -180 dB
    float db_at_zero = helperFcn_lin2db(0.0f);
    ASSERT_NEAR(db_at_zero, -180.0f, 1e-4f);

    // Roundtrip verification: lin -> db -> lin
    for (float lin_val : { 0.01f, 0.1f, 0.5f, 1.0f, 2.5f, 10.0f }) {
        float roundtrip = helperFcn_db2lin(helperFcn_lin2db(lin_val));
        ASSERT_NEAR(roundtrip, lin_val, 1e-4f * lin_val);
    }
}

TEST_CASE(helperFcn_biquad_peak_filter_boost_and_cut) {
    float coeffs[5] = {0};
    const float fs = 48000.0f;
    const float f0 = 1000.0f;
    const float Q = 1.4142f;

    // Peak filter boost (+6 dB)
    helperFcn_calcBiquadCoeffs(1, f0, Q, 6.0f, coeffs, fs);

    // Independent derivation of Peak filter (type 1, boost)
    float V = std::pow(10.0f, 6.0f * 0.05f);
    float K = std::tan(3.14159265358979323846f * f0 / fs);
    float K2 = K * K;
    float norm = 1.0f / (1.0f + 1.0f/Q * K + K2);
    float exp_a0 = (1.0f + V/Q * K + K2) * norm;
    float exp_a1 = (2.0f * (K2 - 1.0f)) * norm;
    float exp_a2 = (1.0f - V/Q * K + K2) * norm;
    float exp_b1 = -(exp_a1);
    float exp_b2 = -((1.0f - 1.0f/Q * K + K2) * norm);

    ASSERT_NEAR(coeffs[0], exp_a0, 1e-6f);
    ASSERT_NEAR(coeffs[1], exp_a1, 1e-6f);
    ASSERT_NEAR(coeffs[2], exp_a2, 1e-6f);
    ASSERT_NEAR(coeffs[3], exp_b1, 1e-6f);
    ASSERT_NEAR(coeffs[4], exp_b2, 1e-6f);

    // Stability: poles of 1 - b1*z^-1 - b2*z^-2 (note the negative sign convention in helperFcn)
    // helperFcn stores coeffs[3] = -actual_pole_b1, coeffs[4] = -actual_pole_b2
    // True denominator is 1 - coeffs[3]*z^-1 - coeffs[4]*z^-2
    // Stability requires |coeffs[4]| < 1.0
    ASSERT_LT(std::fabs(coeffs[4]), 1.0f);

    // Peak filter cut (-6 dB)
    helperFcn_calcBiquadCoeffs(1, f0, Q, -6.0f, coeffs, fs);
    float norm_cut = 1.0f / (1.0f + V/Q * K + K2);
    float exp_cut_a0 = (1.0f + 1.0f/Q * K + K2) * norm_cut;
    ASSERT_NEAR(coeffs[0], exp_cut_a0, 1e-6f);
    ASSERT_LT(std::fabs(coeffs[4]), 1.0f);
}

TEST_CASE(helperFcn_biquad_all_types) {
    const float fs = 48000.0f;
    const float f0 = 2000.0f;
    const float Q = 0.7071f;
    const float gain = 4.0f;

    int types[] = { 2 /* Low-Shelf */, 3 /* High-Shelf */, 4 /* Bandpass */, 6 /* LowPass */, 7 /* HighPass */ };
    for (int t : types) {
        float coeffs[5] = {0};
        helperFcn_calcBiquadCoeffs(t, f0, Q, gain, coeffs, fs);

        // Coefficients must all be finite
        for (int i = 0; i < 5; i++) {
            ASSERT_TRUE(std::isfinite(coeffs[i]));
        }

        // Biquad filter must be stable: |b2| < 1
        ASSERT_LT(std::fabs(coeffs[4]), 1.0f);

        // Test with cut (negative gain)
        helperFcn_calcBiquadCoeffs(t, f0, Q, -gain, coeffs, fs);
        for (int i = 0; i < 5; i++) {
            ASSERT_TRUE(std::isfinite(coeffs[i]));
        }
        ASSERT_LT(std::fabs(coeffs[4]), 1.0f);
    }
}

TEST_CASE(helperFcn_biquad_frequency_boundaries) {
    float coeffs[5] = {0};
    const float fs = 48000.0f;

    // Very low frequency (20 Hz)
    helperFcn_calcBiquadCoeffs(6 /* LowPass */, 20.0f, 0.7071f, 0.0f, coeffs, fs);
    for (int i = 0; i < 5; i++) ASSERT_TRUE(std::isfinite(coeffs[i]));
    ASSERT_LT(std::fabs(coeffs[4]), 1.0f);

    // Very high frequency near Nyquist (22000 Hz at 48kHz fs)
    helperFcn_calcBiquadCoeffs(7 /* HighPass */, 22000.0f, 0.7071f, 0.0f, coeffs, fs);
    for (int i = 0; i < 5; i++) ASSERT_TRUE(std::isfinite(coeffs[i]));
    ASSERT_LT(std::fabs(coeffs[4]), 1.0f);
}

TEST_CASE(helperFcn_lowPassFilter_step_and_boundaries) {
    float output = 0.0f;

    // Coeff = 0.0f: filter holds state, output never changes
    helperFcn_lowPassFilter(10.0f, &output, 0.0f);
    ASSERT_EQ(output, 0.0f);

    // Coeff = 1.0f: filter immediately passes input
    helperFcn_lowPassFilter(7.5f, &output, 1.0f);
    ASSERT_EQ(output, 7.5f);

    // Step response with coeff = 0.2f
    // Analytical solution: y[n] = y_target + (y[0] - y_target) * (1 - c)^n
    output = 0.0f;
    float target = 1.0f;
    float c = 0.2f;
    for (int n = 1; n <= 20; n++) {
        helperFcn_lowPassFilter(target, &output, c);
        float expected = target * (1.0f - std::pow(1.0f - c, (float)n));
        ASSERT_NEAR(output, expected, 1e-5f);
    }

    // Negative input step response
    output = 1.0f;
    target = -1.0f;
    for (int n = 1; n <= 10; n++) {
        helperFcn_lowPassFilter(target, &output, c);
        float expected = 1.0f + (target - 1.0f) * (1.0f - std::pow(1.0f - c, (float)n));
        ASSERT_NEAR(output, expected, 1e-5f);
    }

    // Boundary: Zero input from non-zero state -> exponential decay to zero
    output = 5.0f;
    for (int n = 0; n < 100; n++) {
        helperFcn_lowPassFilter(0.0f, &output, 0.1f);
    }
    ASSERT_NEAR(output, 5.0f * std::pow(0.9f, 100.0f), 1e-6f);
}

int main() {
    return ::HostTest::TestRegistry::instance().runAll("helperFcn");
}
