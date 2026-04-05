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

  This file implements a very basic AI-based DeFeedback-filter
*/

#include "fxDeFeedback.h"

#pragma file_attr("prefersMem=internal") // let the linker know, that all variables should be placed into the internal ram

fxDeFeedback::fxDeFeedback(int fxSlot, float* bufIn[], float* bufOut[], int channelMode) : fx(fxSlot, bufIn, bufOut, channelMode) {
	// constructor
	// code of constructor of baseclass is called first. So add here only effect-specific things

	// get the pointers to the sample-buffers
	_bufIn[0] = bufIn[0];
	_bufIn[1] = bufIn[1];
	_bufOut[0] = bufOut[0];
	_bufOut[1] = bufOut[1];

	_weightsRst = 0;
	_historyRst = 0;

	_envelope = 1;
}

fxDeFeedback::~fxDeFeedback() {
    // destructor
}

void fxDeFeedback::setParameters(float delayMs) {

}

void fxDeFeedback::rxData(float data[], int len) {
	// data received from x32ctrl
}

float fxDeFeedback::nn_inference_scalar(float* __restrict input) {
	float hidden[(SAMPLES_IN_BUFFER * 2)];
    float logit = nn_bias_out;

    // Step 1: Input -> Hidden (ReLU)
    for (int i = 0; i < (SAMPLES_IN_BUFFER * 2); i++) {
        float sum = nn_bias_h[i];
        for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
            sum += input[s] * nn_weights_ih[i][s];
        }
        hidden[i] = (sum > 0.0f) ? sum : 0.0f;
    }

    // Step 2: Hidden -> Output Logit
    for (int i = 0; i < (SAMPLES_IN_BUFFER * 2); i++) {
        logit += hidden[i] * nn_weights_ho[i];
    }

    // Step 3: Sigmoid-Activation
    float probability = 1.0f / (1.0f + expf(-logit));

    return probability;
}

void fxDeFeedback::process() {
	if (_startup) {
		for (int i = 0; i < SAMPLES_IN_BUFFER; i++) {
			if (_weightsRst < TAPS) {
				_weights[_weightsRst++] = 0.0;
			}
			if (_historyRst < (TAPS + SAMPLES_IN_BUFFER)) {
				_history[_historyRst++] = 0.0;
			}else{
				_startup = false;
			}
		}

		return;
	}

	// Step 1: LMS Loop
    // Step 1: AI decides: is it feedback (1.0) or regular audio (0.0)

	// use a limiter for the input-signal
	float inputDb = helperFcn_lin2db(fabsf(_bufIn[0][0]) * INT32_TO_FLOAT_NORM);
	float targetGainDb = 0.0f; // 0dBfs
	if (inputDb > -12.0f) {
		targetGainDb = (-12.0f - inputDb); // hard limit with ratio 1:oo
	}
	float currentGainLinear = helperFcn_db2lin(targetGainDb);
	// as we are using this signal just for the AI, we do not use any envelope-curve
	float bufLimitedNormalized[SAMPLES_IN_BUFFER];
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufLimitedNormalized[s] = _bufIn[0][s] * currentGainLinear * (4.0f * INT32_TO_FLOAT_NORM); // with a threshold at -12dB we have to increase the volume by x4 afterwards
	}


    float ai_decision = nn_inference_scalar(&bufLimitedNormalized[0]); // input has to be normalized to -1.0 ... +1.0

	// map the decision to a small learning-rate. Important: this value
	// has to be 0 when we want normal audio
    float current_mu = ai_decision * MU_MAX;

    for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
        // Step 2: simple FIR Filter for prediction
        float pred = 0;
        for(int i = 0; i < TAPS; i++) {
            pred += _weights[i] * _history[i + (SAMPLES_IN_BUFFER - 1 - s)];
        }

        float error = (_bufIn[0][s] * INT32_TO_FLOAT_NORM) - pred;
        _bufOut[0][s] = error;

        // Step 3: NLMS Update (Energy-Normalizing for stability)
        float energy = 0;
        for(int i = 0; i < TAPS; i++) {
            float h = _history[i + (SAMPLES_IN_BUFFER - 1 - s)];
            energy += h * h;
        }
        energy += 0.01f; // small bias to prevent a DIV/0

        float step = current_mu / energy;

        // Step 4: update of the weights
        for (int i = 0; i < TAPS; i++) {
            _weights[i] = (_weights[i] * LEAKAGE) + step * error * _history[i + (SAMPLES_IN_BUFFER - 1 - s)];
        }
    }

    // Step 5: update the history
    memmove(&_history[SAMPLES_IN_BUFFER], &_history[0], TAPS * sizeof(float));
    for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
        _history[SAMPLES_IN_BUFFER - 1 - s] = _bufOut[0][s];
    }



	// Last Step: intervention logic with a hard threshold
    float targetGain = 1;
	if (ai_decision > INTERVENTION_THRESHOLD) {
		// damp the output-signal
		targetGain = 0;

		// fast attack
		_envelope += 0.1f * (targetGain - _envelope);
	}else{
		// slow release of filter
		_envelope += 0.01f * (targetGain - _envelope);
	}
    for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
        _bufOut[0][s] *= _envelope; // hard damping during feedback
    }



	// rescale back to 32-bit
    for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
        _bufOut[0][s] *= 2147483647.0f;
    }

	// copy data to second channel
    for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
        _bufOut[1][s] = _bufOut[0][s];
    }
}
