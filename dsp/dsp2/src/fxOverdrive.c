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

  This file implements a simple overdrive. Parts of this software are
  leaned on the overdrive-implementation of Philip Salmony:
  https://github.com/pms67/IFX-Guitar-DSP. Thank You!
*/

#include "fxOverdrive.h"

#if FX_USE_OVERDRIVE == 1

#define FX_OVERDRIVE_COEFF_LENGTH 69

void fxOverdriveSetFilters(float lpfInputFreq, float hpfInputFreq, float lpfOutputFreq) {
	overdrive.hpfInputCoef = 1.0f / (1.0f + 2.0f * M_PI * hpfInputFreq * (1.0f/dsp.samplerate)); // 1.0f / (1.0f + 2.0f * M_PI * f_c * (1.0f/f_s))
	overdrive.lpfInputCoef = (2.0f * M_PI * lpfInputFreq) / (dsp.samplerate + 2.0f * M_PI * lpfInputFreq); // (2.0f * M_PI * f_c) / (f_s + 2.0f * M_PI * f_c)
	overdrive.lpfOutputCoef = (2.0f * M_PI * lpfOutputFreq) / (dsp.samplerate + 2.0f * M_PI * lpfOutputFreq); // (2.0f * M_PI * f_c) / (f_s + 2.0f * M_PI * f_c)
}

void fxOverdriveSetGain(float preGain, float Q) {
	overdrive.preGain = preGain;
	overdrive.Q = Q;
	overdrive.clipConst = overdrive.Q / (1.0f - expf(8.0f * overdrive.Q));
}

void fxOverdriveInit(void) {
	fxOverdriveSetFilters(300.0f, 10000.0f, 10000.0f); // hpfInput, lpfInput, lpfOutput
	fxOverdriveSetGain(10.0f, -0.2f); // preGain, Q
}

void fxOverdriveProcess(float* bufIn, float* bufOut) {
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		float signal = bufIn[s];

		// input lowpass: output = zoutput + coeff * (input - zoutput)
		signal = overdrive.lpfInputState + overdrive.lpfInputCoef * (signal - overdrive.lpfInputState);
		overdrive.lpfInputState = signal; // store zoutput

		// input highpass: output = coeff * (zoutput + input - zinput)
		float signalIn = signal;
		signal = overdrive.hpfInputCoef * (overdrive.hpfInputStateOut + signal - overdrive.hpfInputStateIn);
		overdrive.hpfInputStateOut = signal; // store zoutput
		overdrive.hpfInputStateIn = signalIn; // store zinput

		// overdrive with asymmetrical clipping
		signal = overdrive.preGain * signal;
		float clipOut = overdrive.clipConst;
		if ((signal - overdrive.Q) >= 0.00001f) {
			clipOut += (signal - overdrive.Q) / (1.0f - expf(-8.0f * (signal - overdrive.Q)));
		}

		// output lowpass: output = zoutput + coeff * (input - zoutput)
		// here high frequency-components after clipping will be removed
		signal = overdrive.lpfOutputState + overdrive.lpfOutputCoef * (signal - overdrive.lpfOutputState);
		overdrive.lpfOutputState = signal; // store zoutput

		// limit output to +/-2^31
		bufOut[s] = fclipf(clipOut, 2147483647.0f);

		// bypass
		//bufOut[s] = bufIn[s];
	}
}

#endif
