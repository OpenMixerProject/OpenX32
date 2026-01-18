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

fxTransientshaper::fxTransientshaper() { } // we are not using the default constructor here but CCES complains when its missing

fxTransientshaper::fxTransientshaper(int fxSlot, int channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	// initialize delay-lines in external memory
	_delayLine = (float*)(_memoryAddress);
	_memoryAddress += (FX_TRANSIENTSHAPER_BUFFER_SIZE * sizeof(float));

	// set memory content to zero
	//clearMemory(); // TODO: check if this is taking too much time

	// initialize parameter variables
	fxTransientshaperSetParameters(0.15, 0.01, 1.5, 1.5, 1.0); // kFast, kSlow, attack, sustain, delay

	// internal variables
	_delayLineHead = 0;
	_envelopeFast = 0;
	_envelopeSlow = 0;
}

fxTransientshaper::~fxTransientshaper() {
    // destructor
}

void fxTransientshaper::fxTransientshaperSetParameters(float kFast, float kSlow, float attack, float sustain, float delayMs) {
	_kFast = kFast; // attack-envelope: 0.05 = softer response, 0.2 = fast on steep edges
	_kSlow = kSlow; // sustain-envelope: 0.05 = short boost, 0.001 = wide punch
	_attack = attack; // amount of the attack: <1 = less attack, 1 = Neutral, >1 = more attack
	_sustain = sustain; // amount of the sustain: <1 = less sustain, 1 = Neutral, >1 = more sustain

	if (delayMs < FX_TRANSIENTSHAPER_DELAY_MS_MAX) {
		_delayLineTailOffset = (delayMs * _sampleRate * 0.001f); // 1ms
	}
}

void fxTransientshaper::rxData(float data[], int len) {
	// data received from x32ctrl
}

void fxTransientshaper::process(float* bufIn[], float* bufOut[]) {
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		// Step 1: write current sample in delay-line
		_delayLine[_delayLineHead] = bufIn[0][s];
		_delayLineHead++;
		if (_delayLineHead >= FX_TRANSIENTSHAPER_BUFFER_SIZE) {
			_delayLineHead = 0;
		}

		// Step 2: calculate envelope-curve based on current sample
		float abs_x = fabsf(bufIn[0][s]);
		_envelopeFast += _kFast * (abs_x - _envelopeFast);
		_envelopeSlow += _kSlow * (abs_x - _envelopeSlow);

		// Step 3: calculate gain-factor via ratio
		float finalGain = 1.0f;
		float ratio = (_envelopeFast + 1e-6f) / (_envelopeSlow + 1e-6f);
		if (ratio > 1.0f) {
			// ATTACK-Logic, when the signal is steeper than the average
			finalGain += (ratio - 1.0f) * (_attack - 1.0f);
		} else {
			// SUSTAIN-Logic: when signal is more silent than the average

			// we are using reciprocal of ratio to boost the decay
			// sustain > 1.0 rises level, even it is falling in reality
			finalGain += (1.0f - ratio) * (_sustain - 1.0f);
		}
		if (finalGain > 3.0f) finalGain = 3.0f; // safety-limit for sustain-boost

		// Step 4: read the delayed signal from delay-line
		int tail = _delayLineHead - _delayLineTailOffset;
		if (tail < 0) {
			tail += FX_TRANSIENTSHAPER_BUFFER_SIZE;
		}
		float sampleDelayed = _delayLine[tail];

		// Step 5: use gain on delayed sample
		bufOut[0][s] = sampleDelayed * finalGain;

		// bypass
		//bufOut[s] = bufIn[s];
	}
}
