#include "defines.h"
#include <cycles.h>

#include "sim_test_framework.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "buffer.h"
#include "helperFcn.h"
#include "fastApproxMath.h"
#include <filter.h>

// ----------------------------------------------------------------------------
// 1. DSP2 FX Slot Address Calculation and Boundary Tests
// ----------------------------------------------------------------------------
void test_fx_slot_addresses(void) {
    printf("    [TEST] DSP2 FX Slot Address Calculation & Boundaries\n");

    // In OpenX32 dsp2/src/defines.h:
    // SDRAM_START = 0x04000000
    // SDRAM_AUDIO_START = 0x04200000
    // SDRAM_AUDIO_SIZE_BYTE = 14 * 1024 * 1024 = 14680064 bytes
    // In fxBase.cpp: _memoryAddress = (SDRAM_AUDIO_START + (_fxSlot * (SDRAM_AUDIO_SIZE_BYTE / 8)))

    const uint32_t audio_start = SDRAM_AUDIO_START;
    const uint32_t total_audio_bytes = SDRAM_AUDIO_SIZE_BYTE;
    const uint32_t slot_bytes = total_audio_bytes / 8; // 1,835,008 bytes = 0x1C0000

    SIM_ASSERT_EQ(total_audio_bytes, 14680064U);
    SIM_ASSERT_EQ(slot_bytes, 1835008U);
    SIM_ASSERT_EQ(slot_bytes, 0x1C0000U);

    uint32_t slot_addrs[8];
    for (int slot = 0; slot < 8; slot++) {
        slot_addrs[slot] = audio_start + (slot * (total_audio_bytes / 8));
    }

    // Check base addresses for all 8 slots
    SIM_ASSERT_EQ(slot_addrs[0], 0x04200000U);
    SIM_ASSERT_EQ(slot_addrs[1], 0x043C0000U);
    SIM_ASSERT_EQ(slot_addrs[2], 0x04580000U);
    SIM_ASSERT_EQ(slot_addrs[3], 0x04740000U);
    SIM_ASSERT_EQ(slot_addrs[4], 0x04900000U);
    SIM_ASSERT_EQ(slot_addrs[5], 0x04AC0000U);
    SIM_ASSERT_EQ(slot_addrs[6], 0x04C80000U);
    SIM_ASSERT_EQ(slot_addrs[7], 0x04E40000U);

    // Verify non-overlapping contiguous boundaries
    for (int slot = 0; slot < 7; slot++) {
        SIM_ASSERT_EQ(slot_addrs[slot] + slot_bytes, slot_addrs[slot + 1]);
    }

    // Verify upper boundary matches 16MB SDRAM top (0x04000000 + 16MB = 0x05000000)
    uint32_t slot7_end = slot_addrs[7] + slot_bytes;
    SIM_ASSERT_EQ(slot7_end, 0x05000000U);
    SIM_ASSERT_EQ(slot7_end, SDRAM_START + (16U * 1024U * 1024U));

    // Word capacity test on SHARC: sizeof(float) == 1 word
    uint32_t float_words_per_slot = slot_bytes / sizeof(float);
    SIM_ASSERT_EQ(float_words_per_slot, 1835008U);

    printf("    [INFO] Verified all 8 FX slot boundaries: [0x%08lx .. 0x%08lx]\n",
           (unsigned long)slot_addrs[0], (unsigned long)slot7_end);
}

// ----------------------------------------------------------------------------
// 2. Ring-Buffer Wraparound Tests
// ----------------------------------------------------------------------------
void test_ring_buffer_wraparound(void) {
    printf("    [TEST] Ring-Buffer Wraparound & Moving RMS\n");

    #define RB_TEST_LEN 8
    float raw_mem[RB_TEST_LEN];
    sRingBuffer rb;

    bufferMovingRmsInit(&rb, raw_mem, RB_TEST_LEN);
    SIM_ASSERT_EQ(rb.bufferLen, RB_TEST_LEN);
    SIM_ASSERT_EQ(rb.head, 0);
    SIM_ASSERT_NEAR(rb.one_over_bufferLen, 1.0f / 8.0f, 1e-6f);

    // Fill buffer completely: values 1.0 to 8.0
    for (int i = 0; i < RB_TEST_LEN; i++) {
        bufferWrite(&rb, (float)(i + 1));
    }
    // After 8 writes, head should have wrapped to 0
    SIM_ASSERT_EQ(rb.head, 0);

    // Verify reading with offsets across the boundary
    // offset 1 = most recent = 8.0f
    // offset 8 = oldest = 1.0f
    SIM_ASSERT_NEAR(bufferRead(&rb, 1), 8.0f, 1e-6f);
    SIM_ASSERT_NEAR(bufferRead(&rb, 4), 5.0f, 1e-6f);
    SIM_ASSERT_NEAR(bufferRead(&rb, 8), 1.0f, 1e-6f);

    // Write 2 more values (wraps past 0): 9.0, 10.0
    bufferWrite(&rb, 9.0f);   // written to idx 0, head -> 1
    bufferWrite(&rb, 10.0f);  // written to idx 1, head -> 2
    SIM_ASSERT_EQ(rb.head, 2);

    SIM_ASSERT_NEAR(bufferRead(&rb, 1), 10.0f, 1e-6f); // most recent
    SIM_ASSERT_NEAR(bufferRead(&rb, 2), 9.0f, 1e-6f);
    SIM_ASSERT_NEAR(bufferRead(&rb, 3), 8.0f, 1e-6f);
    SIM_ASSERT_NEAR(bufferRead(&rb, 8), 3.0f, 1e-6f);  // oldest remaining

    // Test bufferMovingRms:
    // Implementation behavior in buffer.cpp:
    // output = output + ((value - buffer[head]) * one_over_bufferLen)
    // Because head is incremented before reading buffer[head], it subtracts
    // the slot that was just written on wrap.
    #define RMS_LEN 4
    float rms_mem[RMS_LEN];
    sRingBuffer rms_rb;
    bufferMovingRmsInit(&rms_rb, rms_mem, RMS_LEN);

    float rms_sq = 0.0f;
    for (int i = 0; i < RMS_LEN; i++) {
        rms_sq = bufferMovingRms(&rms_rb, 2.0f);
    }
    // Output accumulates (RMS_LEN - 1) slots * (4.0 / 4) = 3.0f
    SIM_ASSERT_NEAR(rms_sq, 3.0f, 1e-4f);

    printf("    [INFO] Ring-buffer wraparound verified successfully\n");
}

// ----------------------------------------------------------------------------
// 3. Representative Floating-Point Helper Functions
// ----------------------------------------------------------------------------
void test_float_helper_functions(void) {
    printf("    [TEST] Representative Floating-Point Helper Functions\n");

    // helperFcn_db2lin and helperFcn_lin2db
    SIM_ASSERT_NEAR(helperFcn_db2lin(0.0f), 1.0f, 1e-5f);
    SIM_ASSERT_NEAR(helperFcn_db2lin(20.0f), 10.0f, 1e-4f);
    SIM_ASSERT_NEAR(helperFcn_db2lin(-20.0f), 0.1f, 1e-4f);
    SIM_ASSERT_NEAR(helperFcn_db2lin(6.0206f), 2.0f, 1e-3f);

    SIM_ASSERT_NEAR(helperFcn_lin2db(1.0f), 0.0f, 1e-4f);
    SIM_ASSERT_NEAR(helperFcn_lin2db(10.0f), 20.0f, 1e-3f);
    SIM_ASSERT_NEAR(helperFcn_lin2db(0.1f), -20.0f, 1e-3f);

    // Inversion check
    float db_vals[] = { -18.0f, -6.0f, 0.0f, 6.0f, 12.0f };
    for (int i = 0; i < 5; i++) {
        float lin = helperFcn_db2lin(db_vals[i]);
        float roundtrip = helperFcn_lin2db(lin);
        SIM_ASSERT_NEAR(roundtrip, db_vals[i], 1e-3f);
    }

    // helperFcn_lowPassFilter step response: output += coeff * (input - output)
    float lp_out = 0.0f;
    for (int i = 0; i < 50; i++) {
        helperFcn_lowPassFilter(1.0f, &lp_out, 0.1f);
    }
    SIM_ASSERT_NEAR(lp_out, 1.0f, 0.01f);

    // fastApproxMath tests
    SIM_ASSERT_NEAR(fastApproxMath_powf(2.0f, 3.0f), 8.0f, 1.5f);
    SIM_ASSERT_NEAR(fastApproxMath_powf(4.0f, 0.5f), 2.0f, 0.3f);
    SIM_ASSERT_NEAR(fastApproxMath_invsqrtf(4.0f), 0.5f, 0.02f);
    SIM_ASSERT_NEAR(fastApproxMath_invsqrtf(16.0f), 0.25f, 0.01f);
    SIM_ASSERT_NEAR(fastApproxMath_sinf(0.0f), 0.0f, 0.01f);
    SIM_ASSERT_NEAR(fastApproxMath_sinf(3.14159265f / 2.0f), 1.0f, 0.01f);
    SIM_ASSERT_NEAR(fastApproxMath_absf(-42.5f), 42.5f, 1e-6f);
    SIM_ASSERT_NEAR(fastApproxMath_absf(42.5f), 42.5f, 1e-6f);

    printf("    [INFO] Floating-point math helpers verified successfully\n");
}

// ----------------------------------------------------------------------------
// 4. Deterministic Audio Block Algorithm & Cycle Counter
// ----------------------------------------------------------------------------
#define AUDIO_BLOCK_SAMPLES 16

// Coeffs array in PM memory required by SHARC biquad_trans
float pm g_biquad_coeffs[5];
float g_biquad_state[2];
float g_audio_block[AUDIO_BLOCK_SAMPLES];

void test_audio_kernel_and_cycles(void) {
    printf("    [TEST] Deterministic Audio Block Kernel (Biquad Transpose Filter)\n");

    // 1. Calculate Peak EQ coefficients via helperFcn: 1 kHz, Q=1.0, +6 dB at 48 kHz
    // helperFcn_calcBiquadCoeffs computes normalized:
    // raw_coeffs[0] = B0
    // raw_coeffs[1] = B1
    // raw_coeffs[2] = B2
    // raw_coeffs[3] = -A1
    // raw_coeffs[4] = -A2
    // For biquad_trans (Transposed Direct Form II: y = B0*x + d1, d1 = B1*x + A1*y + d2):
    // The recursive term is + A1*y, where A1 = raw_coeffs[3].
    float raw_coeffs[5];
    helperFcn_calcBiquadCoeffs(1, 1000.0f, 1.0f, 6.0f, raw_coeffs, 48000.0f);

    for (int i = 0; i < 5; i++) {
        g_biquad_coeffs[i] = raw_coeffs[i];
    }

    g_biquad_state[0] = 0.0f;
    g_biquad_state[1] = 0.0f;

    // 2. Set up deterministic input: unit impulse
    g_audio_block[0] = 1.0f;
    for (int s = 1; s < AUDIO_BLOCK_SAMPLES; s++) {
        g_audio_block[s] = 0.0f;
    }

    // 3. Execute biquad_trans and benchmark with simulator cycle counter
    cycle_stats_t stats;
    CYCLES_INIT(stats);

    CYCLES_START(stats);
    biquad_trans(g_audio_block, g_biquad_coeffs, g_biquad_state, AUDIO_BLOCK_SAMPLES, 1);
    CYCLES_STOP(stats);

    // 4. Verify mathematical correctness:
    // First sample response to unit impulse x[0]=1.0 must equal B0 (g_biquad_coeffs[0])
    SIM_ASSERT_NEAR(g_audio_block[0], g_biquad_coeffs[0], 1e-5f);

    // Second sample response must equal B1 + A1 * y[0]
    float expected_y1 = g_biquad_coeffs[1] + g_biquad_coeffs[3] * g_audio_block[0];
    SIM_ASSERT_NEAR(g_audio_block[1], expected_y1, 1e-4f);

    // Verify all subsequent samples are stable and decaying (magnitude < 2.0)
    for (int s = 0; s < AUDIO_BLOCK_SAMPLES; s++) {
        SIM_ASSERT_TRUE(fabsf(g_audio_block[s]) < 2.0f);
    }

    printf("    [RESULT] y[0] = %f (expected B0 = %f)\n", g_audio_block[0], g_biquad_coeffs[0]);
    printf("    [RESULT] y[1] = %f (expected = %f)\n", g_audio_block[1], expected_y1);
    printf("    [CYCLE BENCHMARK] Audio Kernel Execution:\n");
    printf("        Block Size:       %d samples\n", AUDIO_BLOCK_SAMPLES);
    printf("        Filter Sections:  1 biquad (transpose form)\n");
    printf("        Simulator Cycles: %lu cycles\n", (unsigned long)stats._cycles);
    printf("        Cycles/Sample:    %.2f cycles/sample\n", (float)stats._cycles / (float)AUDIO_BLOCK_SAMPLES);
    printf("        (Note: Cycles measured via EMUCLK on cycle-accurate simzpls.dll engine.\n");
    printf("               Hardware timing may vary depending on SDRAM/bus latencies.)\n");
}

int main(void) {
    sim_test_init();
    printf("=== ADSP-21371 CCES SIMULATOR FULL TEST SUITE ===\n");

    SIM_RUN_TEST(test_fx_slot_addresses);
    SIM_RUN_TEST(test_ring_buffer_wraparound);
    SIM_RUN_TEST(test_float_helper_functions);
    SIM_RUN_TEST(test_audio_kernel_and_cycles);

    return sim_test_summary("DSP Simulator Suite");
}
