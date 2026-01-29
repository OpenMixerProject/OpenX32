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

// include individual effects
#include "fxBase.h"

// if one of the surround-upmixing-effects is used, the regular FX-Rack
// is not available.
#if FX_USE_UPMIXER == 1
	// this surround-upmixer-effect takes quite a lot ressources so it
	// is not implemented as an effect-class-member of the FX-Rack at the moment
	#include "fxUpmixer.h"
#elif FX_USE_MATRIXUPMIXER == 1
	#include "fxMatrixUpmixer.h"
#else
	#include "fxDemo.h"
	#include "fxReverb.h"
	#include "fxDelay.h"
	#include "fxChorus.h"
	#include "fxOverdrive.h"
	#include "fxTransientshaper.h"
	#include "fxMultibandCompressor.h"
	#include "fxDynamicEQ.h"
#endif

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

#pragma file_attr("prefersMem=internal") // let the linker know, that all variables should be placed into the internal ram

volatile int audioReady = 0;
volatile int audioProcessing = 0;
volatile int spdifSamplePointer = 0;
volatile bool spdifLeftChannel = true;
int audioBufferOffset = 0;

// audio-buffers for transmitting and receiving
// 16 Audiosamples per channel (= 333us latency)
int pm audioRxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // FX IN 0 1-8 | FX IN 1 1-8 | REC IN 1-8
int pm audioTxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // FX OUT 0 1-8 | FX OUT 1 1-8 | PLAY OUT 1-8

// internal buffers for audio-samples
float pm audioBuffer[5][MAX_CHAN][SAMPLES_IN_BUFFER]; // audioBuffer[TAPPOINT][CHANNEL][SAMPLE]

// TCB-arrays for SPORT {CPSPx Chainpointer, ICSPx Internal Count, IMSPx Internal Modifier, IISPx Internal Index}
int pm audioRx_tcb[4][BUFFER_COUNT][4];
int pm audioTx_tcb[4][BUFFER_COUNT][4];

// the next array is our FX-delay-memory in the external SDRAM. It is a dummy-array to display the audio-memory as "used"
// each individual effect has full control of its memory area and can create smaller sub-arrays within this memory-area
//float am fxDelayLine[8][2][SDRAM_AUDIO_SIZE_BYTE / (8 * 2 * 4)]; // 8 FX-Slots, 2 Channels, 229000 samples = 4.77 seconds

float time = 0;

// definitions for effect-rack
#if (FX_USE_UPMIXER == 1)
	float* fxInBuf[2];
	float* fxOutSurroundBuf[6];
#elif (FX_USE_MATRIXUPMIXER == 1)
	fx* fxSlots[1] = {0};
	float* fxInBuf[2];
	float* fxOutSurroundBuf[6];
#else
	fx* fxSlots[8] = {0};
	float* fxInBuf[8][2];
	float* fxOutBuf[8][2];
#endif

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
	//fxDelayLine[0][0][0] = 0.0f; // dummy write to memory-area to prevent removing by compiler/linker

	// initialize effects-rack
	#if (FX_USE_MATRIXUPMIXER == 1) || (FX_USE_UPMIXER == 1)
		fxInBuf[0] = &audioBuffer[TAP_INPUT][0][0];
		fxInBuf[1] = &audioBuffer[TAP_INPUT][1][0];

		fxOutSurroundBuf[0] = &audioBuffer[TAP_OUTPUT][0][0];
		fxOutSurroundBuf[1] = &audioBuffer[TAP_OUTPUT][1][0];
		fxOutSurroundBuf[2] = &audioBuffer[TAP_OUTPUT][2][0];
		fxOutSurroundBuf[3] = &audioBuffer[TAP_OUTPUT][3][0];
		fxOutSurroundBuf[4] = &audioBuffer[TAP_OUTPUT][4][0];
		fxOutSurroundBuf[5] = &audioBuffer[TAP_OUTPUT][5][0];
	#else
		// route first two inputs to all effect-inputs
		fxInBuf[0][0] = &audioBuffer[TAP_INPUT][0][0];
		fxInBuf[0][1] = &audioBuffer[TAP_INPUT][1][0];
		fxInBuf[1][0] = &audioBuffer[TAP_INPUT][0][0];
		fxInBuf[1][1] = &audioBuffer[TAP_INPUT][1][0];
		fxInBuf[2][0] = &audioBuffer[TAP_INPUT][0][0];
		fxInBuf[2][1] = &audioBuffer[TAP_INPUT][1][0];
		fxInBuf[3][0] = &audioBuffer[TAP_INPUT][0][0];
		fxInBuf[3][1] = &audioBuffer[TAP_INPUT][1][0];
		fxInBuf[4][0] = &audioBuffer[TAP_INPUT][0][0];
		fxInBuf[4][1] = &audioBuffer[TAP_INPUT][1][0];
		fxInBuf[5][0] = &audioBuffer[TAP_INPUT][0][0];
		fxInBuf[5][1] = &audioBuffer[TAP_INPUT][1][0];
		fxInBuf[6][0] = &audioBuffer[TAP_INPUT][0][0];
		fxInBuf[6][1] = &audioBuffer[TAP_INPUT][1][0];
		fxInBuf[7][0] = &audioBuffer[TAP_INPUT][0][0];
		fxInBuf[7][1] = &audioBuffer[TAP_INPUT][1][0];

		// route each effect-output to a dedicated DSP2 output
		fxOutBuf[0][0] = &audioBuffer[TAP_OUTPUT][0][0];
		fxOutBuf[0][1] = &audioBuffer[TAP_OUTPUT][1][0];
		fxOutBuf[1][0] = &audioBuffer[TAP_OUTPUT][2][0];
		fxOutBuf[1][1] = &audioBuffer[TAP_OUTPUT][3][0];
		fxOutBuf[2][0] = &audioBuffer[TAP_OUTPUT][4][0];
		fxOutBuf[2][1] = &audioBuffer[TAP_OUTPUT][5][0];
		fxOutBuf[3][0] = &audioBuffer[TAP_OUTPUT][6][0];
		fxOutBuf[3][1] = &audioBuffer[TAP_OUTPUT][7][0];
		fxOutBuf[4][0] = &audioBuffer[TAP_OUTPUT][8][0];
		fxOutBuf[4][1] = &audioBuffer[TAP_OUTPUT][9][0];
		fxOutBuf[5][0] = &audioBuffer[TAP_OUTPUT][10][0];
		fxOutBuf[5][1] = &audioBuffer[TAP_OUTPUT][11][0];
		fxOutBuf[6][0] = &audioBuffer[TAP_OUTPUT][12][0];
		fxOutBuf[6][1] = &audioBuffer[TAP_OUTPUT][13][0];
		fxOutBuf[7][0] = &audioBuffer[TAP_OUTPUT][14][0];
		fxOutBuf[7][1] = &audioBuffer[TAP_OUTPUT][15][0];
	#endif

	// setup the fx-rack
	#if (FX_USE_UPMIXER == 1)
		// initialize surround upmixing using a stereo-decompositing upmixer
		fxUpmixerInit();
	#elif (FX_USE_MATRIXUPMIXER == 1)
		// surround upmixing using a simple matrix-upmixer
		fxSlots[0] = new fxMatrixUpmixer(0, 6); // EffectSlot #0, 5.1 Surround, Ch 1-6
	#else
		// regular stereo-effects

		/*
			==============================================
			Effect		| DSP-Load [%]	| Program-Memory |
			------------|---------------|-----------------
			Reverb		| 18 %			|				 |
			Chorus		| 5 %			|				 |
			TransShapr	| 2.5 %			|				 |
			Delay		| 1.5 %			|				 |
			Overdrive	| 2 %			|				 |
			MltBndComp	| 6 %			|				 |
			==============================================
		*/

		audioFxChangeSlot(0, 0, 2); // install reverb on slot 0
		audioFxChangeSlot(1, 1, 2); // install chorus on slot 1
		audioFxChangeSlot(2, 2, 2); // install transientshaper on slot 2
		audioFxChangeSlot(3, 3, 2); // install overdrive on slot 3
		audioFxChangeSlot(4, 4, 2); // install delay on slot 4
		audioFxChangeSlot(5, 4, 2); // install delay on slot 5
		audioFxChangeSlot(6, 6, 2); // install DynamicEQ on slot 6
		audioFxChangeSlot(7, 7, 2); // install demo-FX on slot 7
	#endif
}

void audioFxData(int fxSlot, float* data, int len) {
	#if (FX_USE_UPMIXER == 0) && (FX_USE_MATRIXUPMIXER == 0)
		// passthrough data to desired fx-slot
		if (fxSlots[fxSlot] != 0) {
			fxSlots[fxSlot]->rxData(data, len);
		}
	#endif
}

void audioFxChangeSlot(int fxSlot, int newFxId, int channelMode) {
	#if (FX_USE_UPMIXER == 0) && (FX_USE_MATRIXUPMIXER == 0)
		if (channelMode < 1) return;
		if (channelMode > 2) return;
		if (fxSlot < 0) return;
		if (fxSlot > 8) return;

		// delete old effect
		if (fxSlots[fxSlot] != 0) {
			delete fxSlots[fxSlot];
			fxSlots[fxSlot] = 0;
		}

		// allocate memory for class in non-standard heap
		//fxSlots[fxSlot] = (fxReverb*)heap_malloc(0, sizeof(fxReverb)); // 0 = heapID

		// set new effect
		switch (newFxId) {
			case 0:
				fxSlots[fxSlot] = new fxReverb(fxSlot, channelMode);
				break;
			case 1:
				fxSlots[fxSlot] = new fxChorus(fxSlot, channelMode);
				break;
			case 2:
				fxSlots[fxSlot] = new fxTransientshaper(fxSlot, channelMode);
				break;
			case 3:
				fxSlots[fxSlot] = new fxOverdrive(fxSlot, channelMode);
				break;
			case 4:
				fxSlots[fxSlot] = new fxDelay(fxSlot, channelMode);
				break;
			case 5:
				fxSlots[fxSlot] = new fxMultibandCompressor(fxSlot, channelMode);
				break;
			case 6:
				fxSlots[fxSlot] = new fxDynamicEQ(fxSlot, channelMode);
				break;
			case 7:
				fxSlots[fxSlot] = new fxDemo(fxSlot, channelMode);
				break;
			default:
				break;
		}
	#endif
}

void audioProcessData(void) {
	audioProcessing = 1; // set global flag that we are processing now

	int bufferSampleIndex;
	int bufferTdmIndex;
	int dspCh;
	int bufferIndex;
	int sampleOffset;
	int tdmOffset;
	int tdmBufferOffset;

	//  ____            ___       _            _                  _
	// |  _ \  ___     |_ _|_ __ | |_ ___ _ __| | ___  __ ___   _(_)_ __   __ _
	// | | | |/ _ \_____| || '_ \| __/ _ \ '__| |/ _ \/ _` \ \ / / | '_ \ / _` |
	// | |_| |  __/_____| || | | | ||  __/ |  | |  __/ (_| |\ V /| | | | | (_| |
	// |____/ \___|    |___|_| |_|\__\___|_|  |_|\___|\__,_| \_/ |_|_| |_|\__, |
	//                                                                    |___/
	// copy interleaved DMA input-buffer into channel buffers
	sampleOffset = 0;
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = audioBufferOffset + sampleOffset;

		// copy channels from DSP1
		tdmOffset = 0;
		tdmBufferOffset = 0;
		for (int i_tdm = 0; i_tdm < TDM_INPUTS; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// input from DSP1 side (we are receiving float data here)
				memcpy(&audioBuffer[TAP_INPUT][dspCh][s], &audioRxBuf[bufferIndex], sizeof(float));
			}

			tdmOffset += CHANNELS_PER_TDM;
			tdmBufferOffset += (BUFFER_COUNT * BUFFER_SIZE);
		}

		sampleOffset += CHANNELS_PER_TDM;
	}

	// reset SPDIF-read-pointer to zero as we have new data in the buffer
	spdifSamplePointer = 0;
	spdifLeftChannel = true;

	// ========================================================

	// PROCESS AUDIO
	// dspCh   0..7 = FX0
	// dspCh  8..15 = FX1
	// dspCh 16..23 = AUX 1-6 + AES/EBU | AUX 1-6 + USB Play


	// DEBUG: copy input-channel 1 to DSP2 AuxOut 1
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		audioBuffer[TAP_OUTPUT][16][s] = audioBuffer[TAP_INPUT][0][s]; // 0..7 = Ch1-8, 8..15 = Ch9-16, 16..23 = AuxCh1-8
	}


	//	 _______  __     ____            _
	//	|  ___\ \/ /    |  _ \ __ _  ___| | __
	//	| |_   \  /_____| |_) / _` |/ __| |/ /
	//	|  _|  /  \_____|  _ < (_| | (__|   <
	//	|_|   /_/\_\    |_| \_\__,_|\___|_|\_\
	//
	// process all effects (if fx-slot is occupied)

	#if FX_USE_UPMIXER == 1
		// perform stereo-decompositing and 5.1 upmixing
		fxUpmixerProcess(&fxInBuf[0], &fxOutSurroundBuf[0]);
	#elif (FX_USE_MATRIXUPMIXER == 1)
		// process a 5.1 surround-effect
		fxSlots[0]->process(&fxInBuf[0], &fxOutSurroundBuf[0]);
	#else
		for (int i = 0; i < 8; i++) {
			if (fxSlots[i] != 0) {
				fxSlots[i]->process(&fxInBuf[i][0], &fxOutBuf[i][0]);
			}
		}
	#endif






	/*
		// copy TAP_POST_EQ to TAP_PRE_FADER (passthrough)
		for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
			memcpy(&audioBuffer[TAP_PRE_FADER][i_ch][0], &audioBuffer[TAP_INPUT][i_ch][0], SAMPLES_IN_BUFFER * sizeof(float));
		}

		// calculate FX Return Volume
		for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
			vecsmltf(&audioBuffer[TAP_PRE_FADER][i_ch][0], dsp.channelFxReturnVolume[i_ch], &audioBuffer[TAP_POST_FADER][i_ch][0], SAMPLES_IN_BUFFER);
		}
	*/
	/*
		// insert sinewave-audio to all channels with increasing frequency starting at 200Hz and ending at 2.5kHz
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			time += (1.0f/48000.0f); // add 20.83us
			if (time > 10) {
				// make sure that the float-value is not losing resolution by overflowing
				time = 0;
			}

			for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
				// create sinewave between 200 Hz and 2500 Hz
				//audioBuffer[TAP_POST_FADER][i_ch][s] = sin(2.0f * M_PI * (200.0f + (float)i_ch * 100.0f) * time) * 2147483648.0f; // scaled as 2^31 (results in 0dBfs)
				audioBuffer[TAP_POST_FADER][i_ch][s] = sin(2.0f * M_PI * (200.0f + (float)i_ch * 100.0f) * time) * 1073741824.0f; // scaled as 2^30 (results in -6dBfs)
				//audioBuffer[TAP_POST_FADER][i_ch][s] = sin(2.0f * M_PI * (200.0f + (float)i_ch * 100.0f) * time) * 268435456.0f; // scaled as 2^28 (results in -18dBfs)
			}
		}
	*/
	/*
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			audioBuffer[TAP_INPUT][0][s] = audioBuffer[TAP_INPUT][0][s] * 0.5f; // reduce volume on channel left to 50% for testing-purposes
			//audioBuffer[TAP_INPUT][1][s] = audioBuffer[TAP_INPUT][1][s] * 0.5f; // reduce volume on channel right to 50% for testing-purposes
		}
	*/
	/*
		// direct copy from the first input-channels to the first output-channels
		for (int i_ch = 0; i_ch < 2; i_ch++) {
			memcpy(&audioBuffer[TAP_OUTPUT][i_ch][0], &audioBuffer[TAP_INPUT][i_ch][0], SAMPLES_IN_BUFFER * sizeof(float));
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
	sampleOffset = 0;
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = audioBufferOffset + sampleOffset;

		// copy data for DSP1
		tdmOffset = 0;
		tdmBufferOffset = 0;
		for (int i_tdm = 0; i_tdm < TDM_INPUTS; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// output to DSP1 as float
				memcpy(&audioTxBuf[bufferIndex], &audioBuffer[TAP_OUTPUT][dspCh][s], sizeof(float));
			}

			tdmOffset += CHANNELS_PER_TDM;
			tdmBufferOffset += (BUFFER_COUNT * BUFFER_SIZE);
		}

		sampleOffset += CHANNELS_PER_TDM;
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
	if (spdifLeftChannel) {
		*pTXSP6A = (int32_t)audioBuffer[TAP_INPUT][DSP_BUF_IDX_SPDIF_LEFT][spdifSamplePointer];
		spdifLeftChannel = false;
	}else{
		*pTXSP6A = (int32_t)audioBuffer[TAP_INPUT][DSP_BUF_IDX_SPDIF_RIGHT][spdifSamplePointer];
		spdifLeftChannel = true;
		spdifSamplePointer++; // increase read-pointer to next sample
	}
}
