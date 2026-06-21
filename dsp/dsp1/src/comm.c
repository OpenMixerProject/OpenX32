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

#include "comm.h"

void commExecCommand(unsigned short classId, unsigned short channel, unsigned short index, unsigned short valueCount, void* values) {
	/*
	SPI ClassIds:
	====================
	'?' request-class
	'r' DSP-routing
		index 0: input routing
		index 1: output routing
	't' set tap-points
		index 0: channel-send-tappoint
	'v' volume
		index 0: dsp-channels
		index 1: mixbus-channels
		index 2: matrix-channels
		index 3: main-channels
		index 4: monitor-volume

		index 10: solo dsp-channels
		index 11: solo mixbus
		index 12: solo matrix
		index 13: solo main
	'd' delay
		'i': input-delay
		'o': output-delay
	's' sends to mixbus
	'g' gate
	'e' equalizer
		index 'l': lowcut
		index 'e': eq
		index 'r': reset
	'c' compressor
	'a' auxiliary
		index 0: led-control
	*/

	float* floatValues = (float*)values;
	unsigned int* intValues = (unsigned int*)values;
	float tmpValueFloat;

	#if USE_SPI_TXD_MODE == 1
		unsigned int parameter;
		unsigned int _classId;
		unsigned int _channel;
		unsigned int _index;
		unsigned int _valueCount;
	#endif

	switch (classId) {
		case '?': // request-class
			switch (channel) {
				case 0:
					// use this for reading data from the txBuffer without putting new data to buffer
					break;
				case 'u': // update-packet
					#if USE_SPI_TXD_MODE == 0
						spiCommData[0] = DSP_VERSION;
						//spiCommData[0] = heap_space_unused(0); // returns free heap in 32-bit words. ID=0: internal RAM, ID=1: external SDRAM
						memcpy(&spiCommData[1], &cyclesTotal, sizeof(float));

						// VU-meters of main-channels
						spiCommData[2] = audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINLEFT][0];
						spiCommData[3] = audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINRIGHT][0];
						spiCommData[4] = audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINSUB][0];
						// VU-meters of all DSP input-channels and dynamics
						for (int i = 0; i < 40; i++) {
							//spiCommData[5 + i] = audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSPCHANNEL + i][0];
							spiCommData[5 + i] = audioBuffer[TAP_INPUT][dsp.inputRouting[i]][0];
							//spiCommData[45 + i] = dsp.compressorGain[i];
							//spiCommData[85 + i] = dsp.gateGain[i];
						}
						spiSendArray('s', 'u', 0, 45, &spiCommData[0]);
						//spiSendArray('s', 'u', 0, 85, &spiCommData[0]);
						//spiSendArray('s', 'u', 0, 125, &spiCommData[0]);
					#elif USE_SPI_TXD_MODE == 1
						parameter = 0x0000002A; // *
						memcpy(&spiCommData[0], &parameter, sizeof(uint32_t));

						_classId = 's';
						_channel = 'u';
						_index = 0;
						_valueCount = 2 + 40 + 8 + 8 + 3;
						parameter = (_valueCount << 24) + (_index << 16) + (_channel << 8) + _classId;
						memcpy(&spiCommData[1], &parameter, sizeof(uint32_t));
						parameter = 0x00000023; // #
						memcpy(&spiCommData[63], &parameter, sizeof(uint32_t));


						spiCommData[2] = DSP_VERSION;
						//spiCommData[0] = heap_space_unused(0); // returns free heap in 32-bit words. ID=0: internal RAM, ID=1: external SDRAM
						memcpy(&spiCommData[3], &cyclesTotal, sizeof(uint32_t));

						// VU-meters of all DSP input-channels and dynamics
						for (int i = 0; i < 48; i++) {
							spiCommData[4 + i] = audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSPCHANNEL + i][0];
						}
						for (int i = 0; i < 8; i++) {
							spiCommData[52 + i] = audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_MIXBUS][0];
						}

						// VU-meters of main-channels
						spiCommData[60] = audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINLEFT][0];
						spiCommData[61] = audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINRIGHT][0];
						spiCommData[62] = audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINSUB][0];

						spiDmaBegin((unsigned int*)&spiCommData[0], _valueCount + 3, false);
					#elif USE_SPI_TXD_MODE == 2
						spiCommData[2] = DSP_VERSION;
						//spiCommData[3] = heap_space_unused(0); // returns free heap in 32-bit words. ID=0: internal RAM, ID=1: external SDRAM
						memcpy(&spiCommData[3], &cyclesTotal, sizeof(uint32_t));
						spiCommData[4] = audioGlitchCounter;

						spiCommData[5] = audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINLEFT][0];
						spiCommData[6] = audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINRIGHT][0];
						spiCommData[7] = audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINSUB][0];

						spiDmaBegin((unsigned int*)&spiCommData[0], 5, false); // start DMA-transmission and transmit the first 5 elements of spiCommData
						// after this the DMA-chain will switch to the next spi_tcb
					#endif

					break;
				default:
					break;
			}
			break;
		case 'r': // DSP routing
			switch (index) {
				case 0:
					if (channel >= MAX_CHAN_FPGA) {
						return;
					}

					// intValues[0] contains the outputRouting for this channel
					// intValues[1] contains the tapPoint for this channel
					dsp.inputSourcePtr[channel] = &audioBuffer[intValues[1]][intValues[0]][0];

					break;
				case 1:
					if (channel >= (MAX_CHAN_FPGA + MAX_CHAN_DSP2)) {
						return;
					}

					// intValues[0] contains the outputRouting for this channel
					// intValues[1] contains the tapPoint for this channel
					dsp.outputSourcePtr[channel] = &audioBuffer[intValues[1]][intValues[0]][0];

					break;
			}

			break;
		case 't': // set tapPoints
			if (valueCount == 2) {
				if (channel >= (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN)) {
					return;
				}

				switch (index) {
					case 0: // ChannelSend-TapPoint
						dsp.channelSendMixbusTapPoint[intValues[0]][channel] = intValues[1];
						break;
/*
					case 1: // MixbusSend-TapPoint
						dsp.sendMatrixTapPoint[intValues[0]][channel] = intValues[1];
						break;
					case 2: // MainSend-TapPoint
						dsp.mainSendMatrixTapPoint[intValues[0]] = intValues[1];
						break;
*/
				}
			}
			break;
		case 'v': // volume
			switch (index) {
				case 0: // Volume DSP-Channels / FX-Return / Mixbusses
					if (channel >= (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS)) {
						return;
					}

					if (valueCount == 4) {
						dsp.channelVolumeSet[channel] = floatValues[0];
						dsp.channelSendMainLeftVolume[channel] = floatValues[1];
						dsp.channelSendMainRightVolume[channel] = floatValues[2];
						dsp.channelSendMainSubVolume[channel] = floatValues[3];
					}
					break;
				case 1: // unused
					break;

				case 2: // Matrix-Channels
/*

					if (valueCount == 1) {
						dsp.matrixVolume[channel] = floatValues[0];
					}
*/
					break;
				case 3: // Main-Channels
					if (valueCount == 3) {
						memcpy(&dsp.mainVolumeSet[0], &floatValues[0], 3 * sizeof(float));
					}
					break;
				case 4: // Monitoring
					if (valueCount == 1) {
						dsp.monitorVolume = floatValues[0];
					}
					break;

				case 10: // Solo DSP-Channel / FX-Return / Mixbusses
					if (channel >= (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS)) {
						return;
					}

					if (valueCount == 2) {
						dsp.dspChannelSolo[channel] = (intValues[0] > 0);
						dsp.soloActive = (intValues[1] > 0);
					}

					break;

				case 11: // unused
					break;

				case 12: // Matrix Solo
					break;

				case 13: // Solo Main
					if (valueCount == 3) {
						dsp.mainLrSolo = (intValues[0] > 0);
						dsp.mainSubSolo = (intValues[1] > 0);
						dsp.soloActive = (intValues[2] > 0);
					}
					break;

				default:
					break;
			}
			break;
		#if DEBUG_DISABLE_DELAYLINE == 0
		case 'd': // delay for input or output
			if (channel >= MAX_CHAN_FPGA) {
				return;
			}

			switch (index) {
				#if DEBUG_DISABLE_INTPUTDELAY == 0
				case 'i': // input-delay
					delayLineTailOffsetInput[channel] = intValues[0];
					break;
				#endif
				#if DEBUG_DISABLE_OUTPUTDELAY == 0
				case 'o': // output-delay
					delayLineTailOffsetOutput[channel] = intValues[0];
					break;
				#endif
			}
			break;
		#endif
		case 's': // sends to Mixbus
			if (valueCount == 16) {
				if (channel >= (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN)) {
					return;
				}

				for (int i = 0; i < 16; i++) {
					#if TEST_MATRIXMULTIPLICATION_MIXBUS == 1
						dsp.channelSendMixbusVolume[channel][i] = floatValues[i];
					#else
						dsp.channelSendMixbusVolume[i][channel] = floatValues[i];
					#endif
				}
			}
			break;
		case 'g': // gate
			if (channel >= (MAX_CHAN_FULLFEATURED)) {
				return;
			}

			if (valueCount == 5) {
				dsp.dspChannelGate[channel].value_threshold = floatValues[0];
				dsp.dspChannelGate[channel].value_gainmin = floatValues[1];
				dsp.dspChannelGate[channel].value_coeff_attack = floatValues[2] * 0.0625f;
				dsp.dspChannelGate[channel].value_hold_ticks = floatValues[3];
				dsp.dspChannelGate[channel].value_coeff_release = floatValues[4] * 0.0625f;
				dsp.dspChannelGate[channel].use_rms = true; // TODO: implement in OMC
			}
			break;
		case 'e': // Equalizer/Filter
			switch (index) {
				case 'l': // LowCut
					if (channel >= (MAX_CHAN_FULLFEATURED)) {
						return;
					}

					if (valueCount == 1) {
						// copy coefficient
						dsp.lowcutCoeff[channel] = floatValues[0]; // equation = 1.0f / (1.0f + 2.0f * M_PI * desiredLowCutFrequency * (1.0f/samplerate));
					}
					break;
				case 'e': // EQ
					if (channel >= (CHANNELS_WITH_4BD_EQ)) {
						return;
					}

					if ((valueCount == (MAX_CHAN_EQS * 5)) && (channel < CHANNELS_WITH_4BD_EQ)) {
						// copy biquad-coefficients
						memcpy(&dsp.peqCoeffs[channel][0], &floatValues[0], valueCount * sizeof(float));
					}
					break;
				case 'r': // reset channel-parameters
					// init single-pole lowcut
					dsp.lowcutCoeff[channel] = 0.993497573586; // 50Hz: equation = 1.0f / (1.0f + 2.0f * M_PI * desiredLowCutFrequency * (1.0f/samplerate));
					dsp.lowcutStatesInput[channel] = 0.0; // reset integrator
					dsp.lowcutStatesOutput[channel] = 0.0; // reset integrator

					// initialize PEQs
					float coeffs[5] = {1, 0, 0, 0, 0}; // a0, a1, a2, b1, b2: direct passthrough
					for (int i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++) {
						fxSetPeqCoeffs(channel, i_peq, &coeffs[0]);
					}
					// init PEQ-states
					for (int s = 0; s < (2 * MAX_CHAN_EQS); s++) {
						dsp.peqStates[channel][s] = 0;
						dsp.peqStates[channel][s] = 0;
					}

					// reset biquad-integrators
					dsp.lowcutStatesInput[channel] = 0;
					dsp.lowcutStatesOutput[channel] = 0;
					memset(&dsp.peqStates[channel][0], 0, MAX_CHAN_EQS * 2 * sizeof(float));

					/*
					// reset the channel-configuration to have a working channel
					dsp.channelVolume[channel] = 1.0f;
					dsp.channelSendMainLeftVolume[channel] = 1.0f;
					dsp.channelSendMainRightVolume[channel] = 1.0f;
					dsp.channelSendMainSubVolume[channel] = 1.0f;
					dsp.outputTapPoint[channel] = TAP_POST_FADER;
					dsp.outputRouting[channel] = DSP_BUF_IDX_MAINLEFT;
					*/

					break;
			}
			break;
		case 'c': // Compressor
			if (channel >= (MAX_CHAN_FULLFEATURED)) {
				return;
			}

			if (valueCount == 6) {
				dsp.dspChannelCompressor[channel].value_thresholdDb = floatValues[0];
				//dsp.dspChannelCompressor[channel].value_threshold = dbToLinear_fast(floatValues[0]) * FLOAT_NORM_TO_INT32;
				dsp.dspChannelCompressor[channel].value_1_minus_1_by_ratio = floatValues[1]; // here the precalculated (1.0f - 1.0f/ratio) is sent by OMC
				dsp.compressorMakeup[channel] = floatValues[2];
				dsp.dspChannelCompressor[channel].value_coeff_attack = floatValues[3] * 0.0625f;
				dsp.dspChannelCompressor[channel].value_hold_ticks = floatValues[4];
				dsp.dspChannelCompressor[channel].value_coeff_release = floatValues[5] * 0.0625f;
				dsp.dspChannelCompressor[channel].use_rms = true; // TODO: implement in OMC
			}
			break;
		case 'a': // Auxiliary
			switch (index) {
				case 0:
					if (valueCount == 1) {
						if (channel == 42) {
							// LED Control
							switch(intValues[0]) {
								case 0:
									break;
								case 1:
									break;
								default:
									break;
							}
						}
					}
					break;
			}
			break;
		default:
			break;
	}
}
