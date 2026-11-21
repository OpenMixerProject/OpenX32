#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Forward declarations matching dsp1 types */
typedef struct {
    float value_threshold;
    float value_gainmin;
    float value_coeff_attack;
    float value_hold_ticks;
    float value_coeff_release;
    int holdTimer;
} HostGate;

typedef struct {
    float value_thresholdDb;
    float value_1_minus_1_by_ratio;
    float value_coeff_attack;
    float value_hold_ticks;
    float value_coeff_release;
    int holdTimer;
} HostCompressor;

/* Test harness management */
void host_dsp1_init(void);
void host_dsp1_reset(void);

/* Direct production audioSmoothVolume() in audio.c */
void host_dsp1_set_main_volume(int idx, float current, float target);
void host_dsp1_get_main_volume(int idx, float* current, float* target);
void host_dsp1_call_audioSmoothVolume(void);

/* Direct PEQ coefficient access from production fx.c */
float host_dsp1_get_peq_coeff(int ch, int idx);

/* Proposed minimal production seams for extracted warning-affected routines */
void host_dsp1_gate_process_block(float* samples, HostGate* gate, float* p_env);
void host_dsp1_compressor_process_block(const float* src, float* dst, HostCompressor* comp, float* p_env, float makeUp);
void host_dsp1_mixbus_accumulate_block(const float* src, const float gains[4], float* acc[4]);

#ifdef __cplusplus
}
#endif
