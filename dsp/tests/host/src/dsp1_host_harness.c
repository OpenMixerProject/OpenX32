#include "dsp1_host_harness.h"
#undef M_PI
#include "dsp1.h"
#include "fx.h"
#include "audio.h"

#include <string.h>
#include <math.h>

void host_dsp1_init(void) {
    memset(&dsp, 0, sizeof(dsp));
    dsp.samplerate = 48000.0f;
}

void host_dsp1_reset(void) {
    memset(&dsp, 0, sizeof(dsp));
    dsp.samplerate = 48000.0f;
}

void host_dsp1_set_main_volume(int idx, float current, float target) {
    if (idx >= 0 && idx < 3) {
        dsp.mainVolume[idx] = current;
        dsp.mainVolumeSet[idx] = target;
    }
}

void host_dsp1_get_main_volume(int idx, float* current, float* target) {
    if (idx >= 0 && idx < 3) {
        if (current) *current = dsp.mainVolume[idx];
        if (target) *target = dsp.mainVolumeSet[idx];
    }
}

void host_dsp1_call_audioSmoothVolume(void) {
    /* Calls real production audioSmoothVolume() from audio.c */
    audioSmoothVolume();
}

float host_dsp1_get_peq_coeff(int ch, int idx) {
    if (ch >= 0 && ch < CHANNELS_WITH_4BD_EQ && idx >= 0 && idx < (5 * EQ_4BD_BANDS)) {
        return dsp.peqCoeffs_4BD_EQ[ch][idx];
    }
    return 0.0f;
}

/*
 * Proposed Seam 1: Gate block algorithm extracted from audio.c:496-554.
 * Resolves misplaced optimize_for_speed pragma at audio.c:508, 544.
 */
void host_dsp1_gate_process_block(float* samples, HostGate* gate, float* p_env) {
    float env = *p_env;
    int holdTimer = gate->holdTimer;
    float threshold = gate->value_threshold;
    float refValue = 0.0f;

    for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
        refValue += samples[s] * samples[s];
    }
    refValue = sqrtf(refValue * 0.0625f);

    float targetGainLinear = (refValue < threshold) ? 0.0f : 1.0f;
    float coeff = 0.0f;

    if (targetGainLinear > env) {
        coeff = gate->value_coeff_attack;
        holdTimer = (int)gate->value_hold_ticks;
    } else {
        if (holdTimer > 0) {
            holdTimer--;
        } else {
            coeff = gate->value_coeff_release;
        }
    }

    gate->holdTimer = holdTimer;

    float a1 = 1.0f - coeff;
    float b0 = coeff * targetGainLinear;

    for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
        env = (env * a1) + b0;
        samples[s] *= env;
    }

    *p_env = env;
}

/*
 * Proposed Seam 2: Compressor block algorithm extracted from audio.c:605-660.
 */
void host_dsp1_compressor_process_block(const float* src, float* dst, HostCompressor* comp, float* p_env, float makeUp) {
    float env = *p_env;
    float coeff = 0.0f;
    float refValue = 0.0f;

    for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
        refValue += src[s] * src[s];
    }
    refValue = sqrtf(refValue * 0.0625f);

    refValue = linearToDb_fast(refValue * INT32_TO_FLOAT_NORM);
    float targetGainDb = 0.0f;
    if (refValue > comp->value_thresholdDb) {
        targetGainDb = (comp->value_thresholdDb - refValue) * comp->value_1_minus_1_by_ratio;
    }

    float targetGainLinear = dbToLinear_fast(targetGainDb);

    if (targetGainLinear < env) {
        coeff = comp->value_coeff_attack;
        comp->holdTimer = (int)comp->value_hold_ticks;
    } else {
        if (comp->holdTimer > 0) {
            comp->holdTimer--;
        } else {
            coeff = comp->value_coeff_release;
        }
    }

    for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
        env += coeff * (targetGainLinear - env);
        dst[s] = src[s] * env * makeUp;
    }

    *p_env = env;
}

/*
 * Proposed Seam 3: Mixbus 4-bus parallel accumulation extracted from audio.c:740-775.
 * Resolves misplaced optimize_for_speed pragma at audio.c:762.
 */
void host_dsp1_mixbus_accumulate_block(const float* src, const float gains[4], float* acc[4]) {
    float g0 = gains[0];
    float g1 = gains[1];
    float g2 = gains[2];
    float g3 = gains[3];

    float* acc0 = acc[0];
    float* acc1 = acc[1];
    float* acc2 = acc[2];
    float* acc3 = acc[3];

    for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
        float in_sample = src[s];
        acc0[s] += g0 * in_sample;
        acc1[s] += g1 * in_sample;
        acc2[s] += g2 * in_sample;
        acc3[s] += g3 * in_sample;
    }
}

/* Storage for spiCommData required by audio.c */
float spiCommData[400];
