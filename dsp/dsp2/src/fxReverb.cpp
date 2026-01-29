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

#pragma file_attr("prefersMem=internal") // let the linker know, that all variables should be placed into the internal ram

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

/*
// regular implementation of Householder-Matrix
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
*/

// SIMD-optimized Householder-Matrix
#if FX_REVERB_INT_CHAN == 8
	static inline void householderMatrix(float* __restrict bufIn, float* __restrict bufOut) {
		// Step 1: load data into register
		float r0 = bufIn[0];
		float r1 = bufIn[1];
		float r2 = bufIn[2];
		float r3 = bufIn[3];
		float r4 = bufIn[4];
		float r5 = bufIn[5];
		float r6 = bufIn[6];
		float r7 = bufIn[7];

		// Step 2: calculate sum (tree-structure for better Pipelining-Latency)
		// instead of ((r0+r1)+r2)... we are calculating part-sums
		float sumA = r0 + r1;
		float sumB = r2 + r3;
		float sumC = r4 + r5;
		float sumD = r6 + r7;

		float sumTotal = (sumA + sumB) + (sumC + sumD);

		// Step 3: scaling-factor
		const float scaleFactor = -2.0f / 8.0f;
		float feedbackScalar = sumTotal * scaleFactor;

		// Step 4: sum and write-back
		bufOut[0] = r0 + feedbackScalar;
		bufOut[1] = r1 + feedbackScalar;
		bufOut[2] = r2 + feedbackScalar;
		bufOut[3] = r3 + feedbackScalar;
		bufOut[4] = r4 + feedbackScalar;
		bufOut[5] = r5 + feedbackScalar;
		bufOut[6] = r6 + feedbackScalar;
		bufOut[7] = r7 + feedbackScalar;
	}
#else
	static inline void householderMatrix(float* __restrict bufIn, float* __restrict bufOut) {
		float sum = 0.0f;

		// calculate sum
		#pragma loop_count(FX_REVERB_INT_CHAN)
		for (int i = 0; i < FX_REVERB_INT_CHAN; i++) {
			sum += bufIn[i];
		}

		// pre-calculate sclaing-factors
		const float scale = sum * (-2.0f / (float)FX_REVERB_INT_CHAN);

		// vector-add (manual instead of using vecsaddf for better pipeline-integration)
		#pragma loop_count(FX_REVERB_INT_CHAN)
		for (int i = 0; i < FX_REVERB_INT_CHAN; i++) {
			bufOut[i] = bufIn[i] + scale;
		}
	}
#endif

/*
// regular implementation of Hadamard-matrix
void hadamardRecursiveUnscaled(float* buf, int size) {
	if (size <= 1) return; // limit the recursion until we reached a single channel
	int hSize = size / 2;

	// calculate two (unscaled) Hadamards of half the size
	hadamardRecursiveUnscaled(buf, hSize);
	hadamardRecursiveUnscaled(buf + hSize, hSize);

	// combine the two halves
	// buf[firstHalf] = buf[firstHalf] + buf[secondHalf]
	// buf[secondHalf] = buf[firstHalf] - buf[secondHalf]
	memcpy(&_fxBufTemp[0], &buf[0], hSize * sizeof(float)); // store first half of data into temporary storage
	vecvaddf(&buf[0], &buf[hSize], &buf[0], hSize);
	vecvsubf(&_fxBufTemp[0], &buf[hSize], &buf[hSize], hSize);
}

void hadamardMatrix(float* buf) {
	hadamardRecursiveUnscaled(buf, FX_REVERB_INT_CHAN);

	// scale the result
	vecsmltf(&buf[0], FX_REVERB_HADAMARD_SCALING, &buf[0], FX_REVERB_INT_CHAN);

}
*/

// SIMD-optimized iterative Hadamard-Transformation using Fast Walsh-Hadamard Transform
#if FX_REVERB_INT_CHAN == 8
	static inline void hadamardMatrix(float* __restrict buf) {
		// Step 1: preload data into register
		float r0 = buf[0];
		float r1 = buf[1];
		float r2 = buf[2];
		float r3 = buf[3];
		float r4 = buf[4];
		float r5 = buf[5];
		float r6 = buf[6];
		float r7 = buf[7];

		float temp; // temporary register for exchange-operations

		// ==================================================
		// Step 2: pairs (0,1), (2,3), (4,5), (6,7)
		// ==================================================
		// Butterfly 0-1
		temp = r0; r0 = temp + r1; r1 = temp - r1;
		// Butterfly 2-3
		temp = r2; r2 = temp + r3; r3 = temp - r3;
		// Butterfly 4-5
		temp = r4; r4 = temp + r5; r5 = temp - r5;
		// Butterfly 6-7
		temp = r6; r6 = temp + r7; r7 = temp - r7;

		// ==================================================
		// Step 3: pairs (0,2), (1,3), (4,6), (5,7)
		// ==================================================
		// Butterfly 0-2
		temp = r0; r0 = temp + r2; r2 = temp - r2;
		// Butterfly 1-3
		temp = r1; r1 = temp + r3; r3 = temp - r3;
		// Butterfly 4-6
		temp = r4; r4 = temp + r6; r6 = temp - r6;
		// Butterfly 5-7
		temp = r5; r5 = temp + r7; r7 = temp - r7;

		// ==================================================
		// Step 4: pairs (0,4), (1,5), (2,6), (3,7)
		// ==================================================
		// Butterfly 0-4
		temp = r0; r0 = temp + r4; r4 = temp - r4;
		// Butterfly 1-5
		temp = r1; r1 = temp + r5; r5 = temp - r5;
		// Butterfly 2-6
		temp = r2; r2 = temp + r6; r6 = temp - r6;
		// Butterfly 3-7
		temp = r3; r3 = temp + r7; r7 = temp - r7;

		// ==================================================
		// Scaling and Write-Back
		// ==================================================
		// Der SHARC kann Multiplikation und Store oft pipelinen
		const float scale = FX_REVERB_HADAMARD_SCALING;

		buf[0] = r0 * scale;
		buf[1] = r1 * scale;
		buf[2] = r2 * scale;
		buf[3] = r3 * scale;
		buf[4] = r4 * scale;
		buf[5] = r5 * scale;
		buf[6] = r6 * scale;
		buf[7] = r7 * scale;
	}
#else
	static inline void hadamardMatrix(float* __restrict buf) {
		// Iterative Butterfly-Structure (Fast Walsh-Hadamard Transform)
		// H1 = [1]
		// H2 = [1  1]
		//      [1 -1]

		// stepsize increases in each loop: 1,2,4,8,...
		for (int h = 1; h < FX_REVERB_INT_CHAN; h <<= 1) {
			// go through vector in chunks
			for (int i = 0; i < FX_REVERB_INT_CHAN; i += (h << 1)) {
				// butterfly-operations within chunks
				for (int j = i; j < i + h; j++) {
					float x = buf[j];
					float y = buf[j + h];

					// SIMD-friendly calculation
					buf[j]     = x + y;
					buf[j + h] = x - y;
				}
			}
		}

		// scale at the end (manual loop-unrolling helps the compiler here)
		#pragma loop_count(FX_REVERB_INT_CHAN)
		for(int i = 0; i < FX_REVERB_INT_CHAN; i++) {
			buf[i] *= FX_REVERB_HADAMARD_SCALING;
		}
	}
#endif

//#pragma section("seg_block2_code")
fxReverb::fxReverb(int fxSlot, int channelMode) : fx(fxSlot, channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	// map memory-pointers to desired address in external RAM (please let me know if you know a better option)
	// initialize delay-lines in external memory
	for (int d = 0; d < FX_REVERB_DIFFUSION_STEPS; d++) {
		for (int c = 0; c < FX_REVERB_INT_CHAN; c++) {
			_diffusionDelayLine[d][c] = (float*)(_memoryAddress);
			_memoryAddress += (FX_REVERB_BUFFER_SIZE * sizeof(float));
		}
	}
	for (int c = 0; c < FX_REVERB_INT_CHAN; c++) {
		_delayLine[c] = (float*)(_memoryAddress);
		_memoryAddress += (FX_REVERB_BUFFER_SIZE * sizeof(float));
	}

	// set memory content to zero
	//clearMemory();
	// we are using multiple clustered delay-lines in this effect, so we are not using the base-class-memory-clear-function
	for (int d = 0; d < FX_REVERB_DIFFUSION_STEPS; d++) {
		for (int c = 0; c < FX_REVERB_INT_CHAN; c++) {
			for (int s = 0; s < FX_REVERB_BUFFER_SIZE; s++) {
				_diffusionDelayLine[d][c][s] = 0.0f;
			}
		}
	}
	for (int c = 0; c < FX_REVERB_INT_CHAN; c++) {
		for (int s = 0; s < FX_REVERB_BUFFER_SIZE; s++) {
			_delayLine[c][s] = 0.0f;
		}
	}


	// initialize variables and the effect itself
	_diffusionDelayLineHead = 0;
	_delay.head = 0;
	setParameters(150.0f, 3.0f, 14000.0f, 1.0f, 0.25f); // roomSizeMs, rt60 (time to fall to -60dB), feedback lowpass frequency, dry, wet

	for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
		_delay.lowPassDelayState[i_ch] = 0.0f;
	}
}

fxReverb::~fxReverb() {
    // destructor
}

//#pragma section("seg_block2_code")
void fxReverb::setParameters(float roomSizeMs, float rt60, float feedbackLowPassFreq, float dry, float wet) {
	_roomSizeMs = roomSizeMs;
	_loopsPerRt60 = rt60 / (roomSizeMs * 1.5f * 0.001f);
	_dbPerCycle = -60.0f / _loopsPerRt60;
	#if FX_REVERN_ALTERNATIVE_POW == 0
		// builtin-function
		_feedbackDecayGain = powf(10, _dbPerCycle * 0.05f); // decay = 10^(dBperCycle/20)  ->  -1.5dB/cycle = x0.85
	#else
		// pow10-approximation
		float tmp = _dbPerCycle * 0.05f;
		Pow10(&tmp);
		_feedbackDecayGain = tmp; // -1.5dB/cycle = x0.85
	#endif
	_delay.lowPassDelayCoeff = (2.0f * M_PI * feedbackLowPassFreq) / (_sampleRate + 2.0f * M_PI * feedbackLowPassFreq); // 7kHz = 43982,297150257105338477007365913 / 91982,297150257105338477007365913 <- alpha = (2 * pi * f_c) / (f_s + 2 * pi * f_c) = (2 * pi * 7000Hz) / (48000Hz + 2 * pi * 7000Hz)
	_dry = dry;
	_wet = wet;

	float diffusionMs = _roomSizeMs;
	for (int d = 0; d < FX_REVERB_DIFFUSION_STEPS; d++) {
		diffusionMs *= 0.5f; // first element has 50% of desired roomSize, next 25%, next 12.5% and so on
		float diffusionDelaySamplesRange = diffusionMs * _sampleRate * 0.001f;

		for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
			float rangeLow = diffusionDelaySamplesRange * (float)i_ch / (float)FX_REVERB_INT_CHAN;
			float rangeHigh = diffusionDelaySamplesRange * (float)(i_ch + 1) / (float)FX_REVERB_INT_CHAN;
			#if FX_REVERN_ALTERNATIVE_RND == 0
				_diffusor[d].delayLineTailOffset[i_ch] = randomInRange(rangeLow, rangeHigh);
			#else
				_diffusor[d].delayLineTailOffset[i_ch] = randomInRangeAlt(&rr, rangeLow, rangeHigh);
			#endif
			_diffusor[d].flipPolarities[i_ch] = rand() % 2;
		}
	}

	// calculate the delay for multichannel-feedback
	float delayLineTailOffsetBase = (_roomSizeMs * _sampleRate * 0.001f);
	for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
		_delay.tailOffset[i_ch] = powf(2.0f, (float)i_ch / (float)FX_REVERB_INT_CHAN) * delayLineTailOffsetBase; // TODO: make sure that we are not exceeding the limits of the delay-array here
	}
}

void fxReverb::rxData(float data[], int len) {
	// data received from x32ctrl
}

void fxReverb::process(float* __restrict bufIn[], float* __restrict bufOut[]) {
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		// Step 1: generate multi-channel fx-input (inL -> _fxBuf[0,2,4,6], inR -> _fxBuf[1,3,5,7])
		// =================================
		float inL = bufIn[0][s];
		float inR = bufIn[1][s];
		#if FX_REVERB_INT_CHAN == 8
			_fxBuf[0] = inL; _fxBuf[1] = inR;
			_fxBuf[2] = inL; _fxBuf[3] = inR;
			_fxBuf[4] = inL; _fxBuf[5] = inR;
			_fxBuf[6] = inL; _fxBuf[7] = inR;
		#else
			for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch+=2) {
				_fxBufInput[i_ch] = inL;
				_fxBufInput[i_ch + 1] = inR;
			}
		#endif



		// Step 2: diffusion-process (_fxBuf[] -> diffusionDelay[] -> _fxBuf[])
		// =================================
        for (int d = 0; d < FX_REVERB_DIFFUSION_STEPS; d++) {
			// pointer to Delay-Line. Mitigates [d][i]-indirection within inner loop
			float** currentDiffLine = _diffusionDelayLine[d];

			// Step 2.1: Write and read data to/from delay-line
			for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
				#if FX_REVERB_INT_CHAN == 8
					currentDiffLine[i_ch][_diffusionDelayLineHead] = _fxBuf[i_ch];
				#else
					currentDiffLine[i_ch][_diffusionDelayLineHead] = _fxBufInput[i_ch];
				#endif

				// Step 2.2: Read data from delay-line
				//int tail = (_diffusionDelayLineHead - _diffusor[d].delayLineTailOffset[i_ch]) & (FX_REVERB_BUFFER_SIZE - 1); // faster version if buffersize is power of 2
				int tail = _diffusionDelayLineHead - _diffusor[d].delayLineTailOffset[i_ch];
				while (tail < 0) {
					tail += FX_REVERB_BUFFER_SIZE;
				}
				_fxBuf[i_ch] = currentDiffLine[i_ch][tail];
			}

			// Step 2.2: mix with Hadamard Matrix
			hadamardMatrix(_fxBuf);

			// Step 2.3: Flip some polarities
			for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
				if (_diffusor[d].flipPolarities[i_ch]) {
					_fxBuf[i_ch] = -_fxBuf[i_ch];
				}
			}
		}
		// Increase the delay-head-pointer after last Diffusor
		_diffusionDelayLineHead++;
		if (_diffusionDelayLineHead == FX_REVERB_BUFFER_SIZE) {
			_diffusionDelayLineHead = 0;
		}



		// Step 3: feedbackProcess (_fxBuf[] + delay[] * decay -> delay[] -> _fxBufOutput[])
		// =================================
		// Step 3.1: read delayed data from delay-line
		for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
			int tail = _delay.head - _delay.tailOffset[i_ch];
			while (tail < 0) {
				tail += FX_REVERB_BUFFER_SIZE;
			}
			_fxBufOutput[i_ch] = _delayLine[i_ch][tail];
		}

		// Step 3.2: calculate householder-matrix
		householderMatrix(&_fxBufOutput[0], &_fxBufFeedback[0]);

		#pragma loop_count(FX_REVERB_INT_CHAN)
		for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
			// implement low-pass-filter on the feedback-line
			// lowpass: output = zoutput + coeff * (input - zoutput)
			float filtered = _delay.lowPassDelayState[i_ch] + _delay.lowPassDelayCoeff * (_fxBufFeedback[i_ch] - _delay.lowPassDelayState[i_ch]);
			_delay.lowPassDelayState[i_ch] = filtered;

			// use MAC for Decay & Sum in a single step
			// _fxBuf is currently the "Diffused Signal"
			// _fxBuf[i_ch] = _fxBuf[i_ch] + (filtered * _feedbackDecayGain);

			// SHARC is able to calculate MAC (Multiply-Accumulate)
			_fxBuf[i_ch] += filtered * _feedbackDecayGain;

			// feedback without lowpass-filter
			//_fxBuf[i_ch] += _fxBufFeedback[i_ch] * _feedbackDecayGain;
		}


		// Step 3.3: write feedback-data to delay-line
		for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
			_delayLine[i_ch][_delay.head] = _fxBuf[i_ch];
		}
		_delay.head++;
		if (_delay.head == FX_REVERB_BUFFER_SIZE) {
			_delay.head = 0;
		}




/*
		// DEBUG: test write/read to SDRAM (_fxBuf[] -> delay[] -> _fxBufOutput[])
		for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch++) {
			// write to SDRAM
			_delayLine[i_ch][_delay.head] = _fxBuf[i_ch];

			// read from SDRAM
			int tail = _delay.head;
			while (tail < 0) {
				tail += FX_REVERB_BUFFER_SIZE;
			}
			_fxBufOutput[i_ch] = _delayLine[i_ch][tail];
		}
		_delay.head++;
		if (_delay.head >= FX_REVERB_BUFFER_SIZE) {
			_delay.head = 0;
		}
*/
/*
		// DEBUG: direct copy input to output
		for (int i = 0; i < FX_REVERB_INT_CHAN; i++) {
			_fxBufOutput[i] = _fxBuf[i];
		}
*/


		// Step 4: generate output-signal
		// =================================
		// Step 4.2: mix all channels back to stereo
		#if FX_REVERB_INT_CHAN == 8
			#if FX_REVERB_AVERAGE_OUTPUT == 1
				float verb0 = _fxBufOutput[0];
				float verb1 = _fxBufOutput[1];
				float verb2 = _fxBufOutput[2];
				float verb3 = _fxBufOutput[3];
				float verb4 = _fxBufOutput[4];
				float verb5 = _fxBufOutput[5];
				float verb6 = _fxBufOutput[6];
				float verb7 = _fxBufOutput[7];
				float sumL = (verb0 + verb2) + (verb4 + verb6);
				float sumR = (verb1 + verb3) + (verb5 + verb7);

				float scaledWet = _wet * (2.0f / (float)FX_REVERB_INT_CHAN);
				bufOut[0][s] = (inL * _dry) + (sumL * scaledWet);
				bufOut[1][s] = (inR * _dry) + (sumR * scaledWet);
			#else
				// take fxOutput 1/2 as left/right-channel directly
				bufOut[0][s] = (inL * _dry) + (_fxBufOutput[0] * _wet);
				bufOut[1][s] = (inR * _dry) + (_fxBufOutput[1] * _wet);
			#endif
		#else
			// general calculation for different amount of channels
			#if FX_REVERB_AVERAGE_OUTPUT == 1
				float sumL = 0.0f;
				float sumR = 0.0f;
				for (int i_ch = 0; i_ch < FX_REVERB_INT_CHAN; i_ch+=2) {
					// Output Calculation
					sumL += _fxBufOutput[i_ch];
					sumR += _fxBufOutput[i_ch + 1];
				}

				float scaledWet = _wet * (2.0f / (float)FX_REVERB_INT_CHAN);
				bufOut[0][s] = (inL * _dry) + (sumL * scaledWet);
				bufOut[1][s] = (inR * _dry) + (sumR * scaledWet);
			#else
				// take fxOutput 1/2 as left/right-channel directly
				bufOut[0][s] = (inL * dry) + (_fxBufOutput[0] * wet);
				bufOut[1][s] = (inR * dry) + (_fxBufOutput[1] * wet);
			#endif
		#endif
	}
}
