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

  This file implements a multiband-compressor using multiple LR24-IIR-filters
  combined with dynamic compressors for each band.
*/

#include "fxMultibandCompressor.h"

#pragma file_attr("prefersMem=internal") // let the linker know, that all variables should be placed into the internal ram

fxMultibandCompressor::fxMultibandCompressor(int fxSlot, int channelMode) : fx(fxSlot, channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	_dualMono = true;

	// set default effect parameters

    // Band 2..4: calculate coeffs for low- and high-cut
    // _____  f0  _____  f1  _____  f2  _____  f3  _____
    //      \    /     \    /     \    /     \    /
    //   b0  \  /  b1   \  /   b2  \  /  b3   \  /   b4
    //        \/         \/         \/         \/
    //        /\         /\         /\         /\
    //       /  \       /  \       /  \       /  \

	// TODO: the following lines of code and the underlying functions
	// are *really* expensive for the DSP. It is only for testing-purposes.
	// The final design will move this code to the iMX25 so that the DSP
	// can focus on the audio-processing instead of calculating the
	// filter-coefficients

	setFrequencies(0, 80.0f, 350.0f, 1500.0f, 7500.0f); // frequency f0...f3

	//           ch band thresh  ratio attck   hold  release makeup
	setParameters(0, 0,  -5.0f,  1.5f, 10.0f, 100.0f, 40.0f, 0.0f);
	setParameters(0, 1, -20.0f,  5.5f, 10.0f, 100.0f, 40.0f, 0.0f);
	setParameters(0, 2, -40.0f, 10.0f, 10.0f, 100.0f, 40.0f, 0.0f);
	setParameters(0, 3, -20.0f,  5.5f, 10.0f, 100.0f, 40.0f, 0.0f);
	setParameters(0, 4,  -5.0f,  1.5f, 10.0f, 100.0f, 40.0f, 0.0f);


    // if we are in stereo-mode, we want to keep channel left and right in sync, so copy
    // data from current channel to other channel in this case
    if (_dualMono) {
    	setFrequencies(1, 80.0f, 350.0f, 1500.0f, 7500.0f); // frequency f0...f3

    	//           ch band thresh  ratio attck   hold  release makeup
    	setParameters(1, 0,  -5.0f,  1.5f, 10.0f, 100.0f, 40.0f, 0.0f);
    	setParameters(1, 1, -20.0f,  5.5f, 10.0f, 100.0f, 40.0f, 0.0f);
    	setParameters(1, 2, -40.0f, 10.0f, 10.0f, 100.0f, 40.0f, 0.0f);
    	setParameters(1, 3, -20.0f,  5.5f, 10.0f, 100.0f, 40.0f, 0.0f);
    	setParameters(1, 4,  -5.0f,  1.5f, 10.0f, 100.0f, 40.0f, 0.0f);
    }else{
		memcpy(&_compressor[1][0], &_compressor[0][0], sizeof(_compressor[0][0]));
		memcpy(&_compressor[1][1], &_compressor[0][1], sizeof(_compressor[0][1]));
		memcpy(&_compressor[1][2], &_compressor[0][2], sizeof(_compressor[0][2]));
		memcpy(&_compressor[1][3], &_compressor[0][3], sizeof(_compressor[0][3]));
		memcpy(&_compressor[1][4], &_compressor[0][4], sizeof(_compressor[0][4]));
    }

    // reset PEQ- and compressor-states
    for (int i_ch = 0; i_ch < 5; i_ch++) {
		for (int b = 0; b < 5; b++) {
			memset(_compressor[i_ch][b].states, 0, sizeof(_compressor[i_ch][b].states));

			_compressor[i_ch][b].gain = 1.0f;
			_compressor[i_ch][b].gainSet = 1.0f;
			_compressor[i_ch][b].state = COMPRESSOR_IDLE;
			_compressor[i_ch][b].triggered = false;
		}
    }
}

fxMultibandCompressor::~fxMultibandCompressor() {
    // destructor
}

void fxMultibandCompressor::setFrequencies(int channel, float f0, float f1, float f2, float f3) {
	// we have 5 bands:
	// band 1 has only double high-cut and two unused IIRs
	// band 2..4 have two low-cuts and two high-cuts
	// band 5 has only double low-cut and two unused IIRs again

	float freq[4];
    freq[0] = f0;
    freq[1] = f1;
    freq[2] = f2;
    freq[3] = f3;

    float gain = 1.0f;
    float Q = 1.0f / sqrtf(2.0f); // with Q=0.707 we achieve a nice damping so that on the cross-over-point of the 24dB/oct filter we have unity-gain
	float V = powf(10.0f, fabsf(gain)/20.0f);
	float K;
	float K2;
	float norm;

	float a0;
	float a1;
	float a2;
	float b1;
	float b2;

	// Band 1: calculate coeffs for high-cut
	K = tanf(M_PI * freq[0] / dsp.samplerate);
	K2 = K * K;
    norm = 1.0f / (1.0f + K / Q + K2);
    a0 = K2 * norm;
    a1 = 2.0f * a0;
    a2 = a0;
    b1 = 2.0f * (K2 - 1.0f) * norm;
    b2 = (1.0f - K / Q + K2) * norm;

    // on even number of coeffs, they have to be placed in interleaving order
    // a0 a0 a1 a1 a2 a2 -b1 -b1 -b2 -b2 a0 a0 a1 a1 a2 a2 -b1 -b1 -b2 -b2
    //
    for (int eq = 0; eq < 2; eq++) {
        int sectionIndex = ((eq / 2) * 2) * 5;
        if ((eq % 2) != 0) {
            // odd section index
            sectionIndex += 1;
        }
        _compressor[channel][0].coeffs[sectionIndex + 0] = a0; // zeros
        _compressor[channel][0].coeffs[sectionIndex + 2] = a1; // zeros
        _compressor[channel][0].coeffs[sectionIndex + 4] = a2; // zeros
        _compressor[channel][0].coeffs[sectionIndex + 6] = -b1; // poles
        _compressor[channel][0].coeffs[sectionIndex + 8] = -b2; // poles
    }
    // for this band, we are not using the second part, so bypass the last two IIR-filters
    for (int eq = 2; eq < 4; eq++) {
        int sectionIndex = ((eq / 2) * 2) * 5;
        if ((eq % 2) != 0) {
            // odd section index
            sectionIndex += 1;
        }
        _compressor[channel][0].coeffs[sectionIndex + 0] = 1; // zeros
        _compressor[channel][0].coeffs[sectionIndex + 2] = 0; // zeros
        _compressor[channel][0].coeffs[sectionIndex + 4] = 0; // zeros
        _compressor[channel][0].coeffs[sectionIndex + 6] = 0; // poles
        _compressor[channel][0].coeffs[sectionIndex + 8] = 0; // poles
    }




    // Band 2..4: calculate coeffs for low- and high-cut
    // _____  f0  _____  f1  _____  f2  _____  f3  _____
    //      \    /     \    /     \    /     \    /
    //   b0  \  /  b1   \  /   b2  \  /  b3   \  /   b4
    //        \/         \/         \/         \/
    //        /\         /\         /\         /\
    //       /  \       /  \       /  \       /  \

    for (int i = 1; i < 4; i++) {
    	// inner bands: calculate low- and high-cut

    	// calculate coeffs for low-cut
    	K = tanf(M_PI * freq[i-1] / dsp.samplerate);
    	K2 = K * K;
        norm = 1.0f / (1.0f + K / Q + K2);
        a0 = 1.0f * norm;
        a1 = -2.0f * a0;
        a2 = a0;
        b1 = 2.0f * (K2 - 1.0f) * norm;
        b2 = (1.0f - K / Q + K2) * norm;

        // on even number of coeffs, they have to be placed in interleaving order
        // a0 a0 a1 a1 a2 a2 -b1 -b1 a0 a0 a1 a1 a2 a2 -b1 -b1
        //
        for (int eq = 0; eq < 2; eq++) {
            int sectionIndex = ((eq / 2) * 2) * 5;
            if ((eq % 2) != 0) {
                // odd section index
                sectionIndex += 1;
            }
            _compressor[channel][i].coeffs[sectionIndex + 0] = a0; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 2] = a1; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 4] = a2; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 6] = -b1; // poles
            _compressor[channel][i].coeffs[sectionIndex + 8] = -b2; // poles
        }

    	// calculate coeffs for high-cut
    	K = tanf(M_PI * freq[i] / dsp.samplerate);
    	K2 = K * K;
        norm = 1.0f / (1.0f + K / Q + K2);
        a0 = K2 * norm;
        a1 = 2.0f * a0;
        a2 = a0;
        b1 = 2.0f * (K2 - 1.0f) * norm;
        b2 = (1.0f - K / Q + K2) * norm;

        // on even number of coeffs, they have to be placed in interleaving order
        // a0 a0 a1 a1 a2 a2 -b1 -b1 a0 a0 a1 a1 a2 a2 -b1 -b1
        //
        for (int eq = 2; eq < 4; eq++) {
            int sectionIndex = ((eq / 2) * 2) * 5;
            if ((eq % 2) != 0) {
                // odd section index
                sectionIndex += 1;
            }
            _compressor[channel][i].coeffs[sectionIndex + 0] = a0; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 2] = a1; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 4] = a2; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 6] = -b1; // poles
            _compressor[channel][i].coeffs[sectionIndex + 8] = -b2; // poles
        }
    }




	// Band 5: calculate coeffs for low-cut
	K = tanf(M_PI * freq[3] / dsp.samplerate);
	K2 = K * K;
    norm = 1.0f / (1.0f + K / Q + K2);
    a0 = 1.0f * norm;
    a1 = -2.0f * a0;
    a2 = a0;
    b1 = 2.0f * (K2 - 1.0f) * norm;
    b2 = (1.0f - K / Q + K2) * norm;

    // on even number of coeffs, they have to be placed in interleaving order
    // a0 a0 a1 a1 a2 a2 -b1 -b1 a0 a0 a1 a1 a2 a2 -b1 -b1
    //
    for (int eq = 0; eq < 2; eq++) {
        int sectionIndex = ((eq / 2) * 2) * 5;
        if ((eq % 2) != 0) {
            // odd section index
            sectionIndex += 1;
        }
        _compressor[channel][4].coeffs[sectionIndex + 0] = a0; // zeros
        _compressor[channel][4].coeffs[sectionIndex + 2] = a1; // zeros
        _compressor[channel][4].coeffs[sectionIndex + 4] = a2; // zeros
        _compressor[channel][4].coeffs[sectionIndex + 6] = -b1; // poles
        _compressor[channel][4].coeffs[sectionIndex + 8] = -b2; // poles
    }
    // for this band, we are not using the second part, so bypass the last two IIR-filters
    for (int eq = 2; eq < 4; eq++) {
        int sectionIndex = ((eq / 2) * 2) * 5;
        if ((eq % 2) != 0) {
            // odd section index
            sectionIndex += 1;
        }
        _compressor[channel][4].coeffs[sectionIndex + 0] = 1; // zeros
        _compressor[channel][4].coeffs[sectionIndex + 2] = 0; // zeros
        _compressor[channel][4].coeffs[sectionIndex + 4] = 0; // zeros
        _compressor[channel][4].coeffs[sectionIndex + 6] = 0; // poles
        _compressor[channel][4].coeffs[sectionIndex + 8] = 0; // poles
    }



    /*
    	// For debugging: Direct-passthrough on all 4 IIR-filters
        for (int eq = 0; eq < 4; eq++) {
            int sectionIndex = ((eq / 2) * 2) * 5;
            if ((eq % 2) != 0) {
                // odd section index
                sectionIndex += 1;
            }
            _compressor[channel][0].coeffs[sectionIndex + 0] = 1; // zeros
            _compressor[channel][0].coeffs[sectionIndex + 2] = 0; // zeros
            _compressor[channel][0].coeffs[sectionIndex + 4] = 0; // zeros
            _compressor[channel][0].coeffs[sectionIndex + 6] = 0; // poles
            _compressor[channel][0].coeffs[sectionIndex + 8] = 0; // poles
        }

        memcpy(&_compressor[channel][1].coeffs[0], &_compressor[channel][0].coeffs[0], 5 * 4 * sizeof(float));
        memcpy(&_compressor[channel][2].coeffs[0], &_compressor[channel][0].coeffs[0], 5 * 4 * sizeof(float));
        memcpy(&_compressor[channel][3].coeffs[0], &_compressor[channel][0].coeffs[0], 5 * 4 * sizeof(float));
        memcpy(&_compressor[channel][4].coeffs[0], &_compressor[channel][0].coeffs[0], 5 * 4 * sizeof(float));
    */

}

void fxMultibandCompressor::setParameters(int channel, int band, float threshold, float ratio, float attack, float hold, float release, float makeup) {
	// TODO: put these calculations into i.MX25 to move calculation away from DSP

	// the compressor will be calculated only every "SAMPLES_IN_BUFFER" samples, so we have to take care of this
	float samplerate = dsp.samplerate/(float)SAMPLES_IN_BUFFER;

	_compressor[channel][band].value_threshold = (powf(2.0f, 31.0f) - 1.0f) * powf(10.0f, threshold/20.0f);
	_compressor[channel][band].value_ratio = ratio;
	_compressor[channel][band].value_coeff_attack = expf(-2197.22457734f/(samplerate * attack)); // ln(10%) - ln(90%) = -2.197224577
	_compressor[channel][band].value_hold_ticks = hold * samplerate / 1000.0f;
	_compressor[channel][band].value_coeff_release = expf(-2197.22457734f/(samplerate * release)); // ln(10%) - ln(90%) = -2.197224577
	_compressor[channel][band].value_makeup = powf(10.0f, makeup/20.0f);
}

void fxMultibandCompressor::rxData(float data[], int len) {
	// data received from x32ctrl
}

void fxMultibandCompressor::process(float* __restrict bufIn[], float* __restrict bufOut[]) {
	// Step 1: clear the output-buffer
	for (int i_ch = 0; i_ch < 2; i_ch++) {
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			bufOut[i_ch][s] = 0;
		}
	}

	// iterate through all bands of this multiband-compressor
	for (int b = 0; b < 5; b++) {
		// Step 2: calculate 24db/oct LR24-filter for each band
		for (int i_ch = 0; i_ch < 2; i_ch++) {
			// copy all samples into temporary buffer
			memcpy(&_buffer[i_ch][0], &bufIn[i_ch][0], SAMPLES_IN_BUFFER);

			// coefficients must be placed in program-memory (pm)
			// as our effects are implemented as classes which places the variables in the heap in the data-memory (dm)
			// we have to copy the coefficients into a global buffer in pm. Not the best solution, but should work
			memcpy(&peqCoeffs[0], &_compressor[i_ch][b].coeffs[0], 5 * 4 * sizeof(float));

			// apply biquad-filter on all samples
			biquad_trans(&_buffer[i_ch][0], &peqCoeffs[0], &_compressor[i_ch][b].states[0], SAMPLES_IN_BUFFER, 4); // 2x two subsequent 12dB-IIR-filters to get 24dB/oct
		}
		// _buffer[][] contains now all samples of the current band

		// Step 3: compress current band for each channel
		// Compressor: gain = (gain * coeff) + gainSet - (gainSet * coeff)
		if (_dualMono) {
			// dual mono processing
			for (int i_ch = 0; i_ch < 2; i_ch++) {
				processLogic(&_compressor[i_ch][b], _buffer[i_ch][0]);
				_compressor[i_ch][b].gain = (_compressor[i_ch][b].gain * _compressor[i_ch][b].coeff) + _compressor[i_ch][b].gainSet - (_compressor[i_ch][b].gainSet * _compressor[i_ch][b].coeff);
			}

			// Step 4: apply new gain and makeup to all samples of current band and sumup all bands to output samples
			for (int i_ch = 0; i_ch < 2; i_ch++) {
				float mult = _compressor[i_ch][b].gain * _compressor[i_ch][b].value_makeup;
				for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
					bufOut[i_ch][s] += _buffer[i_ch][s] * mult;
				}
			}
		}else{
			// stereo processing
			float sample = _buffer[0][0]; // left channel
			if (_buffer[1][0] > sample) {
				// take right channel if its louder
				sample = _buffer[1][0];
			}
			processLogic(&_compressor[0][b], sample); // process with loudest sample
			_compressor[0][b].gain = (_compressor[0][b].gain * _compressor[0][b].coeff) + _compressor[0][b].gainSet - (_compressor[0][b].gainSet * _compressor[0][b].coeff);

			// Step 4: apply new gain and makeup to all samples of current band and sumup all bands to output samples
			float mult = _compressor[0][b].gain * _compressor[0][b].value_makeup;
			for (int i_ch = 0; i_ch < 2; i_ch++) {
				for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
					bufOut[i_ch][s] += _buffer[i_ch][s] * mult;
				}
			}
		}
	}

	// bypass
	//bufOut[0][s] = bufIn[0][s];
	//bufOut[1][s] = bufIn[1][s];
}

void fxMultibandCompressor::processLogic(sCompressor* compressor, float sample) {
	//float input_abs = abs(meanf(samples, SAMPLES_IN_BUFFER)); // takes 2.5% CPU Load
	float input_abs = abs(sample);

	compressor->triggered = (input_abs > compressor->value_threshold);

	// calculate the gate-logic and online-parameters
	switch (compressor->state) {
		case COMPRESSOR_IDLE:
			compressor->gainSet = 1.0f;
			// check if we have to open the gate
			if (!compressor->triggered) {
				break;
			}

			compressor->state = COMPRESSOR_ATTACK;
			// no break by intention: fall-through to ATTACK
		case COMPRESSOR_ATTACK:
			// overshoot = abs(sample) - threshold
			// output = (overshoot / ratio) + threshold
			// gainSet = output / abs(input)
			if ((input_abs > 0) && (compressor->value_ratio != 0)) {
				// gainSet = input_abs
				compressor->gainSet = (((input_abs - compressor->value_threshold) * compressor->value_ratio) + compressor->value_threshold) / input_abs;
			}

			compressor->coeff = compressor->value_coeff_attack;
			compressor->state = COMPRESSOR_ACTIVE;
			// no break by intention: fall-through to ACTIVE
		case COMPRESSOR_ACTIVE:
			// check if we are still triggered
			if (compressor->triggered) {
				// check if we have to compress even more
				float newValue;
				if ((input_abs > 0) && (compressor->value_ratio != 0)) {
					newValue = (((input_abs - compressor->value_threshold) * compressor->value_ratio) + compressor->value_threshold) / input_abs;
					if (newValue < compressor->gainSet) {
						// compress even more
						compressor->gainSet = newValue;
					}
				}

				// stay in this state
				compressor->state = COMPRESSOR_ACTIVE;
				break;
			}

			compressor->holdCounter = compressor->value_hold_ticks;
			compressor->state = COMPRESSOR_HOLD;
			// no break by intention: fall-through to hold
		case COMPRESSOR_HOLD:
			if (compressor->triggered) {
				// re-enter active-state
				float newValue;
				if ((input_abs > 0) && (compressor->value_ratio != 0)) {
					newValue = (((input_abs - compressor->value_threshold) * compressor->value_ratio) + compressor->value_threshold) / input_abs;
					if (newValue < compressor->gainSet) {
						// compress even more
						compressor->gainSet = newValue;
					}
				}
				compressor->state = COMPRESSOR_ACTIVE;
				compressor->holdCounter = compressor->value_hold_ticks;
				break;
			}

			// we are below threshold
			if (compressor->holdCounter >= 0) {
				compressor->holdCounter--;
				break;
			}

			compressor->state = COMPRESSOR_RELEASE;
			// no break by intention: fall-through to RELEASE
		case COMPRESSOR_RELEASE:
			compressor->gainSet = 1.0f;
			compressor->coeff = compressor->value_coeff_release;
			compressor->state = COMPRESSOR_IDLE;
			break;
		default:
			compressor->state = COMPRESSOR_IDLE;
			break;
	}
}
