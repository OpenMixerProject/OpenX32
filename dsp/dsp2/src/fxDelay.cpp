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

  This file implements a demo-plugin as template for new plugins
*/

#include "fxDelay.h"

#pragma file_attr("prefersMem=internal") // let the linker know, that all variables should be placed into the internal ram

fxDelay::fxDelay(int fxSlot, int channelMode) : fx(fxSlot, channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	// calculate the maximum amount of space we need in the external RAM for the maximum samplerate we are supporting
	_delayLineLengthMaxMs = 500;
	_delayLineBufferSize = ((SAMPLERATE_MAX * _delayLineLengthMaxMs) / 1000);

	// set default effect parameters
	//setParameters(350, 450); // set delay of 450ms
	_delayLineTailOffsetL = 16800;
	_delayLineTailOffsetR = 21600;

	// initialize delay-lines in external memory
	_delayLineL = (float*)(_memoryAddress);
	_memoryAddress += (_delayLineBufferSize * sizeof(float));
	_delayLineR = (float*)(_memoryAddress);
	_memoryAddress += (_delayLineBufferSize * sizeof(float));

	// set internal parameters
	_delayLineHead = 0;
}

fxDelay::~fxDelay() {
    // destructor
}

// human-friendly parameter-settings, but more expensive for the DSP
void fxDelay::setParameters(float delayMsL, float delayMsR) {
	if (delayMsL < _delayLineLengthMaxMs) {
		_delayLineTailOffsetL = (delayMsL * _sampleRate * 0.001f);
	}
	if (delayMsR < _delayLineLengthMaxMs) {
		_delayLineTailOffsetR = (delayMsR * _sampleRate * 0.001f);
	}
}

void fxDelay::rxData(float data[], int len) {
	// data received from x32ctrl
	if (len != 2) return;

	_delayLineTailOffsetL = data[0];
	_delayLineTailOffsetR = data[1];
}

void fxDelay::process(float* __restrict bufIn[], float* __restrict bufOut[]) {
	if (_startup) {
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			bufOut[0][s] = 0;
			bufOut[1][s] = 0;

			_delayLineL[_delayLineHead] = 0;
			_delayLineR[_delayLineHead] = 0;
			_delayLineHead++;
			if (_delayLineHead >= _delayLineBufferSize) {
				_delayLineHead = 0;
				_startup = false;
			}
		}

		return;
	}

	int tail;
	float delayedSampleL;
	float delayedSampleR;

	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
	    // Step 1: read sample from delayLine
		tail = _delayLineHead - _delayLineTailOffsetL;
		while (tail < 0) {
			tail += _delayLineBufferSize;
		}
		delayedSampleL = _delayLineL[tail];

		tail = _delayLineHead - _delayLineTailOffsetR;
		while (tail < 0) {
			tail += _delayLineBufferSize;
		}
		delayedSampleR = _delayLineR[tail];



		// Step 2: output data
	    bufOut[0][s] = delayedSampleL;
	    bufOut[1][s] = delayedSampleR;



	    // Step 3: write sample back to delayLine with decayed feedback
		_delayLineL[_delayLineHead] = bufIn[0][s] + _delayLineL[_delayLineHead] * 0.3f; // _feedbackDecayGain = powf(10, _dbPerCycle * 0.05f); // decay = 10^(dBperCycle/20)  ->  -1.5dB/cycle = x0.85
		_delayLineR[_delayLineHead] = bufIn[1][s] + _delayLineR[_delayLineHead] * 0.3f; // _feedbackDecayGain = powf(10, _dbPerCycle * 0.05f); // decay = 10^(dBperCycle/20)  ->  -1.5dB/cycle = x0.85
		_delayLineHead++;
		if (_delayLineHead >= _delayLineBufferSize) {
			_delayLineHead = 0;
		}
	}
}
