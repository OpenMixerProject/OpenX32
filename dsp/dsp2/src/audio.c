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
	SPORT0A   -> TDM FX OUT0
	SPORT0B   -> TDM FX OUT1
	SPORT2A   -> TDM FX PLAY OUT
	SPORT6A	  -> SPDIF OUT

	TDM FX IN0 -> SPORT1A
	TDM FX IN1 -> SPORT1B
	TDM REC IN -> SPORT3A
*/

volatile int audioReady = 0;
volatile int audioProcessing = 0;
int audioBufferOffset = 0;

// audio-buffers for transmitting and receiving
// 16 Audiosamples per channel (= 333us latency)
int audioRxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // FX IN 0 1-8 | FX IN 1 1-8 | REC IN 1-8
int audioTxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // FX OUT 0 1-8 | FX OUT 1 1-8 | PLAY OUT 1-8

// internal buffers for audio-samples
float audioBuffer[5][MAX_CHAN][SAMPLES_IN_BUFFER]; // audioBuffer[TAPPOINT][CHANNEL][SAMPLE]
float audioTempBuffer[SAMPLES_IN_BUFFER] = {0};
float audioTempBufferLarge[MAX_CHAN][SAMPLES_IN_BUFFER] = {0};
float audioTempBufferChan[MAX_CHAN] = {0};

// TCB-arrays for SPORT {CPSPx Chainpointer, ICSPx Internal Count, IMSPx Internal Modifier, IISPx Internal Index}
int audioRx_tcb[4][BUFFER_COUNT][4];
int audioTx_tcb[4][BUFFER_COUNT][4];

float time = 0;

void audioInit(void) {
	// initialize TCB-array with multi-buffering
	int nextBuffer;
	for (int i_buf = 0; i_buf < BUFFER_COUNT; i_buf++) {
		// calc index of next buffer with wrap around at the end
		nextBuffer = (i_buf + 1) % BUFFER_COUNT;

		for (int i_tcb = 0; i_tcb < TDM_INPUTS; i_tcb++) {
			// direct DMA-chain-controller to the subsequent buffer
			audioRx_tcb[i_tcb][i_buf][0] = ((int)&audioRx_tcb[i_tcb][nextBuffer][0] + 3);
			// tell DMA-controller the size of our buffers
			audioRx_tcb[i_tcb][i_buf][1] = BUFFER_SIZE;
			// set modification-value to 1
			audioRx_tcb[i_tcb][i_buf][2] = 1;
			// assign pointer to the Tx-Buffer to tcb
			audioRx_tcb[i_tcb][i_buf][3] = (int)&audioRxBuf[(BUFFER_COUNT * BUFFER_SIZE * i_tcb) + (BUFFER_SIZE * i_buf)];

			// direct DMA-chain-controller to the subsequent buffer
			audioTx_tcb[i_tcb][i_buf][0] = ((int)&audioTx_tcb[i_tcb][nextBuffer][0] + 3);
			// tell DMA-controller the size of our buffers
			audioTx_tcb[i_tcb][i_buf][1] = BUFFER_SIZE;
			// set modification-value to 1
			audioTx_tcb[i_tcb][i_buf][2] = 1;
			// assign pointer to the Tx-Buffer to tcb
			audioTx_tcb[i_tcb][i_buf][3] = (int)&audioTxBuf[(BUFFER_COUNT * BUFFER_SIZE * i_tcb) + (BUFFER_SIZE * i_buf)];
		}
	}

	// initialize memory
	memset(audioBuffer, 0, sizeof(audioBuffer));

	// initialize channel-parameters
	for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
		dsp.channelFxReturnVolume[i_ch] = 1.0f;
	}
}

void audioProcessData(void) {
	audioProcessing = 1; // set global flag that we are processing now

	int bufferSampleIndex;
	int bufferTdmIndex;
	int dspCh;
	int bufferIndex;
	int sOffset;
	int tdmOffset;
	int tdmBufferOffset;

	//  ____            ___       _            _                  _
	// |  _ \  ___     |_ _|_ __ | |_ ___ _ __| | ___  __ ___   _(_)_ __   __ _
	// | | | |/ _ \_____| || '_ \| __/ _ \ '__| |/ _ \/ _` \ \ / / | '_ \ / _` |
	// | |_| |  __/_____| || | | | ||  __/ |  | |  __/ (_| |\ V /| | | | | (_| |
	// |____/ \___|    |___|_| |_|\__\___|_|  |_|\___|\__,_| \_/ |_|_| |_|\__, |
	//                                                                    |___/
	// copy interleaved DMA input-buffer into channel buffers
	sOffset = 0;
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = audioBufferOffset + sOffset;

		// copy channels from FPGA
		tdmOffset = 0;
		tdmBufferOffset = 0;
		for (int i_tdm = 0; i_tdm < TDM_INPUTS; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// input from FPGA side (we are receiving float data here)
				memcpy(&audioBuffer[TAP_INPUT][dspCh][s], &audioRxBuf[bufferIndex], sizeof(float));
			}

			tdmOffset += CHANNELS_PER_TDM;
			tdmBufferOffset += (BUFFER_COUNT * BUFFER_SIZE);
		}

		sOffset += CHANNELS_PER_TDM;
	}

	// ========================================================

	// PROCESS AUDIO
	// dspCh   0..7 = FX0
	// dspCh  8..15 = FX1
	// dspCh 16..23 = AUX 1-6 + AES/EBU | AUX 1-6 + USB Play

	//				  _____                  _ _
	//				 | ____|__ _ _   _  __ _| (_)_______ _ __
	//				 |  _| / _` | | | |/ _` | | |_  / _ \ '__|
	//				 | |__| (_| | |_| | (_| | | |/ /  __/ |
	//				 |_____\__, |\__,_|\__,_|_|_/___\___|_|
	//				          |_|
	// use low-pass filter on EQ-Coefficients to smoothly change parameters

	// Hardware-Accelerated Biquad-Filter
	// Ressource-Demand: ~20%
	for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
		memcpy(&audioBuffer[TAP_POST_EQ][i_ch][0], &audioBuffer[TAP_INPUT][i_ch][0], SAMPLES_IN_BUFFER * sizeof(float));
		//                 input and output                BiQuad-Coefficients                        Delay-Line                  samples           Sections
		biquad_trans(&audioBuffer[TAP_POST_EQ][i_ch][0], &dsp.dspChannel[i_ch].peqCoeffs[0], &dsp.dspChannel[i_ch].peqStates[0], SAMPLES_IN_BUFFER, MAX_CHAN_EQS);
	}


	// copy TAP_POST_EQ to TAP_PRE_FADER (passthrough)
	for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
		memcpy(&audioBuffer[TAP_PRE_FADER][i_ch][0], &audioBuffer[TAP_POST_EQ][i_ch][0], SAMPLES_IN_BUFFER * sizeof(float));
	}


	// calculate FX Return Volume
	for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
		vecsmltf(&audioBuffer[TAP_PRE_FADER][i_ch][0], dsp.channelFxReturnVolume[i_ch], &audioBuffer[TAP_POST_FADER][i_ch][0], SAMPLES_IN_BUFFER);
	}

	/*
		// insert 1kHz test-audio to all channels
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			time += (1.0f/48000.0f); // add 20.83us
			float signal = sin(2.0f * M_PI * 1000.0f * time) * 268435456.0f;

			for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
				audioBuffer[TAP_POST_FADER][i_ch][s] = signal;
			}
		}
	*/

	// ========================================================

	//  ___       _            _                  _
	// |_ _|_ __ | |_ ___ _ __| | ___  __ ___   _(_)_ __   __ _
	//  | || '_ \| __/ _ \ '__| |/ _ \/ _` \ \ / / | '_ \ / _` |
	//  | || | | | ||  __/ |  | |  __/ (_| |\ V /| | | | | (_| |
	// |___|_| |_|\__\___|_|  |_|\___|\__,_| \_/ |_|_| |_|\__, |
	//                                                    |___/
	// copy channel buffers to interleaved output-buffer
	sOffset = 0;
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = audioBufferOffset + sOffset;

		// copy data for FPGA
		tdmOffset = 0;
		tdmBufferOffset = 0;
		for (int i_tdm = 0; i_tdm < TDM_INPUTS; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// output to FPGA as float
				memcpy(&audioTxBuf[bufferIndex], &audioBuffer[TAP_POST_FADER][dspCh][s], sizeof(float));
			}

			tdmOffset += CHANNELS_PER_TDM;
			tdmBufferOffset += (BUFFER_COUNT * BUFFER_SIZE);
		}

		sOffset += CHANNELS_PER_TDM;
	}

	// increment buffer-counter for next call
	audioBufferOffset += BUFFER_SIZE;
    if (audioBufferOffset >= (BUFFER_SIZE * BUFFER_COUNT)) {
    	audioBufferOffset = 0;
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
}

void audioSpdifTxISR(uint32_t iid, void* handlerarg) {
	*pTXSP6A = 0; // TODO: fill with useful audio-data here
}
