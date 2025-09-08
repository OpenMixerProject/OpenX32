#include "audio.h"

/*
	Used Audio-Mapping:
	========================
	SPORT0A   -> TDM OUT0
	SPORT0B   -> TDM OUT1
	SPORT1A   -> TDM OUT2
	SPORT1B   -> TDM OUT3
	SPORT3A   -> TDM OUTAUX

	TDM IN0   -> SPORT1A
	TDM IN1   -> SPORT1B
	TDM IN2   -> SPORT3A
	TDM IN3   -> SPORT3B
	TDM INAUX -> SPORT5A
*/

volatile int audioReady = 0;
volatile int audioProcessing = 0;
volatile int audioIsrCounter = 0;
int audioBufferCounter = 0;

// audio-buffers for transmitting and receiving
// 16 Audiosamples per channel (= 333us latency)
int audioTxBuf0[BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch 1-8
int audioTxBuf1[BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch 9-16
int audioTxBuf2[BUFFER_COUNT * BUFFER_SIZE] = {0}; // P16 Ch 1-8
int audioTxBuf3[BUFFER_COUNT * BUFFER_SIZE] = {0}; // P16 Ch 9-16
int audioTxBuf4[BUFFER_COUNT * BUFFER_SIZE] = {0}; // AUX Ch 1-8

int audioRxBuf0[BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch 1-8
int audioRxBuf1[BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch 9-16
int audioRxBuf2[BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch 17-24
int audioRxBuf3[BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch 25-32
int audioRxBuf4[BUFFER_COUNT * BUFFER_SIZE] = {0}; // AUX Ch 1-8

// pointers to individual audio-samples for more convenient access
int* pAudioOutputSamples[40][BUFFER_COUNT * SAMPLES_IN_BUFFER];
int* pAudioInputSamples[40][BUFFER_COUNT * SAMPLES_IN_BUFFER];

// TCB-arrays for SPORT {CPSPx Chainpointer, ICSPx Internal Count, IMSPx Internal Modifier, IISPx Internal Index}
// TCB-arrays for SPORT {pointer to pointer to buffer, buffer-size, ???, pointer to buffer}
int audioTx0a_tcb[BUFFER_COUNT][4];
int audioTx0b_tcb[BUFFER_COUNT][4];
int audioTx2a_tcb[BUFFER_COUNT][4];
int audioTx2b_tcb[BUFFER_COUNT][4];
int audioTx4a_tcb[BUFFER_COUNT][4];
int audioTx4b_tcb[BUFFER_COUNT][4]; // unused at the moment
int audioTx6a_tcb[BUFFER_COUNT][4]; // unused at the moment
int audioTx6b_tcb[BUFFER_COUNT][4]; // unused at the moment

int audioRx1a_tcb[BUFFER_COUNT][4];
int audioRx1b_tcb[BUFFER_COUNT][4];
int audioRx3a_tcb[BUFFER_COUNT][4];
int audioRx3b_tcb[BUFFER_COUNT][4];
int audioRx5a_tcb[BUFFER_COUNT][4];
int audioRx5b_tcb[BUFFER_COUNT][4]; // unused at the moment
int audioRx7a_tcb[BUFFER_COUNT][4]; // unused at the moment
int audioRx7b_tcb[BUFFER_COUNT][4]; // unused at the moment

void audioInit(void) {
	// init TCB-array with multi-buffering
	for (int i_buf = 0; i_buf < BUFFER_COUNT; i_buf++) {
		// check if we have to direct to first buffer again
		int nextBuffer;
		if (i_buf == (BUFFER_COUNT - 1)) {
			// direct back to the first buffer
			nextBuffer = 0;
		}else{
			// direct to following buffer
			nextBuffer = i_buf + 1;
		}

		// direct DMA-chain-controller to the subsequent buffer
		audioTx0a_tcb[i_buf][0] = ((int)&audioTx0a_tcb[nextBuffer][0] + 3);
		audioTx0b_tcb[i_buf][0] = ((int)&audioTx0b_tcb[nextBuffer][0] + 3);
		audioTx2a_tcb[i_buf][0] = ((int)&audioTx2a_tcb[nextBuffer][0] + 3);
		audioTx2b_tcb[i_buf][0] = ((int)&audioTx2b_tcb[nextBuffer][0] + 3);
		audioTx4a_tcb[i_buf][0] = ((int)&audioTx4a_tcb[nextBuffer][0] + 3);
		// tell DMA-controller the size of our buffers
		audioTx0a_tcb[i_buf][1] = BUFFER_SIZE;
		audioTx0b_tcb[i_buf][1] = BUFFER_SIZE;
		audioTx2a_tcb[i_buf][1] = BUFFER_SIZE;
		audioTx2b_tcb[i_buf][1] = BUFFER_SIZE;
		audioTx4a_tcb[i_buf][1] = BUFFER_SIZE;
		// set modification-value to 1
		audioTx0a_tcb[i_buf][2] = 1;
		audioTx0b_tcb[i_buf][2] = 1;
		audioTx2a_tcb[i_buf][2] = 1;
		audioTx2b_tcb[i_buf][2] = 1;
		audioTx4a_tcb[i_buf][2] = 1;
		// pointer to the buffer
		audioTx0a_tcb[i_buf][3] = (int)&audioTxBuf0[i_buf * BUFFER_SIZE];
		audioTx0b_tcb[i_buf][3] = (int)&audioTxBuf1[i_buf * BUFFER_SIZE];
		audioTx2a_tcb[i_buf][3] = (int)&audioTxBuf2[i_buf * BUFFER_SIZE];
		audioTx2b_tcb[i_buf][3] = (int)&audioTxBuf3[i_buf * BUFFER_SIZE];
		audioTx4a_tcb[i_buf][3] = (int)&audioTxBuf4[i_buf * BUFFER_SIZE];

		// direct DMA-chain-controller to the subsequent buffer
		audioRx1a_tcb[i_buf][0] = ((int)&audioRx1a_tcb[nextBuffer][0] + 3);
		audioRx1b_tcb[i_buf][0] = ((int)&audioRx1b_tcb[nextBuffer][0] + 3);
		audioRx3a_tcb[i_buf][0] = ((int)&audioRx3a_tcb[nextBuffer][0] + 3);
		audioRx3b_tcb[i_buf][0] = ((int)&audioRx3b_tcb[nextBuffer][0] + 3);
		audioRx5a_tcb[i_buf][0] = ((int)&audioRx5a_tcb[nextBuffer][0] + 3);
		// tell DMA-controller the size of our buffers
		audioRx1a_tcb[i_buf][1] = BUFFER_SIZE;
		audioRx1b_tcb[i_buf][1] = BUFFER_SIZE;
		audioRx3a_tcb[i_buf][1] = BUFFER_SIZE;
		audioRx3b_tcb[i_buf][1] = BUFFER_SIZE;
		audioRx5a_tcb[i_buf][1] = BUFFER_SIZE;
		// set modification-value to 1
		audioRx1a_tcb[i_buf][2] = 1;
		audioRx1b_tcb[i_buf][2] = 1;
		audioRx3a_tcb[i_buf][2] = 1;
		audioRx3b_tcb[i_buf][2] = 1;
		audioRx5a_tcb[i_buf][2] = 1;
		// pointer to the buffer
		audioRx1a_tcb[i_buf][3] = (int)&audioRxBuf0[i_buf * BUFFER_SIZE];
		audioRx1b_tcb[i_buf][3] = (int)&audioRxBuf1[i_buf * BUFFER_SIZE];
		audioRx3a_tcb[i_buf][3] = (int)&audioRxBuf2[i_buf * BUFFER_SIZE];
		audioRx3b_tcb[i_buf][3] = (int)&audioRxBuf3[i_buf * BUFFER_SIZE];
		audioRx5a_tcb[i_buf][3] = (int)&audioRxBuf4[i_buf * BUFFER_SIZE];
	}

	// update pointer-array
	// the resulting pointer-array is always sorted from the most recent (index 0) to the oldest sample (max index) per channel
	//
	// on transmitting: audioOutputBufferCounter is the currently transmitted data via DMA to SPORT:
	//     - don't overwrite these samples
	//     - pAudioOutputSamples[ch][0] will point to the subsequent samples that will be transmitted on next DMA-cycle
	// on receiving: audioInputBufferCounter is the currently received data via DMA from SPORT:
	//     - it is safe to read these samples as DMA cycle has been completed when calling the processdata-function
	int bufferRead;
	for (int ch = 0; ch < 8; ch++) {
		for (int buf = 0; buf < BUFFER_COUNT; buf++) {
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
				// calculate the desired buffer as we have circular buffer-structure
				// e.g. if we are in buffer 3 of 3 the older samples are then in buffer 1 of 3 and the oldest are in buffer 2 of 3
				bufferRead = audioBufferCounter + buf;
				while (bufferRead >= BUFFER_COUNT) {
					bufferRead -= BUFFER_COUNT;
				}
				while (bufferRead < 0) {
					bufferRead += BUFFER_COUNT;
				}

				pAudioOutputSamples[ch     ][(buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf0[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 1-8
				pAudioOutputSamples[ch + 8 ][(buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf1[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 9-16
				pAudioOutputSamples[ch + 16][(buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf2[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 17-24
				pAudioOutputSamples[ch + 24][(buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf3[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 25-32
				pAudioOutputSamples[ch + 32][(buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf4[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 33-40

				// most recent samples at position 0
				pAudioInputSamples[ch     ][(buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf0[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 1-8
				pAudioInputSamples[ch + 8 ][(buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf1[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 9-16
				pAudioInputSamples[ch + 16][(buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf2[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 17-24
				pAudioInputSamples[ch + 24][(buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf3[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 25-32
				pAudioInputSamples[ch + 32][(buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf4[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 33-40
			}
		}
	}

	// ============== FOR TESTING ONLY ==============
	// fill TDM-buffer with sinewave-samples with increasing frequency between 1kHz and 8kHz
	float omega = 2.0f * pi * 1000.0f / SAMPLERATE; // w = 2*pi*f between 1kHz and 8kHz
	for (int ch = 0; ch < 8; ch++) {
		for (int s = 0; (s < (BUFFER_COUNT * SAMPLES_IN_BUFFER)); s++) {
			*pAudioOutputSamples[ch     ][s] = sin(omega * (float)(ch + 1) * (float)s) * 8388608;
			*pAudioOutputSamples[ch + 8 ][s] = sin(omega * (float)(ch + 1) * (float)s) * 8388608;
			*pAudioOutputSamples[ch + 16][s] = sin(omega * (float)(ch + 1) * (float)s) * 8388608;
			*pAudioOutputSamples[ch + 24][s] = sin(omega * (float)(ch + 1) * (float)s) * 8388608;
			*pAudioOutputSamples[ch + 32][s] = sin(omega * (float)(ch + 1) * (float)s) * 8388608;
		}
	}
	// ============== FOR TESTING ONLY ==============
}

void audioProcessData(void) {
	audioReady = 0; // clear global flag that audio is not ready anymore
	audioProcessing = 1; // set global flag that we are processing now

	// update pointer-array
	// the resulting pointer-array is always sorted from the most recent (index 0) to the oldest sample (max index) per channel
	//
	// on transmitting: audioOutputBufferCounter is the currently transmitted data via DMA to SPORT:
	//     - don't overwrite these samples
	//     - pAudioOutputSamples[ch][0] will point to the subsequent samples that will be transmitted on next DMA-cycle
	// on receiving: audioInputBufferCounter is the currently received data via DMA from SPORT:
	//     - it is safe to read these samples as DMA cycle has been completed when calling the processdata-function
	int bufferRead;
	for (int ch = 0; ch < 8; ch++) {
		for (int buf = 0; buf < BUFFER_COUNT; buf++) {
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
				// calculate the desired buffer as we have circular buffer-structure
				// e.g. if we are in buffer 3 of 3 the older samples are then in buffer 1 of 3 and the oldest are in buffer 2 of 3
				bufferRead = audioBufferCounter + buf;
				while (bufferRead >= BUFFER_COUNT) {
					bufferRead -= BUFFER_COUNT;
				}
				while (bufferRead < 0) {
					bufferRead += BUFFER_COUNT;
				}

				pAudioOutputSamples[ch     ][(buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf0[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 1-8
				pAudioOutputSamples[ch + 8 ][(buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf1[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 9-16
				pAudioOutputSamples[ch + 16][(buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf2[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 17-24
				pAudioOutputSamples[ch + 24][(buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf3[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 25-32
				pAudioOutputSamples[ch + 32][(buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf4[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 33-40

				// most recent samples at position 0
				pAudioInputSamples[ch     ][(buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf0[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 1-8
				pAudioInputSamples[ch + 8 ][(buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf1[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 9-16
				pAudioInputSamples[ch + 16][(buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf2[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 17-24
				pAudioInputSamples[ch + 24][(buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf3[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 25-32
				pAudioInputSamples[ch + 32][(buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf4[bufferRead * BUFFER_SIZE + ch + (s * 8)];	// channels 33-40
			}
		}
	}

	// do something with the received samples
/*
	// copy RAW input data to RAW output buffer (pass-through)
	memcpy(&audioTxBuf0[0], &audioRxBuf0[0], sizeof(audioRxBuf0));
	memcpy(&audioTxBuf1[0], &audioRxBuf1[0], sizeof(audioRxBuf1));
	memcpy(&audioTxBuf2[0], &audioRxBuf2[0], sizeof(audioRxBuf2));
	memcpy(&audioTxBuf3[0], &audioRxBuf3[0], sizeof(audioRxBuf3));
	memcpy(&audioTxBuf4[0], &audioRxBuf4[0], sizeof(audioRxBuf4));
*/

	/*
	// copy input samples to output with decreasing volume per 8 channels
	for (int ch = 0; ch < 8; ch++) {
		for (int s = 0; (s < SAMPLES_IN_BUFFER); s++) {
			*pAudioOutputSamples[ch     ][s] = (int)((float)*pAudioInputSamples[ch     ][s] * ((float)(8 - ch) / 8.0f));
			*pAudioOutputSamples[ch + 8 ][s] = (int)((float)*pAudioInputSamples[ch + 8 ][s] * ((float)(8 - ch) / 8.0f));
			*pAudioOutputSamples[ch + 16][s] = (int)((float)*pAudioInputSamples[ch + 16][s] * ((float)(8 - ch) / 8.0f));
			*pAudioOutputSamples[ch + 24][s] = (int)((float)*pAudioInputSamples[ch + 24][s] * ((float)(8 - ch) / 8.0f));
			*pAudioOutputSamples[ch + 32][s] = (int)((float)*pAudioInputSamples[ch + 32][s] * ((float)(8 - ch) / 8.0f));
		}
	}
	*/
	/*
	for (int i = 0; i < (OUTPUT_BUFFER_COUNT * BUFFER_SIZE); i++) {
		audioTxBuf0[i] = audioRxBuf0[i];
	}
	*/
	// copy first 8 channels to all DSP outputs
	for (int ch = 0; ch < 8; ch++) {
		for (int s = 0; s < (BUFFER_COUNT * SAMPLES_IN_BUFFER); s++) {
			*pAudioOutputSamples[ch   ][s] = *pAudioInputSamples[ch][s];
			*pAudioOutputSamples[ch+8 ][s] = *pAudioInputSamples[ch][s];
			*pAudioOutputSamples[ch+16][s] = *pAudioInputSamples[ch][s];
			*pAudioOutputSamples[ch+24][s] = *pAudioInputSamples[ch][s];
			*pAudioOutputSamples[ch+32][s] = *pAudioInputSamples[ch][s];
		}
	}

	// increment buffer-counter for next call
    audioBufferCounter++;
    if (audioBufferCounter >= BUFFER_COUNT) {
    	audioBufferCounter = 0;
    }

	audioProcessing = 0; // clear global flag that processing is done
}

void audioTxISR(uint32_t iid, void *handlerarg) {
}

void audioRxISR(uint32_t iid, void *handlerarg) {
	// we received new audio-data
	// check if we are still processing the data, which means >100% CPU Load -> Crash System
    if (audioProcessing) {
    	systemCrash();
    }

    audioReady = 1; // set flag, that we have new data to process

    audioIsrCounter++;
    if (audioIsrCounter >= (SAMPLERATE / SAMPLES_IN_BUFFER)) {
    	audioIsrCounter = 0;
    }
}
