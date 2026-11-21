/*
    ____                  __   ______ ___
   / __ \                 \ \ / /___ \__ \
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / /
  | |__| | |_) |  __/ | | |/ . \ ___) / /_
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |
         |_|

  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "fx.h"

float linearToDb(float linear) {
	if (linear <= 0.00001f) return -100.0f;
	return 20.0f * log10f(linear);
}

// fast approximation to 20.0f * log10(x)
float linearToDb_fast(float linear) {
    if (linear < 0.0000001f) return -140.0f; // Noise floor

    // uses the bit-structure if floats for a fast log2-approximation
    union { float f; uint32_t i; } vx = { linear };
    float log2_approx = (float)vx.i * 1.1920928955078125e-7f - 126.94269504f;

    // conversion of log2 to 20 * log10:
    // 20 * log10(x) = 20 * log2(x) * log10(2) ~ 6.0206 * log2(x)
    return 6.0206f * log2_approx;
}

float dbToLinear(float dB) {
    return powf(10.0f, dB * 0.05f);
}

// Fast 10*log10((sumOfSquares / 16) / (2^31 - 1)^2) without sqrtf()
// Max error < 0.025 dB, ~8 cycles on ADSP-21371
float powerToDb_fast(float sumOfSquares) {
    if (sumOfSquares <= 737870.0f) return -140.0f; // <= -140 dBFS noise floor

    union { float f; uint32_t i; } vx = { sumOfSquares };
    int32_t exp_val = (int32_t)((vx.i >> 23) & 0xFFu) - 127;
    vx.i = (vx.i & 0x007FFFFFu) | 0x3F800000u; // mantissa m in [1.0, 2.0)
    float m = vx.f;
    float log2_m = (m - 1.0f) * (1.6797000f - 0.3398500f * m);
    return 3.01029995664f * ((float)exp_val + log2_m) - 198.67979412f;
}

// Fast 10^(dB/20) = 2^(dB / 6.0206) with quadratic mantissa refinement
// Max error < 0.03 dB, ~10 cycles on ADSP-21371
float dbToLinear_fast(float dB) {
    if (dB <= -140.0f) return 0.0f;
    if (dB > 24.0f) dB = 24.0f;

    float x = dB * 0.16609640474f; // dB / (20 * log10(2))
    int32_t xi = (int32_t)x;
    if ((float)xi > x) xi--;
    float xf = x - (float)xi; // fractional part in [0.0, 1.0)

    union { float f; uint32_t i; } scale;
    scale.i = (uint32_t)(xi + 127) << 23;
    return scale.f * (1.0f + xf * (0.65685425f + 0.34314575f * xf));
}

float calcRMS(const float* __restrict src) {
    float sumOfSquares = 0.0f;

    #pragma loop_count(16, 16, 16)
    #pragma vector_for
    for (int s = 0; s < 16; s++) {
        float sample = src[s];
        sumOfSquares += sample * sample;
    }

    float meanSquare = sumOfSquares * 0.0625f;

    return sqrtf(meanSquare);
}

void fxSetPeqCoeffs(int channel, int index, float coeffs[]) {
	// biquad_trans() needs the coeffs in the following order
	// a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 0/1)
	// a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 2/3)
	// a0 a1 a2 b1 b2 (section 4)
	if (index >= EQ_4BD_BANDS) {
		return;
	}

	// interleave coefficients for biquad_trans()
	if (((EQ_4BD_BANDS % 2) == 0) || (index < (EQ_4BD_BANDS - 1))) {
		// we have even number of PEQ-sections
		// or we have odd number but we are still below the last section

		// store data with interleaving
		int sectionIndex = ((index / 2) * 2) * 5;
		if ((index % 2) != 0) {
			// odd section index
			sectionIndex += 1;
		}
		dsp.peqCoeffs_4BD_EQ[channel][sectionIndex + 0] = coeffs[0]; // a0 (zeros)
		dsp.peqCoeffs_4BD_EQ[channel][sectionIndex + 2] = coeffs[1]; // a1 (zeros)
		dsp.peqCoeffs_4BD_EQ[channel][sectionIndex + 4] = coeffs[2]; // a2 (zeros)
		dsp.peqCoeffs_4BD_EQ[channel][sectionIndex + 6] = -coeffs[3]; // -b1 (poles)
		dsp.peqCoeffs_4BD_EQ[channel][sectionIndex + 8] = -coeffs[4]; // -b2 (poles)
	}else{
		// last section: store without interleaving
		int sectionIndex = (EQ_4BD_BANDS - 1) * 5;
		dsp.peqCoeffs_4BD_EQ[channel][sectionIndex + 0] = coeffs[0]; // a0 (zeros)
		dsp.peqCoeffs_4BD_EQ[channel][sectionIndex + 1] = coeffs[1]; // a1 (zeros)
		dsp.peqCoeffs_4BD_EQ[channel][sectionIndex + 2] = coeffs[2]; // a2 (zeros)
		dsp.peqCoeffs_4BD_EQ[channel][sectionIndex + 3] = -coeffs[3]; // -b1 (poles)
		dsp.peqCoeffs_4BD_EQ[channel][sectionIndex + 4] = -coeffs[4]; // -b2 (poles)
	}
}

/*
void fxSmoothCoeffs(void) {
	// this function is called every 333µs
	float value;
	float coeff = (3.0f / 3000.0f); // 0.3s until value is taken
	for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
		dsp.lowcutCoeff[i_ch] = dsp.lowcutCoeff[i_ch] + coeff * (dsp.lowcutCoeffSet[i_ch] - dsp.lowcutCoeff[i_ch]);

		for (int i = 0; i < (5 * MAX_CHAN_EQS); i++) {
			// calculate PT1: out = out + 0.1 * (set - out);
			dsp.dspChannel[i_ch].peqCoeffs[i] = dsp.dspChannel[i_ch].peqCoeffs[i] + coeff * (dsp.dspChannel[i_ch].peqCoeffsSet[i] - dsp.dspChannel[i_ch].peqCoeffs[i]);
		}
	}
}
*/
