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

  This file implements a simple _ Parts of this software are
  leaned on the overdrive-implementation of Philip Salmony:
  https://github.com/pms67/IFX-Guitar-DSP. Thank You!
*/

#include "fxOverdrive.h"

fxOverdrive::fxOverdrive(int fxSlot, int channelMode) : fx(fxSlot, channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	// set default effect parameters
	fxOverdriveSetFilters(300.0f, 10000.0f, 10000.0f); // hpfInput, lpfInput, lpfOutput
	fxOverdriveSetGain(10.0f, -0.2f); // preGain, Q

	_hpfInputStateIn = 0;
	_hpfInputStateOut = 0;
	_lpfInputState = 0;
	_lpfOutputState = 0;
}

fxOverdrive::~fxOverdrive() {
    // destructor
}

void fxOverdrive::fxOverdriveSetFilters(float hpfInputFreq, float lpfInputFreq, float lpfOutputFreq) {
	_hpfInputCoef = 1.0f / (1.0f + 2.0f * M_PI * hpfInputFreq * (1.0f/_sampleRate)); // 1.0f / (1.0f + 2.0f * M_PI * f_c * (1.0f/f_s))
	_lpfInputCoef = (2.0f * M_PI * lpfInputFreq) / (_sampleRate + 2.0f * M_PI * lpfInputFreq); // (2.0f * M_PI * f_c) / (f_s + 2.0f * M_PI * f_c)
	_lpfOutputCoef = (2.0f * M_PI * lpfOutputFreq) / (_sampleRate + 2.0f * M_PI * lpfOutputFreq); // (2.0f * M_PI * f_c) / (f_s + 2.0f * M_PI * f_c)
}

void fxOverdrive::fxOverdriveSetGain(float preGain, float Q) {
	_preGain = preGain;
	_Q = Q;
	_clipConst = _Q / (1.0f - expf(8.0f * _Q));
}

void fxOverdrive::rxData(float data[], int len) {
	// data received from x32ctrl
}

void fxOverdrive::process(float* bufIn[], float* bufOut[]) {
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		float signal = bufIn[0][s];

		// input lowpass: output = zoutput + coeff * (input - zoutput)
		signal = _lpfInputState + _lpfInputCoef * (signal - _lpfInputState);
		_lpfInputState = signal; // store zoutput

		// input highpass: output = coeff * (zoutput + input - zinput)
		float signalIn = signal;
		signal = _hpfInputCoef * (_hpfInputStateOut + signal - _hpfInputStateIn);
		_hpfInputStateOut = signal; // store zoutput
		_hpfInputStateIn = signalIn; // store zinput

		// overdrive with asymmetrical clipping
		signal = _preGain * signal;
		float clipOut = _clipConst;
		if ((signal - _Q) >= 0.00001f) {
			clipOut += (signal - _Q) / (1.0f - expf(-8.0f * (signal - _Q)));
		}

		// output lowpass: output = zoutput + coeff * (input - zoutput)
		// here high frequency-components after clipping will be removed
		clipOut = _lpfOutputState + _lpfOutputCoef * (clipOut - _lpfOutputState);
		_lpfOutputState = clipOut; // store zoutput

		// limit output to +/-2^31
		bufOut[0][s] = fclipf(clipOut, 2147483647.0f);

		// bypass
		//bufOut[0][s] = bufIn[0][s];
	}
}
