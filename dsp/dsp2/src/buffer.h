#ifndef BUFFER_H_
#define BUFFER_H_

typedef struct {
	float* buffer;
	int bufferLen;
	int head;

	float one_over_bufferLen;
	float output;
} sRingBuffer;

void bufferMovingRmsInit(sRingBuffer* ringBuffer, float* buffer, int len);
void bufferWrite(sRingBuffer* ringBuffer, float in);
float bufferRead(sRingBuffer* ringBuffer, int offset);
float bufferReadMs(sRingBuffer* ringBuffer, float delayMs, int samplerate);
float bufferMovingRms(sRingBuffer* ringBuffer, float in);

#endif /* BUFFER_H_ */
