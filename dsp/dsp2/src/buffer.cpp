#include "buffer.h"

void bufferMovingRmsInit(sRingBuffer* ringBuffer, float* buffer, int len) {
	ringBuffer->buffer = buffer;
	ringBuffer->bufferLen = len;
	ringBuffer->one_over_bufferLen = 1.0f / (float)len;
	ringBuffer->head = 0;
	ringBuffer->output = 0;

	// clear buffer
	for (int i = 0; i < ringBuffer->bufferLen; i++) {
		ringBuffer->buffer[i] = 0.0f;
	}
}

void bufferWrite(sRingBuffer* ringBuffer, float in) {
	ringBuffer->buffer[ringBuffer->head] = in;

	ringBuffer->head++;
	if (ringBuffer->head == ringBuffer->bufferLen) {
		ringBuffer->head = 0;
	}
}

float bufferRead(sRingBuffer* ringBuffer, int offset) {
	int tail = ringBuffer->head - offset;
	if (tail < 0) {
		tail += ringBuffer->bufferLen;
	}

	return ringBuffer->buffer[tail];
}

float bufferReadMs(sRingBuffer* ringBuffer, float delayMs, int samplerate) {
	int offset = delayMs * samplerate * 0.001f;
	return bufferRead(ringBuffer, offset);
}

// returns RMS^2
float bufferMovingRms(sRingBuffer* ringBuffer, float in) {
	float value = in * in;

	ringBuffer->buffer[ringBuffer->head] = value;

	ringBuffer->head++;
	if (ringBuffer->head == ringBuffer->bufferLen) {
		ringBuffer->head = 0;
	}

	// output = output + ((in^2 - oldestValue)/bufferLen)
	ringBuffer->output = ringBuffer->output + ((value - ringBuffer->buffer[ringBuffer->head]) * ringBuffer->one_over_bufferLen);

	return ringBuffer->output;
}
