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

// audio-buffers for transmitting and receiving
// 16 Audiosamples per channel (= 333us latency)
int audioTxBuf0[BUFFER_SIZE] = {0}; // Ch 1-8
int audioTxBuf1[BUFFER_SIZE] = {0}; // Ch 9-16
int audioTxBuf2[BUFFER_SIZE] = {0}; // P16 Ch 1-8
int audioTxBuf3[BUFFER_SIZE] = {0}; // P16 Ch 9-16
int audioTxBuf4[BUFFER_SIZE] = {0}; // AUX Ch 1-8

int audioRxBuf0[BUFFER_SIZE] = {0}; // Ch 1-8
int audioRxBuf1[BUFFER_SIZE] = {0}; // Ch 9-16
int audioRxBuf2[BUFFER_SIZE] = {0}; // Ch 17-24
int audioRxBuf3[BUFFER_SIZE] = {0}; // Ch 25-32
int audioRxBuf4[BUFFER_SIZE] = {0}; // AUX Ch 1-8

// pointers to individual audio-samples for more convenient access
int* pAudioInputSamples[40][SAMPLES_IN_BUFFER];
int* pAudioOutputSamples[40][SAMPLES_IN_BUFFER];

// TCB-arrays for SPORT {CPSPx Chainpointer, ICSPx Internal Count, IMSPx Internal Modifier, IISPx Internal Index}
// TCB-arrays for SPORT {pointer to pointer to buffer, buffer-size, ???, pointer to buffer}
int audioTx0a_tcb[4] = {(int)&audioTx0a_tcb[0] + 3, BUFFER_SIZE, 1, (int)&audioTxBuf0[0]};
int audioTx0b_tcb[4] = {(int)&audioTx0b_tcb[0] + 3, BUFFER_SIZE, 1, (int)&audioTxBuf1[0]};
int audioTx2a_tcb[4] = {(int)&audioTx2a_tcb[0] + 3, BUFFER_SIZE, 1, (int)&audioTxBuf2[0]};
int audioTx2b_tcb[4] = {(int)&audioTx2b_tcb[0] + 3, BUFFER_SIZE, 1, (int)&audioTxBuf3[0]};
int audioTx4a_tcb[4] = {(int)&audioTx4a_tcb[0] + 3, BUFFER_SIZE, 1, (int)&audioTxBuf4[0]};
int audioTx4b_tcb[4] = {0, 0, 1, 0}; // unused at the moment
int audioTx6a_tcb[4] = {0, 0, 1, 0}; // unused at the moment
int audioTx6b_tcb[4] = {0, 0, 1, 0}; // unused at the moment

int audioRx1a_tcb[4] = {(int)&audioRx1a_tcb[0] + 3, BUFFER_SIZE, 1, (int)&audioRxBuf0[0]};
int audioRx1b_tcb[4] = {(int)&audioRx1b_tcb[0] + 3, BUFFER_SIZE, 1, (int)&audioRxBuf1[0]};
int audioRx3a_tcb[4] = {(int)&audioRx3a_tcb[0] + 3, BUFFER_SIZE, 1, (int)&audioRxBuf2[0]};
int audioRx3b_tcb[4] = {(int)&audioRx3b_tcb[0] + 3, BUFFER_SIZE, 1, (int)&audioRxBuf3[0]};
int audioRx5a_tcb[4] = {(int)&audioRx5a_tcb[0] + 3, BUFFER_SIZE, 1, (int)&audioRxBuf4[0]};
int audioRx5b_tcb[4] = {0, 0, 1, 0}; // unused at the moment
int audioRx7a_tcb[4] = {0, 0, 1, 0}; // unused at the moment
int audioRx7b_tcb[4] = {0, 0, 1, 0}; // unused at the moment

void audioInit(void) {
	// init pointer-array
	for (int ch = 0; ch < 8; ch++) {
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			pAudioInputSamples[ch][s] = &audioRxBuf0[ch + (s * 8)];			// channels 1-8
			pAudioInputSamples[ch + 8][s] = &audioRxBuf1[ch + (s * 8)];		// channels 9-16
			pAudioInputSamples[ch + 16][s] = &audioRxBuf2[ch + (s * 8)];	// channels 17-24
			pAudioInputSamples[ch + 24][s] = &audioRxBuf3[ch + (s * 8)];	// channels 25-32
			pAudioInputSamples[ch + 32][s] = &audioRxBuf4[ch + (s * 8)];	// channels 33-40

			pAudioOutputSamples[ch][s] = &audioTxBuf0[ch + (s * 8)];		// channels 1-8
			pAudioOutputSamples[ch + 8][s] = &audioTxBuf1[ch + (s * 8)];	// channels 9-16
			pAudioOutputSamples[ch + 16][s] = &audioTxBuf2[ch + (s * 8)];	// channels 17-24
			pAudioOutputSamples[ch + 24][s] = &audioTxBuf3[ch + (s * 8)];	// channels 25-32
			pAudioOutputSamples[ch + 32][s] = &audioTxBuf4[ch + (s * 8)];	// channels 33-40
		}
	}

	// ============== FOR TESTING ONLY ==============
	// fill TDM-buffer with sinewave-samples with increasing frequency between 1kHz and 8kHz
	float omega = 2.0f * pi * 1000.0f / SAMPLERATE; // w = 2*pi*f between 1kHz and 8kHz
	for (int ch = 0; ch < 8; ch++) {
		for (int s = 0; (s < SAMPLES_IN_BUFFER); s++) {
			*pAudioOutputSamples[ch][s] = sin(omega * (float)(ch + 1) * (float)s) * 8388608;
			*pAudioOutputSamples[ch + 8][s] = sin(omega * (float)(ch + 1) * (float)s) * 8388608;
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

	// do something with the received samples

/*
	// copy input data to output buffer directly (pass-through)
	memcpy(&audioTxBuf0[0], &audioRxBuf0[0], sizeof(audioRxBuf0));
	memcpy(&audioTxBuf1[0], &audioRxBuf1[0], sizeof(audioRxBuf1));
	memcpy(&audioTxBuf2[0], &audioRxBuf2[0], sizeof(audioRxBuf2));
	memcpy(&audioTxBuf3[0], &audioRxBuf3[0], sizeof(audioRxBuf3));
	memcpy(&audioTxBuf4[0], &audioRxBuf4[0], sizeof(audioRxBuf4));
*/
	// copy input samples to output with decreasing volume per 8 channels
	// lets keep channel 1 as sine-wave
	//for (int ch = 0; ch < 8; ch++) {
	for (int ch = 0; ch < 8; ch++) {
		for (int s = 0; (s < SAMPLES_IN_BUFFER); s++) {
			*pAudioOutputSamples[ch][s]      = (int)((float)*pAudioInputSamples[ch][s]      * ((float)(8 - ch) / 8.0f));
			*pAudioOutputSamples[ch + 8][s]  = (int)((float)*pAudioInputSamples[ch + 8][s]  * ((float)(8 - ch) / 8.0f));
			*pAudioOutputSamples[ch + 16][s] = (int)((float)*pAudioInputSamples[ch + 16][s] * ((float)(8 - ch) / 8.0f));
			*pAudioOutputSamples[ch + 24][s] = (int)((float)*pAudioInputSamples[ch + 24][s] * ((float)(8 - ch) / 8.0f));
			*pAudioOutputSamples[ch + 32][s] = (int)((float)*pAudioInputSamples[ch + 32][s] * ((float)(8 - ch) / 8.0f));
		}
	}
	audioProcessing = 0; // clear global flag that processing is done
}

void audioISR(uint32_t iid, void *handlerarg) {
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
