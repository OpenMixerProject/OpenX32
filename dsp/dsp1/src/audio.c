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
//section("seg_ext_data")
int audioTxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch1-8 | Ch9-16 | P16 Ch 1-8 | P16 Ch 9-16 | AUX Ch 1-8
//section("seg_ext_data")
int audioRxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch1-8 | Ch9-16 | Ch 17-24 | Ch 25-32 | AUX Ch 1-8

// TCB-arrays for SPORT {CPSPx Chainpointer, ICSPx Internal Count, IMSPx Internal Modifier, IISPx Internal Index}
// TCB-arrays for SPORT {pointer to pointer to buffer, buffer-size, ???, pointer to buffer}
int audioTx_tcb[8][BUFFER_COUNT][4];
int audioRx_tcb[8][BUFFER_COUNT][4];

void audioInit(void) {
	// init TCB-array with multi-buffering
	int nextBuffer;
	for (int i_buf = 0; i_buf < BUFFER_COUNT; i_buf++) {
		// calc index of next buffer with wrap around at the end
		nextBuffer = (i_buf + 1) % BUFFER_COUNT;

		for (int i_tcb = 0; i_tcb < TDM_INPUTS; i_tcb++) {
			// direct DMA-chain-controller to the subsequent buffer
			audioTx_tcb[i_tcb][i_buf][0] = ((int)&audioTx_tcb[i_tcb][nextBuffer][0] + 3);
			// tell DMA-controller the size of our buffers
			audioTx_tcb[i_tcb][i_buf][1] = BUFFER_SIZE;
			// set modification-value to 1
			audioTx_tcb[i_tcb][i_buf][2] = 1;
			// assign pointer to the Tx-Buffer to tcb
			audioTx_tcb[i_tcb][i_buf][3] = (int)&audioTxBuf[(BUFFER_COUNT * BUFFER_SIZE * i_tcb) + (BUFFER_SIZE * i_buf)];

			// direct DMA-chain-controller to the subsequent buffer
			audioRx_tcb[i_tcb][i_buf][0] = ((int)&audioRx_tcb[i_tcb][nextBuffer][0] + 3);
			// tell DMA-controller the size of our buffers
			audioRx_tcb[i_tcb][i_buf][1] = BUFFER_SIZE;
			// set modification-value to 1
			audioRx_tcb[i_tcb][i_buf][2] = 1;
			// assign pointer to the Tx-Buffer to tcb
			audioRx_tcb[i_tcb][i_buf][3] = (int)&audioRxBuf[(BUFFER_COUNT * BUFFER_SIZE * i_tcb) + (BUFFER_SIZE * i_buf)];
		}
	}

	// ============== FOR TESTING ONLY ==============
	/*
	// fill TDM-buffer with sinewave-samples with increasing frequency between 1kHz and 8kHz
	int bufferTdmIndex;
	int bufferSampleIndex;
	int bufferIndex;
	float omega = 2.0f * PI * 1000.0f / openx32.samplerate; // w = 2*pi*f between 1kHz and 8kHz
	for (int i_tdm = 0; i_tdm < 1; i_tdm++) {
		bufferTdmIndex = (BUFFER_COUNT * BUFFER_SIZE * i_tdm) + (BUFFER_SIZE * audioBufferCounter);

		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			bufferSampleIndex = bufferTdmIndex + (CHANNELS_PER_TDM * s);

			for (int ch = 0; ch < 8; ch++) {
				// calculate bufferOffset with wrap-around the end of the buffer
				bufferIndex = bufferSampleIndex + ch;

				audioTxBuf[bufferIndex] = sin(omega * (float)(ch + 1) * (float)(s + (SAMPLES_IN_BUFFER * audioBufferCounter))) * 268435456; // TDM8 is using 32-bit values, so lets use 28-bit as peak-value (-18dBfs)
			}
		}
	}
	*/
	// ============== FOR TESTING ONLY ==============
}

void audioProcessData(void) {
	audioProcessing = 1; // set global flag that we are processing now

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
	int bufferTdmIndex;
	int bufferSampleIndex;
	int bufferIndex;

	// now iterate through all channels and all samples
	// 24 channels are the hard limit at the moment - on some occasions it will not be able to process all data until next buffer
	// so stay at 8 channels for now
	//for (int i_tdm = 0; i_tdm < TDM_INPUTS; i_tdm++) {
	for (int i_tdm = 0; i_tdm < 1; i_tdm++) {
		bufferTdmIndex = (BUFFER_COUNT * BUFFER_SIZE * i_tdm) + (BUFFER_SIZE * audioBufferCounter);

		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			bufferSampleIndex = bufferTdmIndex + (CHANNELS_PER_TDM * s);

			for (int ch = 0; ch < 8; ch++) {
				// calculate bufferOffset with wrap-around the end of the buffer
				bufferIndex = bufferSampleIndex + ch;

				// every sample will be processed in the following order:
				// input -> Noisegate -> EQ1 -> EQ2 -> EQ3 -> EQ4 -> EQ5 -> Compressor -> output

				audioProcessedSample = audioRxBuf[bufferIndex];

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
				audioProcessedSample *= openx32.channel[ch].volume;

				// limit audio to peak-values of 32-bit (TDM8). X32 will process "only" 24-bits and ignores the lower 8-bits
				if (audioProcessedSample > 2147483648) {
					audioProcessedSample = 2147483648;
				}else if (audioProcessedSample < -2147483648) {
					audioProcessedSample = -2147483648;
				}

				// write processed audio to output directly as we have no "main-bus" at the moment
				// DSP-input -> processing DSP-output
				audioTxBuf[bufferIndex] = audioProcessedSample;
			}
		}
	}

	// increment buffer-counter for next call
    audioBufferCounter++;
    if (audioBufferCounter >= BUFFER_COUNT) {
    	audioBufferCounter = 0;
    }

	audioReady = 0; // clear global flag that audio is not ready anymore
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
