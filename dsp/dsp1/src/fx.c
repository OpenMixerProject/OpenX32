#include "fx.h"

float fxProcessGate(float input, sGate* gate) {
	gate->closed = abs(input) < gate->value_threshold;

	// calculate the gate-logic and online-parameters
	switch (gate->state) {
		case GATE_CLOSED:
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
	compressor->active = (abs(input) > compressor->value_threshold);

	// calculate the gate-logic and online-parameters
	switch (compressor->state) {
		case COMPRESSOR_IDLE:
			// check if we have to open the gate
			if (compressor->active) {
				compressor->state = COMPRESSOR_ATTACK;
			}
			break;
		case COMPRESSOR_ATTACK:
			compressor->gainSet = (((abs(input) - compressor->value_threshold) / compressor->value_ratio) + compressor->value_threshold) / input;
			compressor->coeff = compressor->value_coeff_attack;
			compressor->state = COMPRESSOR_ACTIVE;
			break;
		case COMPRESSOR_ACTIVE:
			if (compressor->active) {
				// check if we have to compress even more
				float newValue = (((abs(input) - compressor->value_threshold) / compressor->value_ratio) + compressor->value_threshold) / input;
				if (newValue < compressor->gainSet) {
					// compress even more
					compressor->gainSet = newValue;
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
	}

	// gainCurrent = (gainCurrent * coeff) + gainSet - (gainSet * coeff);
	compressor->gainCurrent = (compressor->gainCurrent * compressor->coeff) + compressor->gainSet - (compressor->gainSet * compressor->coeff);

	return (input * compressor->gainCurrent) * compressor->value_makeup;
}
