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

void fxSetPeqCoeffs(int channel, int index, float coeffs[]) {
	// biquad_trans() needs the coeffs in the following order
	// a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 0/1)
	// a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 2/3)
	// a0 a1 a2 b1 b2 (section 4)
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

float fxProcessGate(float input, sGate* gate) {
	gate->closed = abs(input) < gate->value_threshold;

	// calculate the gate-logic and online-parameters
	switch (gate->state) {
		case GATE_CLOSED:
			gate->gainSet = gate->value_gainmin;
			// check if we have to open the gate
			if (!gate->closed) {
				gate->state = GATE_ATTACK;
			}
			break;
		case GATE_ATTACK:
			gate->gainSet = 1.0f;
			gate->coeff = gate->value_coeff_attack;
			gate->state = GATE_OPEN;
			break;
		case GATE_OPEN:
			if (gate->closed) {
				gate->holdCounter = gate->value_hold_ticks;
				gate->state = GATE_HOLD;
			}
			break;
		case GATE_HOLD:
			if (!gate->closed) {
				// re-enter on-state
				gate->state = GATE_OPEN;
			}else{
				if (gate->holdCounter == 0) {
					gate->state = GATE_CLOSING;
				}else{
					gate->holdCounter--;
				}
			}
			break;
		case GATE_CLOSING:
			gate->gainSet = gate->value_gainmin;
			gate->coeff = gate->value_coeff_release;
			gate->state = GATE_CLOSED;
			break;
		default:
			gate->state = GATE_CLOSED;
			break;
	}

	// gainCurrent = (gainCurrent * coeff) + (gainSet - gainSet * coeff);
	gate->gainCurrent = (gate->gainCurrent * gate->coeff) + gate->gainSet - (gate->gainSet * gate->coeff);

	return (input * gate->gainCurrent);
}

float fxProcessEq(float input, sPEQ* peq) {
	float output = (peq->a[0] * input) + (peq->a[1] * peq->in[0]) + (peq->a[2] * peq->in[1]) - ((peq->b[1] * peq->out[0]) + (peq->b[2] * peq->out[1]));

	// store values for next calculation cycle
	peq->in[1] = peq->in[0]; // z-2
	peq->in[0] = input; // z-1

	peq->out[1] = peq->out[0];
	peq->out[0] = output;

	return output;
}

float fxProcessCompressor(float input, sCompressor* compressor) {
	float input_abs = abs(input);

	compressor->active = (input_abs > compressor->value_threshold);

	// calculate the gate-logic and online-parameters
	switch (compressor->state) {
		case COMPRESSOR_IDLE:
			compressor->gainSet = 1.0f;
			// check if we have to open the gate
			if (compressor->active) {
				compressor->state = COMPRESSOR_ATTACK;
			}
			break;
		case COMPRESSOR_ATTACK:
			// overshoot = abs(sample) - threshold
			// output = (overshoot / ratio) + threshold
			// gainSet = output / abs(input)
			if ((input_abs > 0) && (compressor->value_ratio != 0)) {
				compressor->gainSet = (((input_abs - compressor->value_threshold) / compressor->value_ratio) + compressor->value_threshold) / input_abs;
			}
			compressor->coeff = compressor->value_coeff_attack;
			compressor->state = COMPRESSOR_ACTIVE;
			break;
		case COMPRESSOR_ACTIVE:
			if (compressor->active) {
				// check if we have to compress even more
				float newValue;
				if ((input_abs > 0) && (compressor->value_ratio != 0)) {
					newValue = (((input_abs - compressor->value_threshold) / compressor->value_ratio) + compressor->value_threshold) / input_abs;
					if (newValue < compressor->gainSet) {
						// compress even more
						compressor->gainSet = newValue;
					}
				}
			}else{
				compressor->holdCounter = compressor->value_hold_ticks;
				compressor->state = COMPRESSOR_HOLD;
			}
			break;
		case COMPRESSOR_HOLD:
			if (compressor->active) {
				// re-enter on-state
				compressor->state = COMPRESSOR_ACTIVE;
			}else{
				// we are below threshold
				if (compressor->holdCounter == 0) {
					compressor->state = COMPRESSOR_RELEASE;
				}else{
					compressor->holdCounter--;
				}
			}
			break;
		case COMPRESSOR_RELEASE:
			compressor->gainSet = 1.0f;
			compressor->coeff = compressor->value_coeff_release;
			compressor->state = COMPRESSOR_IDLE;
			break;
		default:
			compressor->state = COMPRESSOR_IDLE;
			break;
	}

	// gainCurrent = (gainCurrent * coeff) + gainSet - (gainSet * coeff);
	compressor->gainCurrent = (compressor->gainCurrent * compressor->coeff) + compressor->gainSet - (compressor->gainSet * compressor->coeff);

	return input * compressor->gainCurrent * compressor->value_makeup;
}


/*
  Use of CCES biquad filter (scalar):
  ===================================
  #define NSECTIONS  2
  #define NSAMPLES  200
  #define NSTATE    (2*NSECTIONS)

  float input[NSAMPLES];
  float output[NSAMPLES];
  float state[NSTATE];
                                // -a2      -a1        b2       b1        b0
  float pm coeffs[5*NSECTIONS]={-0.74745, 1.72593,  1.00000,  2.00000,  1.00000,
                                -0.88703, 1.86380,  1.00000,  2.00000,  1.00000};

  for (i = 0; i < NSTATE; i++) {
    state[i] = 0;
  }

  biquad (input, output, coeffs, state, NSAMPLES, NSECTIONS);


  Use of CCES biquad filter (vector with SIMD):
  =============================================
  biquad(
  	  const float dm input[]				-> input samples
  	  float dm output[]						-> output samples
  	  const float pm coeffs[5 * sections]	-> a2,a1,b2,b1,b0
  	  float dm state[2 * sections]			->
  	  int samples							-> number of samples in input[] and output[]
  	  int sections							-> 5 for a 5-band EQ
  );

*/
