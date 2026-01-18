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

fxChorus::fxChorus() { } // we are not using the default constructor here but CCES complains when its missing

fxChorus::fxChorus(int fxSlot, int channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	// calculate the maximum amount of space we need in the external RAM for the maximum samplerate we are supporting
	_delayLineLengthMaxMs = 50;
	_delayLineBufferSize = ((SAMPLERATE_MAX * _delayLineLengthMaxMs) / 1000);

	// set default effect parameters
	fxChorusSetParameters(10, 10, 15, 20, 1.5, 1.6, 0, 0, 0.5); // depthA, depthB, delayA, delayB, freqA, freqB, phaseA, phaseB, mix

	// initialize delay-lines in external memory
	_delayLineA = (float*)(_memoryAddress);
	_memoryAddress += (_delayLineBufferSize * sizeof(float));
	_delayLineB = (float*)(_memoryAddress);
	_memoryAddress += (_delayLineBufferSize * sizeof(float));

	// set memory content to zero
	//clearMemory(); // TODO: check if this is taking too much time

	// set internal parameters
	_delayLineHeadA = 0;
	_delayLineHeadB = 0;
}

fxChorus::~fxChorus() {
    // destructor
}

void fxChorus::fxChorusSetParameters(float depthA, float depthB, float delayA, float delayB, float freqA, float freqB, float phaseA, float phaseB, float mix) {
	// depth of effect
	_depthA = depthA;
	_depthB = depthB;

	// general delay
	_delayLineBaseLengthA = (int)(delayA * _sampleRate * 0.001f); // general delay = 15ms
	_delayLineBaseLengthB = (int)(delayB * _sampleRate * 0.001f); // general delay = 20ms

	// phase between both channels
	_lfoPhaseA = phaseA;
	_lfoPhaseB = phaseB;

	// modulation-frequency of chorus
	_phaseIncA = (2.0f * M_PI * freqA) / _sampleRate;
	_phaseIncB = (2.0f * M_PI * freqB) / _sampleRate;

	// dry <-> wet mix-factor
	_mix = mix;
}

void fxChorus::rxData(float data[], int len) {
	// data received from x32ctrl
}

void fxChorus::process(float* bufIn[], float* bufOut[]) {
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
	    float inL = bufIn[0][s];
	    float inR = bufIn[1][s];

	    // write sample into delay-line
	    _delayLineA[_delayLineHeadA] = inL;
	    _delayLineB[_delayLineHeadB] = inR;

	    // Step 1: get LFO value
	    float modA = sinf(_lfoPhaseA);
	    float modB = sinf(_lfoPhaseB);

	    // Step 2: calculate delay as float
	    float delaySamplesA = _delayLineBaseLengthA + (_depthA * modA);
	    float delaySamplesB = _delayLineBaseLengthB + (_depthB * modB);

	    // Step 3: calculate read-position still as float
	    float readPosA = (float)_delayLineHeadA - delaySamplesA;
	    float readPosB = (float)_delayLineHeadB - delaySamplesB;
	    if (readPosA < 0) readPosA += _delayLineBufferSize;
	    if (readPosB < 0) readPosB += _delayLineBufferSize;

	    // Step 4: perform linear interpolation
	    int idxA1 = (int)readPosA;
	    int idxA2 = (idxA1 + 1 >= _delayLineBufferSize) ? 0 : idxA1 + 1;
	    float fracA = readPosA - (float)idxA1; // fractional part (0.0 ... 1.0)
	    int idxB1 = (int)readPosB;
	    int idxB2 = (idxB1 + 1 >= _delayLineBufferSize) ? 0 : idxB1 + 1;
	    float fracB = readPosB - (float)idxB1;

	    // calculate value between the real samples: Sample1 + frac * (Sample2 - Sample1)
	    float chorusL = _delayLineA[idxA1] + fracA * (_delayLineA[idxA2] - _delayLineA[idxA1]);
	    float chorusR = _delayLineB[idxB1] + fracB * (_delayLineB[idxB2] - _delayLineB[idxB1]);

	    // Step 5: Mixing & Output
	    float dryGain = 1.0f - _mix;
	    float wetGain = _mix;
	    bufOut[0][s] = fclipf((dryGain * inL) + (wetGain * chorusL), 2147483647.0f);
	    bufOut[1][s] = fclipf((dryGain * inR) + (wetGain * chorusR), 2147483647.0f);

	    // Step 6: update Pointer & Phase
	    _delayLineHeadA = (_delayLineHeadA + 1 >= _delayLineBufferSize) ? 0 : _delayLineHeadA + 1;
	    _delayLineHeadB = (_delayLineHeadB + 1 >= _delayLineBufferSize) ? 0 : _delayLineHeadB + 1;

	    _lfoPhaseA += _phaseIncA;
	    if (_lfoPhaseA >= 6.283185307f) _lfoPhaseA -= 6.283185307f;

	    _lfoPhaseB += _phaseIncB;
	    if (_lfoPhaseB >= 6.283185307f) _lfoPhaseB -= 6.283185307f;
	}
}
