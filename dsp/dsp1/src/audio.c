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
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

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

// pointers to individual audio-samples for more convenient access
//int* pAudioOutputSamples[40][BUFFER_COUNT * SAMPLES_IN_BUFFER]; // sorted pointer-array that points to the value for the next write cycle
//int* pAudioInputSamples[40][BUFFER_COUNT * SAMPLES_IN_BUFFER]; // sorted pointer-array that points to the recently received audio-samples

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
	// using a samplerate of 48kHz and knowing the peak-perfomance of the ADSP-21371 at 1.596 GFLOPS
	// as we are receiving 16 samples per DMA-transfer, we receive new data with 48,000Hz / 16 = 3,000 Hz
	// we have 1,596,000,000 / 3,000 = 532,000 FLOP available until next samples will arrive via DMA
	// we are processing 16 samples for 40 channels, which means we have 532,000 / (16 * 40) = 831 FLOP per channel available
	//
	// Following steps are processed for all channels
	// 1. Noisegate (3 multiplications, 2 sums -> 5 FLOP)
	// 2. 5-band EQ (5 multiplications, 4 sums -> 9 FLOP)
	// 3. Compressor (4 multiplications, 4 sums, 2 divisions -> at least 10 FLOP, maybe more)
	// 4. Sends to Mix-Busses (not implemented yet)
	// 5. Volume-Control to Main L/R and Sub (3 multiplications, 3 sums -> 6 FLOP)
	// =========================================================================================
	// 30 FLOP per channel * 40 channels = 1,200 FLOP in total

	float audioProcessedSample;
	float mainDebug;
	float mainLeft;
	float mainRight;
	float mainSub;
	int dspCh;
	int bufferTdmIndex;
	int bufferSampleIndex;
	int bufferReadIndex;

	// now iterate through all channels and all samples
	// 24 channels are the hard limit at the moment - on some occasions it will not be able to process all data until next buffer
	// so stay at 8 channels for now
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = (BUFFER_SIZE * audioBufferCounter) + (CHANNELS_PER_TDM * s); // (select correct buffer 0 or 1) + (sample-offset)

		mainLeft = 0.0f;
		mainRight = 0.0f;
		mainSub = 0.0f;
		mainDebug = 0.0f;

		//for (int i_tdm = 0; i_tdm < TDM_INPUTS; i_tdm++) {
		for (int i_tdm = 0; i_tdm < 1; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + (BUFFER_COUNT * BUFFER_SIZE * i_tdm);

			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) { // TODO: this loop could be optimized using SIMD as we are using the same operators on multiple data
				// calculate dspChannel
				dspCh = (CHANNELS_PER_TDM * i_tdm) + i_ch;

				// calculate bufferOffset with wrap-around the end of the buffer
				bufferReadIndex = (bufferTdmIndex + i_ch) % (TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE);

				// every sample will be processed in the following order:
				// input -> Noisegate -> EQ1 -> EQ2 -> EQ3 -> EQ4 -> EQ5 -> Compressor -> output

				audioProcessedSample = audioRxBuf[bufferReadIndex];
				mainDebug += (audioRxBuf[bufferReadIndex]); // direct-through signal on output 1

				// process noisegate
				audioProcessedSample = fxProcessGate(audioProcessedSample, &dsp.dspChannel[dspCh].gate);

				// process all EQs subsequently
				for (int i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++) {
					audioProcessedSample = fxProcessEq(audioProcessedSample, &dsp.dspChannel[dspCh].peq[i_peq]);
				}

				// process compressor
				audioProcessedSample = fxProcessCompressor(audioProcessedSample, &dsp.dspChannel[dspCh].compressor);

				// process sends
				// TODO

/*
				// process channel-volume
				// convert dB into linear value and then process audio
				audioProcessedSample *= dsp.dspChannel[dspCh].volumeLeft;

				// limit audio to peak-values of 32-bit (TDM8). X32 will process "only" 24-bits and ignores the lower 8-bits
				if (audioProcessedSample > 2147483648) {
					audioProcessedSample = 2147483648;
				}else if (audioProcessedSample < -2147483648) {
					audioProcessedSample = -2147483648;
				}

				// write processed audio to output directly (1:1 routing)
				// DSP-input -> processing DSP-output
				audioTxBuf[bufferReadIndex] = audioProcessedSample;
*/

				// main-audio-bus
				mainLeft  += (audioProcessedSample * dsp.dspChannel[dspCh].volumeLeft);
				mainRight += (audioProcessedSample * dsp.dspChannel[dspCh].volumeRight);
				mainSub   += (audioProcessedSample * dsp.dspChannel[dspCh].volumeSub);
			}
		}

		// all channels of this sample processed -> write summarized data to outputs
		// Ch01-08: Output 1-8: Out1 = MainL, Out2 = MainR, Out3 = Sub
		// Ch09-17: Output 9-16
		// Ch17-24: UltraNet 1-8
		// Ch25-32: UltraNet 9-16
		// Ch33-40: AUX 1-6 / MonitorL/R

		// copy mainDebug to TDM0, ch0 for direct passthrough-test
		// copy mainLeft to TDM0, ch1
		// copy mainRight to TDM0, ch2
		// copy mainSub to TDM0, ch3
		audioTxBuf[bufferSampleIndex] = mainDebug;
		audioTxBuf[bufferSampleIndex + 1] = mainLeft;
		audioTxBuf[bufferSampleIndex + 2] = mainRight;
		audioTxBuf[bufferSampleIndex + 3] = mainSub;
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
    	// this is not nice but without a debugger and profiling tools this is the easiest method to check if the algorithms are within the timing
    	systemCrash();
    }

    audioReady = 1; // set flag, that we have new data to process

    audioIsrCounter++;
    if (audioIsrCounter >= (dsp.samplerate / SAMPLES_IN_BUFFER)) {
    	audioIsrCounter = 0;
    }
}
