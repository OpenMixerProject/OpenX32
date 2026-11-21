/*
  OpenX32 - ADSP-21371 SHARC Assembly Dynamics Kernels
  Copyright 2025-2026 OpenMixerProject (GPLv3)
*/

#ifndef __DYNAMICS_ASM_H__
#define __DYNAMICS_ASM_H__

#include "dsp1.h"

#ifdef __cplusplus
extern "C" {
#endif

// Flat 32-bit word-aligned parameter blocks for direct DM(offset, I3) access in SHARC ASM.
// Field offsets MUST stay in sync with dynamics_asm.asm!

typedef struct {
	float thresholdDb;       // offset 0  (in dBFS)
	float slope;             // offset 1  (1.0f - 1.0f / ratio)
	float makeup;            // offset 2  (linear)
	float coeffAttack;       // offset 3  (>= 0.999f => 0ms peak-lookahead mode)
	float holdTicks;         // offset 4
	float coeffRelease;      // offset 5
	float env;               // offset 6  (in/out state)
	int   holdTimer;         // offset 7  (in/out state)
} sCompAsmParams;

typedef struct {
	float threshold;         // offset 0  (linear threshold)
	float gainMin;           // offset 1  (linear min gain from Range)
	float coeffAttack;       // offset 2
	float holdTicks;         // offset 3
	float coeffRelease;      // offset 4
	float env;               // offset 5  (in/out state)
	int   holdTimer;         // offset 6  (in/out state)
} sGateAsmParams;

// C-callable ADSP-21371 assembly kernels (implemented in dynamics_asm.asm)
void fxCompressorBlock_asm(const float* src, float* dst, sCompAsmParams* params);
void fxGateBlock_asm(float* src_dst, sGateAsmParams* params);

#ifdef __cplusplus
}
#endif

#endif /* __DYNAMICS_ASM_H__ */
