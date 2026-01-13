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

#include "fxUpmixer.h"

#if FX_USE_UPMIXER == 1

#define real					0
#define imag					1
#define UPMIX_WINDOW_LEN		128
#define UPMIX_FFT_HOP_VALUE		4
#define UPMIX_RX_SAMPLE_COUNT	(UPMIX_WINDOW_LEN / UPMIX_FFT_HOP_VALUE) // use fine N/4 to get 75% overlap (alternatively use N/2 for 50% overlap for less computational stress)

float hannWindow[UPMIX_WINDOW_LEN];
float lowPassSubState = 0;
float lowPassSubCoeff = 0.01609904178227480397989f; // 125Hz = 785.398163397448 / 48785.398163397448 <- alpha = (2 * pi * f_c) / (f_s + 2 * pi * f_c) = (2 * pi * 125Hz) / (48000Hz + 2 * pi * 125Hz)
//float lowPassSurroundState[2];
//float lowPassSurroundCoeff = 0.4781604560104657892f; // 7kHz = 43982,297150257105338477007365913 / 91982,297150257105338477007365913 <- alpha = (2 * pi * f_c) / (f_s + 2 * pi * f_c) = (2 * pi * 7000Hz) / (48000Hz + 2 * pi * 7000Hz)
// constants for soft low-pass in frequency domain
const float surroundCutoff = 7000.0f; // soft damping for surround channels starting at 7kHz
const float bin_to_hz = 48000.0f / (float)UPMIX_WINDOW_LEN;

// variables for the FFT and calculation
float upmixInputBuffer[2][UPMIX_WINDOW_LEN]; // we need ringbuffer for the full window-size
#pragma align (2 * 2 * UPMIX_WINDOW_LEN)
float upmixFftInputBufferLR[2][2][UPMIX_WINDOW_LEN]; // imag/real for two channels
#pragma align (2 * 2 * UPMIX_WINDOW_LEN)
float upmixFftOutputBufferLR[2][2][UPMIX_WINDOW_LEN]; // imag/real
#pragma align (2 * UPMIX_WINDOW_LEN)
float upmixFftOutputBufferC[2][UPMIX_WINDOW_LEN]; // imag/real
#pragma align (2 * 2 * UPMIX_WINDOW_LEN)
float upmixFftOutputBufferBackLR[2][2][UPMIX_WINDOW_LEN]; // imag/real
#pragma align (2 * UPMIX_WINDOW_LEN/2)
float twidtab[2][UPMIX_WINDOW_LEN/2];
#pragma align (2 * UPMIX_WINDOW_LEN)
float upmixFftTempBuffer[2][UPMIX_WINDOW_LEN]; // imag/real
float upmixOutputBuffer[5][UPMIX_WINDOW_LEN]; // left, right, center, back-left, back-right

float maskCenter[UPMIX_WINDOW_LEN/2];
float maskLeft[UPMIX_WINDOW_LEN/2];
float maskRight[UPMIX_WINDOW_LEN/2];
float maskAmbient[UPMIX_WINDOW_LEN/2];
float maskCenter_z[UPMIX_WINDOW_LEN/2];
float maskLeft_z[UPMIX_WINDOW_LEN/2];
float maskRight_z[UPMIX_WINDOW_LEN/2];
float maskAmbient_z[UPMIX_WINDOW_LEN/2];

/*
	// accessing external RAM
	typedef float fft_block_t[2][UPMIX_WINDOW_LEN];
	fft_block_t* upmixFftBuffer = (fft_block_t*)EXTERNAL_RAM_START;
*/

int upmixInputBufferHead = 0; // start writing at index 0
int upmixInputBufferTail = UPMIX_RX_SAMPLE_COUNT; // start reading at index 1
int upmixInputSampleCounter = 0; // starts always at 0
int upmixOutputSampleCounter = 0; // starts always at 0

int delayLineHead = 0;
float delayLineBackLeft[FX_UPMIXER_BUFFER_SIZE];
float delayLineBackRight[FX_UPMIXER_BUFFER_SIZE];

void fxUpmixerInit(void) {
	gen_hanning(&hannWindow[0], 1, UPMIX_WINDOW_LEN); // pointer to array, a (Window-spacing), N (Window-Length)
	twidfftf(&twidtab[real][0], &twidtab[imag][0], UPMIX_WINDOW_LEN);
}

void fxUpmixerProcess(float* inBuf[2], float* outBuf[6], int samples) {
	// this is the first approach on implementing an Stereo to 5.1 upmixing algorithm based on the
	// PhD of Sebastian Kraft (https://openhsu.ub.hsu-hh.de/server/api/core/bitstreams/d2391f49-8092-4bb3-a1ad-85b558655dd7/content)
	//
	// it decompiles a stereo-signal into direct and indirect components (Primary-Ambient Extraction) and compiles a new 5.1 mix
	//
	// main idea is a Mid-Side (M/S) decomposition within the frequency-domain
	// for each frequency-band a coefficient for coherence and similarity is calculated for left and right
	// the two coefficients represents the primary (direct) and ambience (room) sound
	// Step 1: collect 256 samples (16 intervals)
	// Step 2: perform FFT
	// Step 3: calculation of the interchannel-coherence
	// Step 4: decorrelation for surround-channels
	// Step 5: perform inverse FFT
	// Step 6: assembling by adding the newbuffer on top of the output-buffer
	// Step 7: prepare the output
	// Step 8: delay-line for surround-channels
	// Step 9: Apply LowPass of 7kHz on surround-speakers and 125Hz on LFE

	int arrayIdx = 0;

	// Step 1: Collect data and trigger calculation every N/4 samples
	// ============================================================================================
	for (int i_ch = 0; i_ch < 2; i_ch++) {
		memcpy(&upmixInputBuffer[i_ch][upmixInputBufferHead], inBuf[i_ch], samples * sizeof(float));
	}
	upmixInputSampleCounter += samples; // keep track of received samples

	// increase the head-pointer of the ringbuffer
	upmixInputBufferHead += samples;
	if (upmixInputBufferHead >= UPMIX_WINDOW_LEN) {
		upmixInputBufferHead -= UPMIX_WINDOW_LEN;
	}

	// when N/2 or N/4 samples are received we can compute the algorithm
	if (upmixInputSampleCounter == UPMIX_RX_SAMPLE_COUNT) {
		// we received N/4 new samples -> process data
		upmixInputSampleCounter = 0; // reset input counter

		// read the last UPMIX_WINDOW_LEN samples out of the unsorted circular-buffer into the sorted FFT-buffer
		for (int i_ch = 0; i_ch < 2; i_ch++) {
			memcpy(&upmixFftInputBufferLR[real][i_ch][0], &upmixInputBuffer[i_ch][upmixInputBufferTail], (UPMIX_WINDOW_LEN - upmixInputBufferTail) * sizeof(float));
			// wrap-around of the end of ring-buffer and copy remaining samples
			memcpy(&upmixFftInputBufferLR[real][i_ch][UPMIX_WINDOW_LEN - upmixInputBufferTail], &upmixInputBuffer[i_ch][0], upmixInputBufferTail * sizeof(float));
		}

		// increase the tail-pointer of the ringbuffer by UPMIX_RX_SAMPLE_COUNT
		upmixInputBufferTail += UPMIX_RX_SAMPLE_COUNT;
		if (upmixInputBufferTail >= UPMIX_WINDOW_LEN) {
			upmixInputBufferTail -= UPMIX_WINDOW_LEN;
		}






		// Step 2: perform FFT
		// ============================================================================================
		// now we have a consistent and sorted sample-buffer with the oldest sample at index 0 and the newest sample at the last index
		// apply pre-calculated Hann-Window
		for (int i_ch = 0; i_ch < 2; i_ch++) {
			vecvmltf(&upmixFftInputBufferLR[real][i_ch][0], &hannWindow[0], &upmixFftInputBufferLR[real][i_ch][0], UPMIX_WINDOW_LEN);
		}

		// TODO: allocate different memory-location for data one compared to data two to increase performance
		rfftf_2(&upmixFftInputBufferLR[real][0][0], &upmixFftInputBufferLR[imag][0][0],
				&upmixFftInputBufferLR[real][1][0], &upmixFftInputBufferLR[imag][1][0],
				&twidtab[real][0], &twidtab[imag][0], UPMIX_WINDOW_LEN);




		/*
		// Step 3 (SIMD-optimized functions): calculation of the interchannel-coherence and create mask for direct-parts
		// ============================================================================================
		// magnitude (out = 2 * sqrt(Re² + Im²) / fftsize)
		float magnitude[3][UPMIX_WINDOW_LEN/2];
		float power[2][UPMIX_WINDOW_LEN/2];
		fftf_magnitude(&upmixFftInputBufferLR[real][0][0], &upmixFftInputBufferLR[imag][0][0], &magnitude[0][0], UPMIX_WINDOW_LEN, 2); // mode=2 when using data from rfftf_2()
		fftf_magnitude(&upmixFftInputBufferLR[real][1][0], &upmixFftInputBufferLR[imag][1][0], &magnitude[1][0], UPMIX_WINDOW_LEN, 2); // mode=2 when using data from rfftf_2()
		// calculate power
		vecvmltf(&magnitude[0][0], &magnitude[0][0], &power[0][0], UPMIX_WINDOW_LEN/2); // a, b, output, samples
		vecvmltf(&magnitude[1][0], &magnitude[1][0], &power[1][0], UPMIX_WINDOW_LEN/2); // a, b, output, samples

		// cross-correlation (real-part)
		float crossCorrellation[2][UPMIX_WINDOW_LEN/2]; // (Re(x) * Re(y) +/- Im(x) * Im(y))
		crosscorrf(&crossCorrellation[real][0], &upmixFftInputBufferLR[real][0][0], &upmixFftInputBufferLR[imag][0][0], UPMIX_WINDOW_LEN/2, UPMIX_WINDOW_LEN/2); // output[lags], input_x[samples], input_y[samples], samples, lags

		// cross-correlation (imag-part)
		// first calculate Re²
		vecvmltf(&upmixFftInputBufferLR[real][0][0], &upmixFftInputBufferLR[real][0][0], &upmixFftTempBuffer[real][0], UPMIX_WINDOW_LEN/2); // a, b, output, samples
		// now calculate Im²
		vecvmltf(&upmixFftInputBufferLR[imag][0][0], &upmixFftInputBufferLR[imag][0][0], &upmixFftTempBuffer[imag][0], UPMIX_WINDOW_LEN/2); // a, b, output, samples
		// now calcualte Re² - Im²
		vecvsubf(&upmixFftTempBuffer[real][0], &upmixFftTempBuffer[imag][0], &crossCorrellation[imag][0], UPMIX_WINDOW_LEN/2); // a, b, output, samples

		//magnitude Left-Right
		fftf_magnitude(&crossCorrellation[real][0], &crossCorrellation[imag][0], &magnitude[2][0], UPMIX_WINDOW_LEN, 2); // mode=2 when using data from rfftf_2()

		...
		...
		...
		*/


		// Step 3: calculation of the interchannel-coherence and create mask for direct-parts
		// ============================================================================================
		for (int k = 0; k < (UPMIX_WINDOW_LEN/2); k++) {
			// cross-power: power = Re² + Im²
			float powerLeft = (upmixFftInputBufferLR[real][0][k] * upmixFftInputBufferLR[real][0][k]) + (upmixFftInputBufferLR[imag][0][k] * upmixFftInputBufferLR[imag][0][k]);
			float powerRight = (upmixFftInputBufferLR[real][1][k] * upmixFftInputBufferLR[real][1][k]) + (upmixFftInputBufferLR[imag][1][k] * upmixFftInputBufferLR[imag][1][k]);

			// magnitude = sqrt(Re² + Im²)
			float magLeft = sqrtf(powerLeft);
			float magRight = sqrtf(powerRight);

			// calculate magnitude of the cross-correlation
			float realCross = (upmixFftInputBufferLR[real][0][k] * upmixFftInputBufferLR[real][1][k]) + (upmixFftInputBufferLR[imag][0][k] * upmixFftInputBufferLR[imag][1][k]);
			float imagCross = (upmixFftInputBufferLR[imag][0][k] * upmixFftInputBufferLR[real][1][k]) - (upmixFftInputBufferLR[real][0][k] * upmixFftInputBufferLR[imag][1][k]);
			float magLeftRight = sqrtf((realCross * realCross) + (imagCross * imagCross)); // sqrt(Re² + Im²)

			// estimate the coherence-factor phi
			// phi = 1 -> total coherence: channel left/right are identical
			// phi = 0.5 -> partly coherent: lot of stereo-effect
			// phi = 0 -> no coherence: channel left/right are totally different
			float phi = (2.0f * magLeftRight) / (powerLeft + powerRight + 1e-6f);
			if (phi > 1) { phi = 1.0f; } else if (phi < 0) { phi = 0.0f; } // limit to 0..1
			// TODO: implement transient-detection and force phi=1 on steep transients to focus this to the front

			// take pan-level into account
			// panfactor = 1 -> mono signal
			// panfactor = 0 -> hard left/right signal
			float panFactor;
			if (magLeft + magRight != 0) {
				panFactor = (2.0f * fminf(magLeft, magRight)) / (magLeft + magRight);
				if (panFactor > 1) { panFactor = 1.0f; } else if (panFactor < 0) { panFactor = 0.0f; } // limit to 0..1
			}else{
				panFactor = 1.0f;
			}

			// calculate masks for all channels
			float pannedPrimaryMask = phi * (1.0f - panFactor);
			float totalMag = magLeft + magRight + 1e-6f;
			float weightL;
			float weightR;
			if (totalMag != 0) {
				weightL = magLeft / totalMag;
				weightR = magRight / totalMag;
			}else{
				weightL = 1.0f;
				weightR = 1.0f;
			}
			maskLeft[k]  = pannedPrimaryMask * weightL;
			maskRight[k] = pannedPrimaryMask * weightR;

			maskCenter[k] = phi * panFactor;
			maskAmbient[k] = 1.0f - phi;

			// increase the contrast between individual speaker-channels by using powf() for the channels
			#if FX_UPMIX_CONTRAST_ENHANCEMENT == 0
				// we are not using any contrast enhancement. The transition between channels can be very smooth
			#elif FX_UPMIX_CONTRAST_ENHANCEMENT == 1
				// we are using a power of 1.5 using a rough estimation with sqrtf() to increase speed
				maskLeft[k] = maskLeft[k] * sqrtf(maskLeft[k]);
				maskRight[k] = maskRight[k] * sqrtf(maskRight[k]);
				maskCenter[k] = maskCenter[k] * sqrtf(maskCenter[k]);
				maskAmbient[k] = maskAmbient[k] * sqrtf(maskAmbient[k]);
			#elif FX_UPMIX_CONTRAST_ENHANCEMENT == 2
				// calculating x * x is much faster than powf(x, 2)
				maskLeft[k] = maskLeft[k] * maskLeft[k];
				maskRight[k] = maskRight[k] * maskRight[k];
				maskCenter[k] = maskCenter[k] * maskCenter[k];
				maskAmbient[k] = maskAmbient[k] * maskAmbient[k];
			#else
				// use powf() for values above 2
				maskLeft[k] = powf(maskLeft[k], FX_UPMIX_CONTRAST_ENHANCEMENT);
				maskRight[k] = powf(maskRight[k], FX_UPMIX_CONTRAST_ENHANCEMENT);
				maskCenter[k] = powf(maskCenter[k], FX_UPMIX_CONTRAST_ENHANCEMENT);
				maskAmbient[k] = powf(maskAmbient[k], FX_UPMIX_CONTRAST_ENHANCEMENT);
			#endif

			#if FX_UPMIX_CONTRAST_ENHANCEMENT != 0
				float sumMasks = maskLeft[k] + maskRight[k] + maskCenter[k] + maskAmbient[k];
				float norm;
				if (sumMasks != 0) {
					norm = 1.0f / sumMasks;
				}else{
					norm = 1.0f;
				}
				maskLeft[k] = maskLeft[k] * norm;
				maskRight[k] = maskRight[k] * norm;
				maskCenter[k] = maskCenter[k] * norm;
				maskAmbient[k] = maskAmbient[k] * norm;
			#endif
		}

		// smooth masks using out = (in - in_z) * coeff + in_z
//		for (int k = 0; k < (UPMIX_WINDOW_LEN/2); k++) {
//			maskLeft[k] = (maskLeft[k] - maskLeft_z[k]) * 0.003f + maskLeft_z[k]; // alpha = 0.3 for faster reaction
//			maskRight[k] = (maskRight[k] - maskRight_z[k]) * 0.0003f + maskRight_z[k]; // alpha = 0.3 for faster reaction
//			maskCenter[k] = (maskCenter[k] - maskCenter_z[k]) * 0.3f + maskCenter_z[k]; // alpha = 0.3 for faster reaction
//			maskAmbient[k] = (maskAmbient[k] - maskAmbient_z[k]) * 0.0001f + maskAmbient_z[k]; // alpha = 0.1 for slower reaction (ambience)
//		}
//		memcpy(&maskLeft_z[0], &maskLeft[0], sizeof(maskLeft));
//		memcpy(&maskRight_z[0], &maskRight[0], sizeof(maskRight));
//		memcpy(&maskCenter_z[0], &maskCenter[0], sizeof(maskCenter));
//		memcpy(&maskAmbient_z[0], &maskAmbient[0], sizeof(maskAmbient));



		// Step 4: decorrelation for surround-channels
		// ============================================================================================
		float *reBufL = &upmixFftOutputBufferLR[real][0][0];
		float *imBufL = &upmixFftOutputBufferLR[imag][0][0];
		float *reBufR = &upmixFftOutputBufferLR[real][1][0];
		float *imBufR = &upmixFftOutputBufferLR[imag][1][0];
		float *reBufC = &upmixFftOutputBufferC[real][0];
		float *imBufC = &upmixFftOutputBufferC[imag][0];
		float *reBufBL = &upmixFftOutputBufferBackLR[real][0][0];
		float *imBufBL = &upmixFftOutputBufferBackLR[imag][0][0];
		float *reBufBR = &upmixFftOutputBufferBackLR[real][1][0];
		float *imBufBR = &upmixFftOutputBufferBackLR[imag][1][0];

		for (int k = 0; k < (UPMIX_WINDOW_LEN/2); k++) {
			#if FX_UPMIX_ADD_AMBIENCE_TO_LR == 0
				// left: panned direct sound + 30% ambient
				reBufL[k] = upmixFftInputBufferLR[real][0][k] * maskLeft[k];
				imBufL[k] = upmixFftInputBufferLR[imag][0][k] * maskLeft[k];

				// right: panned direct sound + 30% ambient
				reBufR[k] = upmixFftInputBufferLR[real][1][k] * maskRight[k];
				imBufR[k] = upmixFftInputBufferLR[imag][1][k] * maskRight[k];
			#else
				// left: panned direct sound + 30% ambient
				reBufL[k] = upmixFftInputBufferLR[real][0][k] * (maskLeft[k] + maskAmbient[k] * 0.3f);
				imBufL[k] = upmixFftInputBufferLR[imag][0][k] * (maskLeft[k] + maskAmbient[k] * 0.3f);

				// right: panned direct sound + 30% ambient
				reBufR[k] = upmixFftInputBufferLR[real][1][k] * (maskRight[k] + maskAmbient[k] * 0.3f);
				imBufR[k] = upmixFftInputBufferLR[imag][1][k] * (maskRight[k] + maskAmbient[k] * 0.3f);
			#endif

			// center: direct sound
			reBufC[k] = (upmixFftInputBufferLR[real][0][k] + upmixFftInputBufferLR[real][1][k]) * maskCenter[k] * 0.707f; // 1/sqrt(2)
			imBufC[k] = (upmixFftInputBufferLR[imag][0][k] + upmixFftInputBufferLR[imag][1][k]) * maskCenter[k] * 0.707f; // 1/sqrt(2)

			#if FX_UPMIX_PHASEINVERTED_BACK_LR == 0
				// backLeft: containing all uncorrelated parts
				reBufBL[k] = upmixFftInputBufferLR[real][0][k] * maskAmbient[k];
				imBufBL[k] = upmixFftInputBufferLR[imag][0][k] * maskAmbient[k];

				// backRight: containing all uncorrelated parts
				reBufBR[k] = upmixFftInputBufferLR[real][1][k] * maskAmbient[k];
				imBufBR[k] = upmixFftInputBufferLR[imag][1][k] * maskAmbient[k];
			#else
				// backLeft: containing all uncorrelated parts
				reBufBL[k] = -upmixFftInputBufferLR[imag][0][k] * maskAmbient[k];
				imBufBL[k] = upmixFftInputBufferLR[real][0][k] * maskAmbient[k];

				// backRight: containing all uncorrelated parts
				reBufBR[k] = upmixFftInputBufferLR[imag][1][k] * maskAmbient[k];
				imBufBR[k] = -upmixFftInputBufferLR[real][1][k] * maskAmbient[k];
			#endif





			// calculate soft roll-off starting at 8kHz in frequency domain
			float freq = (float)k * bin_to_hz;
			float surroundDamping = 1.0f;
			// damp only when we are above the desired frequency
			if (freq > surroundCutoff) {
				float f_ratio = freq / surroundCutoff;
				surroundDamping = 1.0f / sqrtf(1.0f + (f_ratio * f_ratio));
			}
			reBufBL[k] *= surroundDamping;
			imBufBL[k] *= surroundDamping;
			reBufBR[k] *= surroundDamping;
			imBufBR[k] *= surroundDamping;
		}


		// calculate the Nyquist frequency -> add 2nd half of the spectrum
		// TODO: check if we can mitigate this or if there is a built-in-function. Maybe ifftf() can be used with different parameters
		for (int i = 1; i < (UPMIX_WINDOW_LEN/2); i++) {
			int mirrIdx = UPMIX_WINDOW_LEN - i;

			reBufL[mirrIdx] = reBufL[i]; // Re=Re, Im=-Im
			imBufL[mirrIdx] = -imBufL[i]; // Re=Re, Im=-Im

			reBufR[mirrIdx] = reBufR[i]; // Re=Re, Im=-Im
			imBufR[mirrIdx] = -imBufR[i]; // Re=Re, Im=-Im

			reBufC[mirrIdx] = reBufC[i]; // Re=Re, Im=-Im
			imBufC[mirrIdx] = -imBufC[i]; // Re=Re, Im=-Im

			reBufBL[mirrIdx] = reBufBL[i]; // Re=Re, Im=-Im
			imBufBL[mirrIdx] = -imBufBL[i]; // Re=Re, Im=-Im

			reBufBR[mirrIdx] = reBufBR[i]; // Re=Re, Im=-Im
			imBufBR[mirrIdx] = -imBufBR[i]; // Re=Re, Im=-Im
		}






		// Step 5: perform inverse-FFT
		// ============================================================================================
		// TODO: allocate different memory-location for real-parts compared to imag-parts to increase performance
		ifftf(reBufL, imBufL, &upmixFftTempBuffer[real][0], &upmixFftTempBuffer[imag][0], &twidtab[real][0], &twidtab[imag][0], UPMIX_WINDOW_LEN);
		ifftf(reBufR, imBufR, &upmixFftTempBuffer[real][0], &upmixFftTempBuffer[imag][0], &twidtab[real][0], &twidtab[imag][0], UPMIX_WINDOW_LEN);
		ifftf(reBufC, imBufC, &upmixFftTempBuffer[real][0], &upmixFftTempBuffer[imag][0], &twidtab[real][0], &twidtab[imag][0], UPMIX_WINDOW_LEN);
		ifftf(reBufBL, imBufBL, &upmixFftTempBuffer[real][0], &upmixFftTempBuffer[imag][0], &twidtab[real][0], &twidtab[imag][0], UPMIX_WINDOW_LEN);
		ifftf(reBufBR, imBufBR, &upmixFftTempBuffer[real][0], &upmixFftTempBuffer[imag][0], &twidtab[real][0], &twidtab[imag][0], UPMIX_WINDOW_LEN);

		// apply pre-calculated Hann-Window again to mitigate harmonics
		vecvmltf(reBufL, &hannWindow[0], reBufL, UPMIX_WINDOW_LEN);
		vecvmltf(reBufR, &hannWindow[0], reBufR, UPMIX_WINDOW_LEN);
		vecvmltf(reBufC, &hannWindow[0], reBufC, UPMIX_WINDOW_LEN);
		vecvmltf(reBufBL, &hannWindow[0], reBufBL, UPMIX_WINDOW_LEN);
		vecvmltf(reBufBR, &hannWindow[0], reBufBR, UPMIX_WINDOW_LEN);






		// Step 6: assembling by adding the newbuffer on top of the output-buffer: upmixOutputBuffer += upmixNewBuffer
		// ============================================================================================
		// remove old samples (shift array by UPMIX_RX_SAMPLE_COUNT to the left)
		for (int i_ch = 0; i_ch < 5; i_ch++) {
			arrayIdx = 0;
			for (int i = 0; i < (UPMIX_FFT_HOP_VALUE - 1); i++) {
				// shift the samples to the left
				memcpy(&upmixOutputBuffer[i_ch][arrayIdx], &upmixOutputBuffer[i_ch][arrayIdx + UPMIX_RX_SAMPLE_COUNT], UPMIX_RX_SAMPLE_COUNT * sizeof(float));
				// increment array index
				arrayIdx += UPMIX_RX_SAMPLE_COUNT;
			}
			// set the last elements to zero
			memset(&upmixOutputBuffer[i_ch][arrayIdx], 0, UPMIX_RX_SAMPLE_COUNT * sizeof(float));
		}

		// now assemble the new values for left, right, center, back-left and back-right
		vecvaddf(&upmixOutputBuffer[0][0], reBufL, &upmixOutputBuffer[0][0], UPMIX_WINDOW_LEN);
		vecvaddf(&upmixOutputBuffer[1][0], reBufR, &upmixOutputBuffer[1][0], UPMIX_WINDOW_LEN);
		vecvaddf(&upmixOutputBuffer[2][0], reBufC, &upmixOutputBuffer[2][0], UPMIX_WINDOW_LEN);
		vecvaddf(&upmixOutputBuffer[3][0], reBufBL, &upmixOutputBuffer[3][0], UPMIX_WINDOW_LEN);
		vecvaddf(&upmixOutputBuffer[4][0], reBufBR, &upmixOutputBuffer[4][0], UPMIX_WINDOW_LEN);





		// Step 7: prepare the output
		// ============================================================================================
		// upmixOutputBuffer contains 1024 samples, but only the first 256 are ready to be output
		// as we are using a 16 sample-TDM-buffer, we have to take track of the already sent samples
		// by using the upmixOutputSampleCounter. First it has to be reset to 0 and then incremented by the
		// number of sent (received) samples
		upmixOutputSampleCounter = 0; // we have calculated the next N/2 or N/4 output-samples, so reset the read-pointer
	}

	// copy data to output-buffer
	memcpy(outBuf[0], &upmixOutputBuffer[0][upmixOutputSampleCounter], SAMPLES_IN_BUFFER * sizeof(float)); // left
	memcpy(outBuf[1], &upmixOutputBuffer[1][upmixOutputSampleCounter], SAMPLES_IN_BUFFER * sizeof(float)); // right
	memcpy(outBuf[2], &upmixOutputBuffer[2][upmixOutputSampleCounter], SAMPLES_IN_BUFFER * sizeof(float)); // center


	// Step 8: delay-line for surround-channels
	// ============================================================================================
	// feed delay line with current surround_signal
	for	(int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		delayLineBackLeft[delayLineHead] = upmixOutputBuffer[3][upmixOutputSampleCounter + s];
		delayLineBackRight[delayLineHead] = upmixOutputBuffer[4][upmixOutputSampleCounter + s];
		delayLineHead++;
		if (delayLineHead == FX_UPMIXER_BUFFER_SIZE) {
			delayLineHead = 0;
		}
	}
	// read surround_signal from delay line
	int delayLineTailLeft = delayLineHead - (FX_UPMIXER_DELAY_BACKLEFT_MS * FX_UPMIXER_SAMPLING_RATE / 1000); // here we set the delay in milliseconds
	int delayLineTailRight = delayLineHead - (FX_UPMIXER_DELAY_BACKRIGHT_MS * FX_UPMIXER_SAMPLING_RATE / 1000); // here we set the delay in milliseconds
	if (delayLineTailLeft < 0) { delayLineTailLeft += FX_UPMIXER_BUFFER_SIZE; } // manual wrap-around
	if (delayLineTailRight < 0) { delayLineTailRight += FX_UPMIXER_BUFFER_SIZE; } // manual wrap-around
	for	(int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		outBuf[3][s] = delayLineBackLeft[delayLineTailLeft++];
		if (delayLineTailLeft == FX_UPMIXER_BUFFER_SIZE) { delayLineTailLeft = 0; }

		outBuf[4][s] = delayLineBackRight[delayLineTailRight++];
		if (delayLineTailRight == FX_UPMIXER_BUFFER_SIZE) { delayLineTailRight = 0; }
	}








	// Step 9: Apply LowPass of 7kHz on surround-speakers and 125Hz on LFE
	// ============================================================================================
	/*
	// Single-Pole LowPass: output = zoutput + coeff * (input - zoutput)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		for (int i_ch = 0; i_ch < 2; i_ch++) {
			outBuf[3 + i_ch][s] = lowPassSurroundState[i_ch] + lowPassSurroundCoeff * (outBuf[3 + i_ch][s] - lowPassSurroundState[i_ch]);
			lowPassSurroundState[i_ch] = outBuf[3 + i_ch][s];
		}
	}
	*/

	// calculate LFE as sum of stereo-channels with 80 Hz HighCut
	// Single-Pole LowPass: output = zoutput + coeff * (input - zoutput)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		outBuf[5][s] = lowPassSubState + lowPassSubCoeff * ((inBuf[0][s] + inBuf[1][s]) * 0.5f - lowPassSubState);
		lowPassSubState = outBuf[5][s];
	}








	// increase pointer for next read
	upmixOutputSampleCounter += samples;
	if (upmixOutputSampleCounter >= UPMIX_RX_SAMPLE_COUNT) {
		upmixOutputSampleCounter -= UPMIX_RX_SAMPLE_COUNT;
	}
}

#endif
