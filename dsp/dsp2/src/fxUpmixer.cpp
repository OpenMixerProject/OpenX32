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

  This file implements a frequency-domain-based Stereo-to-5.1-Upmixer

  First the stereo-signal is transformed into the frequency-domain using an FFT with
  a shorter window-length. Within the frequency-domain the coherence-factor phi
  and a pan-factor is calculated based on the stereo-signal. Based on these two
  values, decorrelation-masks for all channels (left, right, center, back-left, back-right) are
  calculated, while the surround-channels are damped in the higher-frequencies above 8kHz.

  Afterwards all 5 channels are brought back to the time-domain using IFFT.

  The back-channels are fed through a delay-line to slightly increase the room.

  The LFE-channel is calculated based on the center-channel together with a lowpass
  filter at 125Hz. All settings can be changed in the header file or down below in
  the header of this file.

  This upmixing-algorithm is based on the work of Sebastian Kraft. He published the general idea
  in his PhD with the title "Stereo Signal Decomposition and Upmixing to Surround and 3D Audio" which can
  be found here: https://openhsu.ub.hsu-hh.de/server/api/core/bitstreams/d2391f49-8092-4bb3-a1ad-85b558655dd7/content
  Thank you!
 */

#include "fxUpmixer.h"

#if FX_USE_UPMIXER == 1

//inline float fast_inv_sqrt(float x) {
//    float y = rsqrtf(x); // hardware-estimation
//    return y * (1.5f - 0.5f * x * y * y); // one Newton-Raphson-Step for more precision
//}

float pm hannWindow[FX_UPMIX_WINDOW_LEN];
float lowPassSubState = 0;
float lowPassSubCoeff = 0.01609904178227480397989f; // 125Hz = 785.398163397448 / 48785.398163397448 <- alpha = (2 * pi * f_c) / (f_s + 2 * pi * f_c) = (2 * pi * 125Hz) / (48000Hz + 2 * pi * 125Hz)
//float lowPassSurroundState[2];
//float lowPassSurroundCoeff = 0.4781604560104657892f; // 7kHz = 43982,297150257105338477007365913 / 91982,297150257105338477007365913 <- alpha = (2 * pi * f_c) / (f_s + 2 * pi * f_c) = (2 * pi * 7000Hz) / (48000Hz + 2 * pi * 7000Hz)
// constants for soft low-pass in frequency domain
const float surroundCutoff = 7000.0f; // soft damping for surround channels starting at 7kHz
const float bin_to_hz = 48000.0f / (float)FX_UPMIX_WINDOW_LEN;

// buffers for input and output
float pm upmixInputBuffer[2][FX_UPMIX_WINDOW_LEN]; // we need ringbuffer for the full window-size
float pm upmixOutputBuffer[5][FX_UPMIX_WINDOW_LEN]; // left, right, center, back-left, back-right

// variables for the FFT and calculation (distribute real/imag-part over data- and program-memory for higher data-throughput)
#pragma align (2 * FX_UPMIX_WINDOW_LEN)
float dm upmixFftInputBufferLR_real[2][FX_UPMIX_WINDOW_LEN]; // imag/real for two channels
#pragma align (2 * FX_UPMIX_WINDOW_LEN)
float pm upmixFftInputBufferLR_imag[2][FX_UPMIX_WINDOW_LEN]; // imag/real for two channels
#pragma align (2 * FX_UPMIX_WINDOW_LEN)
float dm upmixFftOutputBufferLR_real[2][FX_UPMIX_WINDOW_LEN]; // imag/real
#pragma align (2 * FX_UPMIX_WINDOW_LEN)
float pm upmixFftOutputBufferLR_imag[2][FX_UPMIX_WINDOW_LEN]; // imag/real
#pragma align (FX_UPMIX_WINDOW_LEN)
float dm upmixFftOutputBufferC_real[FX_UPMIX_WINDOW_LEN]; // imag/real
#pragma align (FX_UPMIX_WINDOW_LEN)
float pm upmixFftOutputBufferC_imag[FX_UPMIX_WINDOW_LEN]; // imag/real
#pragma align (2 * FX_UPMIX_WINDOW_LEN)
float dm upmixFftOutputBufferBackLR_real[2][FX_UPMIX_WINDOW_LEN]; // imag/real
#pragma align (2 * FX_UPMIX_WINDOW_LEN)
float pm upmixFftOutputBufferBackLR_imag[2][FX_UPMIX_WINDOW_LEN]; // imag/real
#pragma align (FX_UPMIX_WINDOW_LEN_HALF)
float dm twidtab_real[FX_UPMIX_WINDOW_LEN_HALF];
#pragma align (FX_UPMIX_WINDOW_LEN_HALF)
float pm twidtab_imag[FX_UPMIX_WINDOW_LEN_HALF];
#pragma align (FX_UPMIX_WINDOW_LEN)
float dm upmixFftTempBuffer_real[FX_UPMIX_WINDOW_LEN]; // imag/real
#pragma align (FX_UPMIX_WINDOW_LEN)
float pm upmixFftTempBuffer_imag[FX_UPMIX_WINDOW_LEN]; // imag/real

float pm maskCenter[FX_UPMIX_WINDOW_LEN_HALF];
float pm maskLeft[FX_UPMIX_WINDOW_LEN_HALF];
float pm maskRight[FX_UPMIX_WINDOW_LEN_HALF];
float pm maskAmbient[FX_UPMIX_WINDOW_LEN_HALF];
float pm maskCenter_z[FX_UPMIX_WINDOW_LEN_HALF];
float pm maskLeft_z[FX_UPMIX_WINDOW_LEN_HALF];
float pm maskRight_z[FX_UPMIX_WINDOW_LEN_HALF];
float pm maskAmbient_z[FX_UPMIX_WINDOW_LEN_HALF];

/*
	// accessing external RAM
	typedef float fft_block_t[2][FX_UPMIX_WINDOW_LEN];
	fft_block_t* upmixFftBuffer = (fft_block_t*)EXTERNAL_RAM_START;
*/

int upmixInputBufferHead = 0; // start writing at index 0
int upmixInputBufferTail = FX_UPMIX_RX_SAMPLE_COUNT; // start reading at index 1
int upmixInputSampleCounter = 0; // starts always at 0
int upmixOutputSampleCounter = 0; // starts always at 0

int delayLineHead = 0;
float pm delayLineBackLeft[FX_UPMIXER_BUFFER_SIZE];
float pm delayLineBackRight[FX_UPMIXER_BUFFER_SIZE];

void fxUpmixerInit(void) {
	gen_hanning(&hannWindow[0], 1, FX_UPMIX_WINDOW_LEN); // pointer to array, a (Window-spacing), N (Window-Length)
	twidfftf(&twidtab_real[0], &twidtab_imag[0], FX_UPMIX_WINDOW_LEN);

	for (int i = 0; i < FX_UPMIXER_BUFFER_SIZE; i++) {
		delayLineBackLeft[i] = 0.0f;
		delayLineBackRight[i] = 0.0f;
	}
}

void fxUpmixerProcess(float* __restrict inBuf[2], float* __restrict outBuf[6]) {
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
		memcpy(&upmixInputBuffer[i_ch][upmixInputBufferHead], inBuf[i_ch], SAMPLES_IN_BUFFER * sizeof(float));
	}
	upmixInputSampleCounter += SAMPLES_IN_BUFFER; // keep track of received samples

	// increase the head-pointer of the ringbuffer
	upmixInputBufferHead += SAMPLES_IN_BUFFER;
	if (upmixInputBufferHead >= FX_UPMIX_WINDOW_LEN) {
		upmixInputBufferHead -= FX_UPMIX_WINDOW_LEN;
	}

	// when N/2 or N/4 samples are received we can compute the algorithm
	if (upmixInputSampleCounter == FX_UPMIX_RX_SAMPLE_COUNT) {
		// we received N/4 new samples -> process data
		upmixInputSampleCounter = 0; // reset input counter

		// read the last FX_UPMIX_WINDOW_LEN samples out of the unsorted circular-buffer into the sorted FFT-buffer
		for (int i_ch = 0; i_ch < 2; i_ch++) {
			memcpy(&upmixFftInputBufferLR_real[i_ch][0], &upmixInputBuffer[i_ch][upmixInputBufferTail], (FX_UPMIX_WINDOW_LEN - upmixInputBufferTail) * sizeof(float));
			// wrap-around of the end of ring-buffer and copy remaining samples
			memcpy(&upmixFftInputBufferLR_real[i_ch][FX_UPMIX_WINDOW_LEN - upmixInputBufferTail], &upmixInputBuffer[i_ch][0], upmixInputBufferTail * sizeof(float));
		}

		// increase the tail-pointer of the ringbuffer by FX_UPMIX_RX_SAMPLE_COUNT
		upmixInputBufferTail += FX_UPMIX_RX_SAMPLE_COUNT;
		if (upmixInputBufferTail >= FX_UPMIX_WINDOW_LEN) {
			upmixInputBufferTail -= FX_UPMIX_WINDOW_LEN;
		}






		// Step 2: perform FFT
		// ============================================================================================
		// now we have a consistent and sorted sample-buffer with the oldest sample at index 0 and the newest sample at the last index
		// apply pre-calculated Hann-Window
		for (int i_ch = 0; i_ch < 2; i_ch++) {
			vecvmltf(&upmixFftInputBufferLR_real[i_ch][0], &hannWindow[0], &upmixFftInputBufferLR_real[i_ch][0], FX_UPMIX_WINDOW_LEN);
		}

		// TODO: allocate different memory-location for data one compared to data two to increase performance
		rfftf_2(&upmixFftInputBufferLR_real[0][0], &upmixFftInputBufferLR_imag[0][0],
				&upmixFftInputBufferLR_real[1][0], &upmixFftInputBufferLR_imag[1][0],
				&twidtab_real[0], &twidtab_imag[0], FX_UPMIX_WINDOW_LEN);




		/*
		// Step 3 (SIMD-optimized functions): calculation of the interchannel-coherence and create mask for direct-parts
		// ============================================================================================
		// magnitude (out = 2 * sqrt(Re² + Im²) / fftsize)
		float magnitude[3][FX_UPMIX_WINDOW_LEN_HALF];
		float power[2][FX_UPMIX_WINDOW_LEN_HALF];
		fftf_magnitude(&upmixFftInputBufferLR_real[0][0], &upmixFftInputBufferLR_imag[0][0], &magnitude[0][0], FX_UPMIX_WINDOW_LEN, 2); // mode=2 when using data from rfftf_2()
		fftf_magnitude(&upmixFftInputBufferLR_real[1][0], &upmixFftInputBufferLR_imag[1][0], &magnitude[1][0], FX_UPMIX_WINDOW_LEN, 2); // mode=2 when using data from rfftf_2()
		// calculate power
		vecvmltf(&magnitude[0][0], &magnitude[0][0], &power[0][0], FX_UPMIX_WINDOW_LEN_HALF); // a, b, output, samples
		vecvmltf(&magnitude[1][0], &magnitude[1][0], &power[1][0], FX_UPMIX_WINDOW_LEN_HALF); // a, b, output, samples

		// cross-correlation (real-part)
		float crossCorrellation[2][FX_UPMIX_WINDOW_LEN_HALF]; // (Re(x) * Re(y) +/- Im(x) * Im(y))
		crosscorrf(&crossCorrellation[real][0], &upmixFftInputBufferLR_real[0][0], &upmixFftInputBufferLR_imag[0][0], FX_UPMIX_WINDOW_LEN_HALF, FX_UPMIX_WINDOW_LEN_HALF); // output[lags], input_x[samples], input_y[samples], samples, lags

		// cross-correlation (imag-part)
		// first calculate Re²
		vecvmltf(&upmixFftInputBufferLR_real[0][0], &upmixFftInputBufferLR_real[0][0], &upmixFftTempBuffer_real[0], FX_UPMIX_WINDOW_LEN_HALF); // a, b, output, samples
		// now calculate Im²
		vecvmltf(&upmixFftInputBufferLR_imag[0][0], &upmixFftInputBufferLR_imag[0][0], &upmixFftTempBuffer_imag[0], FX_UPMIX_WINDOW_LEN_HALF); // a, b, output, samples
		// now calcualte Re² - Im²
		vecvsubf(&upmixFftTempBuffer_real[0], &upmixFftTempBuffer_imag[0], &crossCorrellation[imag][0], FX_UPMIX_WINDOW_LEN_HALF); // a, b, output, samples

		//magnitude Left-Right
		fftf_magnitude(&crossCorrellation[real][0], &crossCorrellation[imag][0], &magnitude[2][0], FX_UPMIX_WINDOW_LEN, 2); // mode=2 when using data from rfftf_2()

		...
		...
		...
		*/


		// Step 3: calculation of the interchannel-coherence and create mask for direct-parts
		// ============================================================================================
		#pragma loop_count(FX_UPMIX_WINDOW_LEN_HALF)
		for (int k = 0; k < FX_UPMIX_WINDOW_LEN_HALF; k++) {
			// get current values for left and right
			float reL = upmixFftInputBufferLR_real[0][k];
			float imL = upmixFftInputBufferLR_imag[0][k];
			float reR = upmixFftInputBufferLR_real[1][k];
			float imR = upmixFftInputBufferLR_imag[1][k];

			// cross-power: power = Re² + Im²
			float powerL = reL*reL + imL*imL;
			float powerR = reR*reR + imR*imR;

			// magnitude = sqrt(Re² + Im²)
			float magnitudeL = sqrtf(powerL);
			float magnitudeR = sqrtf(powerR);
			/*
			// TODO: check if following code is faster compared to sqrtf()
			// fast approximation of 1/mag
			float invMagnitudeL = fast_inv_sqrt(powerL + 1e-6f);
			float invMagnitudeR = fast_inv_sqrt(powerR + 1e-6f);
			float magnitudeL = powerL * invMagnitudeL; // mag = power * (1/sqrt(power))
			float magnitudeR = powerR * invMagnitudeR;
			*/

			// calculate magnitude of the cross-correlation
			float reCross = reL * reR + imL * imR;
			float imCross = imL * reR - reL * imR;
			float powerCross = reCross*reCross + imCross*imCross;
			float magnitudeLR = sqrtf(powerCross); // sqrt(Re² + Im²) <- here we keep sqrtf()

			// estimate the coherence-factor phi
			// phi = 1 -> total coherence: channel left/right are identical
			// phi = 0.5 -> partly coherent: lot of stereo-effect
			// phi = 0 -> no coherence: channel left/right are totally different
			float invSumPower = 1.0f / (powerL + powerR + 1e-6f);
			float phi = (2.0f * magnitudeLR) * invSumPower;
			if (phi > 1) { phi = 1.0f; } else if (phi < 0) { phi = 0.0f; } // limit to 0..1
			// TODO: implement transient-detection and force phi=1 on steep transients to focus this to the front

			// take pan-level into account
			// pan = 1 -> mono signal
			// pan = 0 -> hard left/right signal
			float invSumMagnitude = 1.0f / (magnitudeL + magnitudeR + 1e-6f);
			float pan = (2.0f * fminf(magnitudeL, magnitudeR)) * invSumMagnitude;
			if (pan > 1) { pan = 1.0f; } else if (pan < 0) { pan = 0.0f; } // limit to 0..1

			// calculate masks for all channels
			float pannedPrimaryMask = phi * (1.0f - pan);
			float totalMag = magnitudeL + magnitudeR + 1e-6f;
			float weightL;
			float weightR;
			if (totalMag != 0) {
				weightL = magnitudeL / totalMag;
				weightR = magnitudeR / totalMag;
			}else{
				weightL = 1.0f;
				weightR = 1.0f;
			}
			maskLeft[k]  = pannedPrimaryMask * weightL;
			maskRight[k] = pannedPrimaryMask * weightR;

			maskCenter[k] = phi * pan;
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
//		for (int k = 0; k < (FX_UPMIX_WINDOW_LEN_HALF); k++) {
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
		float *reBufL = &upmixFftOutputBufferLR_real[0][0];
		float *imBufL = &upmixFftOutputBufferLR_imag[0][0];
		float *reBufR = &upmixFftOutputBufferLR_real[1][0];
		float *imBufR = &upmixFftOutputBufferLR_imag[1][0];
		float *reBufC = &upmixFftOutputBufferC_real[0];
		float *imBufC = &upmixFftOutputBufferC_imag[0];
		float *reBufBL = &upmixFftOutputBufferBackLR_real[0][0];
		float *imBufBL = &upmixFftOutputBufferBackLR_imag[0][0];
		float *reBufBR = &upmixFftOutputBufferBackLR_real[1][0];
		float *imBufBR = &upmixFftOutputBufferBackLR_imag[1][0];

		for (int k = 0; k < (FX_UPMIX_WINDOW_LEN_HALF); k++) {
			#if FX_UPMIX_ADD_AMBIENCE_TO_LR == 0
				// left: panned direct sound + 30% ambient
				reBufL[k] = upmixFftInputBufferLR_real[0][k] * maskLeft[k];
				imBufL[k] = upmixFftInputBufferLR_imag[0][k] * maskLeft[k];

				// right: panned direct sound + 30% ambient
				reBufR[k] = upmixFftInputBufferLR_real[1][k] * maskRight[k];
				imBufR[k] = upmixFftInputBufferLR_imag[1][k] * maskRight[k];
			#else
				// left: panned direct sound + 30% ambient
				reBufL[k] = upmixFftInputBufferLR_real[0][k] * (maskLeft[k] + maskAmbient[k] * 0.3f);
				imBufL[k] = upmixFftInputBufferLR_imag[0][k] * (maskLeft[k] + maskAmbient[k] * 0.3f);

				// right: panned direct sound + 30% ambient
				reBufR[k] = upmixFftInputBufferLR_real[1][k] * (maskRight[k] + maskAmbient[k] * 0.3f);
				imBufR[k] = upmixFftInputBufferLR_imag[1][k] * (maskRight[k] + maskAmbient[k] * 0.3f);
			#endif

			// center: direct sound
			reBufC[k] = (upmixFftInputBufferLR_real[0][k] + upmixFftInputBufferLR_real[1][k]) * maskCenter[k] * 0.707f; // 1/sqrt(2)
			imBufC[k] = (upmixFftInputBufferLR_imag[0][k] + upmixFftInputBufferLR_imag[1][k]) * maskCenter[k] * 0.707f; // 1/sqrt(2)

			#if FX_UPMIX_PHASEINVERTED_BACK_LR == 0
				// backLeft: containing all uncorrelated parts
				reBufBL[k] = upmixFftInputBufferLR_real[0][k] * maskAmbient[k];
				imBufBL[k] = upmixFftInputBufferLR_imag[0][k] * maskAmbient[k];

				// backRight: containing all uncorrelated parts
				reBufBR[k] = upmixFftInputBufferLR_real[1][k] * maskAmbient[k];
				imBufBR[k] = upmixFftInputBufferLR_imag[1][k] * maskAmbient[k];
			#else
				// backLeft: containing all uncorrelated parts
				reBufBL[k] = -upmixFftInputBufferLR_imag[0][k] * maskAmbient[k];
				imBufBL[k] = upmixFftInputBufferLR_real[0][k] * maskAmbient[k];

				// backRight: containing all uncorrelated parts
				reBufBR[k] = upmixFftInputBufferLR_imag[1][k] * maskAmbient[k];
				imBufBR[k] = -upmixFftInputBufferLR_real[1][k] * maskAmbient[k];
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
		for (int i = 1; i < (FX_UPMIX_WINDOW_LEN_HALF); i++) {
			int mirrIdx = FX_UPMIX_WINDOW_LEN - i;

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
		ifftf(reBufL, imBufL, &upmixFftTempBuffer_real[0], &upmixFftTempBuffer_imag[0], &twidtab_real[0], &twidtab_imag[0], FX_UPMIX_WINDOW_LEN);
		ifftf(reBufR, imBufR, &upmixFftTempBuffer_real[0], &upmixFftTempBuffer_imag[0], &twidtab_real[0], &twidtab_imag[0], FX_UPMIX_WINDOW_LEN);
		ifftf(reBufC, imBufC, &upmixFftTempBuffer_real[0], &upmixFftTempBuffer_imag[0], &twidtab_real[0], &twidtab_imag[0], FX_UPMIX_WINDOW_LEN);
		ifftf(reBufBL, imBufBL, &upmixFftTempBuffer_real[0], &upmixFftTempBuffer_imag[0], &twidtab_real[0], &twidtab_imag[0], FX_UPMIX_WINDOW_LEN);
		ifftf(reBufBR, imBufBR, &upmixFftTempBuffer_real[0], &upmixFftTempBuffer_imag[0], &twidtab_real[0], &twidtab_imag[0], FX_UPMIX_WINDOW_LEN);

		// apply pre-calculated Hann-Window again to mitigate harmonics
		vecvmltf(reBufL, &hannWindow[0], reBufL, FX_UPMIX_WINDOW_LEN);
		vecvmltf(reBufR, &hannWindow[0], reBufR, FX_UPMIX_WINDOW_LEN);
		vecvmltf(reBufC, &hannWindow[0], reBufC, FX_UPMIX_WINDOW_LEN);
		vecvmltf(reBufBL, &hannWindow[0], reBufBL, FX_UPMIX_WINDOW_LEN);
		vecvmltf(reBufBR, &hannWindow[0], reBufBR, FX_UPMIX_WINDOW_LEN);






		// Step 6: assembling by adding the newbuffer on top of the output-buffer: upmixOutputBuffer += upmixNewBuffer
		// ============================================================================================
		// remove old samples (shift array by FX_UPMIX_RX_SAMPLE_COUNT to the left)
		for (int i_ch = 0; i_ch < 5; i_ch++) {
			arrayIdx = 0;
			for (int i = 0; i < (FX_UPMIX_FFT_HOP_VALUE - 1); i++) {
				// shift the samples to the left
				memcpy(&upmixOutputBuffer[i_ch][arrayIdx], &upmixOutputBuffer[i_ch][arrayIdx + FX_UPMIX_RX_SAMPLE_COUNT], FX_UPMIX_RX_SAMPLE_COUNT * sizeof(float));
				// increment array index
				arrayIdx += FX_UPMIX_RX_SAMPLE_COUNT;
			}
			// set the last elements to zero
			memset(&upmixOutputBuffer[i_ch][arrayIdx], 0, FX_UPMIX_RX_SAMPLE_COUNT * sizeof(float));
		}

		// now assemble the new values for left, right, center, back-left and back-right
		vecvaddf(&upmixOutputBuffer[0][0], reBufL, &upmixOutputBuffer[0][0], FX_UPMIX_WINDOW_LEN);
		vecvaddf(&upmixOutputBuffer[1][0], reBufR, &upmixOutputBuffer[1][0], FX_UPMIX_WINDOW_LEN);
		vecvaddf(&upmixOutputBuffer[2][0], reBufC, &upmixOutputBuffer[2][0], FX_UPMIX_WINDOW_LEN);
		vecvaddf(&upmixOutputBuffer[3][0], reBufBL, &upmixOutputBuffer[3][0], FX_UPMIX_WINDOW_LEN);
		vecvaddf(&upmixOutputBuffer[4][0], reBufBR, &upmixOutputBuffer[4][0], FX_UPMIX_WINDOW_LEN);





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

	// calculate LFE as sum of stereo-channels with 125 Hz HighCut
	// Single-Pole LowPass: output = zoutput + coeff * (input - zoutput)
	#pragma loop_count(SAMPLES_IN_BUFFER)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		float inputSum = (inBuf[0][s] + inBuf[1][s]) * 0.5f; // SHARC supports Dual-MAC in a single clock-cycle
		lowPassSubState += lowPassSubCoeff * (inputSum - lowPassSubState); // SHARC supports Dual-MAC in a single clock-cycle
		outBuf[5][s] = lowPassSubState;
	}








	// increase pointer for next read
	upmixOutputSampleCounter += SAMPLES_IN_BUFFER;
	if (upmixOutputSampleCounter >= FX_UPMIX_RX_SAMPLE_COUNT) {
		upmixOutputSampleCounter -= FX_UPMIX_RX_SAMPLE_COUNT;
	}
}

#endif
