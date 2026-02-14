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

  This file implements a nice Real-Time-Analyzer (RTA) using FFT
  The spectrum will be smoothed to display a nice spectrogram
 */

#include "rta.h"

float pm hannWindow[RTA_FFT_SIZE];

#if RTA_FFT_FFT_MODE == 0
	float fft_input[RTA_FFT_SIZE];
	complex_float fft_output[RTA_FFT_SIZE];
	complex_float pm twiddle[RTA_FFT_SIZE_HALF];
	complex_float temp[RTA_FFT_SIZE];
	float *tmp = (float*)temp;
	float rta_rspectrum[RTA_FFT_SIZE_HALF + 1];
#elif RTA_FFT_FFT_MODE == 1
	float pm fft_input[RTA_FFT_SIZE];
	complex_float pm fft_output[RTA_FFT_SIZE_HALF];
	float pm rta_rspectrum[RTA_FFT_SIZE_HALF];
	float pm rta_display[64];
#endif

int head = 0;

void rtaInit() {
	gen_hanning(&hannWindow[0], 1, RTA_FFT_SIZE); // pointer to array, a (Window-spacing), N (Window-Length)

	#if RTA_FFT_FFT_MODE == 0
		twidfft(twiddle, RTA_FFT_SIZE);
	#endif
}

void rtaProcess(float* __restrict inBuf) {
	memcpy(&fft_input[head], &inBuf[0], SAMPLES_IN_BUFFER * sizeof(float));
	head += 16;

	// 16 samples every 333 microseconds -> 1024 samples -> every 21ms the FFT will be calculated
	if (head >= RTA_FFT_SIZE) {
		// apply hann-window
		vecvmltf(&fft_input[0], &hannWindow[0], &fft_input[0], RTA_FFT_SIZE);

		// perform FFT
		#if RTA_FFT_FFT_MODE == 0
			rfft(&fft_input[0], tmp, &fft_output[0], twiddle, 1, RTA_FFT_SIZE);
			fft_magnitude(fft_output, rta_rspectrum, RTA_FFT_SIZE, 2);
		#elif RTA_FFT_FFT_MODE == 1
			#if RTA_FFT_SIZE == 1024
				rfft1024(&fft_input[0], &fft_output[0]);
			#elif RTA_FFT_SIZE == 512
				rfft512(&fft_input[0], &fft_output[0]);
			#elif RTA_FFT_SIZE == 256
				rfft256()&fft_input[0], &fft_output[0]);
			#elif RTA_FFT_SIZE == 128
				rfft128(&fft_input[0], &fft_output[0]);
			#elif RTA_FFT_SIZE == 64
				rfft64(&fft_input[0], &fft_output[0]);
			#endif
			fft_magnitude(&fft_output[0], &rta_rspectrum[0], RTA_FFT_SIZE, 1);

			// direct copy of the first 64 bins (low frequency)
			// this is very rough and not nice
			//memcpy(&rta_display[0], &rta_rspectrum[0], 64 * sizeof(float));

			// better solution: convert the 512 bins of the spectrum to logarithmic 64 bins to display at the iMX25
			float attack = 0.8f;
			float release = 0.3f;
			const float k = 0.07f;
			const float denom = 1.0f / (expf(k * (float)(RTA_DISPLAY_BANDS - 1)) - 1.0f);

			for (int i = 0; i < RTA_DISPLAY_BANDS; i++) {
				// Step 1: logarithmix map-equation
				// calculate the Bar-index i to the FFT-Bin-Index
				float logIndex = (float)(RTA_FFT_SIZE_HALF - 1) * (expf(k * (float)i) - 1.0f) * denom;

				// security check for all arrays
				int idxLo = (int)logIndex;
				if (idxLo < 0) idxLo = 0;
				int idxHi = idxLo + 1;

				// limit to the array-borders
				if (idxHi >= RTA_FFT_SIZE_HALF) {
					idxHi = RTA_FFT_SIZE_HALF - 1;
					idxLo = idxHi - 1;
					if(idxLo < 0) idxLo = 0;
				}

				float frac = logIndex - (float)idxLo;

				// linear interpolation between two Bins
				float targetVal = rta_rspectrum[idxLo] + frac * (rta_rspectrum[idxHi] - rta_rspectrum[idxLo]);

				// Step 2: temporal smoothing of the amplitude (ballistic optimization to smooth the spectrum)
				if (targetVal > rta_display[i]) {
					rta_display[i] += attack * (targetVal - rta_display[i]);
				} else {
					rta_display[i] += release * (targetVal - rta_display[i]);
				}
			}
		#endif

		// reset head for next read
		head = 0;
	}
}
