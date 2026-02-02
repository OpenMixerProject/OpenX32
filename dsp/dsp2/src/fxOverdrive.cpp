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

#pragma file_attr("prefersMem=internal") // let the linker know, that all variables should be placed into the internal ram

fxOverdrive::fxOverdrive(int fxSlot, int channelMode) : fx(fxSlot, channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	// set default effect parameters
	//setParameters(10.0f, -0.2f, 300, 10000, 10000);
	_preGain = 10.0f;
	_Q = -0.2f;
	_clipConst = -0.250594070204370662f;
	_hpfInputCoef = 0.962213946674328602487f;
	_lpfInputCoef = 0.566911509014416f;
	_lpfOutputCoef = 0.566911509014416f;

	_hpfInputStateIn[0] = 0;
	_hpfInputStateOut[0] = 0;
	_lpfInputState[0] = 0;
	_lpfOutputState[0] = 0;
	_hpfInputStateIn[1] = 0;
	_hpfInputStateOut[1] = 0;
	_lpfInputState[1] = 0;
	_lpfOutputState[1] = 0;
}

fxOverdrive::~fxOverdrive() {
    // destructor
}

// human-friendly parameter-settings, but more expensive for the DSP
void fxOverdrive::setParameters(float preGain, float Q, float hpfInputFreq, float lpfInputFreq, float lpfOutputFreq) {
/*
	_preGain = preGain;
	_Q = Q;
	_clipConst = clipConst;
	_hpfInputCoef = hpfInputCoef;
	_lpfInputCoef = lpfInputCoef;
	_lpfOutputCoef = lpfOutputCoef;
*/

	_preGain = preGain;
	_Q = Q;

	float denum = 1.0f - expf(8.0f * _Q);
	if (denum != 0) {
		_clipConst = _Q / denum;
	}

	_hpfInputCoef = 1.0f / (1.0f + 2.0f * M_PI * hpfInputFreq * (1.0f/_sampleRate)); // 1.0f / (1.0f + 2.0f * M_PI * f_c * (1.0f/f_s))
	_lpfInputCoef = (2.0f * M_PI * lpfInputFreq) / (_sampleRate + 2.0f * M_PI * lpfInputFreq); // (2.0f * M_PI * f_c) / (f_s + 2.0f * M_PI * f_c)
	_lpfOutputCoef = (2.0f * M_PI * lpfOutputFreq) / (_sampleRate + 2.0f * M_PI * lpfOutputFreq); // (2.0f * M_PI * f_c) / (f_s + 2.0f * M_PI * f_c)

	_startup = false;
}

void fxOverdrive::rxData(float data[], int len) {
	// data received from x32ctrl
	if (len != 6) return;

	_preGain = data[0];
	_Q = data[1];
	_clipConst = data[2];
	_hpfInputCoef = data[3];
	_lpfInputCoef = data[4];
	_lpfOutputCoef = data[5];
}

void fxOverdrive::process(float* __restrict bufIn[], float* __restrict bufOut[]) {
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		float signal[2];
		signal[0] = bufIn[0][s];
		signal[1] = bufIn[1][s];

		for (int i_ch = 0; i_ch < 2; i_ch++) {
			// input lowpass: output = zoutput + coeff * (input - zoutput)
			signal[i_ch] = _lpfInputState[i_ch] + _lpfInputCoef * (signal[i_ch] - _lpfInputState[i_ch]);
			_lpfInputState[i_ch] = signal[i_ch]; // store zoutput

			// input highpass: output = coeff * (zoutput + input - zinput)
			float signalIn = signal[i_ch];
			signal[i_ch] = _hpfInputCoef * (_hpfInputStateOut[i_ch] + signal[i_ch] - _hpfInputStateIn[i_ch]);
			_hpfInputStateOut[i_ch] = signal[i_ch]; // store zoutput
			_hpfInputStateIn[i_ch] = signalIn; // store zinput

			// overdrive with asymmetrical clipping
			signal[i_ch] = _preGain * signal[i_ch];
			float clipOut = _clipConst;
			float denum = 1.0f - expf(-8.0f * (signal[i_ch] - _Q));
			if (denum != 0) {
				clipOut += (signal[i_ch] - _Q) / denum;
			}

			// output lowpass: output = zoutput + coeff * (input - zoutput)
			// here high frequency-components after clipping will be removed
			clipOut = _lpfOutputState[i_ch] + _lpfOutputCoef * (clipOut - _lpfOutputState[i_ch]);
			_lpfOutputState[i_ch] = clipOut; // store zoutput

			// limit output to +/-2^31
			bufOut[i_ch][s] = fclipf(clipOut, 2147483647.0f);
		}

		// bypass
		//bufOut[0][s] = bufIn[0][s];
		//bufOut[1][s] = bufIn[1][s];
	}
}
