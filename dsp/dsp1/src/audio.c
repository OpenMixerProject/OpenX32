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
int audioBufferCounter = 0;

// audio-buffers for transmitting and receiving
// 16 Audiosamples per channel (= 333us latency)
//section("seg_ext_data")
int audioTxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch1-8 | Ch9-16 | P16 Ch 1-8 | P16 Ch 9-16 | AUX Ch 1-8
//section("seg_ext_data")
int audioRxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch1-8 | Ch9-16 | Ch 17-24 | Ch 25-32 | AUX Ch 1-8

// internal buffers
float audioDspChannelBuffer[MAX_CHAN][SAMPLES_IN_BUFFER] = {0}; // In 1-32, Aux 1-8
float audioOutputChannelBuffer[MAX_CHAN][SAMPLES_IN_BUFFER] = {0}; // Out 1-16, P1-16, Aux 1-8
float audioTempBuffer[SAMPLES_IN_BUFFER] = {0};
float audioTempBufferLarge[MAX_CHAN][SAMPLES_IN_BUFFER] = {0};
float audioTempBufferChan[MAX_CHAN];

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

	//float coeffs[5] = {2.86939942317678, -0.6369199596053572, -1.8013330773336653, -0.6369199596053572, 0.06806634584311462}; // a0, a1, a2, b1, b2: +14dB @ 7kHz with Q=0.46
	float coeffs[5] = {1, 0, 0, 0, 0}; // a0, a1, a2, b1, b2: direct passthrough
	for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
		// init single-pole lowcut
		dsp.lowcutStatesInput[i_ch] = 0.0;
		dsp.lowcutStatesOutput[i_ch] = 0.0;
		dsp.lowcutCoeff[i_ch] = 1.0f / (1.0f + 2.0f * M_PI * 20000.0f * (1.0f/dsp.samplerate));
		// init single-pole highcut
		//dsp.highcutStates[i_ch] = 0.0;
		//dsp.highcutCoeff[i_ch] = (2.0f * M_PI * 500.0f) / (dsp.samplerate + 2.0f * M_PI * 500.0f);

		// init PEQ-states
		for (int s = 0; s < (2 * MAX_CHAN_EQS); s++) {
			dsp.dspChannel[i_ch].peqStates[s] = 0;
			dsp.dspChannel[i_ch].peqStates[s] = 0;
		}

		// initialize PEQs
		for (int i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++) {
			fxSetPeqCoeffs(i_ch, i_peq, &coeffs[0]);
		}
	}
}

void audioProcessData(void) {
	audioProcessing = 1; // set global flag that we are processing now

	int bufferSampleIndex;
	int bufferTdmIndex;
	int dspCh;
	int bufferIndex;

	//  ____            ___       _            _                  _
	// |  _ \  ___     |_ _|_ __ | |_ ___ _ __| | ___  __ ___   _(_)_ __   __ _
	// | | | |/ _ \_____| || '_ \| __/ _ \ '__| |/ _ \/ _` \ \ / / | '_ \ / _` |
	// | |_| |  __/_____| || | | | ||  __/ |  | |  __/ (_| |\ V /| | | | | (_| |
	// |____/ \___|    |___|_| |_|\__\___|_|  |_|\___|\__,_| \_/ |_|_| |_|\__, |
	//                                                                    |___/
	// copy interleaved DMA input-buffer into channel buffers
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = (BUFFER_SIZE * audioBufferCounter) + (CHANNELS_PER_TDM * s); // (select correct buffer 0 or 1) + (sample-offset)
		for (int i_tdm = 0; i_tdm < TDM_INPUTS; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + (BUFFER_COUNT * BUFFER_SIZE * i_tdm);
			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = (CHANNELS_PER_TDM * i_tdm) + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);
				if (bufferIndex >= (TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE)) {
					bufferIndex -= (TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE);
				}
				audioDspChannelBuffer[dspCh][s] = audioRxBuf[bufferIndex];
			}
		}
	}

	// process the audio data per channel
	// ========================================================
	// used SIMD-functions:
	// vecvmltf(input_a[], input_b[], output[], sampleCount)
	// vecvaddf(input_a[], input_b[], output[], sampleCount)
	// vecvsubf(input_a[], input_b[], output[], sampleCount)
	// vecsmltf(input_a[], scalar, output[], sampleCount)
	//  _                            _
	// | |    _____      _____ _   _| |_
	// | |   / _ \ \ /\ / / __| | | | __|
	// | |__| (_) \ V  V / (__| |_| | |_
	// |_____\___/ \_/\_/ \___|\__,_|\__|
	//
	// Single-Pole LOW-CUT
	// output = coeff * (zoutput + input - zinput)
	// 1. temp = input - zinput
	// 2. temp = zoutput + temp
	// 3. zoutput = coeff * temp
	// 4. output = zoutput
	// 5. zinput = input
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
			audioTempBufferChan[i_ch] = audioDspChannelBuffer[i_ch][s];
		}
		vecvsubf(&audioTempBufferChan[0], &dsp.lowcutStatesInput[0], &audioTempBufferChan[0], MAX_CHAN_FULLFEATURED);
		vecvaddf(&audioTempBufferChan[0], &dsp.lowcutStatesOutput[0], &audioTempBufferChan[0], MAX_CHAN_FULLFEATURED);
		vecvmltf(&dsp.lowcutCoeff[0], &audioTempBufferChan[0], &dsp.lowcutStatesOutput[0], MAX_CHAN_FULLFEATURED);

		for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
			dsp.lowcutStatesInput[i_ch] = audioDspChannelBuffer[i_ch][s];
			audioDspChannelBuffer[i_ch][s] = dsp.lowcutStatesOutput[i_ch];
		}
	}

/*
	// Single-Pole HIGH-CUT
	// output = zoutput + coeff * (input - zoutput)
	// 1. temp = input - zoutput
	// 2. temp = coeff * temp
	// 3. zoutput = zoutput + temp
	// 4. output = zoutput
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
			audioTempBufferChan[i_ch] = audioDspChannelBuffer[i_ch][s];
		}
		vecvsubf(&audioTempBufferChan[0], &dsp.highcutStates[0], &audioTempBufferChan[0], MAX_CHAN_FULLFEATURED);
		vecvmltf(&dsp.highcutCoeff[0], &audioTempBufferChan[0], &audioTempBufferChan[0], MAX_CHAN_FULLFEATURED);
		vecvaddf(&dsp.highcutStates[0], &audioTempBufferChan[0], &dsp.highcutStates[0], MAX_CHAN_FULLFEATURED);
		for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
			audioDspChannelBuffer[i_ch][s] = dsp.highcutStates[i_ch];
		}
	}
*/

	//  _   _       _                      _
	// | \ | | ___ (_)___  ___  __ _  __ _| |_ ___
	// |  \| |/ _ \| / __|/ _ \/ _` |/ _` | __/ _ \
	// | |\  | (_) | \__ \  __/ (_| | (_| | ||  __/
	// |_| \_|\___/|_|___/\___|\__, |\__,_|\__\___|
	//                         |___/

	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		fxProcessGateLogic(i_ch, &audioDspChannelBuffer[i_ch][0]);
	}
	// gain = (gain * coeff) + gainSet - (gainSet * coeff)
	vecvmltf(&dsp.gateGain[0], &dsp.gateCoeff[0], &dsp.gateGain[0], MAX_CHAN_FULLFEATURED);
	vecvaddf(&dsp.gateGain[0], &dsp.gateGainSet[0], &dsp.gateGain[0], MAX_CHAN_FULLFEATURED);
	vecvmltf(&dsp.gateGainSet[0], &dsp.gateCoeff[0], &audioTempBufferChan[0], MAX_CHAN_FULLFEATURED);
	vecvsubf(&dsp.gateGain[0], &audioTempBufferChan[0], &dsp.gateGain[0], MAX_CHAN_FULLFEATURED);
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		vecsmltf(&audioDspChannelBuffer[i_ch][0], dsp.gateGain[i_ch], &audioDspChannelBuffer[i_ch][0], SAMPLES_IN_BUFFER);
	}

	//  _____                  _ _
	// | ____|__ _ _   _  __ _| (_)_______ _ __
	// |  _| / _` | | | |/ _` | | |_  / _ \ '__|
	// | |__| (_| | |_| | (_| | | |/ /  __/ |
	// |_____\__, |\__,_|\__,_|_|_/___\___|_|
	//          |_|
	for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
		//                 input and output                BiQuad-Coefficients                        Delay-Line                  samples           Sections
		biquad_trans(&audioDspChannelBuffer[i_ch][0], &dsp.dspChannel[i_ch].peqCoeffs[0], &dsp.dspChannel[i_ch].peqStates[0], SAMPLES_IN_BUFFER, MAX_CHAN_EQS);
	}

	//  ____                              _
	// |  _ \ _   _ _ __   __ _ _ __ ___ (_) ___ ___
	// | | | | | | | '_ \ / _` | '_ ` _ \| |/ __/ __|
	// | |_| | |_| | | | | (_| | | | | | | | (__\__ \
	// |____/ \__, |_| |_|\__,_|_| |_| |_|_|\___|___/
	//        |___/
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		fxProcessCompressorLogic(i_ch, &audioDspChannelBuffer[i_ch][0]);
	}
	// gain = (gain * coeff) + gainSet - (gainSet * coeff)
	vecvmltf(&dsp.compressorGain[0], &dsp.compressorCoeff[0], &dsp.compressorGain[0], MAX_CHAN_FULLFEATURED);
	vecvaddf(&dsp.compressorGain[0], &dsp.compressorGainSet[0], &dsp.compressorGain[0], MAX_CHAN_FULLFEATURED);
	vecvmltf(&dsp.compressorGainSet[0], &dsp.compressorCoeff[0], &audioTempBufferChan[0], MAX_CHAN_FULLFEATURED);
	vecvsubf(&dsp.compressorGain[0], &audioTempBufferChan[0], &dsp.compressorGain[0], MAX_CHAN_FULLFEATURED);
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		vecsmltf(&audioDspChannelBuffer[i_ch][0], dsp.compressorGain[i_ch], &audioDspChannelBuffer[i_ch][0], SAMPLES_IN_BUFFER);
		vecsmltf(&audioDspChannelBuffer[i_ch][0], dsp.compressorMakeup[i_ch], &audioDspChannelBuffer[i_ch][0], SAMPLES_IN_BUFFER);
	}

	// calculate channel volume
	// --------------------------------------------------------
	for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
		vecsmltf(&audioDspChannelBuffer[i_ch][0], dsp.channelVolume[i_ch], &audioDspChannelBuffer[i_ch][0], SAMPLES_IN_BUFFER);
	}

	//  __  __       _              ___        _                     _   ____                 _
	// |  \/  | __ _(_)_ __        / _ \ _   _| |_    __ _ _ __   __| | / ___|  ___ _ __   __| |___
	// | |\/| |/ _` | | '_ \ _____| | | | | | | __|  / _` | '_ \ / _` | \___ \ / _ \ '_ \ / _` / __|
	// | |  | | (_| | | | | |_____| |_| | |_| | |_  | (_| | | | | (_| |  ___) |  __/ | | | (_| \__ \
	// |_|__|_|\__,_|_|_| |_|      \___/ \__,_|\__|  \__,_|_| |_|\__,_| |____/ \___|_| |_|\__,_|___/
	// reset data in output-buffers
	memset(audioOutputChannelBuffer, 0, sizeof(audioOutputChannelBuffer));

	// calculate main left
	for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
		vecsmltf(&audioDspChannelBuffer[i_ch][0], dsp.channelVolumeLeft[i_ch], &audioTempBufferLarge[i_ch][0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBufferLarge[i_ch][0], &audioOutputChannelBuffer[0][0], &audioOutputChannelBuffer[0][0], SAMPLES_IN_BUFFER);
	}

	// calculate main right
	for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
		vecsmltf(&audioDspChannelBuffer[i_ch][0], dsp.channelVolumeRight[i_ch], &audioTempBufferLarge[i_ch][0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBufferLarge[i_ch][0], &audioOutputChannelBuffer[1][0], &audioOutputChannelBuffer[1][0], SAMPLES_IN_BUFFER);
	}

	// calculate main sub
	for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
		vecsmltf(&audioDspChannelBuffer[i_ch][0], dsp.channelVolumeSub[i_ch], &audioTempBufferLarge[i_ch][0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBufferLarge[i_ch][0], &audioOutputChannelBuffer[2][0], &audioOutputChannelBuffer[2][0], SAMPLES_IN_BUFFER);
	}
	// ========================================================

	//  ___       _            _                  _
	// |_ _|_ __ | |_ ___ _ __| | ___  __ ___   _(_)_ __   __ _
	//  | || '_ \| __/ _ \ '__| |/ _ \/ _` \ \ / / | '_ \ / _` |
	//  | || | | | ||  __/ |  | |  __/ (_| |\ V /| | | | | (_| |
	// |___|_| |_|\__\___|_|  |_|\___|\__,_| \_/ |_|_| |_|\__, |
	//                                                    |___/
	// copy channel buffers to interleaved output-buffer
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = (BUFFER_SIZE * audioBufferCounter) + (CHANNELS_PER_TDM * s); // (select correct buffer 0 or 1) + (sample-offset)
		for (int i_tdm = 0; i_tdm < TDM_INPUTS; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + (BUFFER_COUNT * BUFFER_SIZE * i_tdm);
			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = (CHANNELS_PER_TDM * i_tdm) + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);
				if (bufferIndex >= (TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE)) {
					bufferIndex -= (TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE);
				}
				audioTxBuf[bufferIndex] = audioOutputChannelBuffer[dspCh][s];
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
    	// this is not nice but without a debugger and profiling tools this is the easiest method to check if the algorithms are within the timing
    	systemCrash();
    }

    audioReady = 1; // set flag, that we have new data to process
}
