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

#define UPMIX_WINDOW_LEN		128
#define UPMIX_FFT_HOP_VALUE		4
#define UPMIX_RX_SAMPLE_COUNT	(UPMIX_WINDOW_LEN / UPMIX_FFT_HOP_VALUE) // use fine N/4 to get 75% overlap (alternatively use N/2 for 50% overlap for less computational stress)

float hannWindow[UPMIX_WINDOW_LEN];

float upmixInputBuffer[2][UPMIX_WINDOW_LEN]; // we need ringbuffer for the full window-size
float upmixFftBuffer[2][UPMIX_WINDOW_LEN]; // ordered buffer for all received samples
float upmixNewBuffer[5][UPMIX_WINDOW_LEN]; // left, right, center, back-left, back-right
float upmixOutputBuffer[5][UPMIX_WINDOW_LEN]; // left, right, center, back-left, back-right

complex_float inputSignalFrequencyDomain[2][UPMIX_WINDOW_LEN]; // FFT of input signal
complex_float outputSignalFrequencyDomain[5][UPMIX_WINDOW_LEN]; // left, right, center, back-left, back-right
complex_float outputSignalsTimeDomain[UPMIX_WINDOW_LEN];

float maskCenter[UPMIX_WINDOW_LEN/2];
float maskLeft[UPMIX_WINDOW_LEN/2];
float maskRight[UPMIX_WINDOW_LEN/2];
float maskAmbient[UPMIX_WINDOW_LEN/2];
float maskCenter_z[UPMIX_WINDOW_LEN/2];
float maskLeft_z[UPMIX_WINDOW_LEN/2];
float maskRight_z[UPMIX_WINDOW_LEN/2];
float maskAmbient_z[UPMIX_WINDOW_LEN/2];

int upmixInputBufferHead = 0; // start writing at index 0
int upmixInputBufferTail = UPMIX_RX_SAMPLE_COUNT; // start reading at index 1
int upmixInputSampleCounter = 0; // starts always at 0
int upmixOutputSampleCounter = 0; // starts always at 0

void fxUpmixerInit(void) {
	gen_hanning(&hannWindow[0], 1, UPMIX_WINDOW_LEN); // pointer to array, a (Window-spacing), N (Window-Length)
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
	// Step 6: upmixing
	// Step 7: output

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
			memcpy(&upmixFftBuffer[i_ch][0], &upmixInputBuffer[i_ch][upmixInputBufferTail], (UPMIX_WINDOW_LEN - upmixInputBufferTail) * sizeof(float));
			// wrap-around of the end of ring-buffer and copy remaining samples
			memcpy(&upmixFftBuffer[i_ch][UPMIX_WINDOW_LEN - upmixInputBufferTail], &upmixInputBuffer[i_ch][0], upmixInputBufferTail * sizeof(float));
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
			vecvmltf(&upmixFftBuffer[i_ch][0], &hannWindow[0], &upmixFftBuffer[i_ch][0], UPMIX_WINDOW_LEN);
		}

		for (int i_ch = 0; i_ch < 2; i_ch++) {
			#if UPMIX_WINDOW_LEN == 64
				rfft64(&upmixFftBuffer[i_ch][0], &inputSignalFrequencyDomain[i_ch][0]);
			#elif UPMIX_WINDOW_LEN == 128
				rfft128(&upmixFftBuffer[i_ch][0], &inputSignalFrequencyDomain[i_ch][0]);
			#elif UPMIX_WINDOW_LEN == 256
				rfft256(&upmixFftBuffer[i_ch][0], &inputSignalFrequencyDomain[i_ch][0]);
			#elif UPMIX_WINDOW_LEN == 512
				rfft512(&upmixFftBuffer[i_ch][0], &inputSignalFrequencyDomain[i_ch][0]);
			#elif UPMIX_WINDOW_LEN == 1024
				rfft1024(&upmixFftBuffer[i_ch][0], &inputSignalFrequencyDomain[i_ch][0]);
			#endif

			// the following lines are only needed, when using iFFT on inputSignalFrequencyDomain
			// calculate the Nyquist frequency -> add 2nd half of the spectrum (only necessary for iFFT lateron)
			//for (int i = 1; i < (UPMIX_WINDOW_LEN/2); i++) {
			//	inputSignalFrequencyDomain[i_ch][UPMIX_WINDOW_LEN - i] = conjf(inputSignalFrequencyDomain[i_ch][i]);
			//}
		}





		// Step 3: calculation of the interchannel-coherence and create mask for direct-parts
		// ============================================================================================
		for (int k = 0; k < (UPMIX_WINDOW_LEN/2); k++) {
			// cross-power: power = Re² + Im²
			float powerLeft = (inputSignalFrequencyDomain[0][k].re * inputSignalFrequencyDomain[0][k].re) + (inputSignalFrequencyDomain[0][k].im * inputSignalFrequencyDomain[0][k].im);
			float powerRight = (inputSignalFrequencyDomain[1][k].re * inputSignalFrequencyDomain[1][k].re) + (inputSignalFrequencyDomain[1][k].im * inputSignalFrequencyDomain[1][k].im);

			// magnitude = sqrt(Re² + Im²)
			float magLeft = sqrtf(powerLeft);
			float magRight = sqrtf(powerRight);

			// calculate magnitude of the cross-correlation
			float realCross = (inputSignalFrequencyDomain[0][k].re * inputSignalFrequencyDomain[1][k].re) + (inputSignalFrequencyDomain[0][k].im * inputSignalFrequencyDomain[1][k].im);
			float imagCross = (inputSignalFrequencyDomain[0][k].im * inputSignalFrequencyDomain[1][k].re) - (inputSignalFrequencyDomain[0][k].re * inputSignalFrequencyDomain[1][k].im);
			float magLeftRight = sqrtf((realCross * realCross) + (imagCross * imagCross)); // sqrt(Re² + Im²)

			// estimate the coherence-factor phi
			// phi = 1 -> total coherence: channel left/right are identical
			// phi = 0.5 -> partly coherent: lot of stereo-effect
			// phi = 0 -> no coherence: channel left/right are totally different
			float phi = (2.0f * magLeftRight) / (powerLeft + powerRight + 1e-6f);
			if (phi > 1) { phi = 1.0f; } else if (phi < 0) { phi = 0.0f; } // limit to 0..1

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
		for (int k = 0; k < (UPMIX_WINDOW_LEN/2); k++) {
			#if FX_UPMIX_ADD_AMBIENCE_TO_LR == 0
				// left: panned direct sound + 30% ambient
				outputSignalFrequencyDomain[0][k].re = inputSignalFrequencyDomain[0][k].re * maskLeft[k];
				outputSignalFrequencyDomain[0][k].im = inputSignalFrequencyDomain[0][k].im * maskLeft[k];

				// right: panned direct sound + 30% ambient
				outputSignalFrequencyDomain[1][k].re = inputSignalFrequencyDomain[1][k].re * maskRight[k];
				outputSignalFrequencyDomain[1][k].im = inputSignalFrequencyDomain[1][k].im * maskRight[k];
			#else
				// left: panned direct sound + 30% ambient
				outputSignalFrequencyDomain[0][k].re = inputSignalFrequencyDomain[0][k].re * (maskLeft[k] + maskAmbient[k] * 0.3f);
				outputSignalFrequencyDomain[0][k].im = inputSignalFrequencyDomain[0][k].im * (maskLeft[k] + maskAmbient[k] * 0.3f);

				// right: panned direct sound + 30% ambient
				outputSignalFrequencyDomain[1][k].re = inputSignalFrequencyDomain[1][k].re * (maskRight[k] + maskAmbient[k] * 0.3f);
				outputSignalFrequencyDomain[1][k].im = inputSignalFrequencyDomain[1][k].im * (maskRight[k] + maskAmbient[k] * 0.3f);
			#endif

			// center: direct sound
			outputSignalFrequencyDomain[2][k].re = (inputSignalFrequencyDomain[0][k].re + inputSignalFrequencyDomain[1][k].re) * maskCenter[k] * 0.707f; // 1/sqrt(2)
			outputSignalFrequencyDomain[2][k].im = (inputSignalFrequencyDomain[0][k].im + inputSignalFrequencyDomain[1][k].im) * maskCenter[k] * 0.707f; // 1/sqrt(2)

			#if FX_UPMIX_PHASEINVERTED_BACK_LR == 0
				// backLeft: containing all uncorrelated parts
				outputSignalFrequencyDomain[3][k].re = inputSignalFrequencyDomain[0][k].re * maskAmbient[k];
				outputSignalFrequencyDomain[3][k].im = inputSignalFrequencyDomain[0][k].im * maskAmbient[k];

				// backRight: containing all uncorrelated parts
				outputSignalFrequencyDomain[4][k].re = inputSignalFrequencyDomain[1][k].re * maskAmbient[k];
				outputSignalFrequencyDomain[4][k].im = inputSignalFrequencyDomain[1][k].im * maskAmbient[k];
			#else
				// backLeft: containing all uncorrelated parts
				outputSignalFrequencyDomain[3][k].re = -inputSignalFrequencyDomain[0][k].im * maskAmbient[k];
				outputSignalFrequencyDomain[3][k].im = inputSignalFrequencyDomain[0][k].re * maskAmbient[k];

				// backRight: containing all uncorrelated parts
				outputSignalFrequencyDomain[4][k].re = inputSignalFrequencyDomain[1][k].im * maskAmbient[k];
				outputSignalFrequencyDomain[4][k].im = -inputSignalFrequencyDomain[1][k].re * maskAmbient[k];
			#endif
		}


		// calculate the Nyquist frequency -> add 2nd half of the spectrum
		for (int i_ch = 0; i_ch < 5; i_ch++) {
			for (int i = 1; i < (UPMIX_WINDOW_LEN/2); i++) {
				outputSignalFrequencyDomain[i_ch][UPMIX_WINDOW_LEN - i] = conjf(outputSignalFrequencyDomain[i_ch][i]); // Re=Re, Im=-Im
			}
		}






		// Step 5: perform inverse-FFT
		// ============================================================================================
		for (int i_ch = 0; i_ch < 5; i_ch++) {
			#if UPMIX_WINDOW_LEN == 64
				ifft64(&outputSignalFrequencyDomain[i_ch][0], &outputSignalsTimeDomain[0]);
			#elif UPMIX_WINDOW_LEN == 128
				ifft128(&outputSignalFrequencyDomain[i_ch][0], &outputSignalsTimeDomain[0]);
			#elif UPMIX_WINDOW_LEN == 256
				ifft256(&outputSignalFrequencyDomain[i_ch][0], &outputSignalsTimeDomain[0]);
			#elif UPMIX_WINDOW_LEN == 512
				ifft512(&outputSignalFrequencyDomain[i_ch][0], &outputSignalsTimeDomain[0]);
			#elif UPMIX_WINDOW_LEN == 1024
				ifft1024(&outputSignalFrequencyDomain[i_ch][0], &outputSignalsTimeDomain[0]);
			#endif

			// copy samples
			for (int s = 0; s < UPMIX_WINDOW_LEN; s++) {
				upmixNewBuffer[i_ch][s] = outputSignalsTimeDomain[s].re;
			}

			// apply pre-calculated Hann-Window again
			vecvmltf(&upmixNewBuffer[i_ch][0], &hannWindow[0], &upmixNewBuffer[i_ch][0], UPMIX_WINDOW_LEN);
		}


		// TODO: use an IIR allpass for the ambience-channels to mitigate phase cancellations






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
		for (int i_ch = 0; i_ch < 5; i_ch++) {
			vecvaddf(&upmixOutputBuffer[i_ch][0], &upmixNewBuffer[i_ch][0], &upmixOutputBuffer[i_ch][0], UPMIX_WINDOW_LEN);
		}





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
	memcpy(outBuf[3], &upmixOutputBuffer[3][upmixOutputSampleCounter], SAMPLES_IN_BUFFER * sizeof(float)); // back-left
	memcpy(outBuf[4], &upmixOutputBuffer[4][upmixOutputSampleCounter], SAMPLES_IN_BUFFER * sizeof(float)); // back-right

	// calculate LFE as sum of stereo-channels with 80 Hz HighCut
	// TODO: use data of inBuf[2] to create new mono-signal -> HighCut-Filter -> outBuf[5]

	// increase pointer for next read
	upmixOutputSampleCounter += samples;
	if (upmixOutputSampleCounter >= UPMIX_RX_SAMPLE_COUNT) {
		upmixOutputSampleCounter -= UPMIX_RX_SAMPLE_COUNT;
	}
}

#endif
