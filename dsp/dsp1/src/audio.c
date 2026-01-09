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
	SPORT4B   -> TDM FX OUT0 DSP2
	SPORT6A   -> TDM FX OUT1 DSP2
	SPORT6B   -> TDM REC OUT DSP2

	TDM IN0   -> SPORT1A
	TDM IN1   -> SPORT1B
	TDM IN2   -> SPORT3A
	TDM IN3   -> SPORT3B
	TDM INAUX -> SPORT5A
	TDM FX IN0-> SPORT5B
	TDM FX IN1-> SPORT7A
	TDM PLAY IN-> SPORT7B

	dspCh mapping:
	========================
*/

volatile int audioReady = 0;
volatile int audioProcessing = 0;
int audioBufferOffset = 0;

// audio-buffers for transmitting and receiving
// 16 Audiosamples per channel (= 333us latency)
int audioRxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch1-8 | Ch9-16 | Ch 17-24 | Ch 25-32 | AUX Ch 1-8
int audioTxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch1-8 | Ch9-16 | P16 Ch 1-8 | P16 Ch 9-16 | AUX Ch 1-8

// internal buffers for audio-samples
float audioBuffer[5][SAMPLES_IN_BUFFER][1 + MAX_CHAN_FPGA + MAX_CHAN_DSP2 + MAX_MIXBUS + MAX_MATRIX + MAX_MAIN + MAX_MONITOR]; // audioBuffer[TAPPOINT][SAMPLE][CHANNEL]
float audioTempBufferChanA[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN] = {0};
float audioTempBufferChanB[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN] = {0};
float biquadBuffer[CHANNELS_WITH_4BD_EQ][SAMPLES_IN_BUFFER];

// TCB-arrays for SPORT {CPSPx Chainpointer, ICSPx Internal Count, IMSPx Internal Modifier, IISPx Internal Index}
int audioRx_tcb[8][BUFFER_COUNT][4];
int audioTx_tcb[8][BUFFER_COUNT][4];

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

	// initialize PEQs
	//float coeffs[5] = {2.86939942317678, -0.6369199596053572, -1.8013330773336653, -0.6369199596053572, 0.06806634584311462}; // a0, a1, a2, b1, b2: +14dB @ 7kHz with Q=0.46
	float coeffs[5] = {1, 0, 0, 0, 0}; // a0, a1, a2, b1, b2: direct passthrough
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		// init single-pole lowcut
		dsp.lowcutStatesInput[i_ch] = 0.0;
		dsp.lowcutStatesOutput[i_ch] = 0.0;
	}

	for (int i_ch = 0; i_ch < CHANNELS_WITH_4BD_EQ; i_ch++) {
		// init PEQ-states
		for (int s = 0; s < (2 * MAX_CHAN_EQS); s++) {
			dsp.peqStates[i_ch][s] = 0;
			dsp.peqStates[i_ch][s] = 0;
		}
		// initialize PEQs
		for (int i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++) {
			fxSetPeqCoeffs(i_ch, i_peq, &coeffs[0]);
		}
	}

	// initialize variables
	dsp.monitorTapPoint = TAP_INPUT;

	// initialize memory
	memset(audioBuffer, 0, sizeof(audioBuffer));
}

void audioSmoothVolume(void) {
	// this function smoothes the set audio-volume for a nice user-experience. It is called every 333µs

	// smooth audio-volume for individual channels and FX-returns
	vecvsubf(&dsp.channelVolumeSet[0], &dsp.channelVolume[0], &audioTempBufferChanA[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN); // temp = (volumeSet - volume)
	vecsmltf(&audioTempBufferChanA[0], audioVolumeSmootherCoeff, &audioTempBufferChanA[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN); // temp = temp * coeff
	vecvaddf(&dsp.channelVolume[0], &audioTempBufferChanA[0], &dsp.channelVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN); // volume = volume + temp

	// smooth audio-volume for mains
	vecvsubf(&dsp.mainVolumeSet[0], &dsp.mainVolume[0], &audioTempBufferChanA[0], 3); // temp = (volumeSet - volume)
	vecsmltf(&audioTempBufferChanA[0], audioVolumeSmootherCoeff, &audioTempBufferChanA[0], 3); // temp = temp * coeff
	vecvaddf(&dsp.mainVolume[0], &audioTempBufferChanA[0], &dsp.mainVolume[0], 3); // volume = volume + temp
}

void audioProcessData(void) {
/*
	Ressource-Demand:
	=============================
	- De-Interleaving:	%
	- LowCut:			5.5%
	- Noisegate:		4.2%
	- 4x EQ:			11.5% for 32 4-band-EQs (=0.36% per EQ)
	- Dynamics:			3.8%
	- Channelfader:		%
	- Main-Bus:			6.2%
	- Interleaving:		%
	- Remaining parts:	%
	==============================
			Total Load	81% for 40 FullFeatured channels
			Total Load	73% for 32 FullFeatured channels
*/
	audioProcessing = 1; // set global flag that we are processing now

	int bufferSampleIndex;
	int bufferTdmIndex;
	int dspCh;
	int bufferIndex;
	int sampleOffset;
	int tdmOffset;
	int tdmBufferOffset;

	audioSmoothVolume();

	//  ___                   _     ____             _   _
	// |_ _|_ __  _ __  _   _| |_  |  _ \ ___  _   _| |_(_)_ __   __ _
	//  | || '_ \| '_ \| | | | __| | |_) / _ \| | | | __| | '_ \ / _` |
	//  | || | | | |_) | |_| | |_  |  _ < (_) | |_| | |_| | | | | (_| |
	// |___|_| |_| .__/ \__,_|\__| |_| \_\___/ \__,_|\__|_|_| |_|\__, |
	//           |_|                                             |___/
	// copy interleaved DMA input-buffer into channel buffers
	sampleOffset = 0;
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = audioBufferOffset + sampleOffset;

		// copy channels from FPGA
		tdmOffset = 0;
		tdmBufferOffset = 0;
		for (int i_tdm = 0; i_tdm < TDM_INPUTS_FPGA; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// input from FPGA side (we are receiving int32_t data here)
				// TODO: check if we can use DMA to convert and copy data to destination
				audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_DSPCHANNEL + dspCh] = (float)audioRxBuf[bufferIndex]; // copy int32_t-data to dsp channel as float
			}

			tdmOffset += CHANNELS_PER_TDM;
			tdmBufferOffset += (BUFFER_COUNT * BUFFER_SIZE);
		}

		// copy channels from DSP2
		// keep value of tdmBufferOffset from last loop
		dspCh = 0;
		for (int i_tdm = TDM_INPUTS_FPGA; i_tdm < TDM_INPUTS-1; i_tdm++) {
			bufferIndex = bufferSampleIndex + tdmBufferOffset;

			// input from DSP2 side (we are receiving float data here)
			// TODO: use DMA to store data in the destination as we already receive float-values
			memcpy(&audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_DSP2_FX + dspCh], &audioRxBuf[bufferIndex], CHANNELS_PER_TDM * sizeof(float)); // copy 8 consecutive channels at once

			dspCh += CHANNELS_PER_TDM;
			tdmBufferOffset += (BUFFER_COUNT * BUFFER_SIZE);
		}
		{
			bufferIndex = bufferSampleIndex + tdmBufferOffset;

			// input from DSP2 side (we are receiving float data here)
			// TODO: use DMA to store data in the destination as we already receive float-values
			memcpy(&audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_DSP2_AUX], &audioRxBuf[bufferIndex], CHANNELS_PER_TDM * sizeof(float)); // copy 8 consecutive channels at once
		}

		sampleOffset += CHANNELS_PER_TDM;
	}


	//   ____ _   _    _    _   _ _   _ _____ _     ____ _____ ____  ___ ____
	//  / ___| | | |  / \  | \ | | \ | | ____| |   / ___|_   _|  _ \|_ _|  _ \
	// | |   | |_| | / _ \ |  \| |  \| |  _| | |   \___ \ | | | |_) || || |_) |
	// | |___|  _  |/ ___ \| |\  | |\  | |___| |___ ___) || | |  _ < | ||  __/
	//  \____|_| |_/_/   \_\_| \_|_| \_|_____|_____|____/ |_| |_| \_\___|_|
	// process the audio data per channel

/*
	              32 Channels from FPGA              8 AUX + 8 FX-Ret from DSP2
	=========================================================================	-> TAP_INPUT
	            (MAX_CHANNEL_FULLFEATURED)          |    (AUX + FX Return)
	  -------------------\/--------------------------------------\/---------
	                    GATE                        |          bypass
	  -------------------\/--------------------------------------\/---------	-> TAP_PRE_EQ
	                     EQ                         |            EQ
	  -------------------\/--------------------------------------\/---------	-> TAP_POST_EQ
	                  DYNAMICS                      |          bypass
	  -------------------\/--------------------------------------\/---------	-> TAP_PRE_FADER
	                                CHANNELFADER
	  -----------------------------------\/---------------------------------	-> TAP_POST_FADER

*/

	// ========================================================
	// used SIMD-functions:
	// vecvaddf(input_a[], input_b[], output[], sampleCount)
	// vecvsubf(input_a[], input_b[], output[], sampleCount)
	// vecvmltf(input_a[], input_b[], output[], sampleCount)
	// vecsmltf(input_a[], scalar, output[], sampleCount)

	// The input of the lowcut can be choosen: either the direct input or another channel

	#if DEBUG_DISABLE_LOWCUT == 0
	//				  _                            _
	//				 | |    _____      _____ _   _| |_
	//				 | |   / _ \ \ /\ / / __| | | | __|
	//				 | |__| (_) \ V  V / (__| |_| | |_
	//				 |_____\___/ \_/\_/ \___|\__,_|\__|
	//
	// Single-Pole LOW-CUT: output = coeff * (zoutput + input - zinput)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
			// here no memcpy is possible as we are performing input-routing here
			audioTempBufferChanA[i_ch] = audioBuffer[dsp.inputTapPoint[i_ch]][s][dsp.inputRouting[i_ch]];
		}
		// copy data for the non-fullfeatured channels to the TAP_PRE_EQ directly without processing
		for (int i_ch = MAX_CHAN_FULLFEATURED; i_ch < MAX_CHAN_FPGA; i_ch++) {
			audioBuffer[TAP_PRE_EQ][s][DSP_BUF_IDX_DSPCHANNEL + i_ch] = audioBuffer[dsp.inputTapPoint[i_ch]][s][dsp.inputRouting[i_ch]];
		}

		vecvsubf(&audioTempBufferChanA[0], &dsp.lowcutStatesInput[0], &audioTempBufferChanB[0], MAX_CHAN_FULLFEATURED); // temp = input - zinput
		memcpy(&dsp.lowcutStatesInput[0], &audioTempBufferChanA[0], MAX_CHAN_FULLFEATURED * sizeof(float)); // zinput = input
		vecvaddf(&audioTempBufferChanB[0], &dsp.lowcutStatesOutput[0], &audioTempBufferChanA[0], MAX_CHAN_FULLFEATURED); // temp = temp + zoutput
		vecvmltf(&dsp.lowcutCoeff[0], &audioTempBufferChanA[0], &dsp.lowcutStatesOutput[0], MAX_CHAN_FULLFEATURED); // zoutput = coeff * temp
		memcpy(&audioBuffer[TAP_PRE_EQ][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.lowcutStatesOutput[0], MAX_CHAN_FULLFEATURED * sizeof(float)); // output = zoutput
	}
	#else
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		for (int i_ch = 0; i_ch < MAX_CHAN_FPGA; i_ch++) {
			audioBuffer[TAP_PRE_EQ][s][DSP_BUF_IDX_DSPCHANNEL + i_ch] = audioBuffer[dsp.inputTapPoint[i_ch]][s][dsp.inputRouting[i_ch]];
		}
	}
	#endif

/*
	// Single-Pole HIGH-CUT: output = zoutput + coeff * (input - zoutput)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
			audioTempBufferChanA[i_ch] = audioChannelBufferInput[i_ch][s];
		}
		vecvsubf(&audioTempBufferChanA[0], &dsp.highcutStates[0], &audioTempBufferChan[0], MAX_CHAN_FULLFEATURED); // temp = input - zoutput
		vecvmltf(&dsp.highcutCoeff[0], &audioTempBufferChan[0], &audioTempBufferChan[0], MAX_CHAN_FULLFEATURED); // temp = coeff * temp
		vecvaddf(&dsp.highcutStates[0], &audioTempBufferChan[0], &dsp.highcutStates[0], MAX_CHAN_FULLFEATURED); // zoutput = zoutput + temp
		for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
			audioChannelBufferPreEQ[i_ch][s] = dsp.highcutStates[i_ch]; // output = zoutput
		}
	}
*/

	#if DEBUG_DISABLE_GATE == 0
	//				  _   _       _                      _
	//				 | \ | | ___ (_)___  ___  __ _  __ _| |_ ___
	//				 |  \| |/ _ \| / __|/ _ \/ _` |/ _` | __/ _ \
	//				 | |\  | (_) | \__ \  __/ (_| | (_| | ||  __/
	//				 |_| \_|\___/|_|___/\___|\__, |\__,_|\__\___|
	//				                         |___/
	// Noisegate: gain = (gain * coeff) + gainSet - (gainSet * coeff)
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		// the logic for the gate is relying on a single sample here. Its a compromise, but working
		fxProcessGateLogic(i_ch, audioBuffer[TAP_PRE_EQ][0][DSP_BUF_IDX_DSPCHANNEL + i_ch]);
	}
	vecvmltf(&dsp.gateGain[0], &dsp.gateCoeff[0], &dsp.gateGain[0], MAX_CHAN_FULLFEATURED);
	vecvaddf(&dsp.gateGain[0], &dsp.gateGainSet[0], &dsp.gateGain[0], MAX_CHAN_FULLFEATURED);
	vecvmltf(&dsp.gateGainSet[0], &dsp.gateCoeff[0], &audioTempBufferChanA[0], MAX_CHAN_FULLFEATURED);
	vecvsubf(&dsp.gateGain[0], &audioTempBufferChanA[0], &dsp.gateGain[0], MAX_CHAN_FULLFEATURED);
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		vecvmltf(&audioBuffer[TAP_PRE_EQ][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.gateGain[0], &audioBuffer[TAP_PRE_EQ][s][DSP_BUF_IDX_DSPCHANNEL], MAX_CHAN_FULLFEATURED);
	}
	#endif

	#if DEBUG_DISABLE_EQ == 0
	//				  _____                  _ _
	//				 | ____|__ _ _   _  __ _| (_)_______ _ __
	//				 |  _| / _` | | | |/ _` | | |_  / _ \ '__|
	//				 | |__| (_| | |_| | (_| | | |/ /  __/ |
	//				 |_____\__, |\__,_|\__,_|_|_/___\___|_|
	//				          |_|
	// Hardware-Accelerated Biquad-Filter
	// copy samples into new array
	for (int i_ch = 0; i_ch < (CHANNELS_WITH_4BD_EQ - MAX_MAIN); i_ch++) {
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		// EQ for all 40 input channels + 8 FX Returns from DSP2
			biquadBuffer[i_ch][s] = audioBuffer[TAP_PRE_EQ][s][DSP_BUF_IDX_DSPCHANNEL + i_ch];
		}

		// call biquad trans
		biquad_trans(&biquadBuffer[i_ch][0], &dsp.peqCoeffs[i_ch][0], &dsp.peqStates[i_ch][0], SAMPLES_IN_BUFFER, MAX_CHAN_EQS);

		// copy samples back
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			audioBuffer[TAP_POST_EQ][s][DSP_BUF_IDX_DSPCHANNEL + i_ch] = biquadBuffer[i_ch][s];
		}
	}
	#else
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		memcpy(&audioBuffer[TAP_POST_EQ][s][DSP_BUF_IDX_DSPCHANNEL], &audioBuffer[TAP_PRE_EQ][s][DSP_BUF_IDX_DSPCHANNEL], MAX_CHAN_FPGA * sizeof(float));
	}
	#endif

	#if DEBUG_DISABLE_DYNAMICS == 0
	//				  ____                              _
	//				 |  _ \ _   _ _ __   __ _ _ __ ___ (_) ___ ___
	//				 | | | | | | | '_ \ / _` | '_ ` _ \| |/ __/ __|
	//				 | |_| | |_| | | | | (_| | | | | | | | (__\__ \
	//				 |____/ \__, |_| |_|\__,_|_| |_| |_|_|\___|___/
	//				        |___/
	// Compressor: gain = (gain * coeff) + gainSet - (gainSet * coeff)
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		// the logic for the compressor is relying on a single sample here. Its a compromise, but working
		fxProcessCompressorLogic(i_ch, audioBuffer[TAP_POST_EQ][0][DSP_BUF_IDX_DSPCHANNEL + i_ch]);
	}
	vecvmltf(&dsp.compressorGain[0], &dsp.compressorCoeff[0], &dsp.compressorGain[0], MAX_CHAN_FULLFEATURED);
	vecvaddf(&dsp.compressorGain[0], &dsp.compressorGainSet[0], &dsp.compressorGain[0], MAX_CHAN_FULLFEATURED);
	vecvmltf(&dsp.compressorGainSet[0], &dsp.compressorCoeff[0], &audioTempBufferChanA[0], MAX_CHAN_FULLFEATURED);
	vecvsubf(&dsp.compressorGain[0], &audioTempBufferChanA[0], &dsp.compressorGain[0], MAX_CHAN_FULLFEATURED);
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		vecvmltf(&audioBuffer[TAP_POST_EQ][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.compressorGain[0], &audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_DSPCHANNEL], MAX_CHAN_FULLFEATURED);
		vecvmltf(&audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.compressorMakeup[0], &audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_DSPCHANNEL], MAX_CHAN_FULLFEATURED);

		// bypass audio that is not used in full-featured channel
		memcpy(&audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_DSPCHANNEL + MAX_CHAN_FULLFEATURED], &audioBuffer[TAP_POST_EQ][s][DSP_BUF_IDX_DSPCHANNEL + MAX_CHAN_FULLFEATURED], MAX_CHAN_REDUCED * sizeof(float));
	}
	#else
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		memcpy(&audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &audioBuffer[TAP_POST_EQ][s][DSP_BUF_IDX_DSPCHANNEL], MAX_CHAN_FPGA * sizeof(float));
	}
	#endif

	// copy data for DSP2-FX-Return-Channels from TAP_INPUT to TAP_PRE_FADER without processing. All other DSP2-channel have no volume-control yet
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		memcpy(&audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_DSP2_FX], &audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_DSP2_FX], MAX_DSP2_FXRETURN * sizeof(float));
	}

	//   ____ _                            _   _____         _
	//  / ___| |__   __ _ _ __  _ __   ___| | |  ___|_ _  __| | ___ _ __
	// | |   | '_ \ / _` | '_ \| '_ \ / _ \ | | |_ / _` |/ _` |/ _ \ '__|
	// | |___| | | | (_| | | | | | | |  __/ | |  _| (_| | (_| |  __/ |
	//  \____|_| |_|\__,_|_| |_|_| |_|\___|_| |_|  \__,_|\__,_|\___|_|
	// calculate channel volume
	// --------------------------------------------------------
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		vecvmltf(&audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelVolume[0], &audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN));
	}

	#if DEBUG_DISABLE_MIXBUS == 0
	//				  __  __ _____  ______  _   _ ____
	//				 |  \/  |_ _\ \/ / __ )| | | / ___|
	//				 | |\/| || | \  /|  _ \| | | \___ \
	//				 | |  | || | /  \| |_) | |_| |___) |
	//				 |_|  |_|___/_/\_\____/ \___/|____/
	// calculate mixbus
	for (int i_mixbus = 0; i_mixbus < MAX_MIXBUS; i_mixbus++) {
		// calculated summarized audio for each mixbus-channel
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			// vecdotf(const float dm a[],	const float dm b[], int samples) -> A dot B = A0*B0 + A1*B1 + A2*B2 + ...
			audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MIXBUS + i_mixbus] = vecdotf(&audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMixbusVolume[i_mixbus][0], (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN));
		}

		// TODO: process dynamics on mixbusses
		// TODO: process 6-band PEQ on mixbusses
	}
	// mixbus-volume
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		vecvmltf(&audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MIXBUS], &dsp.mixbusVolume[0], &audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_MIXBUS], MAX_MIXBUS);
	}

	//				  __  __       _              ___        _
	//				 |  \/  | __ _(_)_ __        / _ \ _   _| |_
	//				 | |\/| |/ _` | | '_ \ _____| | | | | | | __|
	//				 | |  | | (_| | | | | |_____| |_| | |_| | |_
	//				 |_|  |_|\__,_|_|_| |_|      \___/ \__,_|\__|
	// calculate summarized main left, right and sub. Source: 40 Channels from FPGA, 24 Channels from DSP2, 16 Channels Mixbus
	// vecdotf(const float dm a[],	const float dm b[], int samples) -> A dot B = A0*B0 + A1*B1 + A2*B2 + ...
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINLEFT] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainLeftVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS);
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINRIGHT] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainRightVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS);
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINSUB] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainSubVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS);
	}
	#else
	//				  __  __       _              ___        _
	//				 |  \/  | __ _(_)_ __        / _ \ _   _| |_
	//				 | |\/| |/ _` | | '_ \ _____| | | | | | | __|
	//				 | |  | | (_| | | | | |_____| |_| | |_| | |_
	//				 |_|  |_|\__,_|_|_| |_|      \___/ \__,_|\__|
	// calculate summarized main left, right and sub. Source: 40 Channels from FPGA, 24 Channels from DSP2
	// vecdotf(const float dm a[],	const float dm b[], int samples) -> A dot B = A0*B0 + A1*B1 + A2*B2 + ...
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINLEFT] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainLeftVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN);
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINRIGHT] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainRightVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN);
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINSUB] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainSubVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN);
	}
	#endif

	//				  _____                  _ _
	//				 | ____|__ _ _   _  __ _| (_)_______ _ __
	//				 |  _| / _` | | | |/ _` | | |_  / _ \ '__|
	//				 | |__| (_| | |_| | (_| | | |/ /  __/ |
	//				 |_____\__, |\__,_|\__,_|_|_/___\___|_|
	//				          |_|
	// Hardware-Accelerated Biquad-Filter
	// copy samples into new array
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		for (int i_ch = 0; i_ch < 3; i_ch++) {
			biquadBuffer[i_ch][s] = audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINLEFT + i_ch];
		}
	}
	// call biquad trans
	for (int i_ch = 0; i_ch < 3; i_ch++) {
		biquad_trans(&biquadBuffer[i_ch][0], &dsp.peqCoeffs[i_ch][0], &dsp.peqStates[i_ch][0], SAMPLES_IN_BUFFER, MAX_CHAN_EQS);
	}
	// copy samples back
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		for (int i_ch = 0; i_ch < 3; i_ch++) {
			audioBuffer[TAP_POST_EQ][s][DSP_BUF_IDX_MAINLEFT + i_ch] = biquadBuffer[i_ch][s];
		}
	}

	// TODO: process 6-band PEQ on main L/R/S
	// TODO: process dynamics on main L/R/S

	// main-volume
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		vecvmltf(&audioBuffer[TAP_POST_EQ][s][DSP_BUF_IDX_MAINLEFT], &dsp.mainVolume[0], &audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_MAINLEFT], 3);
	}

	//  __  __    _  _____ ____  _____  __
	// |  \/  |  / \|_   _|  _ \|_ _\ \/ /
	// | |\/| | / _ \ | | | |_) || | \  /
	// | |  | |/ ___ \| | |  _ < | | /  \
	// |_|  |_/_/   \_\_| |_| \_\___/_/\_\
	// calculate matrices
/*
	for (int i_matrix = 0; i_matrix < 6; i_matrix++) {
		// calculated summarized audio for each matrix-channel
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			// vecdotf(const float dm a[],	const float dm b[], int samples) -> A dot B = A0*B0 + A1*B1 + A2*B2 + ...
			audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MATRIX + i_matrix] = vecdotf(&audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_MIXBUS], &dsp.sendMatrixVolume[i_matrix][0], MAX_MIXBUS + 3);
		}

		// TODO: process dynamics on mixbusses
		// TODO: process 6-band PEQ on mixbusses
	}
	// matrix-volume
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		vecvmltf(&audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MATRIX], &dsp.matrixVolume[0], &audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_MATRIX], MAX_MATRIX);
	}
*/

	//  __  __  ___  _   _ ___ _____ ___  ____  ___ _   _  ____
	// |  \/  |/ _ \| \ | |_ _|_   _/ _ \|  _ \|_ _| \ | |/ ___|
	// | |\/| | | | |  \| || |  | || | | | |_) || ||  \| | |  _
	// | |  | | |_| | |\  || |  | || |_| |  _ < | || |\  | |_| |
	// |_|  |_|\___/|_| \_|___| |_| \___/|_| \_\___|_| \_|\____|
/*
	// if no Solo is used, output Main Left and Main Right, otherwise put soloed-channels in place
	if (dsp.soloActive) {
		// accumulate the soloed channels pre-fader into MonitorL/R
		for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
			if (dsp.dspChannel[i_ch].solo) {
				vecvaddf(&audioBuffer[dsp.monitorChannelTapPoint][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], SAMPLES_IN_BUFFER);
			}
		}
		for (int i_mixbus = 0; i_mixbus < MAX_CHAN; i_mixbus++) {
			if (dsp.mixbusSolo[i_mixbus]) {
				vecvaddf(&audioBuffer[dsp.monitorMixbusTapPoint][DSP_BUF_IDX_MIXBUS + i_mixbus][0], &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], SAMPLES_IN_BUFFER);
			}
		}
		for (int i_matrix = 0; i_matrix < MAX_CHAN; i_matrix++) {
			if (dsp.matrixSolo[i_matrix]) {
				vecvaddf(&audioBuffer[dsp.monitorMatrixTapPoint][DSP_BUF_IDX_MATRIX + i_matrix][0], &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], SAMPLES_IN_BUFFER);
			}
		}
		if (dsp.mainLrSolo) {
			vecvaddf(&audioBuffer[dsp.monitorMainTapPoint][DSP_BUF_IDX_MAINLEFT][0], &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], SAMPLES_IN_BUFFER);
		}
		if (dsp.mainSubSolo) {
			vecvaddf(&audioBuffer[dsp.monitorMainTapPoint][DSP_BUF_IDX_MAINSUB][0], &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], SAMPLES_IN_BUFFER);
		}

		// copy left data to right channel
		memcpy(&audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONRIGHT][0], &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], SAMPLES_IN_BUFFER * sizeof(float));
	}else{
		// no soloed channels. Put MainL/R to MonitorL/R
		vecsmltf(&audioBuffer[dsp.monitorMainTapPoint][DSP_BUF_IDX_MAINLEFT][0], dsp.monitorVolume, &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONLEFT][0], SAMPLES_IN_BUFFER);
		vecsmltf(&audioBuffer[dsp.monitorMainTapPoint][DSP_BUF_IDX_MAINRIGHT][0], dsp.monitorVolume, &audioBuffer[dsp.monitorTapPoint][DSP_BUF_IDX_MONRIGHT][0], SAMPLES_IN_BUFFER);
	}
*/
	// ========================================================

	//   ___        _               _     ____             _   _
	//  / _ \ _   _| |_ _ __  _   _| |_  |  _ \ ___  _   _| |_(_)_ __   __ _
	// | | | | | | | __| '_ \| | | | __| | |_) / _ \| | | | __| | '_ \ / _` |
	// | |_| | |_| | |_| |_) | |_| | |_  |  _ < (_) | |_| | |_| | | | | (_| |
	//  \___/ \__,_|\__| .__/ \__,_|\__| |_| \_\___/ \__,_|\__|_|_| |_|\__, |
	//                 |_|                                             |___/
	// copy channel buffers to interleaved output-buffer
	sampleOffset = 0;
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = audioBufferOffset + sampleOffset;

		// copy data for FPGA
		tdmOffset = 0;
		tdmBufferOffset = 0;
		for (int i_tdm = 0; i_tdm < TDM_INPUTS_FPGA; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// output to FPGA as int32_t
				audioTxBuf[bufferIndex] = audioBuffer[dsp.outputTapPoint[dspCh]][s][dsp.outputRouting[dspCh]];
			}

			tdmOffset += CHANNELS_PER_TDM;
			tdmBufferOffset += (BUFFER_COUNT * BUFFER_SIZE);
		}

		// copy data for DSP2
		// keep values of tdmOffset and tdmBufferOffset from previous loop
		for (int i_tdm = TDM_INPUTS_FPGA; i_tdm < TDM_INPUTS; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			// output to DSP2 (FX) as float
			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// output to DSP2 as float
				memcpy(&audioTxBuf[bufferIndex], &audioBuffer[dsp.outputTapPoint[dspCh]][s][dsp.outputRouting[dspCh]], sizeof(float));
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
