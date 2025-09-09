#include "audio.h"
#include "system.h"
#include "fx.h"

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
int* pAudioOutputSamples[40][BUFFER_COUNT * SAMPLES_IN_BUFFER]; // sorted pointer-array that points to the value for the next write cycle
int* pAudioInputSamples[40][BUFFER_COUNT * SAMPLES_IN_BUFFER]; // sorted pointer-array that points to the recently received audio-samples

// TCB-arrays for SPORT {CPSPx Chainpointer, ICSPx Internal Count, IMSPx Internal Modifier, IISPx Internal Index}
// TCB-arrays for SPORT {pointer to pointer to buffer, buffer-size, ???, pointer to buffer}
int audioTx_tcb[8][BUFFER_COUNT][4];
int audioRx_tcb[8][BUFFER_COUNT][4];

void audioUpdatePointerArray(void) {
	// update pointer-array -> the resulting pointer-array is always sorted from the most recent (index 0) to the oldest sample (max index) per channel
	int bufferOffset;
	for (int ch = 0; ch < 8; ch++) {
		for (int i_buf = 0; i_buf < BUFFER_COUNT; i_buf++) {
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) { // TODO: Check if samples are in the correct order. Maybe it is not 0,1,2,..7 but 7,6,5,..,0
				// calculate the pointer-address to the subsequent output-buffer
				bufferOffset = audioBufferCounter + i_buf;
				// put the bufferOffset within the limits of the pointer-array again
				while (bufferOffset >= BUFFER_COUNT) {
					bufferOffset -= BUFFER_COUNT;
				}
				while (bufferOffset < 0) {
					bufferOffset += BUFFER_COUNT;
				}

				// pointer-array has most recent samples at position 0, so we have to rearrange the samples for the DMA-buffer back to expected order by DMA
				pAudioOutputSamples[ch     ][(i_buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf0[(bufferOffset * BUFFER_SIZE) + ch + ((SAMPLES_IN_BUFFER - 1 - s) * 8)];	// channels 1-8
				pAudioOutputSamples[ch + 8 ][(i_buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf1[(bufferOffset * BUFFER_SIZE) + ch + ((SAMPLES_IN_BUFFER - 1 - s) * 8)];	// channels 9-16
				pAudioOutputSamples[ch + 16][(i_buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf2[(bufferOffset * BUFFER_SIZE) + ch + ((SAMPLES_IN_BUFFER - 1 - s) * 8)];	// channels 17-24
				pAudioOutputSamples[ch + 24][(i_buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf3[(bufferOffset * BUFFER_SIZE) + ch + ((SAMPLES_IN_BUFFER - 1 - s) * 8)];	// channels 25-32
				pAudioOutputSamples[ch + 32][(i_buf * SAMPLES_IN_BUFFER) + s] = &audioTxBuf4[(bufferOffset * BUFFER_SIZE) + ch + ((SAMPLES_IN_BUFFER - 1 - s) * 8)];	// channels 33-40

				// pointer-array will have most recent samples at position 0, so we have to rearrange the received samples from DMA-buffer
				pAudioInputSamples[ch     ][(i_buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf0[(bufferOffset * BUFFER_SIZE) + ch + ((SAMPLES_IN_BUFFER - 1 - s) * 8)];	// channels 1-8
				pAudioInputSamples[ch + 8 ][(i_buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf1[(bufferOffset * BUFFER_SIZE) + ch + ((SAMPLES_IN_BUFFER - 1 - s) * 8)];	// channels 9-16
				pAudioInputSamples[ch + 16][(i_buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf2[(bufferOffset * BUFFER_SIZE) + ch + ((SAMPLES_IN_BUFFER - 1 - s) * 8)];	// channels 17-24
				pAudioInputSamples[ch + 24][(i_buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf3[(bufferOffset * BUFFER_SIZE) + ch + ((SAMPLES_IN_BUFFER - 1 - s) * 8)];	// channels 25-32
				pAudioInputSamples[ch + 32][(i_buf * SAMPLES_IN_BUFFER) + s] = &audioRxBuf4[(bufferOffset * BUFFER_SIZE) + ch + ((SAMPLES_IN_BUFFER - 1 - s) * 8)];	// channels 33-40
			}
		}
	}
}

void audioInit(void) {
	// init TCB-array with multi-buffering
	for (int i_buf = 0; i_buf < BUFFER_COUNT; i_buf++) {
		// check if we have to direct to first buffer again
		int nextBuffer;
		if (i_buf == (BUFFER_COUNT - 1)) {
			// jump back to first buffer
			nextBuffer = 0;
		}else{
			// direct to subsequent buffer
			nextBuffer = i_buf + 1;
		}

		for (int i_tcb = 0; i_tcb < 8; i_tcb++) {
			// direct DMA-chain-controller to the subsequent buffer
			audioTx_tcb[i_tcb][i_buf][0] = ((int)&audioTx_tcb[i_tcb][nextBuffer][0] + 3);
			// tell DMA-controller the size of our buffers
			audioTx_tcb[i_tcb][i_buf][1] = BUFFER_SIZE;
			// set modification-value to 1
			audioTx_tcb[i_tcb][i_buf][2] = 1;

			// direct DMA-chain-controller to the subsequent buffer
			audioRx_tcb[i_tcb][i_buf][0] = ((int)&audioRx_tcb[i_tcb][nextBuffer][0] + 3);
			// tell DMA-controller the size of our buffers
			audioRx_tcb[i_tcb][i_buf][1] = BUFFER_SIZE;
			// set modification-value to 1
			audioRx_tcb[i_tcb][i_buf][2] = 1;
		}

		// pointer to the buffer
		audioTx_tcb[0][i_buf][3] = (int)&audioTxBuf0[i_buf * BUFFER_SIZE];
		audioTx_tcb[1][i_buf][3] = (int)&audioTxBuf1[i_buf * BUFFER_SIZE];
		audioTx_tcb[2][i_buf][3] = (int)&audioTxBuf2[i_buf * BUFFER_SIZE];
		audioTx_tcb[3][i_buf][3] = (int)&audioTxBuf3[i_buf * BUFFER_SIZE];
		audioTx_tcb[4][i_buf][3] = (int)&audioTxBuf4[i_buf * BUFFER_SIZE];

		audioRx_tcb[0][i_buf][3] = (int)&audioRxBuf0[i_buf * BUFFER_SIZE];
		audioRx_tcb[1][i_buf][3] = (int)&audioRxBuf1[i_buf * BUFFER_SIZE];
		audioRx_tcb[2][i_buf][3] = (int)&audioRxBuf2[i_buf * BUFFER_SIZE];
		audioRx_tcb[3][i_buf][3] = (int)&audioRxBuf3[i_buf * BUFFER_SIZE];
		audioRx_tcb[4][i_buf][3] = (int)&audioRxBuf4[i_buf * BUFFER_SIZE];
	}

	audioUpdatePointerArray();

	// ============== FOR TESTING ONLY ==============
	/*
	// fill TDM-buffer with sinewave-samples with increasing frequency between 1kHz and 8kHz
	float omega = 2.0f * PI * 1000.0f / SAMPLERATE; // w = 2*pi*f between 1kHz and 8kHz
	for (int ch = 1; ch < 8; ch++) {
		for (int s = 0; (s < (BUFFER_COUNT * SAMPLES_IN_BUFFER)); s++) {
			*pAudioOutputSamples[ch     ][s] = sin(omega * (float)(ch + 1) * (float)s) * 268435456; // TDM8 is using 32-bit values, so lets use 28-bit as peak-value (-18dBfs)
			*pAudioOutputSamples[ch + 8 ][s] = sin(omega * (float)(ch + 1) * (float)s) * 268435456;
			*pAudioOutputSamples[ch + 16][s] = sin(omega * (float)(ch + 1) * (float)s) * 268435456;
			*pAudioOutputSamples[ch + 24][s] = sin(omega * (float)(ch + 1) * (float)s) * 268435456;
			*pAudioOutputSamples[ch + 32][s] = sin(omega * (float)(ch + 1) * (float)s) * 268435456;
		}
	}
	*/
	// ============== FOR TESTING ONLY ==============
}

void audioProcessData(void) {
	audioReady = 0; // clear global flag that audio is not ready anymore
	audioProcessing = 1; // set global flag that we are processing now
	audioUpdatePointerArray();

	// do something with the received samples
/*
	// TEST1: copy RAW input data to RAW output buffer (pass-through)
	memcpy(&audioTxBuf0[0], &audioRxBuf0[0], sizeof(audioRxBuf0));
	memcpy(&audioTxBuf1[0], &audioRxBuf1[0], sizeof(audioRxBuf1));
	memcpy(&audioTxBuf2[0], &audioRxBuf2[0], sizeof(audioRxBuf2));
	memcpy(&audioTxBuf3[0], &audioRxBuf3[0], sizeof(audioRxBuf3));
	memcpy(&audioTxBuf4[0], &audioRxBuf4[0], sizeof(audioRxBuf4));
*/

	/*
	// TEST2: copy input samples to output with decreasing volume per 8 channels
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
	// TEST3: copy individual samples
	for (int i = 0; i < (OUTPUT_BUFFER_COUNT * BUFFER_SIZE); i++) {
		audioTxBuf0[i] = audioRxBuf0[i];
	}
	*/

	// TEST4: copy received first 8 channels to all DSP outputs for the next TDM8-transmission
	/*
	for (int ch = 1; ch < 8; ch++) {
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			*pAudioOutputSamples[ch   ][s] = *pAudioInputSamples[ch][s];
			*pAudioOutputSamples[ch+8 ][s] = *pAudioInputSamples[ch][s];
			*pAudioOutputSamples[ch+16][s] = *pAudioInputSamples[ch][s];
			*pAudioOutputSamples[ch+24][s] = *pAudioInputSamples[ch][s];
			*pAudioOutputSamples[ch+32][s] = *pAudioInputSamples[ch][s];
		}
	}
	*/

	// we are receiving "SAMPLES_IN_BUFFER" samples at once
	// as we have to calculate from oldest sample to newest, we have to start at end of currently received buffer
	for (int s = (SAMPLES_IN_BUFFER - 1); s >= 0; s--) {
		fxBiquad(s, pAudioInputSamples[0], pAudioOutputSamples[0], openx32.channel[0].peq.a, openx32.channel[0].peq.b); // only calculate for channel 1
	}

	// increment buffer-counter for next call
    audioBufferCounter++;
    if (audioBufferCounter >= BUFFER_COUNT) {
    	audioBufferCounter = 0;
    }

	audioProcessing = 0; // clear global flag that processing is done
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
