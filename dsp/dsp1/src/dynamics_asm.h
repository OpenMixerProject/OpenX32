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

// Flat 32-bit word-aligned parameter blocks for direct DM(offset, I4) access in SHARC ASM.
// Field offsets MUST stay in sync with dynamics_asm.asm!

typedef struct {
	float thresholdDb;       // offset 0
	float slope;             // offset 1  (1.0f - 1.0f / ratio)
	float makeup;            // offset 2  (linear)
	float coeffAttack;       // offset 3  (>= 0.999f => 0ms peak-lookahead mode)
	float holdTicks;         // offset 4
	float coeffRelease;      // offset 5
	float kneeHalf;          // offset 6  (0.5f * kneeWidthDb)
	float kneeCoeff;         // offset 7  (-slope / (2.0f * kneeWidthDb), or 0.0f)
	float env;               // offset 8  (in/out state)
	int   holdTimer;         // offset 9  (in/out state)
} sCompAsmParams;

typedef struct {
	float thresholdSq16;     // offset 0  (16 * threshold_lin^2)
	float gainMin;           // offset 1  (linear min gain from Range)
	float coeffAttack;       // offset 2
	float holdTicks;         // offset 3
	float coeffRelease;      // offset 4
	float thresholdDb;       // offset 5
	float negRangeDb;        // offset 6
	float expanderSlope;     // offset 7  (0.0f = hard Gate, 1/2/3 = Exp 2:1/3:1/4:1)
	float env;               // offset 8  (in/out state)
	int   holdTimer;         // offset 9  (in/out state)
} sGateAsmParams;

// C-callable ADSP-21371 assembly kernels (implemented in dynamics_asm.asm)
void fxCompressorBlock_asm(const float* src, float* dst, sCompAsmParams* params);
void fxGateBlock_asm(float* src_dst, sGateAsmParams* params);

#ifdef __cplusplus
}
#endif

#endif /* __DYNAMICS_ASM_H__ */
