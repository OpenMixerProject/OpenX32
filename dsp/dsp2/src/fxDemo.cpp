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

#include "fxDemo.h"

#pragma file_attr("prefersMem=internal") // let the linker know, that all variables should be placed into the internal ram

fxDemo::fxDemo(int fxSlot, float* bufIn[], float* bufOut[], int channelMode) : fx(fxSlot, bufIn, bufOut, channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	// get the pointers to the sample-buffers
	_bufIn[0] = bufIn[0];
	_bufIn[1] = bufIn[1];
	_bufOut[0] = bufOut[0];
	_bufOut[1] = bufOut[1];

	// calculate the maximum amount of space we need in the external RAM for the maximum samplerate we are supporting
	_delayLineLengthMaxMs = 1000;
	_delayLineBufferSize = ((SAMPLERATE_MAX * _delayLineLengthMaxMs) / 1000);

	// set default effect parameters
	setParameters(350); // set delay of 350ms

	// initialize delay-lines in external memory
	_delayLineL = (float*)(_memoryAddress);
	_memoryAddress += (_delayLineBufferSize * sizeof(float));
	_delayLineR = (float*)(_memoryAddress);
	_memoryAddress += (_delayLineBufferSize * sizeof(float));

	// set internal parameters
	_delayLineHead = 0;
}

fxDemo::~fxDemo() {
    // destructor
}

void fxDemo::setParameters(float delayMs) {
	if (delayMs < _delayLineLengthMaxMs) {
		_delayLineTailOffset = (delayMs * _sampleRate * 0.001f);
	}
}

void fxDemo::rxData(float data[], int len) {
	// data received from x32ctrl
	if (len != 1) return;

	setParameters(data[0]);
}

void fxDemo::process() {
	if (_startup) {
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			_bufOut[0][s] = 0;
			_bufOut[1][s] = 0;

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
    float sampleL;
    float sampleR;

	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
	    // Step 1: read sample from delayLine
		tail = _delayLineHead - _delayLineTailOffset;
		while (tail < 0) {
			tail += _delayLineBufferSize;
		}
		sampleL = _delayLineL[tail];
		sampleR = _delayLineR[tail];

	    // Step 2: process delayed data
		// do something here

	    // Step 3: output samples
	    _bufOut[0][s] = sampleL;
	    _bufOut[1][s] = sampleR;

	    // Step 4: write sample to delayLine
		_delayLineL[_delayLineHead] = _bufIn[0][s];
		_delayLineR[_delayLineHead] = _bufIn[1][s];
		_delayLineHead++;
		if (_delayLineHead >= _delayLineBufferSize) {
			_delayLineHead = 0;
		}
	}
}
