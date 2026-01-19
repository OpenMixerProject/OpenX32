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

  This file implements a simple matrix-upmixer that uses the information
  of left and right channel to calculate the information for the center
  by using basic math (Center = (L+R)/2). The surround-speakers are calculated using
  basic math as well (Back = (L-R)), combined with a delay-line and a low-pass-filter
  with a corner-frequency of 7kHz to keep the focus on the front-speakers.

  The LFE-channel is calculated based on the center-channel together with a lowpass
  filter at 120Hz. All settings can be changed in the header file or down below in
  the header of this file.
*/

#include "fxMatrixUpmixer.h"

fxMatrixUpmixer::fxMatrixUpmixer(int fxSlot, int channelMode) : fx(fxSlot, channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	// calculate the maximum amount of space we need in the external RAM for the maximum samplerate we are supporting
	_delayLineLengthMaxMs = 25;
	_delayLineBufferSize = ((SAMPLERATE_MAX * _delayLineLengthMaxMs) / 1000);

	// initialize delay-lines in external memory
	_delayLine = (float*)(_memoryAddress);
	_memoryAddress += (_delayLineBufferSize * sizeof(float));

	// set memory content to zero
	//clearMemory(); // TODO: check if this is taking too much time

	// set internal parameters
	_delayLineHead = 0;

	_lowPassSubState = 0;
	_lowPassSubCoeff = 0.01609904178227480397989f; // 125Hz = 785.398163397448 / 48785.398163397448 <- alpha = (2 * pi * f_c) / (f_s + 2 * pi * f_c) = (2 * pi * 125Hz) / (48000Hz + 2 * pi * 125Hz)
	_lowPassSurroundState = 0;
	_lowPassSurroundCoeff = 0.4781604560104657892f; // 7kHz = 43982,297150257105338477007365913 / 91982,297150257105338477007365913 <- alpha = (2 * pi * f_c) / (f_s + 2 * pi * f_c) = (2 * pi * 7000Hz) / (48000Hz + 2 * pi * 7000Hz)
}

fxMatrixUpmixer::~fxMatrixUpmixer() {
    // destructor
}

void fxMatrixUpmixer::rxData(float data[], int len) {
	// data received from x32ctrl
}

void fxMatrixUpmixer::process(float* bufIn[], float* bufOut[]) {
	// channel center
	// =========================================================
	// C = (Lin + Rin) / 2
	vecvaddf(bufIn[0], bufIn[1], bufOut[2], SAMPLES_IN_BUFFER);
	vecsmltf(bufOut[2], 0.5f, bufOut[2], SAMPLES_IN_BUFFER);





	// channels left and right
	// =========================================================
	// first calculate C / 2
	vecsmltf(bufOut[2], 0.5f, &_bufTemp[0], SAMPLES_IN_BUFFER);
	// L = Lin - (C / 2)
	vecvsubf(bufIn[0], &_bufTemp[0], bufOut[0], SAMPLES_IN_BUFFER);
	// R = Rin - (C / 2)
	vecvsubf(bufIn[1], &_bufTemp[0], bufOut[1], SAMPLES_IN_BUFFER);





	// channel surroundLeft and surroundRight
	// =========================================================
	// calc surround-signal: surround_signal = (Lin - Rin) / 2
	vecvsubf(bufIn[0], bufIn[1], &_bufTemp[0], SAMPLES_IN_BUFFER);
	vecsmltf(&_bufTemp[0], 0.5f, &_bufTemp[0], SAMPLES_IN_BUFFER);

	// feed delay line with current surround_signal
	for	(int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		_delayLine[_delayLineHead++] = _bufTemp[s];
		if (_delayLineHead == FX_MATRIXUPMIXER_BUFFER_SIZE) {
			_delayLineHead = 0;
		}
	}
	// read surround_signal from delay line
	int delayLineTail = _delayLineHead - (FX_MATRIXUPMIXER_DELAYBACK_MS * _sampleRate / 1000); // here we set the delay in milliseconds
	if (delayLineTail < 0) {
		delayLineTail += FX_MATRIXUPMIXER_BUFFER_SIZE;
	}
	for	(int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		_bufTemp[s] = _delayLine[delayLineTail++];
		if (delayLineTail == FX_MATRIXUPMIXER_BUFFER_SIZE) {
			delayLineTail = 0;
		}
	}

	// apply 7kHz low-pass-filter to surround channel
	// Single-Pole LowPass: output = zoutput + coeff * (input - zoutput)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		_bufTemp[s] = _lowPassSurroundState + _lowPassSurroundCoeff * (_bufTemp[s] - _lowPassSurroundState);
		_lowPassSurroundState = _bufTemp[s];
	}

	// sL = surround_signal
	memcpy(bufOut[3], &_bufTemp[0], SAMPLES_IN_BUFFER);
	// sR = -surround_signal
	vecsmltf(&_bufTemp[0], -1.0f, bufOut[4], SAMPLES_IN_BUFFER);





	// channel LFE
	// =========================================================
	// LFE = low_pass_120Hz(Center) as Center = (Lin + Rin) / 2

	// Single-Pole LowPass: output = zoutput + coeff * (input - zoutput)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufOut[5][s] = _lowPassSubState + _lowPassSubCoeff * (bufOut[2][s] - _lowPassSubState);
		_lowPassSubState = bufOut[5][s];
	}
}
