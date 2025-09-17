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

#include "fx.h"

void fxProcessGateLogic(int channel, float samples[]) {
	float input_abs = abs(meanf(samples, SAMPLES_IN_BUFFER));

	dsp.dspChannel[channel].gate.closed = input_abs < dsp.dspChannel[channel].gate.value_threshold;

	// calculate the gate-logic and online-parameters
	switch (dsp.dspChannel[channel].gate.state) {
		case GATE_CLOSED:
			dsp.gateGainSet[channel] = dsp.dspChannel[channel].gate.value_gainmin;
			// check if we have to open the gate
			if (!dsp.dspChannel[channel].gate.closed) {
				dsp.dspChannel[channel].gate.state = GATE_ATTACK;
			}
			break;
		case GATE_ATTACK:
			dsp.gateGainSet[channel] = 1.0f;
			dsp.gateCoeff[channel] = dsp.dspChannel[channel].gate.value_coeff_attack;
			dsp.dspChannel[channel].gate.state = GATE_OPEN;
			break;
		case GATE_OPEN:
			if (dsp.dspChannel[channel].gate.closed) {
				dsp.dspChannel[channel].gate.holdCounter = dsp.dspChannel[channel].gate.value_hold_ticks;
				dsp.dspChannel[channel].gate.state = GATE_HOLD;
			}
			break;
		case GATE_HOLD:
			if (!dsp.dspChannel[channel].gate.closed) {
				// re-enter on-state
				dsp.dspChannel[channel].gate.state = GATE_OPEN;
			}else{
				if (dsp.dspChannel[channel].gate.holdCounter == 0) {
					dsp.dspChannel[channel].gate.state = GATE_CLOSING;
				}else{
					dsp.dspChannel[channel].gate.holdCounter--;
				}
			}
			break;
		case GATE_CLOSING:
			dsp.gateGainSet[channel] = dsp.dspChannel[channel].gate.value_gainmin;
			dsp.gateCoeff[channel] = dsp.dspChannel[channel].gate.value_coeff_release;
			dsp.dspChannel[channel].gate.state = GATE_CLOSED;
			break;
		default:
			dsp.dspChannel[channel].gate.state = GATE_CLOSED;
			break;
	}
}

void fxSetPeqCoeffs(int channel, int index, float coeffs[]) {
	// biquad_trans() needs the coeffs in the following order
	// a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 0/1)
	// a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 2/3)
	// a0 a1 a2 b1 b2 (section 4)
	if (index >= MAX_CHAN_EQS) {
		return;
	}

	if (((MAX_CHAN_EQS % 2) == 0) || (index < (MAX_CHAN_EQS - 1))) {
		// we have even number of PEQ-sections
		// or we have odd number but we are still below the last section

		// store data with interleaving
		int sectionIndex = ((index / 2) * 2) * 5;
		if ((index % 2) != 0) {
			// odd section index
			sectionIndex += 1;
		}
		dsp.dspChannel[channel].peqCoeffs[sectionIndex + 0] = coeffs[0]; // a0 (zeros)
		dsp.dspChannel[channel].peqCoeffs[sectionIndex + 2] = coeffs[1]; // a1 (zeros)
		dsp.dspChannel[channel].peqCoeffs[sectionIndex + 4] = coeffs[2]; // a2 (zeros)
		dsp.dspChannel[channel].peqCoeffs[sectionIndex + 6] = -coeffs[3]; // -b1 (poles)
		dsp.dspChannel[channel].peqCoeffs[sectionIndex + 8] = -coeffs[4]; // -b2 (poles)
	}else{
		// last section: store without interleaving
		int sectionIndex = (MAX_CHAN_EQS - 1) * 5;
		dsp.dspChannel[channel].peqCoeffs[sectionIndex + 0] = coeffs[0]; // a0 (zeros)
		dsp.dspChannel[channel].peqCoeffs[sectionIndex + 1] = coeffs[1]; // a1 (zeros)
		dsp.dspChannel[channel].peqCoeffs[sectionIndex + 2] = coeffs[2]; // a2 (zeros)
		dsp.dspChannel[channel].peqCoeffs[sectionIndex + 3] = -coeffs[3]; // -b1 (poles)
		dsp.dspChannel[channel].peqCoeffs[sectionIndex + 4] = -coeffs[4]; // -b2 (poles)
	}
}

void fxProcessCompressorLogic(int channel, float samples[]) {
	float input_abs = abs(meanf(samples, SAMPLES_IN_BUFFER));

	dsp.dspChannel[channel].compressor.triggered = (input_abs > dsp.dspChannel[channel].compressor.value_threshold);

	// calculate the gate-logic and online-parameters
	switch (dsp.dspChannel[channel].compressor.state) {
		case COMPRESSOR_IDLE:
			dsp.compressorGainSet[channel] = 1.0f;
			// check if we have to open the gate
			if (!dsp.dspChannel[channel].compressor.triggered) {
				break;
			}

			dsp.dspChannel[channel].compressor.state = COMPRESSOR_ATTACK;
			// fall-through to ATTACK
		case COMPRESSOR_ATTACK:
			// overshoot = abs(sample) - threshold
			// output = (overshoot / ratio) + threshold
			// gainSet = output / abs(input)
			if ((input_abs > 0) && (dsp.dspChannel[channel].compressor.value_ratio != 0)) {
				// gainSet = input_abs
				dsp.compressorGainSet[channel] = (((input_abs - dsp.dspChannel[channel].compressor.value_threshold) * dsp.dspChannel[channel].compressor.value_ratio) + dsp.dspChannel[channel].compressor.value_threshold) / input_abs;
			}

			dsp.compressorCoeff[channel] = dsp.dspChannel[channel].compressor.value_coeff_attack;
			dsp.dspChannel[channel].compressor.state = COMPRESSOR_ACTIVE;
			// fall-through to ACTIVE
		case COMPRESSOR_ACTIVE:
			// check if we are still triggered
			if (dsp.dspChannel[channel].compressor.triggered) {
				// check if we have to compress even more
				float newValue;
				if ((input_abs > 0) && (dsp.dspChannel[channel].compressor.value_ratio != 0)) {
					newValue = (((input_abs - dsp.dspChannel[channel].compressor.value_threshold) * dsp.dspChannel[channel].compressor.value_ratio) + dsp.dspChannel[channel].compressor.value_threshold) / input_abs;
					if (newValue < dsp.compressorGainSet[channel]) {
						// compress even more
						dsp.compressorGainSet[channel] = newValue;
					}
				}

				// stay in this state
				dsp.dspChannel[channel].compressor.state = COMPRESSOR_ACTIVE;
				break;
			}

			dsp.dspChannel[channel].compressor.holdCounter = dsp.dspChannel[channel].compressor.value_hold_ticks;
			dsp.dspChannel[channel].compressor.state = COMPRESSOR_HOLD;
			// fall-through to hold
		case COMPRESSOR_HOLD:
			if (dsp.dspChannel[channel].compressor.triggered) {
				// re-enter active-state
				float newValue;
				if ((input_abs > 0) && (dsp.dspChannel[channel].compressor.value_ratio != 0)) {
					newValue = (((input_abs - dsp.dspChannel[channel].compressor.value_threshold) * dsp.dspChannel[channel].compressor.value_ratio) + dsp.dspChannel[channel].compressor.value_threshold) / input_abs;
					if (newValue < dsp.compressorGainSet[channel]) {
						// compress even more
						dsp.compressorGainSet[channel] = newValue;
					}
				}
				dsp.dspChannel[channel].compressor.state = COMPRESSOR_ACTIVE;
				dsp.dspChannel[channel].compressor.holdCounter = dsp.dspChannel[channel].compressor.value_hold_ticks;
				break;
			}

			// we are below threshold
			if (dsp.dspChannel[channel].compressor.holdCounter >= 0) {
				dsp.dspChannel[channel].compressor.holdCounter--;
				break;
			}

			dsp.dspChannel[channel].compressor.state = COMPRESSOR_RELEASE;
			// fall-through to RELEASE
		case COMPRESSOR_RELEASE:
			dsp.compressorGainSet[channel] = 1.0f;
			dsp.compressorCoeff[channel] = dsp.dspChannel[channel].compressor.value_coeff_release;
			dsp.dspChannel[channel].compressor.state = COMPRESSOR_IDLE;
			break;
		default:
			dsp.dspChannel[channel].compressor.state = COMPRESSOR_IDLE;
			break;
	}
}
