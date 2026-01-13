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
*/

#include "fxMatrixUpmixer.h"

#if FX_USE_MATRIXUPMIXER == 1


float delayLine[FX_MATRIXUPMIXER_BUFFER_SIZE];
int delayLineHead = 0;
float lowPassSubState = 0;
float lowPassSubCoeff = 0.01609904178227480397989f; // 125Hz = 785.398163397448 / 48785.398163397448 <- alpha = (2 * pi * f_c) / (f_s + 2 * pi * f_c) = (2 * pi * 125Hz) / (48000Hz + 2 * pi * 125Hz)
float lowPassSurroundState = 0;
float lowPassSurroundCoeff = 0.4781604560104657892f; // 7kHz = 43982,297150257105338477007365913 / 91982,297150257105338477007365913 <- alpha = (2 * pi * f_c) / (f_s + 2 * pi * f_c) = (2 * pi * 7000Hz) / (48000Hz + 2 * pi * 7000Hz)

void fxMatrixUpmixerInit(void) {
}

void fxMatrixUpmixerProcess(float* inBuf[2], float* outBuf[6], int samples) {
	float temp[SAMPLES_IN_BUFFER];

	// channel center
	// =========================================================
	// C = (Lin + Rin) / 2
	vecvaddf(inBuf[0], inBuf[1], outBuf[2], SAMPLES_IN_BUFFER);
	vecsmltf(outBuf[2], 0.5f, outBuf[2], SAMPLES_IN_BUFFER);





	// channels left and right
	// =========================================================
	// first calculate C / 2
	vecsmltf(outBuf[2], 0.5f, &temp[0], SAMPLES_IN_BUFFER);
	// L = Lin - (C / 2)
	vecvsubf(inBuf[0], &temp[0], outBuf[0], SAMPLES_IN_BUFFER);
	// R = Rin - (C / 2)
	vecvsubf(inBuf[1], &temp[0], outBuf[1], SAMPLES_IN_BUFFER);





	// channel surroundLeft and surroundRight
	// =========================================================
	// calc surround-signal: surround_signal = (Lin - Rin) / 2
	vecvsubf(inBuf[0], inBuf[1], &temp[0], SAMPLES_IN_BUFFER);
	vecsmltf(&temp[0], 0.5f, &temp[0], SAMPLES_IN_BUFFER);

	// feed delay line with current surround_signal
	for	(int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		delayLine[delayLineHead++] = temp[s];
		if (delayLineHead == FX_MATRIXUPMIXER_BUFFER_SIZE) {
			delayLineHead = 0;
		}
	}
	// read surround_signal from delay line
	int delayLineTail = delayLineHead - (FX_MATRIXUPMIXER_DELAYBACK_MS * FX_MATRIXUPMIXER_SAMPLING_RATE / 1000); // here we set the delay in milliseconds
	if (delayLineTail < 0) {
		delayLineTail += FX_MATRIXUPMIXER_BUFFER_SIZE;
	}
	for	(int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		temp[s] = delayLine[delayLineTail++];
		if (delayLineTail == FX_MATRIXUPMIXER_BUFFER_SIZE) {
			delayLineTail = 0;
		}
	}

	// apply 7kHz low-pass-filter to surround channel
	// Single-Pole LowPass: output = zoutput + coeff * (input - zoutput)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		temp[s] = lowPassSurroundState + lowPassSurroundCoeff * (temp[s] - lowPassSurroundState);
		lowPassSurroundState = temp[s];
	}

	// sL = surround_signal
	memcpy(outBuf[3], &temp[0], SAMPLES_IN_BUFFER);
	// sR = -surround_signal
	vecsmltf(&temp[0], -1.0f, outBuf[4], SAMPLES_IN_BUFFER);





	// channel LFE
	// =========================================================
	// LFE = low_pass_120Hz(Center) as Center = (Lin + Rin) / 2

	// Single-Pole LowPass: output = zoutput + coeff * (input - zoutput)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		outBuf[5][s] = lowPassSubState + lowPassSubCoeff * (outBuf[2][s] - lowPassSubState);
		lowPassSubState = outBuf[5][s];
	}
}

#endif
