#include "test_framework.h"
#include "dsp1_host_harness.h"

#include <cmath>
#include <limits>

extern "C" {
    float linearToDb(float linear);
    float linearToDb_fast(float linear);
    float dbToLinear(float dB);
    float dbToLinear_fast(float dB);
    float calcRMS(const float* __restrict src);
    void fxSetPeqCoeffs(int channel, int index, float coeffs[]);
}

TEST_CASE(dsp1_linearToDb_accurate_and_noise_floor) {
    // Exact standard formula: 20 * log10(x)
    struct TestEntry {
        float lin;
        float exp_db;
        float tol;
    } cases[] = {
        { 1.0f, 0.0f, 1e-5f },
        { 2.0f, 6.0205999f, 1e-4f },
        { 0.5f, -6.0205999f, 1e-4f },
        { 10.0f, 20.0f, 1e-4f },
        { 0.1f, -20.0f, 1e-4f },
        { 100.0f, 40.0f, 1e-4f }
    };

    for (const auto& c : cases) {
        float actual = linearToDb(c.lin);
        ASSERT_NEAR(actual, c.exp_db, c.tol);
    }

    // Noise floor clamp in production: linear <= 0.00001f returns -100.0f
    ASSERT_EQ(linearToDb(0.00001f), -100.0f);
    ASSERT_EQ(linearToDb(0.000001f), -100.0f);
    ASSERT_EQ(linearToDb(0.0f), -100.0f);
    ASSERT_EQ(linearToDb(-1.0f), -100.0f);
}

TEST_CASE(dsp1_linearToDb_fast_approximation) {
    // Fast log2 approximation: (vx.i * 1.1920928955078125e-7f - 126.94269504f) * 6.0206f
    // Noise floor clamp in production: linear < 0.0000001f returns -140.0f
    ASSERT_EQ(linearToDb_fast(0.0f), -140.0f);
    ASSERT_EQ(linearToDb_fast(5.0e-8f), -140.0f);
    ASSERT_EQ(linearToDb_fast(-0.5f), -140.0f);

    float test_points[] = { 0.001f, 0.01f, 0.1f, 0.5f, 1.0f, 2.0f, 10.0f, 50.0f };
    for (float v : test_points) {
        float expected_exact = 20.0f * std::log10(v);
        float actual = linearToDb_fast(v);
        // Fast approximation has bounded accuracy within ~0.4 dB of true log
        ASSERT_NEAR(actual, expected_exact, 0.45f);

        // Also assert against independent bit-level analytical formula
        union { float f; uint32_t i; } vx = { v };
        float indep_log2 = (float)vx.i * 1.1920928955078125e-7f - 126.94269504f;
        float indep_db = 6.0206f * indep_log2;
        ASSERT_NEAR(actual, indep_db, 1e-5f);
    }
}

TEST_CASE(dsp1_dbToLinear_exact_and_fast) {
    // dbToLinear: powf(10.0f, dB * 0.05f)
    struct DbEntry {
        float db;
        float exp_lin;
        float tol;
    } db_cases[] = {
        { 0.0f, 1.0f, 1e-5f },
        { 6.0205999f, 2.0f, 1e-4f },
        { -6.0205999f, 0.5f, 1e-4f },
        { 20.0f, 10.0f, 1e-4f },
        { -20.0f, 0.1f, 1e-4f }
    };

    for (const auto& c : db_cases) {
        ASSERT_NEAR(dbToLinear(c.db), c.exp_lin, c.tol);
    }

    // dbToLinear_fast: bit trick 2^(dB / 6.0206)
    for (const auto& c : db_cases) {
        float actual_fast = dbToLinear_fast(c.db);
        // Fast approximation within ~7% of exact linear gain
        ASSERT_NEAR(actual_fast, c.exp_lin, 0.07f * c.exp_lin);

        // Verify exact match with independent bit manipulation formula
        float x = c.db * (1.0f / 6.0206f);
        union { float f; int32_t i; } vx;
        vx.i = (int32_t)(x * 8388608.0f) + 1065353216;
        ASSERT_EQ(actual_fast, vx.f);
    }

    // Zero dB identity: exact bit trick at 0.0 dB produces 1065353216 which is exactly 1.0f
    ASSERT_EQ(dbToLinear_fast(0.0f), 1.0f);
}

TEST_CASE(dsp1_calcRMS_deterministic_and_boundary) {
    // 1. Zero signal -> RMS = 0.0f
    float zeros[16] = {0};
    ASSERT_EQ(calcRMS(zeros), 0.0f);

    // 2. DC signal of 3.0f -> RMS = 3.0f
    float dc[16];
    for (int i = 0; i < 16; i++) dc[i] = 3.0f;
    ASSERT_NEAR(calcRMS(dc), 3.0f, 1e-6f);

    // 3. Negative DC signal of -4.5f -> RMS = 4.5f (sign invariance)
    float neg_dc[16];
    for (int i = 0; i < 16; i++) neg_dc[i] = -4.5f;
    ASSERT_NEAR(calcRMS(neg_dc), 4.5f, 1e-6f);

    // 4. Single impulse of magnitude 8.0f:
    // sum = 8^2 = 64; mean = 64 / 16 = 4; sqrt(4) = 2.0f
    float impulse[16] = {0};
    impulse[5] = 8.0f;
    ASSERT_NEAR(calcRMS(impulse), 2.0f, 1e-6f);

    // 5. Arbitrary waveform with independently computed expected RMS
    float wave[16] = {
        0.5f, -0.3f, 1.2f, -0.8f, 0.0f, 0.9f, -1.5f, 0.4f,
        -0.2f, 0.7f, -1.1f, 0.6f, -0.4f, 1.0f, -0.7f, 0.3f
    };
    double indep_sum = 0.0;
    for (int i = 0; i < 16; i++) {
        indep_sum += (double)wave[i] * (double)wave[i];
    }
    float expected_rms = (float)std::sqrt(indep_sum / 16.0);
    ASSERT_NEAR(calcRMS(wave), expected_rms, 1e-6f);
}

TEST_CASE(dsp1_fxSetPeqCoeffs_interleaving_and_negation) {
    host_dsp1_reset();

    // Section 0 (even)
    float coeffs0[5] = { 1.1f, 2.2f, 3.3f, 4.4f, 5.5f };
    fxSetPeqCoeffs(0, 0, coeffs0);

    // Expected layout in peqCoeffs_4BD_EQ[0]:
    // section 0 (even): sectionIndex = 0
    // [0] = a0 (1.1f), [2] = a1 (2.2f), [4] = a2 (3.3f), [6] = -b1 (-4.4f), [8] = -b2 (-5.5f)
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 0), 1.1f, 1e-6f);
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 2), 2.2f, 1e-6f);
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 4), 3.3f, 1e-6f);
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 6), -4.4f, 1e-6f);
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 8), -5.5f, 1e-6f);

    // Section 1 (odd)
    float coeffs1[5] = { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f };
    fxSetPeqCoeffs(0, 1, coeffs1);

    // Expected interleaved layout for odd section:
    // [1] = a0 (0.1f), [3] = a1 (0.2f), [5] = a2 (0.3f), [7] = -b1 (-0.4f), [9] = -b2 (-0.5f)
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 1), 0.1f, 1e-6f);
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 3), 0.2f, 1e-6f);
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 5), 0.3f, 1e-6f);
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 7), -0.4f, 1e-6f);
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 9), -0.5f, 1e-6f);

    // Section 0 coeffs must have remained intact after Section 1 was set
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 0), 1.1f, 1e-6f);
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 6), -4.4f, 1e-6f);

    // Out of bounds check: index 4 (EQ_4BD_BANDS is 4)
    float coeffs_oob[5] = { 99.0f, 99.0f, 99.0f, 99.0f, 99.0f };
    fxSetPeqCoeffs(0, 4, coeffs_oob);
    // Should have silently returned without overwriting past bands
    ASSERT_NEAR(host_dsp1_get_peq_coeff(0, 0), 1.1f, 1e-6f);
}

int main() {
    return ::HostTest::TestRegistry::instance().runAll("dsp1_fx");
}
