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

  This file implements a simple transient shaper with settings
  for attack and sustain
*/

#include "fxTransientshaper.h"

#if FX_USE_TRANSIENTSHAPER == 1

float* delayLine;

void fxTransientshaperInit(void) {
	// map memory-pointer to desired address in external RAM (please let me know if you know a better option)
	delayLine = (float*)(memoryAddress);
	memoryAddress += (FX_TRANSIENTSHAPER_BUFFER_SIZE * sizeof(float));

	// initialize memory
	// TODO: check if we can use memset() here
	for (int b = 0; b < FX_TRANSIENTSHAPER_BUFFER_SIZE; b++) {
		delayLine[b] = 0.0f;
	}

	// initialize parameter variables
	fxTransientshaperSetParameter(0.15, 0.01, 1.5, 1.5, 1.0); // kFast, kSlow, attack, sustain, delay

	// internal variables
	transientshaper.delayLineHead = 0;
	transientshaper.envelopeFast = 0;
	transientshaper.envelopeSlow = 0;
}

void fxTransientshaperSetParameter(float kFast, float kSlow, float attack, float sustain, float delayMs) {
	transientshaper.kFast = kFast; // attack-envelope: 0.05 = softer response, 0.2 = fast on steep edges
	transientshaper.kSlow = kSlow; // sustain-envelope: 0.05 = short boost, 0.001 = wide punch
	transientshaper.attack = attack; // amount of the attack: <1 = less attack, 1 = Neutral, >1 = more attack
	transientshaper.sustain = sustain; // amount of the sustain: <1 = less sustain, 1 = Neutral, >1 = more sustain

	if (delayMs < FX_TRANSIENTSHAPER_DELAY_MS_MAX) {
		transientshaper.delayLineTailOffset = (delayMs * dsp.samplerate * 0.001f); // 1ms
	}
}

void fxTransientshaperProcess(float* bufIn, float* bufOut) {
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		// Step 1: write current sample in delay-line
		delayLine[transientshaper.delayLineHead] = bufIn[s];
		transientshaper.delayLineHead++;
		if (transientshaper.delayLineHead >= FX_TRANSIENTSHAPER_BUFFER_SIZE) {
			transientshaper.delayLineHead = 0;
		}

		// Step 2: calculate envelope-curve based on current sample
		float abs_x = fabsf(bufIn[s]);
		transientshaper.envelopeFast += transientshaper.kFast * (abs_x - transientshaper.envelopeFast);
		transientshaper.envelopeSlow += transientshaper.kSlow * (abs_x - transientshaper.envelopeSlow);

		// Step 3: calculate gain-factor via ratio
		float finalGain = 1.0f;
		float ratio = (transientshaper.envelopeFast + 1e-6f) / (transientshaper.envelopeSlow + 1e-6f);
		if (ratio > 1.0f) {
			// ATTACK-Logic, when the signal is steeper than the average
			finalGain += (ratio - 1.0f) * (transientshaper.attack - 1.0f);
		} else {
			// SUSTAIN-Logic: when signal is more silent than the average

			// we are using reciprocal of ratio to boost the decay
			// sustain > 1.0 rises level, even it is falling in reality
			finalGain += (1.0f - ratio) * (transientshaper.sustain - 1.0f);
		}
		if (finalGain > 3.0f) finalGain = 3.0f; // safety-limit for sustain-boost

		// Step 4: read the delayed signal from delay-line
		int tail = transientshaper.delayLineHead - transientshaper.delayLineTailOffset;
		if (tail < 0) {
			tail += FX_TRANSIENTSHAPER_BUFFER_SIZE;
		}
		float sampleDelayed = delayLine[tail];

		// Step 5: use gain on delayed sample
		bufOut[s] = sampleDelayed * finalGain;

		// bypass
		//bufOut[s] = bufIn[s];
	}
}

#endif
