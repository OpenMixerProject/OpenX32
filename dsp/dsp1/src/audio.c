#include "audio.h"
#include "system.h"
#include "fx.h"

/*
	Used Audio-Mapping:
	========================
	SPORT0A   -> TDM OUT0
	SPORT0B   -> TDM OUT1
	SPORT2A   -> TDM OUT2
	SPORT2B   -> TDM OUT3
	SPORT4A   -> TDM OUTAUX

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
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
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

		// assign pointer to the buffer to tcb
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

	// ============== FOR TESTING ONLY ==============
	/*
	// fill TDM-buffer with sinewave-samples with increasing frequency between 1kHz and 8kHz
	audioUpdatePointerArray(); // we need initialized pointer-array to access buffer
	float omega = 2.0f * PI * 1000.0f / openx32.samplerate; // w = 2*pi*f between 1kHz and 8kHz
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

	// process the received samples sample by sample
	// the ADSP-21371 has support for SIMD. The compile should detect what we are doing here and
	// hopefully process the data using SingleInstructionMultipleData to optimize our processing
	//
	// Following steps are processed for all channels
	// 1. Noisegate
	// 2. 5-band EQ
	// 3. Compressor
	// 4. Sends to Mix-Busses
	// 5. Volume-Control to Main L/R

	float audioProcessedSample;
	// iterate through all channels
	//for (int ch = 0; ch < MAX_CHAN; ch++) {
	for (int ch = 0; ch < 8; ch++) {
		// we have to calculate from oldest sample to newest, so we have to start at end of currently received buffer
		for (int s = (SAMPLES_IN_BUFFER - 1); s >= 0; s--) {
			// every sample will be processed in the following order:
			// input -> Noisegate -> EQ1 -> EQ2 -> EQ3 -> EQ4 -> EQ5 -> Compressor -> output

			audioProcessedSample = (float)*pAudioInputSamples[ch][s];

			// process noisegate
			audioProcessedSample = fxProcessGate(audioProcessedSample, &openx32.channel[ch].gate);

			// process all EQs subsequently
			for (int i_peq = 0; i_peq < openx32.channel[ch].peqMax; i_peq++) {
				audioProcessedSample = fxProcessEq(audioProcessedSample, &openx32.channel[ch].peq[i_peq]);
			}

			// process compressor
			audioProcessedSample = fxProcessCompressor(audioProcessedSample, &openx32.channel[ch].compressor);

			// process sends
			// TODO

			// process channel-volume
			// convert dB into linear value and then process audio
			audioProcessedSample *= openx32.channel[ch].value_volume;

			// limit audio to peak-values of 32-bit (TDM8). X32 will process "only" 24-bits and ignores the lower 8-bits
			if (audioProcessedSample > 2147483648) {
				audioProcessedSample = 2147483648;
			}else if (audioProcessedSample < -2147483648) {
				audioProcessedSample = -2147483648;
			}

			// write processed audio to output directly as we have no "main-bus" at the moment
			// DSP-input -> processing DSP-output
			*pAudioOutputSamples[ch][s] = (int)audioProcessedSample;
		}
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
    if (audioIsrCounter >= (openx32.samplerate / SAMPLES_IN_BUFFER)) {
    	audioIsrCounter = 0;
    }
}
