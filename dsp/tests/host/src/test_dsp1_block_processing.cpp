#include "test_framework.h"
#include "dsp1_host_harness.h"
#include "filter.h"
#undef M_PI
#include "helperFcn.h"

#include <cmath>
#include <vector>
#include <limits>

extern "C" {
    float linearToDb_fast(float linear);
    float dbToLinear_fast(float dB);
    void fxSetPeqCoeffs(int channel, int index, float coeffs[]);
}

TEST_CASE(dsp1_audioSmoothVolume_production_call) {
    // Tests real production audioSmoothVolume() from dsp1/src/audio.c:163-191.
    // Production formula: dsp.mainVolume[i] += (dsp.mainVolumeSet[i] - dsp.mainVolume[i]) * 0.01f;
    host_dsp1_reset();

    // 1. Initial values: current 0.0f, target 1.0f for Left (0), Right (1), Sub (2)
    host_dsp1_set_main_volume(0, 0.0f, 1.0f);
    host_dsp1_set_main_volume(1, 0.5f, 0.5f); // already at target
    host_dsp1_set_main_volume(2, 1.0f, 0.0f); // ramp down

    const float coeff = 0.01f;

    // Run 50 iterations of audioSmoothVolume()
    for (int k = 1; k <= 50; k++) {
        host_dsp1_call_audioSmoothVolume();

        float cur0, cur1, cur2;
        host_dsp1_get_main_volume(0, &cur0, nullptr);
        host_dsp1_get_main_volume(1, &cur1, nullptr);
        host_dsp1_get_main_volume(2, &cur2, nullptr);

        // Independent analytical formula: V[k] = Target + (V0 - Target) * (1 - coeff)^k
        float exp0 = 1.0f + (0.0f - 1.0f) * std::pow(1.0f - coeff, (float)k);
        float exp1 = 0.5f; // remains at 0.5
        float exp2 = 0.0f + (1.0f - 0.0f) * std::pow(1.0f - coeff, (float)k);

        ASSERT_NEAR(cur0, exp0, 1e-5f);
        ASSERT_NEAR(cur1, exp1, 1e-5f);
        ASSERT_NEAR(cur2, exp2, 1e-5f);
    }
}

TEST_CASE(dsp1_gate_block_processing_and_boundaries) {
    // Tests gate block math (proposed seam for audio.c:496-554)
    HostGate gate;
    gate.value_threshold = 0.1f;
    gate.value_gainmin = 0.0f;
    gate.value_coeff_attack = 0.5f;  // fast attack
    gate.value_coeff_release = 0.1f; // slow release
    gate.value_hold_ticks = 2.0f;    // hold for 2 blocks
    gate.holdTimer = 0;

    float env = 0.0f;

    // 1. Block 1: Signal well above threshold (0.5f) -> Gate opens (attack)
    float block1[16];
    for (int i = 0; i < 16; i++) block1[i] = 0.5f;
    host_dsp1_gate_process_block(block1, &gate, &env);

    // RMS is 0.5f > 0.1f -> targetGainLinear = 1.0f
    // env rises: target > env -> coeff = attack (0.5), holdTimer reset to 2
    ASSERT_EQ(gate.holdTimer, 2);
    ASSERT_GT(env, 0.0f);
    // Samples scaled by rising envelope
    for (int s = 0; s < 16; s++) {
        ASSERT_GT(block1[s], 0.0f);
        ASSERT_LE(block1[s], 0.5f);
    }

    // 2. Block 2: Signal drops below threshold (0.001f) -> Hold state active
    float block2[16];
    for (int i = 0; i < 16; i++) block2[i] = 0.001f;
    float env_before = env;
    host_dsp1_gate_process_block(block2, &gate, &env);

    // RMS < threshold -> target = 0.0f. Since holdTimer was 2 (>0), holdTimer decrements to 1.
    // coeff is 0.0f during hold! So env does not decay yet.
    ASSERT_EQ(gate.holdTimer, 1);
    ASSERT_NEAR(env, env_before, 1e-6f); // envelope held constant

    // 3. Block 3: Signal still below threshold -> Hold state decrements to 0
    host_dsp1_gate_process_block(block2, &gate, &env);
    ASSERT_EQ(gate.holdTimer, 0);
    ASSERT_NEAR(env, env_before, 1e-6f);

    // 4. Block 4: Signal still below threshold -> Release phase begins!
    host_dsp1_gate_process_block(block2, &gate, &env);
    // coeff = release (0.1f), env decays towards 0.0f
    ASSERT_LT(env, env_before);

    // 5. Boundary: Zero signal (silence)
    float block_zero[16] = {0};
    host_dsp1_gate_process_block(block_zero, &gate, &env);
    for (int s = 0; s < 16; s++) ASSERT_EQ(block_zero[s], 0.0f);

    // 6. Boundary: Denormal signal
    float block_denorm[16];
    for (int s = 0; s < 16; s++) block_denorm[s] = 1.0e-39f;
    host_dsp1_gate_process_block(block_denorm, &gate, &env);
    ASSERT_TRUE(std::isfinite(env));
}

TEST_CASE(dsp1_compressor_block_processing) {
    // Tests compressor block math (proposed seam for audio.c:605-660)
    HostCompressor comp;
    comp.value_thresholdDb = -20.0f;
    comp.value_1_minus_1_by_ratio = 0.75f; // 4:1 ratio -> 1 - 1/4 = 0.75
    comp.value_coeff_attack = 0.2f;
    comp.value_coeff_release = 0.05f;
    comp.value_hold_ticks = 1.0f;
    comp.holdTimer = 0;

    float env = 1.0f;
    float makeUp = 2.0f; // +6 dB makeup gain

    // 1. Signal below threshold: -30 dB RMS
    float src_quiet[16];
    float dst_quiet[16];
    for (int i = 0; i < 16; i++) src_quiet[i] = 100.0f;

    host_dsp1_compressor_process_block(src_quiet, dst_quiet, &comp, &env, makeUp);

    // Target gain is 0 dB (linear 1.0f).
    // Envelope remains near 1.0f, dst = src * env * makeUp = 100.0f * 1.0f * 2.0f = 200.0f
    for (int s = 0; s < 16; s++) {
        ASSERT_NEAR(dst_quiet[s], 200.0f, 2.0f);
    }

    // 2. Signal above threshold: high amplitude
    float src_loud[16];
    float dst_loud[16];
    for (int i = 0; i < 16; i++) src_loud[i] = 1073741824.0f; // ~0.5 full-scale -> ~ -6 dBFS

    host_dsp1_compressor_process_block(src_loud, dst_loud, &comp, &env, 1.0f);

    // Since signal is above -20 dBFS, gain reduction should occur -> envelope < 1.0f
    ASSERT_LT(env, 1.0f);
    ASSERT_GT(env, 0.0f);

    // Verify output was compressed: dst < src
    for (int s = 0; s < 16; s++) {
        ASSERT_LT(dst_loud[s], src_loud[s]);
        ASSERT_GT(dst_loud[s], 0.0f);
    }

    // 3. Boundary: Zero signal
    float src_zero[16] = {0};
    float dst_zero[16] = {0};
    host_dsp1_compressor_process_block(src_zero, dst_zero, &comp, &env, 1.0f);
    for (int s = 0; s < 16; s++) {
        ASSERT_EQ(dst_zero[s], 0.0f);
    }
}

TEST_CASE(dsp1_mixbus_4bus_parallel_accumulation) {
    // Tests 4-bus parallel accumulation (proposed seam for audio.c:740-775)
    float src[16];
    for (int s = 0; s < 16; s++) src[s] = (float)(s + 1);

    float b0[16], b1[16], b2[16], b3[16];
    for (int s = 0; s < 16; s++) {
        b0[s] = 10.0f;
        b1[s] = 20.0f;
        b2[s] = 30.0f;
        b3[s] = 40.0f;
    }

    float* acc[4] = { b0, b1, b2, b3 };
    float gains[4] = { 0.5f, -1.0f, 2.0f, 0.0f };

    host_dsp1_mixbus_accumulate_block(src, gains, acc);

    // Verify all 64 samples against independent calculation
    for (int s = 0; s < 16; s++) {
        float in_sample = (float)(s + 1);
        ASSERT_NEAR(b0[s], 10.0f + 0.5f * in_sample, 1e-6f);
        ASSERT_NEAR(b1[s], 20.0f - 1.0f * in_sample, 1e-6f);
        ASSERT_NEAR(b2[s], 30.0f + 2.0f * in_sample, 1e-6f);
        ASSERT_NEAR(b3[s], 40.0f + 0.0f * in_sample, 1e-6f);
    }
}

TEST_CASE(dsp1_biquad_trans_block_filter_independent_verification) {
    // Tests biquad_trans block filtering against independent difference equation
    // 1. Calculate Peak EQ coefficients: 1000 Hz, Q = 1.414, +6 dB at 48 kHz
    float coeffs[5];
    helperFcn_calcBiquadCoeffs(1, 1000.0f, 1.414f, 6.0f, coeffs, 48000.0f);

    // 2. Process impulse through biquad_trans (1 section)
    float audio[16] = {0};
    audio[0] = 1.0f; // unit impulse
    float state[2] = {0};

    biquad_trans(audio, coeffs, state, 16, 1);

    // 3. Independently calculate expected impulse response from difference equation:
    // Direct Form II Transposed:
    // y[n] = a0 * x[n] + w1[n-1]
    // w1[n] = a1 * x[n] + (-b1) * y[n] + w2[n-1]
    // w2[n] = a2 * x[n] + (-b2) * y[n]
    float a0 = coeffs[0];
    float a1 = coeffs[1];
    float a2 = coeffs[2];
    float b1 = coeffs[3]; // already includes negation from helperFcn
    float b2 = coeffs[4]; // already includes negation from helperFcn

    float w1 = 0.0f;
    float w2 = 0.0f;
    for (int n = 0; n < 16; n++) {
        float x = (n == 0) ? 1.0f : 0.0f;
        float expected_y = a0 * x + w1;
        w1 = a1 * x + b1 * expected_y + w2;
        w2 = a2 * x + b2 * expected_y;

        ASSERT_NEAR(audio[n], expected_y, 1e-6f);
    }
}

int main() {
    return ::HostTest::TestRegistry::instance().runAll("dsp1_block_processing");
}
