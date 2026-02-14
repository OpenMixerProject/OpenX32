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

  This file implements a dynamic EQ
*/

#include "fxDynamicEQ.h"

#pragma file_attr("prefersMem=internal") // let the linker know, that all variables should be placed into the internal ram

fxDynamicEQ::fxDynamicEQ(int fxSlot, int channelMode) : fx(fxSlot, channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	// set default parameters
	//		                     static  maxDyn
	//           band type freq   Gain    Gain   Q    Tresh  Ratio Attack Release
	//setParameters(0, 1, 1000.0f, 0.0f, -5.0f, 1.0f, -20.0f, 2.0f, 50.0f, 300.0f);
	for (int band = 0; band < FX_DYNAMICEQ_BANDS; band++) {
		_deq[band].type = 1; // Peak-Filter
		_deq[band].typeCtrl = 4; // Band-Pass-Filter
		_deq[band].maxDynamicGain = -10; // -10dB
		_deq[band].Q = 1; // Q=1
		_deq[band].threshold = -20; // -20dB
		_deq[band].ratio = 2; // ratio: 1:2
		_deq[band].attack = 0.006644493744965f; // 50ms
		_deq[band].release = 0.0011104940557206f; // 300ms

		if (band == 0) {
			_deq[band].frequency = 300; // 300Hz
			_deq[band].staticGain = 0; // 0dB
		}
		if (band == 1) {
			_deq[band].frequency = 1000; // 1kHz
			_deq[band].staticGain = 0; // 0dB
		}
		if (band == 2) {
			_deq[band].frequency = 5000; // 5kHz
			_deq[band].staticGain = 0; // 0dB
		}

		helperFcn_calcBiquadCoeffs(_deq[band].typeCtrl, _deq[band].frequency, 0.3, 1.0f, &_deq[band].biquadCoeffsCtrl[0], dsp.samplerate); // use a broad Q here
	}

	for (int i = 0; i < FX_DYNAMICEQ_BANDS; i++) {
		// setup the smoothing-filter for the control-signal
		_deq[i].smoothingCoeff = 0.1;	// coeff = 1 -> no smoothing, coeff = 0.001 -> slow response
	    _deq[i].smoothedTargetGain = 0.0f;	// start-parameter
	    _deq[i].envelope = 0.0f;
	}

	_startup = false;
}

fxDynamicEQ::~fxDynamicEQ() {
    // destructor
}

// human-friendly parameter-settings, but more expensive for the DSP
void fxDynamicEQ::setParameters(int band, int type, float frequency, float staticGain, float maxDynamicGain, float Q, float threshold, float ratio, float attack, float release) {
	_deq[band].type = type;

	// set control-type
	switch(type) {
		case 1: // Peak-Filter
			// here the control-filter has to be a band-pass
			_deq[band].typeCtrl = 4;
			break;
		case 2: // Low-Shelf
			// here the control-filter has to be a low-pass
			_deq[band].typeCtrl = 6;
			break;
		case 3: // High-Shelf
			// here the control-filter has to be a high-pass
			_deq[band].typeCtrl = 7;
			break;
	}

	_deq[band].frequency = frequency;
	_deq[band].staticGain = staticGain;
    _deq[band].maxDynamicGain = maxDynamicGain;
	_deq[band].Q = Q;
    _deq[band].threshold = threshold;
    _deq[band].ratio = ratio;

    // attack and release are performed only every "SAMPLES_IN_BUFFER"-samples
    // so we have to calculate the real samplerate which is
	// so the calling-interval is SAMPLERATE / SAMPLES_IN_BUFFER
	// 48000Hz / 16 = 3 kHz -> every 333 microseconds
    float fs = (dsp.samplerate/(float)SAMPLES_IN_BUFFER);
    _deq[band].attack = 1.0f - expf(-1000.0f / (fs * attack)); // convert ms to coeff
    _deq[band].release = 1.0f - expf(-1000.0f / (fs * release)); // convert ms to coeff

	// update the biquad-coefficients for the control-signal
	helperFcn_calcBiquadCoeffs(_deq[band].typeCtrl, _deq[band].frequency, _deq[band].Q, 1.0f, &_deq[band].biquadCoeffsCtrl[0], dsp.samplerate);
}

void fxDynamicEQ::rxData(float data[], int len) {
	// data received from x32ctrl
	if (len != 11) return;

	int band = (int)data[0];
	_deq[band].type = (int)data[1];
	_deq[band].typeCtrl = (int)data[2];
	_deq[band].frequency = data[3];
	_deq[band].staticGain = data[4];
    _deq[band].maxDynamicGain = data[5];
	_deq[band].Q = data[6];
    _deq[band].threshold = data[7];
    _deq[band].ratio = data[8];
    _deq[band].attack = data[9];
    _deq[band].release = data[10];

	// update the biquad-coefficients for the control-signal
	helperFcn_calcBiquadCoeffs(_deq[band].typeCtrl, _deq[band].frequency, _deq[band].Q, 1.0f, &_deq[band].biquadCoeffsCtrl[0], dsp.samplerate);
}

void fxDynamicEQ::process(float* __restrict bufIn[], float* __restrict bufOut[]) {
	// this function is called every "SAMPLES_IN_BUFFER"-samples
	// so the calling-interval is SAMPLERATE / SAMPLES_IN_BUFFER
	// 48000Hz / 16 = 3 kHz -> every 333 microseconds

	for (int band = 0; band < FX_DYNAMICEQ_BANDS; band++) {
		// Step 1: level detection and gain-calculation
		// ==============================================================
		// Step 1.1: apply desired biquad-filter on control-signal first
		//
		for (int i_ch = 0; i_ch < 2; i_ch++) {
			memcpy(&bufOut[i_ch][0], &bufIn[i_ch][0], SAMPLES_IN_BUFFER * sizeof(float));

			memcpy(&peqCoeffs[0], &_deq[band].biquadCoeffsCtrl[0], 5 * sizeof(float));
			biquad_trans(&bufOut[i_ch][0], &peqCoeffs[0], &_deq[band].biquadStatesCtrl[i_ch][0], SAMPLES_IN_BUFFER, 1);
		}

		// Step 1.2: take the first sample of the filtered control-signal and update envelope
		float ctrlSignalL = abs(bufOut[0][0]); // we are using the output-buffer as a temporary storage for the control-signal
		float ctrlSignalR = abs(bufOut[1][0]); // we are using the output-buffer as a temporary storage for the control-signal
		float ctrlSignal = ctrlSignalL;
		if (ctrlSignalR > ctrlSignalL) {
			ctrlSignal = ctrlSignalR;
		}

		ctrlSignal /= 2147483648.0f; // normalize control-signal between 0 and +1
		if (ctrlSignal > _deq[band].envelope) {
			_deq[band].envelope += _deq[band].attack * (ctrlSignal - _deq[band].envelope);
		}else{
			_deq[band].envelope += _deq[band].release * (ctrlSignal - _deq[band].envelope);
		}




		// Step 2: calculation of the gain-reduction
		// ==============================================================
		float envelope_dB = helperFcn_lin2db(_deq[band].envelope); // convert absolute value between 0 and 1 to -oodBfs and 0dBfs
		float targetGain = 0.0f;

		// if envelope is above threshold, change gain
		if (envelope_dB > _deq[band].threshold) {
			// calculate the overshoot
			float overshoot = envelope_dB - _deq[band].threshold;
			// apply the ratio: gainreduction = (1 - 1/ratio) * overshoot
			float delta_dB = overshoot * (1.0f - (1.0f / _deq[band].ratio));

			if (_deq[band].maxDynamicGain < 0) {
				// reduction-mode / compression-mode
				targetGain = -delta_dB;

				if (targetGain < _deq[band].maxDynamicGain) {
					targetGain = _deq[band].maxDynamicGain;
				}
			}else{
				// boost-mode / expander-mode
				targetGain = delta_dB;

				if (targetGain > _deq[band].maxDynamicGain) {
					targetGain = _deq[band].maxDynamicGain;
				}
			}
		}
		// Smooth the parameters with first-order low-pass
		_deq[band].smoothedTargetGain = _deq[band].smoothedTargetGain + _deq[band].smoothingCoeff * (targetGain - _deq[band].smoothedTargetGain);
	}



	// Step 3: update coefficients and apply biquad-filter on all samples
	// ==============================================================
	#if FX_DYNAMICEQ_BANDS == 1
		helperFcn_calcBiquadCoeffs(_deq[band].type, _deq[band].frequency, _deq[band].Q, _deq[band].smoothedTargetGain + _deq[band].staticGain, &peqCoeffs[0], dsp.samplerate);
	#elif FX_DYNAMICEQ_BANDS == 2
		float peqCoeffsTmp[10];
		helperFcn_calcBiquadCoeffs(_deq[0].type, _deq[0].frequency, _deq[0].Q, _deq[0].smoothedTargetGain + _deq[0].staticGain, &peqCoeffsTmp[0], dsp.samplerate);
		helperFcn_calcBiquadCoeffs(_deq[1].type, _deq[1].frequency, _deq[1].Q, _deq[1].smoothedTargetGain + _deq[1].staticGain, &peqCoeffsTmp[5], dsp.samplerate);
		for (int i = 0; i < 5; i++) {
			peqCoeffs[i * 2] = peqCoeffsTmp[i];
			peqCoeffs[(i * 2) + 1] = peqCoeffsTmp[5 + i];
		}
	#elif FX_DYNAMICEQ_BANDS == 3
		float peqCoeffsTmp[10];
		helperFcn_calcBiquadCoeffs(_deq[0].type, _deq[0].frequency, _deq[0].Q, _deq[0].smoothedTargetGain + _deq[0].staticGain, &peqCoeffsTmp[0], dsp.samplerate);
		helperFcn_calcBiquadCoeffs(_deq[1].type, _deq[1].frequency, _deq[1].Q, _deq[1].smoothedTargetGain + _deq[1].staticGain, &peqCoeffsTmp[5], dsp.samplerate);
		for (int i = 0; i < 5; i++) {
			peqCoeffs[i * 2] = peqCoeffsTmp[i];
			peqCoeffs[(i * 2) + 1] = peqCoeffsTmp[5 + i];
		}
		// last element is not interleaved
		helperFcn_calcBiquadCoeffs(_deq[2].type, _deq[2].frequency, _deq[2].Q, _deq[2].smoothedTargetGain + _deq[2].staticGain, &peqCoeffs[10], dsp.samplerate);
	#endif

	for (int i_ch = 0; i_ch < 2; i_ch++) {
		memcpy(&bufOut[i_ch][0], &bufIn[i_ch][0], SAMPLES_IN_BUFFER * sizeof(float));
		biquad_trans(&bufOut[i_ch][0], &peqCoeffs[0], &_biquadStates[i_ch][0], SAMPLES_IN_BUFFER, FX_DYNAMICEQ_BANDS);
	}
}
