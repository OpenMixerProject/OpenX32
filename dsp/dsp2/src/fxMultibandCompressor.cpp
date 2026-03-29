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

#define ONE_OVER_SQRT2			0.70710678118654752440f

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

	for (int i = 0; i < 5; i++) {
		_compressor[0][i].value_threshold = -5.0; // dB
		_compressor[0][i].value_ratio = 1.5f;
		_compressor[0][i].value_coeff_attack = 0.92937699360542739010302f; // attach = 10ms
		_compressor[0][i].value_hold_ticks = 4800.0f; // 100ms
		_compressor[0][i].value_coeff_release = 0.98185640853383583712187f;
		_compressor[0][i].value_makeup = 1.0f;

		// set all coefficients to direct passthrough
		for (int c = 0; c < (5 * 4); c++) {
			_compressor[0][i].coeffs[c] = 0;
		}
		_compressor[0][i].coeffs[0] = 1;
		_compressor[0][i].coeffs[1] = 1;
		_compressor[0][i].coeffs[10] = 1;
		_compressor[0][i].coeffs[11] = 1;
	}
	// copy settings for channel 1 to channel 2
	memcpy(&_compressor[1][0], &_compressor[0][0], sizeof(_compressor[0][0]) * 5);

/*
	// the following lines of code and the underlying functions
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
*/

    // reset PEQ- and compressor-states
    for (int i_ch = 0; i_ch < 5; i_ch++) {
		for (int b = 0; b < 5; b++) {
			memset(_compressor[i_ch][b].states, 0, sizeof(_compressor[i_ch][b].states));

			_compressor[i_ch][b].envelope = 1.0f;
		}
    }

    _startup = false;
}

fxMultibandCompressor::~fxMultibandCompressor() {
    // destructor
}

void fxMultibandCompressor::setFrequencies(int channel, float f0, float f1, float f2, float f3) {
	// we have 5 bands:
	// band 1 has only double high-cut and two unused IIRs
	// band 2..4 have two low-cuts and two high-cuts
	// band 5 has only double low-cut and two unused IIRs again

    // _____  f0  _____  f1  _____  f2  _____  f3  _____
    //      \    /     \    /     \    /     \    /
    //   b0  \  /  b1   \  /   b2  \  /  b3   \  /   b4
    //        \/         \/         \/         \/
    //        /\         /\         /\         /\
    //       /  \       /  \       /  \       /  \

    float coeffs[5];
	float freq[4];
    freq[0] = f0;
    freq[1] = f1;
    freq[2] = f2;
    freq[3] = f3;


	// calculate coeffs for high-cut (low-pass)
	helperFcn_calcBiquadCoeffs(6, freq[0], ONE_OVER_SQRT2, 1.0f, &coeffs[0], dsp.samplerate);

    // on even number of coeffs, they have to be placed in interleaving order
    // a0 a0 a1 a1 a2 a2 -b1 -b1 -b2 -b2 a0 a0 a1 a1 a2 a2 -b1 -b1 -b2 -b2
    //
    for (int eq = 0; eq < 2; eq++) {
        int sectionIndex = ((eq / 2) * 2) * 5;
        if ((eq % 2) != 0) {
            // odd section index
            sectionIndex += 1;
        }
        _compressor[channel][0].coeffs[sectionIndex + 0] = coeffs[0]; // zeros
        _compressor[channel][0].coeffs[sectionIndex + 2] = coeffs[1]; // zeros
        _compressor[channel][0].coeffs[sectionIndex + 4] = coeffs[2]; // zeros
        _compressor[channel][0].coeffs[sectionIndex + 6] = coeffs[3]; // poles
        _compressor[channel][0].coeffs[sectionIndex + 8] = coeffs[4]; // poles
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
    for (int i = 1; i < 4; i++) {
    	// inner bands: calculate low- and high-cut

    	// calculate coeffs for low-cut (high-pass)
    	helperFcn_calcBiquadCoeffs(7, freq[i-1], ONE_OVER_SQRT2, 1.0f, &coeffs[0], dsp.samplerate);

        // on even number of coeffs, they have to be placed in interleaving order
        // a0 a0 a1 a1 a2 a2 -b1 -b1 a0 a0 a1 a1 a2 a2 -b1 -b1
        //
        for (int eq = 0; eq < 2; eq++) {
            int sectionIndex = ((eq / 2) * 2) * 5;
            if ((eq % 2) != 0) {
                // odd section index
                sectionIndex += 1;
            }
            _compressor[channel][i].coeffs[sectionIndex + 0] = coeffs[0]; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 2] = coeffs[1]; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 4] = coeffs[2]; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 6] = coeffs[3]; // poles
            _compressor[channel][i].coeffs[sectionIndex + 8] = coeffs[4]; // poles
        }

    	// calculate coeffs for high-cut (low-pass)
    	helperFcn_calcBiquadCoeffs(6, freq[i], ONE_OVER_SQRT2, 1.0f, &coeffs[0], dsp.samplerate);

        // on even number of coeffs, they have to be placed in interleaving order
        // a0 a0 a1 a1 a2 a2 -b1 -b1 a0 a0 a1 a1 a2 a2 -b1 -b1
        //
        for (int eq = 2; eq < 4; eq++) {
            int sectionIndex = ((eq / 2) * 2) * 5;
            if ((eq % 2) != 0) {
                // odd section index
                sectionIndex += 1;
            }
            _compressor[channel][i].coeffs[sectionIndex + 0] = coeffs[0]; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 2] = coeffs[1]; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 4] = coeffs[2]; // zeros
            _compressor[channel][i].coeffs[sectionIndex + 6] = coeffs[3]; // poles
            _compressor[channel][i].coeffs[sectionIndex + 8] = coeffs[4]; // poles
        }
    }




	// Band 5: calculate coeffs for low-cut (high-pass)
	helperFcn_calcBiquadCoeffs(6, freq[3], ONE_OVER_SQRT2, 1.0f, &coeffs[0], dsp.samplerate);

    // on even number of coeffs, they have to be placed in interleaving order
    // a0 a0 a1 a1 a2 a2 -b1 -b1 a0 a0 a1 a1 a2 a2 -b1 -b1
    //
    for (int eq = 0; eq < 2; eq++) {
        int sectionIndex = ((eq / 2) * 2) * 5;
        if ((eq % 2) != 0) {
            // odd section index
            sectionIndex += 1;
        }
        _compressor[channel][4].coeffs[sectionIndex + 0] = coeffs[0]; // zeros
        _compressor[channel][4].coeffs[sectionIndex + 2] = coeffs[1]; // zeros
        _compressor[channel][4].coeffs[sectionIndex + 4] = coeffs[2]; // zeros
        _compressor[channel][4].coeffs[sectionIndex + 6] = coeffs[3]; // poles
        _compressor[channel][4].coeffs[sectionIndex + 8] = coeffs[4]; // poles
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

	// For debugging: Direct-passthrough on all 4 IIR-filters
	//for (int eq = 0; eq < 4; eq++) {
	//    int sectionIndex = ((eq / 2) * 2) * 5;
	//    if ((eq % 2) != 0) {
	//        // odd section index
	//        sectionIndex += 1;
	//    }
	//    _compressor[channel][0].coeffs[sectionIndex + 0] = 1; // zeros
	//    _compressor[channel][0].coeffs[sectionIndex + 2] = 0; // zeros
	//    _compressor[channel][0].coeffs[sectionIndex + 4] = 0; // zeros
	//    _compressor[channel][0].coeffs[sectionIndex + 6] = 0; // poles
	//    _compressor[channel][0].coeffs[sectionIndex + 8] = 0; // poles
	//}
	//memcpy(&_compressor[channel][1].coeffs[0], &_compressor[channel][0].coeffs[0], 5 * 4 * sizeof(float));
	//memcpy(&_compressor[channel][2].coeffs[0], &_compressor[channel][0].coeffs[0], 5 * 4 * sizeof(float));
	//memcpy(&_compressor[channel][3].coeffs[0], &_compressor[channel][0].coeffs[0], 5 * 4 * sizeof(float));
	//memcpy(&_compressor[channel][4].coeffs[0], &_compressor[channel][0].coeffs[0], 5 * 4 * sizeof(float));
}

// human-friendly parameter-settings, but more expensive for the DSP
void fxMultibandCompressor::setParameters(int channel, int band, float threshold, float ratio, float attack, float hold, float release, float makeup) {
	// TODO: put these calculations into i.MX25 to move calculation away from DSP

	// the compressor will be calculated only every "SAMPLES_IN_BUFFER" samples, so we have to take care of this
	float samplerate = dsp.samplerate/(float)SAMPLES_IN_BUFFER;

	_compressor[channel][band].value_threshold = threshold;
	_compressor[channel][band].value_ratio = ratio;
	_compressor[channel][band].value_coeff_attack = 1.0f - expf(-2197.22457734f/(samplerate * attack)); // ln(10%) - ln(90%) = -2.197224577
	_compressor[channel][band].value_hold_ticks = hold * samplerate / 1000.0f;
	_compressor[channel][band].value_coeff_release = 1.0f - expf(-2197.22457734f/(samplerate * release)); // ln(10%) - ln(90%) = -2.197224577
	_compressor[channel][band].value_makeup = helperFcn_db2lin(makeup); //powf(10.0f, makeup/20.0f);
}


void fxMultibandCompressor::rxData(float data[], int len) {
	// data received from x32ctrl
	if (len == 8) {
		// get new parameters

		int channel = data[0];
		int band = data[1];
		_compressor[channel][band].value_threshold = data[2];
		_compressor[channel][band].value_ratio = data[3];
		_compressor[channel][band].value_coeff_attack = data[4];
		_compressor[channel][band].value_hold_ticks = data[5];
		_compressor[channel][band].value_coeff_release = data[6];
		_compressor[channel][band].value_makeup = data[7];
	}

	if (len == 41) {
		// get new set of frequency-coefficients
		int channel = data[0];

		// set pair of coefficients (we are using two 12dB/oct biquad-filters to create a 24dB/oct)
		for (int i = 0; i < 5; i++) {
			// band 1
			_compressor[channel][0].coeffs[0 + (i*2)] = data[1 + i];
			_compressor[channel][0].coeffs[1 + (i*2)] = data[1 + i];

			// band 5
			_compressor[channel][4].coeffs[0 + (i*2)] = data[36 + i];
			_compressor[channel][4].coeffs[1 + (i*2)] = data[36 + i];
		}

		// band 2..4
		for (int band = 1; band < 4; band++) {
			int offset = 6 + ((band - 1) * 10);

			for (int i = 0; i < 5; i++) {
				_compressor[channel][band].coeffs[0 + (i*2)] = data[offset + i];
				_compressor[channel][band].coeffs[1 + (i*2)] = data[offset + i];

				_compressor[channel][band].coeffs[0 + (i*2) + 10] = data[offset + 5 + i];
				_compressor[channel][band].coeffs[1 + (i*2) + 10] = data[offset + 5 + i];
			}
		}
	}
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
		if (_dualMono) {
			// dual mono processing
			for (int i_ch = 0; i_ch < 2; i_ch++) {
				float inputDb = helperFcn_lin2db(fabsf(_buffer[i_ch][0]) * INT32_TO_FLOAT_NORM);
				float targetGainDb = 0.0f;
				if (inputDb > _compressor[i_ch][b].value_threshold) {
					targetGainDb = (_compressor[i_ch][b].value_threshold - inputDb) * (1.0f - 1.0f / _compressor[i_ch][b].value_ratio);
				}
				float currentGainLinear = helperFcn_db2lin(targetGainDb);
				if (currentGainLinear < _compressor[i_ch][b].envelope) {
					// Attack
					_compressor[i_ch][b].envelope += _compressor[i_ch][b].value_coeff_attack * (currentGainLinear - _compressor[i_ch][b].envelope);
					_compressor[i_ch][b].holdTimer = _compressor[i_ch][b].value_hold_ticks;
				}else{
					// Hold -> Release
					if (_compressor[i_ch][b].holdTimer > 0) {
						_compressor[i_ch][b].holdTimer--;
					}else{
						_compressor[i_ch][b].envelope += _compressor[i_ch][b].value_coeff_release * (currentGainLinear - _compressor[i_ch][b].envelope);
					}
				}
			}

			// Step 4: apply new gain and makeup to all samples of current band and sumup all bands to output samples
			for (int i_ch = 0; i_ch < 2; i_ch++) {
				float mult = _compressor[i_ch][b].envelope * _compressor[i_ch][b].value_makeup;
				for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
					bufOut[i_ch][s] += _buffer[i_ch][s] * mult;
				}
			}
		}else{
			// stereo processing
			float sample = fabsf(_buffer[0][0]); // left channel
			float sampleR = fabsf(_buffer[1][0]); // right channel
			if (sampleR > sample) {
				// take right channel if its louder
				sample = sampleR;
			}

			float inputDb = helperFcn_lin2db(sample * INT32_TO_FLOAT_NORM);
			float targetGainDb = 0.0f;
			if (inputDb > _compressor[0][b].value_threshold) {
				targetGainDb = (_compressor[0][b].value_threshold - inputDb) * (1.0f - 1.0f / _compressor[0][b].value_ratio);
			}
			float currentGainLinear = helperFcn_db2lin(targetGainDb);
			if (currentGainLinear < _compressor[0][b].envelope) {
				// Attack
				_compressor[0][b].envelope += _compressor[0][b].value_coeff_attack * (currentGainLinear - _compressor[0][b].envelope);
				_compressor[0][b].holdTimer = _compressor[0][b].value_hold_ticks;
			}else{
				// Hold -> Release
				if (_compressor[0][b].holdTimer > 0) {
					_compressor[0][b].holdTimer--;
				}else{
					_compressor[0][b].envelope += _compressor[0][b].value_coeff_release * (currentGainLinear - _compressor[0][b].envelope);
				}
			}

			// Step 4: apply new gain and makeup to all samples of current band and sumup all bands to output samples
			float mult = _compressor[0][b].envelope * _compressor[0][b].value_makeup;
			for (int i_ch = 0; i_ch < 2; i_ch++) {
				for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
					bufOut[i_ch][s] += _buffer[i_ch][s] * mult;
				}
			}
		}
	}
/*
	// bypass
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufOut[0][s] = bufIn[0][s];
		bufOut[1][s] = bufIn[1][s];
	}
*/
}
