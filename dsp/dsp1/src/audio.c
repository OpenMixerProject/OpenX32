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

// audio-buffers for transmitting and receiving
// 16 Audiosamples per channel (= 333us latency)
int audioTxBuf0[BUFFER_SIZE]; // Ch 1-8
int audioTxBuf1[BUFFER_SIZE]; // Ch 9-16
int audioTxBuf2[BUFFER_SIZE]; // P16 Ch 1-8
int audioTxBuf3[BUFFER_SIZE]; // P16 Ch 9-16
int audioTxBuf4[BUFFER_SIZE]; // AUX Ch 1-8

int audioRxBuf0[BUFFER_SIZE]; // Ch 1-8
int audioRxBuf1[BUFFER_SIZE]; // Ch 9-16
int audioRxBuf2[BUFFER_SIZE]; // Ch 17-24
int audioRxBuf3[BUFFER_SIZE]; // Ch 25-32
int audioRxBuf4[BUFFER_SIZE]; // AUX Ch 1-8

// TCB-arrays for SPORT {pointer to buffer-size, buffer-size, ???, pointer to buffer}
unsigned int audioTx0a_tcb[4] = {(unsigned int)&audioTx0a_tcb[0] + 3, BUFFER_SIZE, 1, (unsigned int)&audioTxBuf0[0]};
unsigned int audioTx0b_tcb[4] = {(unsigned int)&audioTx0b_tcb[0] + 3, BUFFER_SIZE, 1, (unsigned int)&audioTxBuf1[0]};
unsigned int audioTx2a_tcb[4] = {(unsigned int)&audioTx2a_tcb[0] + 3, BUFFER_SIZE, 1, (unsigned int)&audioTxBuf2[0]};
unsigned int audioTx2b_tcb[4] = {(unsigned int)&audioTx2b_tcb[0] + 3, BUFFER_SIZE, 1, (unsigned int)&audioTxBuf3[0]};
unsigned int audioTx4a_tcb[4] = {(unsigned int)&audioTx4a_tcb[0] + 3, BUFFER_SIZE, 1, (unsigned int)&audioTxBuf4[0]};
unsigned int audioTx4b_tcb[4] = {0, 0, 1, 0}; // unused at the moment
unsigned int audioTx6a_tcb[4] = {0, 0, 1, 0}; // unused at the moment
unsigned int audioTx6b_tcb[4] = {0, 0, 1, 0}; // unused at the moment

unsigned int audioRx1a_tcb[4] = {(unsigned int)&audioRx1a_tcb[0] + 3, BUFFER_SIZE, 1, (unsigned int)&audioRxBuf0[0]};
unsigned int audioRx1b_tcb[4] = {(unsigned int)&audioRx1b_tcb[0] + 3, BUFFER_SIZE, 1, (unsigned int)&audioRxBuf1[0]};
unsigned int audioRx3a_tcb[4] = {(unsigned int)&audioRx3a_tcb[0] + 3, BUFFER_SIZE, 1, (unsigned int)&audioRxBuf2[0]};
unsigned int audioRx3b_tcb[4] = {(unsigned int)&audioRx3b_tcb[0] + 3, BUFFER_SIZE, 1, (unsigned int)&audioRxBuf3[0]};
unsigned int audioRx5a_tcb[4] = {(unsigned int)&audioRx5a_tcb[0] + 3, BUFFER_SIZE, 1, (unsigned int)&audioRxBuf4[0]};
unsigned int audioRx5b_tcb[4] = {0, 0, 1, 0}; // unused at the moment
unsigned int audioRx7a_tcb[4] = {0, 0, 1, 0}; // unused at the moment
unsigned int audioRx7b_tcb[4] = {0, 0, 1, 0}; // unused at the moment

void audioInit(void) {
	// init audio-buffers with some sinewave data
	int i = 0;
	int t = 0;

	// fill TDM-buffer with sinewave-samples with increasing frequency between 1kHz and 8kHz
	float omega = 2 * pi * 1000 / SAMPLERATE; // w = 2*pi*f between 1kHz and 8kHz
	for (i = 0; (i < SAMPLES_IN_BUFFER); i++) {
		t = i; // time
		audioTxBuf0[i * 8 + 0] = sin(omega * 1 * t) * 8388608; // TDM channel 0
		audioTxBuf0[i * 8 + 1] = sin(omega * 2 * t) * 8388608; // TDM channel 1
		audioTxBuf0[i * 8 + 2] = sin(omega * 3 * t) * 8388608; // TDM channel 2
		audioTxBuf0[i * 8 + 3] = sin(omega * 4 * t) * 8388608; // TDM channel 3
		audioTxBuf0[i * 8 + 4] = sin(omega * 5 * t) * 8388608; // TDM channel 4
		audioTxBuf0[i * 8 + 5] = sin(omega * 6 * t) * 8388608; // TDM channel 5
		audioTxBuf0[i * 8 + 6] = sin(omega * 7 * t) * 8388608; // TDM channel 6
		audioTxBuf0[i * 8 + 7] = sin(omega * 8.1f * t) * 8388608; // TDM channel 7
	}
	// copy to all other tx-buffers
	memcpy(&audioTxBuf1[0], &audioTxBuf0[0], sizeof(audioTxBuf0));
	memcpy(&audioTxBuf2[0], &audioTxBuf0[0], sizeof(audioTxBuf0));
	memcpy(&audioTxBuf3[0], &audioTxBuf0[0], sizeof(audioTxBuf0));
	memcpy(&audioTxBuf4[0], &audioTxBuf0[0], sizeof(audioTxBuf0));

	// clear receive buffer
	for (i = 0; i < 2048; i++) {
		audioRxBuf0[i] = 0;
		audioRxBuf1[i] = 0;
		audioRxBuf2[i] = 0;
		audioRxBuf3[i] = 0;
		audioRxBuf4[i] = 0;
	}
}

void audioProcessData(void) {
	audioReady = 0; // clear global flag that audio is not ready anymore
	audioProcessing = 1; // set global flag that we are processing now

	// do something with the received samples

	audioProcessing = 0; // clear global flag that processing is done
}

void audioISR(uint32_t iid, void *handlerarg) {
	// we received new audio-data
	// check if we are still processing the data, which means >100% CPU Load -> Crash System
    if (audioProcessing) {
    	systemCrash();
    }

    audioReady = 1; // set flag, that we have new data to process
}
