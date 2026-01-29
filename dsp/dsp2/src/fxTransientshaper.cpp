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

#pragma file_attr("prefersMem=internal") // let the linker know, that all variables should be placed into the internal ram

// small helper-function that converts milliseconds to k-factor
float ms_to_k(float ms, float fs) {
    if (ms <= 0.0f) return 1.0f;
    return 1.0f - expf(-1.0f / (fs * (ms / 1000.0f)));
}

fxTransientshaper::fxTransientshaper(int fxSlot, int channelMode) : fx(fxSlot, channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	// initialize delay-lines in external memory
	_delayLine = (float*)(_memoryAddress);
	_memoryAddress += (FX_TRANSIENTSHAPER_BUFFER_SIZE * sizeof(float));

	// set memory content to zero
	//clearMemory(); // TODO: check if this is taking too much time
	for (int i = 0; i < FX_TRANSIENTSHAPER_BUFFER_SIZE; i++) {
		_delayLine[i] = 0.0f;
	}

	// initialize parameter variables
	// high attack (timeFast seems be fine at delayMs*2 to move the maximum gain in the near of the real audio-peak)
	setParameters(1, 15, 150, 3, 1.0, 1); // timeFast, timeMed, timeSlow, attack, sustain, delayMs

	// high sustain
	//fxTransientshaperSetParameters(0.5, 15, 150, 1.0, 3, 1); // timeFast, timeMed, timeSlow, attack, sustain, delayMs

	// internal variables
	_delayLineHead = 0;
	_envelopeFast = 0;
	_envelopeMed = 0;
	_envelopeSlow = 0;
}

fxTransientshaper::~fxTransientshaper() {
    // destructor
}

void fxTransientshaper::setParameters(float timeFast, float timeMed, float timeSlow, float attack, float sustain, float delayMs) {
	_kFast = ms_to_k(timeFast, _sampleRate); // attack-envelope: 0.05 = softer response, 0.2 = fast on steep edges
	_kMed = ms_to_k(timeMed, _sampleRate);
	_kSlow = ms_to_k(timeSlow, _sampleRate); // sustain-envelope: 0.05 = short boost, 0.001 = wide punch
	_attack = attack; // amount of the attack: <1 = less attack, 1 = Neutral, >1 = more attack
	_sustain = sustain; // amount of the sustain: <1 = less sustain, 1 = Neutral, >1 = more sustain

	if (delayMs < FX_TRANSIENTSHAPER_DELAY_MS_MAX) {
		_delayLineTailOffset = (delayMs * _sampleRate * 0.001f); // 1ms
	}
}

void fxTransientshaper::rxData(float data[], int len) {
	// data received from x32ctrl
}

void fxTransientshaper::process(float* __restrict bufIn[], float* __restrict bufOut[]) {
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		// Step 1: write current sample in delay-line
		_delayLine[_delayLineHead] = bufIn[0][s];
		_delayLineHead++;
		if (_delayLineHead >= FX_TRANSIENTSHAPER_BUFFER_SIZE) {
			_delayLineHead = 0;
		}

		// Step 2: calculate envelope-curve based on current sample (look-ahead as we are using a delay-line with ~1ms)
		float abs_x = fabsf(bufIn[0][s]);
		_envelopeFast += _kFast * (abs_x - _envelopeFast);
		_envelopeMed  += _kMed  * (abs_x - _envelopeMed);
		_envelopeSlow += _kSlow * (abs_x - _envelopeSlow);

		float transientGain;
		if ((_envelopeMed != 0) && (_envelopeSlow != 0)) {
			// Step 2.1: attack-gain: compare fast-envelope with medium-envelope
			// so it reacts only on the very first attack-front
			float attackRatio = _envelopeFast / _envelopeMed;
			float attackGain = 1.0f + (attackRatio - 1.0f) * (_attack - 1.0f);

			// Step 2.2: sustain-gain: compares medium-envelope with slow-envelope
			// so it reacts only on the wide decay of the sound
			float sustainRatio = _envelopeMed / _envelopeSlow;
			float sustainGain = 1.0f + (sustainRatio - 1.0f) * (_sustain - 1.0f);

			// Step 2.3: calculate new gain-factor
			transientGain = attackGain * sustainGain;
		}else{
			// backup to avoid DIV/0
			transientGain = 1.0;
		}

		// Step 3: read the delayed signal from delay-line
		int tail = _delayLineHead - _delayLineTailOffset;
		while (tail < 0) {
			tail += FX_TRANSIENTSHAPER_BUFFER_SIZE;
		}
		float sampleDelayed = _delayLine[tail];

		// Step 4: use gain on delayed sample
		bufOut[0][s] = sampleDelayed * transientGain;
		bufOut[1][s] = sampleDelayed * transientGain;

		// bypass
		//bufOut[0][s] = bufIn[0][s];
		//bufOut[1][s] = bufIn[1][s];
	}
}
