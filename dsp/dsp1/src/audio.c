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
volatile uint32_t audioGlitchCounter = 0;
int audioBufferOffset = 0;

// audio-buffers for transmitting and receiving
// 16 Audiosamples per channel (= 333us latency)
int audioRxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch1-8 | Ch9-16 | Ch 17-24 | Ch 25-32 | AUX Ch 1-8 | DSP2 1-24
int audioTxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0}; // Ch1-8 | Ch9-16 | P16 Ch 1-8 | P16 Ch 9-16 | AUX Ch 1-8 | DSP2 1-24

// internal buffers for audio-samples
#if DEBUG_DISABLE_INTPUTDELAY == 0
	// delay-lines in the external SDRAM
	//float em delayLineInput[MAX_CHAN_FPGA][SAMPLES_IN_DELAYLINE];
	float (*delayLineInput)[SAMPLES_IN_DELAYLINE] = (float (*)[SAMPLES_IN_DELAYLINE])((void*)SDRAM_AUDIO_START);

	int delayLineHeadInput;
	int delayLineTailOffsetInput[MAX_CHAN_FPGA]; // offset = (delayMs * sampleRate / 1000)
#endif

#if DEBUG_DISABLE_OUTPUTDELAY == 0
	// delay-lines in the external SDRAM
	//float em delayLineOutput[MAX_CHAN_FPGA][SAMPLES_IN_DELAYLINE];
	float (*delayLineOutput)[SAMPLES_IN_DELAYLINE] = (float (*)[SAMPLES_IN_DELAYLINE])((void*)(SDRAM_AUDIO_START + (MAX_CHAN_FPGA * SAMPLES_IN_DELAYLINE * sizeof(float))));

	int delayLineHeadOutput;
	int delayLineTailOffsetOutput[MAX_CHAN_FPGA]; // offset = (delayMs * sampleRate / 1000)

	#pragma align 8
	float delayReadBuffer[MAX_CHAN_FPGA][SAMPLES_IN_BUFFER];
#endif

#pragma align 8 // align for 2 floats
float audioBuffer[5][1 + MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS + MAX_DSP2_FXINSERT + MAX_MATRIX + MAX_MAIN + MAX_DSP2_AUX + MAX_MONITOR][SAMPLES_IN_BUFFER]; // audioBuffer[TAPPOINT][CHANNEL][SAMPLE]
//float audioTempBufferChanA[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN] = {0};
//float audioTempBufferChanB[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN] = {0};
float sampleBuffer[SAMPLES_IN_BUFFER]; // main channels can be calculated only after the other channels, so we can save some memory here

// TCB-arrays for SPORT {CPSPx Chainpointer, ICSPx Internal Count, IMSPx Internal Modifier, IISPx Internal Index}
int audioRx_tcb[8][BUFFER_COUNT][4];
int audioTx_tcb[8][BUFFER_COUNT][4];

void audioInit(void) {
	// initialize TCB-array with multi-buffering
	// caution: chain-pointer registers must point to the LAST location in the TCB, hence tcb_address + 3
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

	// initialize variables
	dsp.monitorChannelTapPoint = TAP_INPUT;
	dsp.monitorMatrixTapPoint = TAP_INPUT; // TODO: set to TAP_PRE_FADER when EQ and dynamics are working on MixBus
	dsp.monitorMainTapPoint = TAP_POST_FADER;
	dsp.monitorVolume = 1.0f;

/*
	// initialize memories
	memset(dsp.lowcutStatesInput, 0, sizeof(dsp.lowcutStatesInput));
	memset(dsp.lowcutStatesOutput, 0, sizeof(dsp.lowcutStatesOutput));
	memset(dsp.peqStates, 0, sizeof(dsp.peqStates));
	memset(audioBuffer, 0, sizeof(audioBuffer));
*/

	#if DEBUG_DISABLE_INTPUTDELAY == 0
		delayLineHeadInput = 0;
		delayLineHeadOutput = 0;
	#endif
}

void audioSmoothVolume(void) {
	// this function smoothes the set audio-volume for a nice user-experience. It is called every 333µs
	// out = (volumeSet - volume) * coeff + volume

	// smooth audio-volume for individual channels and FX-returns
	/*
	vecvsubf(&dsp.channelVolumeSet[0], &dsp.channelVolume[0], &audioTempBufferChanA[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + ACTIVE_MIX_BUSSES); // temp = (volumeSet - volume)
	vecsmltf(&audioTempBufferChanA[0], audioVolumeSmootherCoeff, &audioTempBufferChanA[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + ACTIVE_MIX_BUSSES); // temp = temp * coeff
	vecvaddf(&dsp.channelVolume[0], &audioTempBufferChanA[0], &dsp.channelVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + ACTIVE_MIX_BUSSES); // volume = volume + temp
	*/
	for (int i = 0; i < (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + ACTIVE_MIX_BUSSES); i++) {
		dsp.channelVolume[i] = dsp.channelVolume[i] + ((dsp.channelVolumeSet[i] - dsp.channelVolume[i]) * audioVolumeSmootherCoeff);
	}

	// smooth audio-volume for mains
	/*
	vecvsubf(&dsp.mainVolumeSet[0], &dsp.mainVolume[0], &audioTempBufferChanA[0], 3); // temp = (volumeSet - volume)
	vecsmltf(&audioTempBufferChanA[0], audioVolumeSmootherCoeff, &audioTempBufferChanA[0], 3); // temp = temp * coeff
	vecvaddf(&dsp.mainVolume[0], &audioTempBufferChanA[0], &dsp.mainVolume[0], 3); // volume = volume + temp
	*/
	for (int i = 0; i < 3; i++) {
		dsp.mainVolume[i] = dsp.mainVolume[i] + ((dsp.mainVolumeSet[i] - dsp.mainVolume[i]) * audioVolumeSmootherCoeff);
	}
}

#pragma optimize_for_speed
void audioProcessData(void) {
/*
	Ressource-Demand:
	=============================
	- Baseload:			20%
	- LowCut:			2.5%
	- Noisegate:		2.0%
	- 4x EQ:			16.0% for 32 4-band-EQs (=0.5% per EQ)
	- Dynamics:			4.0%
	- Channelfader:
	- Mix-Bus (8x):     4%
	- Main-Bus:
	==============================
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

	//  ____        _          ___                   _
	// |  _ \  __ _| |_ __ _  |_ _|_ __  _ __  _   _| |_
	// | | | |/ _` | __/ _` |  | || '_ \| '_ \| | | | __|
	// | |_| | (_| | || (_| |  | || | | | |_) | |_| | |_
	// |____/ \__,_|\__\__,_| |___|_| |_| .__/ \__,_|\__|
	//                                  |_|
	// copy channels from FPGA and convert it to float
	#pragma loop_count(5) // TDM_INPUTS_FPGA
	for (int i_tdm = 0; i_tdm < TDM_INPUTS_FPGA; i_tdm++) {
	    int chBase = i_tdm * CHANNELS_PER_TDM;
	    int rxBase = (BUFFER_COUNT * BUFFER_SIZE * i_tdm) + audioBufferOffset;

	    #pragma loop_count(CHANNELS_PER_TDM)
	    for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
	        int*   src = &audioRxBuf[rxBase + i_ch];
	        float* dst = &audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSPCHANNEL + chBase + i_ch][0];

	        // Stride-Access from src to dst
	        #pragma loop_count(SAMPLES_IN_BUFFER)
	        for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
	            dst[s] = (float)src[s * CHANNELS_PER_TDM];
	        }
	    }
	}

	// copy channels from DSP2 using DMA-friendly memcpy
	static const int dsp2DstIdx[TDM_INPUTS_DSP2] = {
	    DSP_BUF_IDX_DSP2_FXRET,
	    DSP_BUF_IDX_DSP2_FXINS,
	    DSP_BUF_IDX_DSP2_AUX
	};

	#pragma loop_count(TDM_INPUTS_DSP2)
	for (int i_tdm = 0; i_tdm < TDM_INPUTS_DSP2; i_tdm++) {
	    int rxBase = (BUFFER_COUNT * BUFFER_SIZE * (TDM_INPUTS_FPGA + i_tdm)) + audioBufferOffset;
	    int dstBase = dsp2DstIdx[i_tdm];

	    #pragma loop_count(CHANNELS_PER_TDM)
	    for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
	        float* dst = &audioBuffer[TAP_INPUT][dstBase + i_ch][0];

	        #pragma loop_count(SAMPLES_IN_BUFFER)
	        for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
	            dst[s] = *(float*)&audioRxBuf[rxBase + s * CHANNELS_PER_TDM + i_ch];
	        }
	    }
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

	//  ___                   _     ____       _                  __  ____             _   _
	// |_ _|_ __  _ __  _   _| |_  |  _ \  ___| | __ _ _   _     / / |  _ \ ___  _   _| |_(_)_ __   __ _
	//  | || '_ \| '_ \| | | | __| | | | |/ _ \ |/ _` | | | |   / /  | |_) / _ \| | | | __| | '_ \ / _` |
	//  | || | | | |_) | |_| | |_  | |_| |  __/ | (_| | |_| |  / /   |  _ < (_) | |_| | |_| | | | | (_| |
	// |___|_| |_| .__/ \__,_|\__| |____/ \___|_|\__,_|\__, | /_/    |_| \_\___/ \__,_|\__|_|_| |_|\__, |
	//           |_|                                   |___/                                       |___/
	#if DEBUG_DISABLE_INTPUTDELAY == 0

		// write to SDRAM
		int wrapPoint = SAMPLES_IN_DELAYLINE - delayLineHeadInput;
		#pragma loop_count(MAX_CHAN_FPGA)
		for (int i_ch = 0; i_ch < MAX_CHAN_FPGA; i_ch++) {
			float* src = dsp.inputSourcePtr[i_ch];
			float* dst = &delayLineInput[i_ch][delayLineHeadInput];

			if (wrapPoint >= SAMPLES_IN_BUFFER) {
				// no warp-around -> use memcpy with burst-mode
				memcpy(dst, src, SAMPLES_IN_BUFFER * sizeof(float));
			} else {
				// warp-around: use two burst-writes
				memcpy(dst,                      src,            wrapPoint                       * sizeof(float));
				memcpy(&delayLineInput[i_ch][0], src + wrapPoint,(SAMPLES_IN_BUFFER - wrapPoint) * sizeof(float));
			}
		}
		// update head only once
		delayLineHeadInput += SAMPLES_IN_BUFFER;
		if (delayLineHeadInput >= SAMPLES_IN_DELAYLINE) {
			delayLineHeadInput -= SAMPLES_IN_DELAYLINE;
		}

		// read from SDRAM
		#pragma loop_count(MAX_CHAN_FPGA)
		for (int i_ch = 0; i_ch < MAX_CHAN_FPGA; i_ch++) {
			int tail = delayLineHeadInput - SAMPLES_IN_BUFFER - delayLineTailOffsetInput[i_ch];
			if (tail < 0) {
				tail += SAMPLES_IN_DELAYLINE;
			}

			float* dst = &audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0];
			int readWrap = SAMPLES_IN_DELAYLINE - tail;

			if (readWrap >= SAMPLES_IN_BUFFER) {
				// no warp-around -> use memcpy with burst-mode
				memcpy(dst, &delayLineInput[i_ch][tail], SAMPLES_IN_BUFFER * sizeof(float));
			} else {
				// warp-around: use two burst-reads
				memcpy(dst,            &delayLineInput[i_ch][tail], readWrap                        * sizeof(float));
				memcpy(dst + readWrap, &delayLineInput[i_ch][0],   (SAMPLES_IN_BUFFER - readWrap)  * sizeof(float));
			}
		}

	#else
		// route desired input-sources to one of the 40 DSP-channels directly
		#pragma loop_count(MAX_CHAN_FPGA)
		for (int i_ch = 0; i_ch < MAX_CHAN_FPGA; i_ch++) {
			float* src = &dsp.inputSourcePtr[i_ch][0];
			float* dst = &audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0];

			#pragma loop_count(SAMPLES_IN_BUFFER)
			#pragma vector_for
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
				dst[s] = src[s];
			}
		}
	#endif

	#if DEBUG_DISABLE_LOWCUT == 0
	//				  _                            _
	//				 | |    _____      _____ _   _| |_
	//				 | |   / _ \ \ /\ / / __| | | | __|
	//				 | |__| (_) \ V  V / (__| |_| | |_
	//				 |_____\___/ \_/\_/ \___|\__,_|\__|
	//
	// Single-Pole LOW-CUT: output = coeff * (zoutput + input - zinput)
	#pragma loop_count(MAX_CHAN_FULLFEATURED)
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		float* buf    = &audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0];
		float  coeff  = dsp.lowcutCoeff[i_ch];
		float  zinput = dsp.lowcutStatesInput[i_ch];
		float  zout   = dsp.lowcutStatesOutput[i_ch];

		// optimized loop for SHARC-pipeline
		#pragma loop_count(SAMPLES_IN_BUFFER)
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			float in  = buf[s];
			float out = coeff * (zout + in - zinput);
			buf[s]    = out;
			zinput    = in;
			zout      = out;
		}

		dsp.lowcutStatesInput[i_ch]  = zinput; // zinput = input
		dsp.lowcutStatesOutput[i_ch] = zout; // zoutput = output
	}
	#endif

	#if USE_HIGHCUT == 1
	//                _   _ _       _      ____      _
	//               | | | (_) __ _| |__  / ___|   _| |_
	//               | |_| | |/ _` | '_ \| |  | | | | __|
	//               |  _  | | (_| | | | | |__| |_| | |_
	//               |_| |_|_|\__, |_| |_|\____\__,_|\__|
	//                        |___/

	// Single-Pole HIGH-CUT: output = zoutput + coeff * (input - zoutput)
	#pragma loop_count(MAX_CHAN_FULLFEATURED)
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		float* buf    = &audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0];
		float  coeff  = dsp.highcutCoeff[i_ch];
		float  zout   = dsp.highcutStates[i_ch];

		// optimized loop for SHARC-pipeline
		#pragma loop_count(SAMPLES_IN_BUFFER)
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			float in  = buf[s];
			float out = zout + coeff * (in - zout);
			buf[s]    = out;
			zout      = out;
		}
		dsp.highcutStates[i_ch] = zout; // zoutput = output
	}
	#endif

	#if DEBUG_DISABLE_GATE == 0
	//				  _   _       _                      _
	//				 | \ | | ___ (_)___  ___  __ _  __ _| |_ ___
	//				 |  \| |/ _ \| / __|/ _ \/ _` |/ _` | __/ _ \
	//				 | |\  | (_) | \__ \  __/ (_| | (_| | ||  __/
	//				 |_| \_|\___/|_|___/\___|\__, |\__,_|\__\___|
	//				                         |___/
	#pragma loop_count(MAX_CHAN_FULLFEATURED)
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		float* src_dst = &audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0];
		float coeff = 0.0f;
		float refValue = 0.0f;

		if (dsp.dspChannelGate[i_ch].use_rms) {
			// calculate RMS over all 16 samples
			#pragma loop_count(16, 16, 16)
			#pragma vector_for
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
				refValue += src_dst[s] * src_dst[s];
			}
			refValue = sqrtf(refValue * 0.0625f);
		}else{
			// calculate peak over all 16 samples in buffer
			#pragma loop_count(SAMPLES_IN_BUFFER)
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
				float abs = fabsf(src_dst[s]);
				if (abs > refValue) refValue = abs;
			}
		}

		// calculation of the envelope
		float targetGainLinear = 1.0f;
		if (refValue < dsp.dspChannelGate[i_ch].value_threshold) {
			targetGainLinear = 0.0f;
		}

	    // Attack / Hold / Release Logic
	    if (targetGainLinear > dsp.gateEnvelope[i_ch]) {
	    	// Attack
	    	coeff = dsp.dspChannelGate[i_ch].value_coeff_attack;
	    	dsp.dspChannelGate[i_ch].holdTimer = dsp.dspChannelGate[i_ch].value_hold_ticks;
	    }else{
	    	// Hold -> Release
	    	if (dsp.dspChannelGate[i_ch].holdTimer > 0) {
				dsp.dspChannelGate[i_ch].holdTimer--;
				// coeff stays 0 here
			} else {
				// Release
				coeff = dsp.dspChannelGate[i_ch].value_coeff_release;
			}
	    }

		// apply calculated gain to samples
		#pragma loop_count(SAMPLES_IN_BUFFER)
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			dsp.gateEnvelope[i_ch] += coeff * (targetGainLinear - dsp.gateEnvelope[i_ch]);

			src_dst[s] *= dsp.gateEnvelope[i_ch];
		}
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
	// copy PRE_EQ-Tap to POST_EQ-TAP
	memcpy(&audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_DSPCHANNEL][0], &audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL][0], (CHANNELS_WITH_4BD_EQ - MAX_MAIN) * SAMPLES_IN_BUFFER * sizeof(float));

	#pragma loop_count(48) // CHANNELS_WITH_4BD_EQ - MAX_MAIN
	for (int i_ch = 0; i_ch < (CHANNELS_WITH_4BD_EQ - MAX_MAIN); i_ch++) {
		// apply biquad EQ on POST_EQ-Tap directly
		biquad_trans(&audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0],
					 &dsp.peqCoeffs[i_ch][0],
					 &dsp.peqStates[i_ch][0],
					 SAMPLES_IN_BUFFER,
					 MAX_CHAN_EQS);
	}
	#else
	// copy PRE_EQ-Tap to POST_EQ-TAP
	memcpy(&audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_DSPCHANNEL][0], &audioBuffer[TAP_PRE_EQ][DSP_BUF_IDX_DSPCHANNEL][0], (CHANNELS_WITH_4BD_EQ - MAX_MAIN) * SAMPLES_IN_BUFFER * sizeof(float));
	#endif

	#if DEBUG_DISABLE_DYNAMICS == 0
	//				  ____                              _
	//				 |  _ \ _   _ _ __   __ _ _ __ ___ (_) ___ ___
	//				 | | | | | | | '_ \ / _` | '_ ` _ \| |/ __/ __|
	//				 | |_| | |_| | | | | (_| | | | | | | | (__\__ \
	//				 |____/ \__, |_| |_|\__,_|_| |_| |_|_|\___|___/
	//				        |___/

	#pragma loop_count(MAX_CHAN_FULLFEATURED)
	for (int i_ch = 0; i_ch < MAX_CHAN_FULLFEATURED; i_ch++) {
		float* src = &audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_DSPCHANNEL + i_ch][0];
		float* dst = &audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0];
		float makeUp = dsp.compressorMakeup[i_ch];
		float coeff;
		float refValue = 0.0f;

		if (dsp.dspChannelGate[i_ch].use_rms) {
			// calculate RMS over all 16 samples
			#pragma loop_count(16, 16, 16)
			#pragma vector_for
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
				refValue += src[s] * src[s];
			}
			refValue = sqrtf(refValue * 0.0625f);
		}else{
			// calculate peak over all 16 samples in buffer
			#pragma loop_count(SAMPLES_IN_BUFFER)
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
				float abs = fabsf(src[s]);
				if (abs > refValue) refValue = abs;
			}
		}

		// calculation of the envelope
		refValue = linearToDb_fast(refValue * INT32_TO_FLOAT_NORM);
		float targetGainDb = 0.0f;
		if (refValue > dsp.dspChannelCompressor[i_ch].value_thresholdDb) {
			targetGainDb = (dsp.dspChannelCompressor[i_ch].value_thresholdDb - refValue) * dsp.dspChannelCompressor[i_ch].value_1_minus_1_by_ratio;
		}

		// Attack / Hold / Release Logic
		float targetGainLinear = dbToLinear_fast(targetGainDb);

		// calculation of the envelope
		if (targetGainLinear < dsp.compressorEnvelope[i_ch]) {
			// Attack
			coeff = dsp.dspChannelCompressor[i_ch].value_coeff_attack;
			dsp.dspChannelCompressor[i_ch].holdTimer = dsp.dspChannelCompressor[i_ch].value_hold_ticks; // Reset Hold-Timer
		} else {
			// Hold -> Release
			if (dsp.dspChannelCompressor[i_ch].holdTimer > 0) {
				dsp.dspChannelCompressor[i_ch].holdTimer--;
			}else{
				coeff = dsp.dspChannelCompressor[i_ch].value_coeff_release;
			}
		}

		// apply calculated gain to samples
		#pragma loop_count(SAMPLES_IN_BUFFER)
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			dsp.compressorEnvelope[i_ch] += coeff * (targetGainLinear - dsp.compressorEnvelope[i_ch]);

			// use small low-pass-filter to smooth the envelope even more
			//dsp.compressorGainSmoothed[i_ch] = dsp.compressorEnvelope[i_ch] + 0.990f * (dsp.compressorGainSmoothed[i_ch] - dsp.compressorEnvelope[i_ch]);
			//dst[s] = src[s] * dsp.compressorGainSmoothed[i_ch] * makeUp;

			// direct use of envelope
			dst[s] = src[s] * dsp.compressorEnvelope[i_ch] * makeUp;
		}
	}

    // bypass: channels that don't have full-featured processing
    int bypassStart = DSP_BUF_IDX_DSPCHANNEL + MAX_CHAN_FULLFEATURED;
    int bypassCount = MAX_CHAN_FPGA + MAX_DSP2_FXRETURN - MAX_CHAN_FULLFEATURED;

    memcpy(&audioBuffer[TAP_PRE_FADER][bypassStart][0], &audioBuffer[TAP_POST_EQ] [bypassStart][0], bypassCount * SAMPLES_IN_BUFFER * sizeof(float));

	#else
    // no dynamics: copy all channels POST_EQ -> PRE_FADER
    memcpy(&audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], &audioBuffer[TAP_POST_EQ] [DSP_BUF_IDX_DSPCHANNEL + i_ch][0], (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN) * SAMPLES_IN_BUFFER * sizeof(float));
	#endif

	// copy data for DSP2-FX-Return-Channels from TAP_INPUT to TAP_PRE_FADER without processing. All other DSP2-channel have no volume-control yet
	memcpy(&audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSP2_FXRET][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSP2_FXRET][0], MAX_DSP2_FXRETURN * SAMPLES_IN_BUFFER * sizeof(float));

	//   ____ _                            _   _____         _
	//  / ___| |__   __ _ _ __  _ __   ___| | |  ___|_ _  __| | ___ _ __
	// | |   | '_ \ / _` | '_ \| '_ \ / _ \ | | |_ / _` |/ _` |/ _ \ '__|
	// | |___| | | | (_| | | | | | | |  __/ | |  _| (_| | (_| |  __/ |
	//  \____|_| |_|\__,_|_| |_|_| |_|\___|_| |_|  \__,_|\__,_|\___|_|
	// calculate channel volume
	// --------------------------------------------------------

	#pragma loop_count(48) // MAX_CHAN_FPGA + MAX_DSP2_FXRETURN
	for (int i_ch = 0; i_ch < (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN); i_ch++) {
		float* src = &audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0];
		float* dst = &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0];

		#pragma loop_count(SAMPLES_IN_BUFFER)
		#pragma vector_for
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			dst[s] = dsp.channelVolume[i_ch] * src[s];
		}
	}

	#if DEBUG_DISABLE_MIXBUS == 0
	//				  __  __ _____  ______  _   _ ____
	//				 |  \/  |_ _\ \/ / __ )| | | / ___|
	//				 | |\/| || | \  /|  _ \| | | \___ \
	//				 | |  | || | /  \| |_) | |_| |___) |
	//				 |_|  |_|___/_/\_\____/ \___/|____/
	// calculate mixbus

	// Step 1: Set Mixbus-Buffer to zero using memset
	float mixbusAcc[ACTIVE_MIX_BUSSES][SAMPLES_IN_BUFFER];
	memset(mixbusAcc, 0, sizeof(mixbusAcc));

	// Step 2: Accumulate all channels
	#pragma loop_count(48) // MAX_CHAN_FPGA + MAX_DSP2_FXRETURN
	for (int i_ch = 0; i_ch < (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN); i_ch++) {
		float* src = &audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][0];

		#pragma loop_count(ACTIVE_MIX_BUSSES)
		for (int i_bus = 0; i_bus < ACTIVE_MIX_BUSSES; i_bus++) {
			float gain = dsp.channelSendMixbusVolume[i_bus][i_ch];

			// only calculate if channel is routed to mixbus
			if (gain == 0.0f) continue;

			float* acc = &mixbusAcc[i_bus][0];

			#pragma loop_count(SAMPLES_IN_BUFFER)
			#pragma vector_for
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
				acc[s] += gain * src[s];
			}
		}
	}

	// Step 3: write new samples back to main-buffer
	memcpy(&audioBuffer[TAP_INPUT][DSP_BUF_IDX_MIXBUS][0], &mixbusAcc[0][0], ACTIVE_MIX_BUSSES * SAMPLES_IN_BUFFER * sizeof(float));

	/*
	// the following code takes 30% DSP-Load for 16 Mix-Busses, so we reduce it to 8 at the moment
	// the following code takes 15% DSP-Load for 8 Mix-Busses (obviously it scales pretty linear)
	for (int i_mixbus = 0; i_mixbus < ACTIVE_MIX_BUSSES; i_mixbus++) {
		// calculated summarized audio for each mixbus-channel
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			// vecdotf(const float dm a[],	const float dm b[], int samples) -> A dot B = A0*B0 + A1*B1 + A2*B2 + ...
			audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MIXBUS + i_mixbus] = vecdotf(&audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMixbusVolume[i_mixbus][0], (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN));
		}

		// TODO: process dynamics on mixbusses
		// TODO: process 6-band PEQ on mixbusses
	}
	*/

/*
	// the following code takes 30% DSP-Load for only 8 Mix-Busses, so vecdotf() seems to be a good choice here
	// multiply mixbus-signals using SIMD-support
	// vecdotf(...) seems to produce quite a lot of overhead, so we use simple loops for multiplication so that the compiler will
	// translate these loops into SIMD-commands using the parallel-MAC-feature of the SHARC
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		float* src = &audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_DSPCHANNEL];
	    float sum[8] = {0};

	    for (int i_ch = 0; i_ch < (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN); i_ch++) {
	    	for (int i_mixbus = 0; i_mixbus < 8; i_mixbus++) {
	    		sum[i_mixbus] += src[i_ch] * dsp.channelSendMixbusVolume[i_mixbus][i_ch];
	    	}
	    }

	    for (int i_mixbus = 0; i_mixbus < 8; i_mixbus++) {
	    	audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MIXBUS + i_mixbus] = sum[i_mixbus];
	    }
	}
*/


	// mixbus-volume
	/*
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		vecvmltf(&audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MIXBUS], &dsp.mixbusVolume[0], &audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_MIXBUS], ACTIVE_MIX_BUSSES);
	}
	*/

	#if DEBUG_DISABLE_EQMIXBUS == 0

	// Hardware-Accelerated Biquad-Filter
	#pragma loop_count(ACTIVE_MIX_BUSSES)
	for (int i_ch = 0; i_ch < ACTIVE_MIX_BUSSES; i_ch++) {
		// copy INPUT-Tap to POST_EQ-TAP
		memcpy(&audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_MIXBUS + i_ch][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MIXBUS + i_ch][0], SAMPLES_IN_BUFFER * sizeof(float));

		// apply biquad EQ on POST_EQ-Tap directly
		biquad_trans(&audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_MIXBUS + i_ch][0],
					 &dsp.peqCoeffs[i_ch][0], // TODO: add coefficients for MixBus
					 &dsp.peqStates[i_ch][0], // TODO: add states for MixBus
					 SAMPLES_IN_BUFFER,
					 MAX_CHAN_EQS);
	}


	#pragma loop_count(ACTIVE_MIX_BUSSES)
	for (int i_ch = 0; i_ch < ACTIVE_MIX_BUSSES; i_ch++) {
		float* src = &audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_MIXBUS + i_ch][0];
		float* dst = &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MIXBUS + i_ch][0];

		#pragma loop_count(SAMPLES_IN_BUFFER)
		#pragma vector_for
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			dst[s] = dsp.channelVolume[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + i_ch] * src[s];
		}
	}
	#else
	#pragma loop_count(ACTIVE_MIX_BUSSES)
	for (int i_ch = 0; i_ch < ACTIVE_MIX_BUSSES; i_ch++) {
		float* src = &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MIXBUS + i_ch][0];
		float* dst = &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MIXBUS + i_ch][0];

		#pragma loop_count(SAMPLES_IN_BUFFER)
		#pragma vector_for
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			dst[s] = dsp.channelVolume[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + i_ch] * src[s];
		}
	}
	#endif

	//				  __  __       _              ___        _
	//				 |  \/  | __ _(_)_ __        / _ \ _   _| |_
	//				 | |\/| |/ _` | | '_ \ _____| | | | | | | __|
	//				 | |  | | (_| | | | | |_____| |_| | |_| | |_
	//				 |_|  |_|\__,_|_|_| |_|      \___/ \__,_|\__|
	// calculate summarized main left, right and sub. Source: 40 Channels from FPGA, 24 Channels from DSP2, 16 Channels Mixbus

	#pragma loop_count(SAMPLES_IN_BUFFER)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		float sumL = 0;
		float sumR = 0;
		float sumS = 0;

		#pragma loop_count(64) // MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + ACTIVE_MIX_BUSSES
		for (int i_ch = 0; i_ch < (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + ACTIVE_MIX_BUSSES); i_ch++) {
			float src = audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][s];
			sumL += src * dsp.channelSendMainLeftVolume[i_ch];
			sumR += src * dsp.channelSendMainRightVolume[i_ch];
			sumS += src * dsp.channelSendMainSubVolume[i_ch];
		}

		audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINLEFT][s]  = sumL;
		audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINRIGHT][s] = sumR;
		audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINSUB][s]   = sumS;
	}


	// vecdotf(const float dm a[],	const float dm b[], int samples) -> A dot B = A0*B0 + A1*B1 + A2*B2 + ...
/*
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINLEFT] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainLeftVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + ACTIVE_MIX_BUSSES);
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINRIGHT] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainRightVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + ACTIVE_MIX_BUSSES);
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINSUB] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainSubVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + ACTIVE_MIX_BUSSES);
	}
*/

	#else
	//				  __  __       _              ___        _
	//				 |  \/  | __ _(_)_ __        / _ \ _   _| |_
	//				 | |\/| |/ _` | | '_ \ _____| | | | | | | __|
	//				 | |  | | (_| | | | | |_____| |_| | |_| | |_
	//				 |_|  |_|\__,_|_|_| |_|      \___/ \__,_|\__|
	// calculate summarized main left, right and sub. Source: 40 Channels from FPGA, 24 Channels from DSP2
	// vecdotf(const float dm a[],	const float dm b[], int samples) -> A dot B = A0*B0 + A1*B1 + A2*B2 + ...

/*
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINLEFT] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainLeftVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN);
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINRIGHT] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainRightVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN);
		audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MAINSUB] = vecdotf(&audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_DSPCHANNEL], &dsp.channelSendMainSubVolume[0], MAX_CHAN_FPGA + MAX_DSP2_FXRETURN);
	}
*/

	// multiply main-signals using SIMD-support
	// vecdotf(...) seems to produce quite a lot of overhead, so we use simple loops for multiplication so that the compiler will
	// translate these loops into SIMD-commands using the parallel-MAC-feature of the SHARC
	// this takes 5% less load compared to vecdotf()-function
	#pragma loop_count(SAMPLES_IN_BUFFER)
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
	    float sumL = 0;
		float sumR = 0;
	    float sumS = 0;

		#pragma loop_count(48) // MAX_CHAN_FPGA + MAX_DSP2_FXRETURN
	    for (int i_ch = 0; i_ch < (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN); i_ch++) {
		    float src = audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL + i_ch][s];
	        sumL += src * dsp.channelSendMainLeftVolume[i_ch];
	        sumR += src * dsp.channelSendMainRightVolume[i_ch];
	        sumS += src * dsp.channelSendMainSubVolume[i_ch];
	    }

	    audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINLEFT][s]  = sumL;
	    audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINRIGHT][s] = sumR;
	    audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINSUB][s]   = sumS;
	}
    #endif

	//	 __  __       _             _______  __  _____                  _ _
	//	|  \/  | __ _(_)_ __    _  |  ___\ \/ / | ____|__ _ _   _  __ _| (_)_______ _ __
	//	| |\/| |/ _` | | '_ \ _| |_| |_   \  /  |  _| / _` | | | |/ _` | | |_  / _ \ '__|
	//	| |  | | (_| | | | | |_   _|  _|  /  \  | |__| (_| | |_| | (_| | | |/ /  __/ |
	//	|_|  |_|\__,_|_|_| |_| |_| |_|   /_/\_\ |_____\__, |\__,_|\__,_|_|_/___\___|_|
	//	                                                 |_|
	// Hardware-Accelerated Biquad-Filter for the Main-Channels Left/Right/Sub
	// copy samples into new array
	#if DEBUG_DISABLE_EQMAIN == 0

	// copy INPUT-Tap to POST_EQ-TAP
	memcpy(&audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_DSPCHANNEL][0], &audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSPCHANNEL][0], MAX_MAIN * SAMPLES_IN_BUFFER * sizeof(float));

	for (int i_ch = 0; i_ch < MAX_MAIN; i_ch++) {
		// apply biquad EQ on POST_EQ-Tap directly
		biquad_trans(&audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_MAINLEFT + i_ch][0],
					 &dsp.peqCoeffs[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + i_ch][0],
					 &dsp.peqStates[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + i_ch][0],
					 SAMPLES_IN_BUFFER,
					 MAX_CHAN_EQS);
	}

	// TODO: process dynamics on main L/R/S

	// main-volume
	#pragma loop_count(3)
	for (int i_ch = 0; i_ch < 3; i_ch++) {
		float* src = &audioBuffer[TAP_POST_EQ][DSP_BUF_IDX_MAINLEFT + i_ch][0];
		float gain = dsp.mainVolume[i_ch];
		float* dst = &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINLEFT + i_ch][0];

		#pragma loop_count(SAMPLES_IN_BUFFER)
		#pragma vector_for
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			dst[s] = gain * src[s];
		}
	}
	#else
	// main-volume
	#pragma loop_count(3)
	for (int i_ch = 0; i_ch < 3; i_ch++) {
		float* src = &audioBuffer[TAP_INPUT][DSP_BUF_IDX_MAINLEFT + i_ch][0];
		float gain = dsp.mainVolume[i_ch];
		float* dst = &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINLEFT + i_ch][0];

		#pragma loop_count(SAMPLES_IN_BUFFER)
		#pragma vector_for
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			dst[s] = gain * src[s];
		}
	}
	#endif

	//  __  __    _  _____ ____  _____  __
	// |  \/  |  / \|_   _|  _ \|_ _\ \/ /
	// | |\/| | / _ \ | | | |_) || | \  /
	// | |  | |/ ___ \| | |  _ < | | /  \
	// |_|  |_/_/   \_\_| |_| \_\___/_/\_\
	// calculate matrices
	#if DEBUG_DISABLE_MATRIX == 0
	for (int i_matrix = 0; i_matrix < 6; i_matrix++) {
		// calculated summarized audio for each matrix-channel
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			// vecdotf(const float dm a[],	const float dm b[], int samples) -> A dot B = A0*B0 + A1*B1 + A2*B2 + ...
			audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MATRIX + i_matrix] = vecdotf(&audioBuffer[TAP_PRE_FADER][s][DSP_BUF_IDX_MIXBUS], &dsp.sendMatrixVolume[i_matrix][0], ACTIVE_MIX_BUSSES + 3);
		}

		// TODO: process dynamics on mixbusses
		// TODO: process 6-band PEQ on mixbusses
	}
	// matrix-volume
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		vecvmltf(&audioBuffer[TAP_INPUT][s][DSP_BUF_IDX_MATRIX], &dsp.matrixVolume[0], &audioBuffer[TAP_POST_FADER][s][DSP_BUF_IDX_MATRIX], MAX_MATRIX);
	}
	#endif

	//  __  __  ___  _   _ ___ _____ ___  ____  ___ _   _  ____
	// |  \/  |/ _ \| \ | |_ _|_   _/ _ \|  _ \|_ _| \ | |/ ___|
	// | |\/| | | | |  \| || |  | || | | | |_) || ||  \| | |  _
	// | |  | | |_| | |\  || |  | || |_| |  _ < | || |\  | |_| |
	// |_|  |_|\___/|_| \_|___| |_| \___/|_| \_\___|_| \_|\____|

	#if DEBUG_DISABLE_MONITOR == 0
	if (dsp.soloActive) {
		float* monLeft = &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MONLEFT][0];
		float* monRight = &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MONRIGHT][0];

		// clear monitor-buffer
		memset(monLeft, 0, SAMPLES_IN_BUFFER * sizeof(float));

		// accumulate the soloed channels pre-fader into MonitorL/R
		for (int i_ch = 0; i_ch < (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + ACTIVE_MIX_BUSSES); i_ch++) {
			if (dsp.dspChannelSolo[i_ch]) {
			    float* src = &audioBuffer[dsp.monitorChannelTapPoint][DSP_BUF_IDX_DSPCHANNEL + i_ch][0];

				#pragma loop_count(SAMPLES_IN_BUFFER)
				#pragma vector_for
				for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
					monLeft[s] += src[s];
				}
			}
		}

		if (dsp.mainLrSolo) {
			#pragma loop_count(SAMPLES_IN_BUFFER)
			#pragma vector_for
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
				monLeft[s] += audioBuffer[dsp.monitorMainTapPoint][DSP_BUF_IDX_MAINLEFT][s];
				monLeft[s] += audioBuffer[dsp.monitorMainTapPoint][DSP_BUF_IDX_MAINRIGHT][s];
			}
		}

		if (dsp.mainSubSolo) {
			#pragma loop_count(SAMPLES_IN_BUFFER)
			#pragma vector_for
			for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
				monLeft[s] += audioBuffer[dsp.monitorMainTapPoint][DSP_BUF_IDX_MAINSUB][s];
			}
		}

		#pragma loop_count(SAMPLES_IN_BUFFER)
		#pragma vector_for
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			monLeft[s] = monLeft[s] * dsp.monitorVolume;
		}
		// copy left channel to right channel
		memcpy(&monRight[0], &monLeft[0], SAMPLES_IN_BUFFER * sizeof(float));
	}else{
		// no soloed channels. Put MainL/R to MonitorL/R
		#pragma loop_count(SAMPLES_IN_BUFFER)
		#pragma vector_for
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MONLEFT][s] = dsp.monitorVolume * audioBuffer[dsp.monitorMainTapPoint][DSP_BUF_IDX_MAINLEFT][s];
			audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MONRIGHT][s] = dsp.monitorVolume * audioBuffer[dsp.monitorMainTapPoint][DSP_BUF_IDX_MAINRIGHT][s];
		}
	}
	#endif

	// ========================================================

	//  ____             _   _                  __   ___        _               _     ____       _
	// |  _ \ ___  _   _| |_(_)_ __   __ _     / /  / _ \ _   _| |_ _ __  _   _| |_  |  _ \  ___| | __ _ _   _
	// | |_) / _ \| | | | __| | '_ \ / _` |   / /  | | | | | | | __| '_ \| | | | __| | | | |/ _ \ |/ _` | | | |
	// |  _ < (_) | |_| | |_| | | | | (_| |  / /   | |_| | |_| | |_| |_) | |_| | |_  | |_| |  __/ | (_| | |_| |
	// |_| \_\___/ \__,_|\__|_|_| |_|\__, | /_/     \___/ \__,_|\__| .__/ \__,_|\__| |____/ \___|_|\__,_|\__, |
	//                               |___/                         |_|                                   |___/

	#if DEBUG_DISABLE_OUTPUTDELAY == 0
	// write to SDRAM
		#if DEBUG_DISABLE_INPUTDELAY == 0
			wrapPoint = SAMPLES_IN_DELAYLINE - delayLineHeadOutput;
		#else
			int wrapPoint = SAMPLES_IN_DELAYLINE - delayLineHeadOutput;
		#endif
	#pragma loop_count(MAX_CHAN_FPGA)
	for (int i_ch = 0; i_ch < MAX_CHAN_FPGA; i_ch++) {
		float* src = dsp.outputSourcePtr[i_ch];
		float* dst = &delayLineOutput[i_ch][delayLineHeadOutput];

		if (wrapPoint >= SAMPLES_IN_BUFFER) {
			// no warp-around -> use memcpy with burst-mode
			memcpy(dst, src, SAMPLES_IN_BUFFER * sizeof(float));
		} else {
			// warp-around: use two burst-writes
			memcpy(dst,                       src,             wrapPoint                       * sizeof(float));
			memcpy(&delayLineOutput[i_ch][0], src + wrapPoint, (SAMPLES_IN_BUFFER - wrapPoint) * sizeof(float));
		}
	}
	// update head only once
	delayLineHeadOutput += SAMPLES_IN_BUFFER;
	if (delayLineHeadOutput >= SAMPLES_IN_DELAYLINE) {
		delayLineHeadOutput -= SAMPLES_IN_DELAYLINE;
	}

	// read from SDRAM
	#pragma loop_count(MAX_CHAN_FPGA)
	for (int i_ch = 0; i_ch < MAX_CHAN_FPGA; i_ch++) {
		int tail = delayLineHeadOutput - SAMPLES_IN_BUFFER - delayLineTailOffsetOutput[i_ch];
		if (tail < 0) tail += SAMPLES_IN_DELAYLINE;

		float* dst = &delayReadBuffer[i_ch][0];
		int readWrap = SAMPLES_IN_DELAYLINE - tail;

		if (readWrap >= SAMPLES_IN_BUFFER) {
			// no warp-around -> use memcpy with burst-mode
			memcpy(dst, &delayLineOutput[i_ch][tail], SAMPLES_IN_BUFFER * sizeof(float));
		} else {
			// wrap-around: use two burst-reads
			memcpy(dst,            &delayLineOutput[i_ch][tail], readWrap                        * sizeof(float));
			memcpy(dst + readWrap, &delayLineOutput[i_ch][0],   (SAMPLES_IN_BUFFER - readWrap)  * sizeof(float));
		}
	}
	#endif

	// copy channel buffers to interleaved output-buffer
	sampleOffset = 0;
	#pragma loop_count(SAMPLES_IN_BUFFER)
	#pragma vector_for
	for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
		bufferSampleIndex = audioBufferOffset + sampleOffset;

		// copy data as interleaved audio for FPGA (with output-delay)
		tdmOffset = 0;
		tdmBufferOffset = 0;
		for (int i_tdm = 0; i_tdm < TDM_INPUTS_FPGA; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			#pragma loop_count(CHANNELS_PER_TDM)
			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// output to FPGA as int32_t
				#if DEBUG_DISABLE_OUTPUTDELAY == 0
					audioTxBuf[bufferIndex] = delayReadBuffer[dspCh][s];
				#else
					audioTxBuf[bufferIndex] = dsp.outputSourcePtr[dspCh][s];
				#endif
			}

			tdmOffset += CHANNELS_PER_TDM;
			tdmBufferOffset += (BUFFER_COUNT * BUFFER_SIZE);
		}

		// copy data for DSP2 (without delay)
		// keep values of tdmOffset and tdmBufferOffset from previous loop
		#pragma loop_count(3)
		for (int i_tdm = TDM_INPUTS_FPGA; i_tdm < TDM_INPUTS; i_tdm++) {
			bufferTdmIndex = bufferSampleIndex + tdmBufferOffset;

			// output to DSP2 (FX) as float
			for (int i_ch = 0; i_ch < CHANNELS_PER_TDM; i_ch++) {
				dspCh = tdmOffset + i_ch;
				bufferIndex = (bufferTdmIndex + i_ch);

				// output to DSP2 as float
				*(float*)&audioTxBuf[bufferIndex] = dsp.outputSourcePtr[dspCh][s];
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
    	audioGlitchCounter++;

    	// this is not nice but without a debugger and profiling tools this is the easiest method to check if the algorithms are within the timing
    	systemCrash();
    }

    audioReady = 1; // set flag, that we have new data to process
}
