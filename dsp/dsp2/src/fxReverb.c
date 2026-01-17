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

  This file implements a high-quality Feedback-Delay-Network (FDN) Reverb with multiple diffusors
  Reverb is based on work of Geraint Luff (Smithaudio) who published his work on his blog
  (https://signalsmith-audio.co.uk/writing/2021/lets-write-a-reverb) as well as on GitHub
  (https://github.com/Signalsmith-Audio/reverb-example-code) Thank You!
*/

#include "fxReverb.h"

#if FX_USE_REVERB == 1

float* diffusionDelayLine[FX_REVERB_DIFFUSION_STEPS][FX_REVERB_INT_CHAN];
float* delayLine[FX_REVERB_INT_CHAN];

float fxBufInput[FX_REVERB_INT_CHAN];
float fxBuf[FX_REVERB_INT_CHAN];
float fxBufTemp[FX_REVERB_INT_CHAN];
float fxBufOutput[FX_REVERB_INT_CHAN];
float fxBufFeedback[FX_REVERB_INT_CHAN];

#if FX_REVERN_ALTERNATIVE_POW == 1
	// Fast power-of-10 approximation, with RMS error of 1.77%. 
	// This approximation developed by Nicol Schraudolph (Neural Computation vol 11, 1999).  
	// Adapted for 32-bit floats by Lippold Haken of Haken Audio, April 2010.
	// Set float variable's bits to integer expression.
	// f=b^f is approximated by
	//   (int)f = f*0x00800000*log(b)/log(2) + 0x3F800000-60801*8
	// f=10^f is approximated by
	//   (int)f = f*27866352.6 + 1064866808.0
	inline void Pow10(float *f) { *(int *)f = *f * 27866352.6f + 1064866808.0f; };
#endif

#if FX_REVERN_ALTERNATIVE_RND == 1
	// Random number generator that sounds pleasant in audio algorithms.
	// The argument and return value is a 30-bit (positive nonzero) integer.
	// This is Duttaro's 30-bit pseudorandom number generator, p.124 J.AES, vol 50 no 3 March 2002; 
	// I found this 30-bit pseudorandom number generator sounds far superior to the 31-bit 
	// pseudorandom number generator presented in the same article.
	// To make result a signed fixed-point value, do not shift left into the sign bit;
	// instead, subtract 0x20000000 then multiply by a scale factor.
	#define NextRand(rr) (((((rr >> 16) ^ (rr >> 15) ^ (rr >> 1) ^ rr) & 1) << 29) | (rr >> 1))
	int rr;
#endif

// ====================================================================
// Begin of internal functions for this effect
// ====================================================================

#if FX_REVERN_ALTERNATIVE_RND == 0
	float randomInRange(float low, float high) {
		float unitRand = fabsf((float)rand() / 2147483648.0f); // 2^31 = 2147483648.0f
		return low + unitRand * (high - low);
	}
#else
	float randomInRangeAlt(int* rr, float low, float high) {
		*rr = NextRand(*rr);
		float unitRand = (float)*rr/1073741824.0f; // 2^30 = 1073741824
		return low + unitRand * (high - low);
	}
#endif

void householderMatrix(float* bufIn, float* bufOut) {
	// calculate sum over all input-channels
	float sum = 0;
	for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
		sum += bufIn[i_ch];
	}

	// add the scaled sum to each channel of the delay-path
	sum = sum * (-2.0f / (float)FX_REVERB_INT_CHAN);
	vecsaddf(&bufIn[0], sum, &bufOut[0], FX_REVERB_INT_CHAN);
}

void hadamardRecursiveUnscaled(float* buf, int size) {
	if (size <= 1) return; // limit the recursion until we reached a single channel
	int hSize = size / 2;
	
	// calculate two (unscaled) Hadamards of half the size
	hadamardRecursiveUnscaled(buf, hSize);
	hadamardRecursiveUnscaled(buf + hSize, hSize);
	
	// combine the two halves
	// buf[firstHalf] = buf[firstHalf] + buf[secondHalf]
	// buf[secondHalf] = buf[firstHalf] - buf[secondHalf]
	memcpy(&fxBufTemp[0], &buf[0], hSize * sizeof(float)); // store first half of data into temporary storage
	vecvaddf(&buf[0], &buf[hSize], &buf[0], hSize);	
	vecvsubf(&fxBufTemp[0], &buf[hSize], &buf[hSize], hSize);
}

void hadamardMatrix(float* buf) {
	hadamardRecursiveUnscaled(buf, FX_REVERB_INT_CHAN);
	
	// scale the result
	vecsmltf(&buf[0], FX_REVERB_HADAMARD_SCALING, &buf[0], FX_REVERB_INT_CHAN);

}

// ====================================================================
// Begin of public functions for this effect
// ====================================================================

void fxReverbInit(void) {
	// map memory-pointers to desired address in external RAM (please let me know if you know a better option)
	int memoryAddress = SDRAM_START;
	for (int d = 0; d < FX_REVERB_DIFFUSION_STEPS; d++) {
		for (int i = 0; i < FX_REVERB_INT_CHAN; i++) {
			diffusionDelayLine[d][i] = (float*)(memoryAddress);
			memoryAddress += (FX_REVERB_BUFFER_SIZE * sizeof(float));
		}
	}
	for (int i = 0; i < FX_REVERB_INT_CHAN; i++) {
		delayLine[i] = (float*)(memoryAddress);
		memoryAddress += (FX_REVERB_BUFFER_SIZE * sizeof(float));
	}

	// initialize variables and the effect itself
	reverb.diffusionDelayLineHead = 0;
	reverb.delay.head = 0;
	fxReverbSetParameters(100.0f, 3.0f, 1.0f, 0.25f); // roomSizeMs, rt60, dry, wet
}

void fxReverbSetParameters(float roomSizeMs, float rt60, float dry, float wet) {
	reverb.roomSizeMs = roomSizeMs;
	reverb.loopsPerRt60 = rt60 / (roomSizeMs * 1.5f * 0.001f);
	reverb.dbPerCycle = -60.0f / reverb.loopsPerRt60;
	#if FX_REVERN_ALTERNATIVE_POW == 0
		// builtin-function
		reverb.feedbackDecayGain = powf(10, reverb.dbPerCycle * 0.05f); // -1.5dB/cycle = x0.85
	#else
		// pow10-approximation
		reverb.feedbackDecayGain = Pow10(reverb.dbPerCycle * 0.05f); // -1.5dB/cycle = x0.85
	#endif
	reverb.dry = dry;
	reverb.wet = wet;
	
	float diffusionMs = reverb.roomSizeMs;
	for (int d = 0; d < FX_REVERB_DIFFUSION_STEPS; d++) {
		diffusionMs *= 0.5f; // first element has 50% of desired roomSize, next 25%, next 12.5% and so on
		float diffusionDelaySamplesRange = diffusionMs * (float)FX_REVERB_SAMPLING_RATE * 0.001f;
		
		for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
			float rangeLow = diffusionDelaySamplesRange * (float)i_ch / (float)FX_REVERB_INT_CHAN;
			float rangeHigh = diffusionDelaySamplesRange * (float)(i_ch + 1) / (float)FX_REVERB_INT_CHAN;
			#if FX_REVERN_ALTERNATIVE_RND == 0
				reverb.diffusor[d].delayLineTailOffset[i_ch] = randomInRange(rangeLow, rangeHigh);
			#else
				reverb.diffusor[d].delayLineTailOffset[i_ch] = randomInRangeAlt(&rr, rangeLow, rangeHigh);
			#endif
			reverb.diffusor[d].flipPolarities[i_ch] = rand() % 2;
		}
	}

	// calculate the delay for multichannel-feedback
	float delayLineTailOffsetBase = (reverb.roomSizeMs * (float)FX_REVERB_SAMPLING_RATE * 0.001f);
	for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
		reverb.delay.tailOffset[i_ch] = powf(2.0f, (float)i_ch / (float)FX_REVERB_INT_CHAN) * delayLineTailOffsetBase; // TODO: make sure that we are not exceeding the limits of the delay-array here
	}
}

void fxReverbProcess(float* bufIn[2], float* bufOut[2]) {
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		// Step 1: generate multichannel fx-input
		// =================================
		#if FX_REVERB_INT_CHAN >= 2
			fxBufInput[0] = bufIn[0][s]; // left
			fxBufInput[1] = bufIn[1][s]; // right
		#endif
		#if FX_REVERB_INT_CHAN >= 4
			fxBufInput[2] = bufIn[0][s]; // left
			fxBufInput[3] = bufIn[1][s]; // right
		#endif
		#if FX_REVERB_INT_CHAN >= 6
			fxBufInput[4] = bufIn[0][s]; // left
			fxBufInput[5] = bufIn[1][s]; // right
		#endif
		#if FX_REVERB_INT_CHAN >= 8
			fxBufInput[6] = bufIn[0][s]; // left
			fxBufInput[7] = bufIn[1][s]; // right
		#endif
		
		
		

		
		

		// Step 2: diffusion-process
		// =================================
		// copy samples from input to internal buffer
		memcpy(&fxBuf[0], &fxBufInput[0], FX_REVERB_INT_CHAN * sizeof(float));
		
		// Calculate desired amount of diffusion-steps
		for (int d = 0; d < FX_REVERB_DIFFUSION_STEPS; d++) {
			// Diffusor #1 ... #n
			// ---------------------------------
			// Step 2.1: Write data to delay-line
			for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
				diffusionDelayLine[d][i_ch][reverb.diffusionDelayLineHead] = fxBuf[i_ch];
			}

			// Step 2.2: Read data from delay-line
			for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
				int diffusionDelayLineTail = reverb.diffusionDelayLineHead - reverb.diffusor[d].delayLineTailOffset[i_ch];
				if (diffusionDelayLineTail < 0) {
					diffusionDelayLineTail += FX_REVERB_BUFFER_SIZE;
				}
				fxBuf[i_ch] = diffusionDelayLine[d][i_ch][diffusionDelayLineTail];
			}

			// Step 2.3: mix with Hadamard Matrix
			hadamardMatrix(&fxBuf[0]);

			// Step 2.4: Flip some polarities
			for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
				if (reverb.diffusor[d].flipPolarities[i_ch]) fxBuf[i_ch] *= -1.0f;
			}
		}
		// Increase the delay-head-pointer after last Diffusor
		reverb.diffusionDelayLineHead++;
		if (reverb.diffusionDelayLineHead == FX_REVERB_BUFFER_SIZE) {
			reverb.diffusionDelayLineHead = 0;
		}

		// bypass the diffusor
		//memcpy(&fxBuf[0], &fxBufInput[0], FX_REVERB_INT_CHAN * sizeof(float));




		// Step 3: feedbackProcess
		// =================================
		// Step 3.1: read delayed data from delay-line
		for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
			int delayLineTail = reverb.delay.head - reverb.delay.tailOffset[i_ch];
			if (delayLineTail < 0) {
				delayLineTail += FX_REVERB_BUFFER_SIZE;
			}
			fxBufOutput[i_ch] = delayLine[i_ch][delayLineTail];
		}
		
		// Step 3.2: calculate householder-matrix
		householderMatrix(&fxBufOutput[0], &fxBufFeedback[0]);
		vecsmltf(&fxBufFeedback[0], reverb.feedbackDecayGain, &fxBufFeedback[0], FX_REVERB_INT_CHAN);
		vecvaddf(&fxBuf[0], &fxBufFeedback[0], &fxBuf[0], FX_REVERB_INT_CHAN); // temp = diffusedSignal + feedback
		
		// Step 3.3: write temp-data to delay-line
		for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
			delayLine[i_ch][reverb.delay.head] = fxBuf[i_ch];
		}
		reverb.delay.head++;
		if (reverb.delay.head == FX_REVERB_BUFFER_SIZE) {
			reverb.delay.head = 0;
		}



/*
		// direct write/read to external SDRAM
		for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
			delayLine[i_ch][reverb.delay.head] = fxBuf[i_ch];
		}
		for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
			int delayLineTail = reverb.delay.head - reverb.delay.tailOffset[i_ch];
			if (delayLineTail < 0) {
				delayLineTail += FX_REVERB_BUFFER_SIZE;
			}
			fxBufOutput[i_ch] = delayLine[i_ch][delayLineTail];
		}
		reverb.delay.head++;
		if (reverb.delay.head == FX_REVERB_BUFFER_SIZE) {
			reverb.delay.head = 0;
		}
*/
		// bypass the feedback
//		memcpy(&fxBufOutput[0], &fxBufInput[0], FX_REVERB_INT_CHAN * sizeof(float));



		// Step 4: generate output-signal
		// =================================
		// Step 4.1: calculate wet/dry output
		vecsmltf(&fxBufInput[0], reverb.dry, &fxBuf[0], FX_REVERB_INT_CHAN); // temp = fxInput * dry
		vecsmltf(&fxBufOutput[0], reverb.wet, &fxBufOutput[0], FX_REVERB_INT_CHAN); // fxDelayOutput * wet
		vecvaddf(&fxBuf[0], &fxBufOutput[0], &fxBufOutput[0], FX_REVERB_INT_CHAN); // output = dry + wet
		
		// Step 4.2: mix all channels back to stereo
		#if FX_REVERB_AVERAGE_OUTPUT == 1
			// mix multichannel effect-output back into stereo
			bufOut[0][s] = fxBufOutput[0];
			bufOut[1][s] = fxBufOutput[1];
			for (int i_ch = 2; i_ch < FX_REVERB_INT_CHAN; i_ch +=2) {
				bufOut[0][s] += fxBufOutput[i_ch];
				bufOut[1][s] += fxBufOutput[i_ch + 1];
			}
			// scale output
			float scaling = 2.0f / (float)FX_REVERB_INT_CHAN;
			bufOut[0][s] *= scaling;
			bufOut[1][s] *= scaling;
		#else
			// take fxOutput 1/2 as left/right-channel directly
			bufOut[0][s] = fxBufOutput[0];
			bufOut[1][s] = fxBufOutput[1];
		#endif
	}
}

#endif
