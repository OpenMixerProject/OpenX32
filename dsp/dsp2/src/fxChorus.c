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
  Copyright 2025-2026 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  This file implements a nice chorus with plenty of options to tune the effect
*/

#include "fxChorus.h"

#if FX_USE_CHORUS == 1

float delayLineA[FX_CHORUS_BUFFER_SIZE];
float delayLineB[FX_CHORUS_BUFFER_SIZE];

void fxChorusInit(void) {
	// effect parameters
	fxChorusSetParameters(10, 10, 15, 20, 1.5, 1.6, 0, 0, 0.5); // depthA, depthB, delayA, delayB, freqA, freqB, phaseA, phaseB, mix

	// set internal parameters
	chorus.delayLineHeadA = 0;
	chorus.delayLineHeadB = 0;
}

void fxChorusSetParameters(float depthA, float depthB, float delayA, float delayB, float freqA, float freqB, float phaseA, float phaseB, float mix) {
	// depth of effect
	chorus.depthA = depthA;
	chorus.depthB = depthB;

	// general delay
	chorus.delayLineBaseLengthA = (int)(delayA * dsp.samplerate * 0.001f); // general delay = 15ms
	chorus.delayLineBaseLengthB = (int)(delayB * dsp.samplerate * 0.001f); // general delay = 20ms

	// phase between both channels
	chorus.lfoPhaseA = phaseA;
	chorus.lfoPhaseB = phaseB;

	// modulation-frequency of chorus
	chorus.phaseIncA = (2.0f * M_PI * freqA) / dsp.samplerate;
	chorus.phaseIncB = (2.0f * M_PI * freqB) / dsp.samplerate;

	// dry <-> wet mix-factor
	chorus.mix = mix;
}

void fxChorusProcess(float* bufIn[2], float* bufOut[2]) {
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
	    float inL = bufIn[0][s];
	    float inR = bufIn[1][s];

	    // write sample into delay-line
	    delayLineA[chorus.delayLineHeadA] = inL;
	    delayLineB[chorus.delayLineHeadB] = inR;

	    // Step 1: get LFO value
	    float modA = sinf(chorus.lfoPhaseA);
	    float modB = sinf(chorus.lfoPhaseB);

	    // Step 2: calculate delay as float
	    float delaySamplesA = chorus.delayLineBaseLengthA + (chorus.depthA * modA);
	    float delaySamplesB = chorus.delayLineBaseLengthB + (chorus.depthB * modB);

	    // Step 3: calculate read-position still as float
	    float readPosA = (float)chorus.delayLineHeadA - delaySamplesA;
	    float readPosB = (float)chorus.delayLineHeadB - delaySamplesB;
	    if (readPosA < 0) readPosA += FX_CHORUS_BUFFER_SIZE;
	    if (readPosB < 0) readPosB += FX_CHORUS_BUFFER_SIZE;

	    // Step 4: perform linear interpolation
	    int idxA1 = (int)readPosA;
	    int idxA2 = (idxA1 + 1 >= FX_CHORUS_BUFFER_SIZE) ? 0 : idxA1 + 1;
	    float fracA = readPosA - (float)idxA1; // fractional part (0.0 ... 1.0)
	    int idxB1 = (int)readPosB;
	    int idxB2 = (idxB1 + 1 >= FX_CHORUS_BUFFER_SIZE) ? 0 : idxB1 + 1;
	    float fracB = readPosB - (float)idxB1;

	    // calculate value between the real samples: Sample1 + frac * (Sample2 - Sample1)
	    float chorusL = delayLineA[idxA1] + fracA * (delayLineA[idxA2] - delayLineA[idxA1]);
	    float chorusR = delayLineB[idxB1] + fracB * (delayLineB[idxB2] - delayLineB[idxB1]);

	    // Step 5: Mixing & Output
	    float dryGain = 1.0f - chorus.mix;
	    float wetGain = chorus.mix;
	    bufOut[0][s] = fclipf((dryGain * inL) + (wetGain * chorusL), 2147483647.0f);
	    bufOut[1][s] = fclipf((dryGain * inR) + (wetGain * chorusR), 2147483647.0f);

	    // Step 6: update Pointer & Phase
	    chorus.delayLineHeadA = (chorus.delayLineHeadA + 1 >= FX_CHORUS_BUFFER_SIZE) ? 0 : chorus.delayLineHeadA + 1;
	    chorus.delayLineHeadB = (chorus.delayLineHeadB + 1 >= FX_CHORUS_BUFFER_SIZE) ? 0 : chorus.delayLineHeadB + 1;

	    chorus.lfoPhaseA += chorus.phaseIncA;
	    if (chorus.lfoPhaseA >= 6.283185307f) chorus.lfoPhaseA -= 6.283185307f;

	    chorus.lfoPhaseB += chorus.phaseIncB;
	    if (chorus.lfoPhaseB >= 6.283185307f) chorus.lfoPhaseB -= 6.283185307f;
	}
}

#endif
