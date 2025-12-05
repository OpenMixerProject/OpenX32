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
float audioBuffer[5][1 + MAX_CHAN + MAX_MIXBUS + MAX_MATRIX + MAX_MAIN + MAX_MONITOR + MAX_DSP2][SAMPLES_IN_BUFFER]; // audioBuffer[TAPPOINT][CHANNEL][SAMPLE]
float audioTempBuffer[SAMPLES_IN_BUFFER] = {0};
float audioTempBufferLarge[MAX_CHAN][SAMPLES_IN_BUFFER] = {0};
float audioTempBufferChanA[MAX_CHAN] = {0};
float audioTempBufferChanB[MAX_CHAN] = {0};

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

	// initialize variables
	dsp.monitorTapPoint = TAP_INPUT;

	// initialize memory
	memset(audioBuffer, 0, sizeof(audioBuffer));
}

void audioProcessData(void) {
/*
	Ressource-Demand:
	=============================
	- De-Interleaving:	6%
	- LowCut:			8%
	- Noisegate:		8%
	- 4x EQ:			20%
	- Dynamics:			3%
	- Channelfader:		11%	<- TODO: check why this takes so much load
	- Main-Bus:			16%
	- Interleaving:		7%
	- Remaining parts:	2%
	==============================
			Total Load	81% for 40 FullFeatured channels
			Total Load	73% for 32 FullFeatured channels
*/
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
	// Ressource-Demand: ~6%
	sOffset = 0;
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = audioBufferOffset + sOffset;

		// copy channels from FPGA
		tdmOffset = 0;
		tdmBufferOffset = 0;
		for (int i_tdm = 0; i_tdm < TDM_INPUTS_FPGA; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// input from FPGA side (we are receiving int32_t data here)
				audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSPCHANNEL + dspCh][s] = (float)audioRxBuf[bufferIndex]; // copy int32_t-data to dsp channel as float
			}

			tdmOffset += CHANNELS_PER_TDM;
			tdmBufferOffset += (BUFFER_COUNT * BUFFER_SIZE);
		}

		// copy channels from DSP2
		tdmOffset = 0; // we have to reset tdmOffset as we have a new bufferIndex for DSP2
		// keep value of tdmBufferOffset from last loop
		for (int i_tdm = TDM_INPUTS_FPGA; i_tdm < TDM_INPUTS; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// input from DSP2 side (we are receving float data here)
				memcpy(&audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSP2_FX + dspCh][s], &audioRxBuf[bufferIndex], sizeof(float));
			}

			tdmOffset += CHANNELS_PER_TDM;
			tdmBufferOffset += (BUFFER_COUNT * BUFFER_SIZE);
		}

		sOffset += CHANNELS_PER_TDM;
	}


	//   ____ _   _    _    _   _ _   _ _____ _     ____ _____ ____  ___ ____
	//  / ___| | | |  / \  | \ | | \ | | ____| |   / ___|_   _|  _ \|_ _|  _ \
	// | |   | |_| | / _ \ |  \| |  \| |  _| | |   \___ \ | | | |_) || || |_) |
	// | |___|  _  |/ ___ \| |\  | |\  | |___| |___ ___) || | |  _ < | ||  __/
	//  \____|_| |_/_/   \_\_| \_|_| \_|_____|_____|____/ |_| |_| \_\___|_|
	// process the audio data per channel
	// ========================================================
	// used SIMD-functions:
	// vecvaddf(input_a[], input_b[], output[], sampleCount)
	// vecvsubf(input_a[], input_b[], output[], sampleCount)
	// vecvmltf(input_a[], input_b[], output[], sampleCount)
	// vecsmltf(input_a[], scalar, output[], sampleCount)

	// The input of the lowcut can be choosen: either the direct input or another channel

	//				  _                            _
	//				 | |    _____      _____ _   _| |_
	//				 | |   / _ \ \ /\ / / __| | | | __|
	//				 | |__| (_) \ V  V / (__| |_| | |_
	//				 |_____\___/ \_/\_/ \___|\__,_|\__|
	//
	// Single-Pole LOW-CUT: output = coeff * (zoutput + input - zinput)
	// Ressource-Demand: ~5% for 40 Channels
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
			audioTempBufferChanA[i_ch] = audioBuffer[dsp.inputTapPoint[i_ch]][dsp.inputRouting[i_ch]][s];
		}

		vecvsubf(&audioTempBufferChanA[0], &dsp.lowcutStatesInput[0], &audioTempBufferChanB[0], MAX_CHAN_FULLFEATURED); // temp = input - zinput
		memcpy(&dsp.lowcutStatesInput[0], &audioTempBufferChanA[0], MAX_CHAN_FULLFEATURED * sizeof(float)); // zinput = input
		vecvaddf(&audioTempBufferChanB[0], &dsp.lowcutStatesOutput[0], &audioTempBufferChanB[0], MAX_CHAN_FULLFEATURED); // temp = temp + zoutput
		vecvmltf(&dsp.lowcutCoeff[0], &audioTempBufferChanB[0], &dsp.lowcutStatesOutput[0], MAX_CHAN_FULLFEATURED); // zoutput = coeff * temp

		for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
			audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][s] = dsp.lowcutStatesOutput[i_ch]; // output = zoutput
		}
	}

/*
	// Single-Pole HIGH-CUT: output = zoutput + coeff * (input - zoutput)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
			audioTempBufferChan[i_ch] = audioChannelBufferInput[i_ch][s];
		}
		vecvsubf(&audioTempBufferChan[0], &dsp.highcutStates[0], &audioTempBufferChan[0], MAX_CHAN_FULLFEATURED); // temp = input - zoutput
		vecvmltf(&dsp.highcutCoeff[0], &audioTempBufferChan[0], &audioTempBufferChan[0], MAX_CHAN_FULLFEATURED); // temp = coeff * temp
		vecvaddf(&dsp.highcutStates[0], &audioTempBufferChan[0], &dsp.highcutStates[0], MAX_CHAN_FULLFEATURED); // zoutput = zoutput + temp
		for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
			audioChannelBufferPreEQ[i_ch][s] = dsp.highcutStates[i_ch]; // output = zoutput
		}
	}
*/

	//				  _   _       _                      _
	//				 | \ | | ___ (_)___  ___  __ _  __ _| |_ ___
	//				 |  \| |/ _ \| / __|/ _ \/ _` |/ _` | __/ _ \
	//				 | |\  | (_) | \__ \  __/ (_| | (_| | ||  __/
	//				 |_| \_|\___/|_|___/\___|\__, |\__,_|\__\___|
	//				                         |___/
	// Noisegate: gain = (gain * coeff) + gainSet - (gainSet * coeff)
	// Ressource-Demand: ~8%
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		fxProcessGateLogic(i_ch, &audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0]);
	}
	vecvmltf(&dsp.gateGain[0], &dsp.gateCoeff[0], &dsp.gateGain[0], MAX_CHAN_FULLFEATURED);
	vecvaddf(&dsp.gateGain[0], &dsp.gateGainSet[0], &dsp.gateGain[0], MAX_CHAN_FULLFEATURED);
	vecvmltf(&dsp.gateGainSet[0], &dsp.gateCoeff[0], &audioTempBufferChanA[0], MAX_CHAN_FULLFEATURED);
	vecvsubf(&dsp.gateGain[0], &audioTempBufferChanA[0], &dsp.gateGain[0], MAX_CHAN_FULLFEATURED);
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		vecsmltf(&audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], dsp.gateGain[i_ch], &audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], SAMPLES_IN_BUFFER);
	}

	/*
	// copy data for the non-fullfeatured channels
	for (int i_ch = MAX_CHAN_FULLFEATURED; i_ch < MAX_CHAN; i_ch++) {
		memcpy(&audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], &audioBuffer[dsp.inputTapPoint[i_ch]][dsp.inputRouting[i_ch]][0], SAMPLES_IN_BUFFER * sizeof(float));
	}
	*/

	//				  _____                  _ _
	//				 | ____|__ _ _   _  __ _| (_)_______ _ __
	//				 |  _| / _` | | | |/ _` | | |_  / _ \ '__|
	//				 | |__| (_| | |_| | (_| | | |/ /  __/ |
	//				 |_____\__, |\__,_|\__,_|_|_/___\___|_|
	//				          |_|
	// use low-pass filter on EQ-Coefficients to smoothly change parameters
	//fxSmoothCoeffs();

	// Hardware-Accelerated Biquad-Filter
	// Ressource-Demand: ~20%
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		memcpy(&audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], &audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], SAMPLES_IN_BUFFER * sizeof(float));
		//                 input and output                BiQuad-Coefficients                        Delay-Line                  samples           Sections
		biquad_trans(&audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], &dsp.dspChannel[i_ch].peqCoeffs[0], &dsp.dspChannel[i_ch].peqStates[0], SAMPLES_IN_BUFFER, MAX_CHAN_EQS);

		// caution: biquad() without "_trans" takes way(!) more cpu-cycles. Dont use it.
		//biquad(&audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], &audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], &dsp.dspChannel[i_ch].peqCoeffs[0], &dsp.dspChannel[i_ch].peqStates[0], SAMPLES_IN_BUFFER, MAX_CHAN_EQS);
	}

	//				  ____                              _
	//				 |  _ \ _   _ _ __   __ _ _ __ ___ (_) ___ ___
	//				 | | | | | | | '_ \ / _` | '_ ` _ \| |/ __/ __|
	//				 | |_| | |_| | | | | (_| | | | | | | | (__\__ \
	//				 |____/ \__, |_| |_|\__,_|_| |_| |_|_|\___|___/
	//				        |___/
	// Compressor: gain = (gain * coeff) + gainSet - (gainSet * coeff)
	// Ressource-Demand: ~3%
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		fxProcessCompressorLogic(i_ch, &audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0]);
	}
	vecvmltf(&dsp.compressorGain[0], &dsp.compressorCoeff[0], &dsp.compressorGain[0], MAX_CHAN_FULLFEATURED);
	vecvaddf(&dsp.compressorGain[0], &dsp.compressorGainSet[0], &dsp.compressorGain[0], MAX_CHAN_FULLFEATURED);
	vecvmltf(&dsp.compressorGainSet[0], &dsp.compressorCoeff[0], &audioTempBufferChanA[0], MAX_CHAN_FULLFEATURED);
	vecvsubf(&dsp.compressorGain[0], &audioTempBufferChanA[0], &dsp.compressorGain[0], MAX_CHAN_FULLFEATURED);
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		vecsmltf(&audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], dsp.compressorGain[i_ch], &audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], SAMPLES_IN_BUFFER);
		vecsmltf(&audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], dsp.compressorMakeup[i_ch], &audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], SAMPLES_IN_BUFFER);
	}

	// calculate channel volume
	// Ressource-Demand: ~11%
	// --------------------------------------------------------
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		vecsmltf(&audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], dsp.channelVolume[i_ch], &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], SAMPLES_IN_BUFFER);
	}

/*
	//				  __  __ _____  ______  _   _ ____
	//				 |  \/  |_ _\ \/ / __ )| | | / ___|
	//				 | |\/| || | \  /|  _ \| | | \___ \
	//				 | |  | || | /  \| |_) | |_| |___) |
	//				 |_|  |_|___/_/\_\____/ \___/|____/

	// calculate mixbus
	for (int i_mixbus = 0; i_mixbus < MAX_MIXBUS; i_mixbus++) {
		for (int i_ch = 0; i_ch < MAX_CHAN; i_ch++) {
			if (dsp.channelSendMixbusTapPoint[i_ch][i_mixbus] < 5) {
				vecsmltf(&audioBuffer[dsp.channelSendMixbusTapPoint[i_ch][i_mixbus]][i_ch][0], dsp.channelSendMixbusVolume[i_ch][i_mixbus], &audioTempBufferLarge[i_ch][0], SAMPLES_IN_BUFFER);
				vecvaddf(&audioTempBufferLarge[i_ch][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MIXBUS + i_mixbus][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MIXBUS + i_mixbus][0], SAMPLES_IN_BUFFER);
			}else{
				vecvaddf(&audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MIXBUS + i_mixbus][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MIXBUS +i_mixbus][0], SAMPLES_IN_BUFFER);
			}
		}

		// TODO: process dynamics on mixbusses
		// TODO: process 6-band PEQ on mixbusses

		// volume of mixbus
		vecsmltf(&audioBuffer[TAP_INPUT][DSP_BUF_IDX_MIXBUS + i_mixbus][0], dsp.mixbusVolume[i_mixbus], &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MIXBUS + i_mixbus][0], SAMPLES_IN_BUFFER);
	}
*/

	//				  __  __       _              ___        _
	//				 |  \/  | __ _(_)_ __        / _ \ _   _| |_
	//				 | |\/| |/ _` | | '_ \ _____| | | | | | | __|
	//				 | |  | | (_| | | | | |_____| |_| | |_| | |_
	//				 |_|  |_|\__,_|_|_| |_|      \___/ \__,_|\__|

	// process first channel while overwriting old values on main-channels
	// calculate main left, right and sub
	// vecsmltf(input_a[], scalar, output[], sampleCount)
	// vecvaddf(input_a[], input_b[], output[], sampleCount)
	// Ressource-Demand: ~16%
	vecsmltf(&audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL][0], dsp.channelSendMainLeftVolume[0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINLEFT][0], SAMPLES_IN_BUFFER);
	vecsmltf(&audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL][0], dsp.channelSendMainRightVolume[0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINRIGHT][0], SAMPLES_IN_BUFFER);
	vecsmltf(&audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL][0], dsp.channelSendMainSubVolume[0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINSUB][0], SAMPLES_IN_BUFFER);
	// add internal DSP-Channels 2-40 to main
	for (int i_ch = 1; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		// calculate main left
		vecsmltf(&audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], dsp.channelSendMainLeftVolume[i_ch], &audioTempBuffer[0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBuffer[0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINLEFT][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINLEFT][0], SAMPLES_IN_BUFFER);

		// calculate main right
		vecsmltf(&audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], dsp.channelSendMainRightVolume[i_ch], &audioTempBuffer[0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBuffer[0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINRIGHT][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINRIGHT][0], SAMPLES_IN_BUFFER);

		// calculate main sub
		vecsmltf(&audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], dsp.channelSendMainSubVolume[i_ch], &audioTempBuffer[0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBuffer[0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINSUB][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINSUB][0], SAMPLES_IN_BUFFER);
	}
	// add FX- and AUX-Return to main-channels
	for (int i_ch = 0; i_ch < MAX_DSP2; i_ch++) {
		// calculate main left
		vecsmltf(&audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSP2_FX + i_ch][0], dsp.channelSendMainLeftVolume[MAX_CHAN + i_ch], &audioTempBuffer[0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBuffer[0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINLEFT][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINLEFT][0], SAMPLES_IN_BUFFER);

		// calculate main right
		vecsmltf(&audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSP2_FX + i_ch][0], dsp.channelSendMainRightVolume[MAX_CHAN + i_ch], &audioTempBuffer[0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBuffer[0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINRIGHT][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINRIGHT][0], SAMPLES_IN_BUFFER);

		// calculate main sub
		vecsmltf(&audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSP2_FX + i_ch][0], dsp.channelSendMainSubVolume[MAX_CHAN + i_ch], &audioTempBuffer[0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBuffer[0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINSUB][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINSUB][0], SAMPLES_IN_BUFFER);
	}

/*
	// reset data in mixbus-buffer
	for (int i_mixbus = 0; i_mixbus < MAX_MIXBUS; i_mixbus++) {
		memset(audioBuffer[TAP_INPUT][DSP_BUF_IDX_MIXBUS + i_mixbus], 0, sizeof(audioBuffer[TAP_INPUT][DSP_BUF_IDX_MIXBUS + i_mixbus]));
	}
	// add mixbus-outputs to main
	for (int i_mixbus = 0; i_mixbus < MAX_MIXBUS; i_mixbus++) {
		// calculate main left
		vecsmltf(&audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MIXBUS + i_mixbus][0], dsp.mixbusSendMainLeftVolume[i_mixbus], &audioTempBufferLarge[i_mixbus][0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBufferLarge[i_mixbus][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINLEFT][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINLEFT][0], SAMPLES_IN_BUFFER);

		// calculate main right
		vecsmltf(&audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MIXBUS + i_mixbus][0], dsp.mixbusSendMainRightVolume[i_mixbus], &audioTempBufferLarge[i_mixbus][0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBufferLarge[i_mixbus][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINRIGHT][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINRIGHT][0], SAMPLES_IN_BUFFER);

		// calculate main sub
		vecsmltf(&audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MIXBUS + i_mixbus][0], dsp.mixbusSendMainSubVolume[i_mixbus], &audioTempBufferLarge[i_mixbus][0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBufferLarge[i_mixbus][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINSUB][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINSUB][0], SAMPLES_IN_BUFFER);
	}
*/
	// TODO: process 6-band PEQ on main L/R/S
	// TODO: process dynamics on main L/R/S

	// main-volume
	vecsmltf(&audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINLEFT][0], dsp.mainLeftVolume, &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINLEFT][0], SAMPLES_IN_BUFFER);
	vecsmltf(&audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINRIGHT][0], dsp.mainRightVolume, &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINRIGHT][0], SAMPLES_IN_BUFFER);
	vecsmltf(&audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINSUB][0], dsp.mainSubVolume, &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINSUB][0], SAMPLES_IN_BUFFER);

/*
	//  __  __    _  _____ ____  _____  __
	// |  \/  |  / \|_   _|  _ \|_ _\ \/ /
	// | |\/| | / _ \ | | | |_) || | \  /
	// | |  | |/ ___ \| | |  _ < | | /  \
	// |_|  |_/_/   \_\_| |_| \_\___/_/\_\

	// calculate matrices
	for (int i_matrix = 0; i_matrix < 6; i_matrix++) {
		// add mixbus to matrix
		for (int i_mixbus = 0; i_mixbus < MAX_MIXBUS; i_mixbus++) {
			vecsmltf(&audioBuffer[dsp.mixbusSendMatrixTapPoint[i_mixbus]][DSP_BUF_IDX_MIXBUS + i_mixbus][0], dsp.mixbusSendMatrixVolume[i_mixbus][i_matrix], &audioTempBufferLarge[i_mixbus][0], SAMPLES_IN_BUFFER);
			vecvaddf(&audioTempBufferLarge[i_mixbus][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MATRIX + i_matrix][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MATRIX + i_matrix][0], SAMPLES_IN_BUFFER);
		}

		// add main left to matrix
		vecsmltf(&audioBuffer[dsp.mainSendMatrixTapPoint[i_matrix]][DSP_BUF_IDX_MAINLEFT][0], dsp.mainSendMatrixVolume[i_matrix], &audioTempBufferLarge[i_matrix][0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBufferLarge[i_matrix][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MATRIX + i_matrix][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MATRIX + i_matrix][0], SAMPLES_IN_BUFFER);
		// add main right to matrix
		vecsmltf(&audioBuffer[dsp.mainSendMatrixTapPoint[i_matrix]][DSP_BUF_IDX_MAINRIGHT][0], dsp.mainSendMatrixVolume[i_matrix], &audioTempBufferLarge[i_matrix][0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBufferLarge[i_matrix][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MATRIX + i_matrix][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MATRIX + i_matrix][0], SAMPLES_IN_BUFFER);
		// add sub to matrix
		vecsmltf(&audioBuffer[dsp.mainSendMatrixTapPoint[i_matrix]][DSP_BUF_IDX_MAINSUB][0], dsp.mainSendMatrixVolume[i_matrix], &audioTempBufferLarge[i_matrix][0], SAMPLES_IN_BUFFER);
		vecvaddf(&audioTempBufferLarge[i_matrix][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MATRIX + i_matrix][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MATRIX + i_matrix][0], SAMPLES_IN_BUFFER);

		// TODO: process 6-band PEQ on matrices
		// TODO: process dynamics on matrices

		// volume of matrix
		vecsmltf(&audioBuffer[TAP_INPUT][DSP_BUF_IDX_MATRIX + i_matrix][0], dsp.matrixVolume[i_matrix], &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MATRIX + i_matrix][0], SAMPLES_IN_BUFFER);
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

	//  ___       _            _                  _
	// |_ _|_ __ | |_ ___ _ __| | ___  __ ___   _(_)_ __   __ _
	//  | || '_ \| __/ _ \ '__| |/ _ \/ _` \ \ / / | '_ \ / _` |
	//  | || | | | ||  __/ |  | |  __/ (_| |\ V /| | | | | (_| |
	// |___|_| |_|\__\___|_|  |_|\___|\__,_| \_/ |_|_| |_|\__, |
	//                                                    |___/
	// copy channel buffers to interleaved output-buffer
	// Ressource-Demand: ~7%
	sOffset = 0;
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = audioBufferOffset + sOffset;

		// copy data for FPGA
		tdmOffset = 0;
		tdmBufferOffset = 0;
		for (int i_tdm = 0; i_tdm < TDM_INPUTS_FPGA; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// output to FPGA as int32_t
				audioTxBuf[bufferIndex] = audioBuffer[dsp.outputTapPoint[dspCh]][dsp.outputRouting[dspCh]][s];
			}

			tdmOffset += CHANNELS_PER_TDM;
			tdmBufferOffset += (BUFFER_COUNT * BUFFER_SIZE);
		}

		// copy data for DSP2
		// keep values of tdmOffset and tdmBufferOffset from previous loop
		for (int i_tdm = TDM_INPUTS_FPGA; i_tdm < TDM_INPUTS; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// output to DSP2 (FX) as float
				memcpy(&audioTxBuf[bufferIndex], &audioBuffer[dsp.outputTapPoint[dspCh]][dsp.outputRouting[dspCh]][s], sizeof(float));
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
