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
		index 1: mixbus-send-tappoint
		index 2: main-send-tappoint
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
	'm' sends to matrix
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


	switch (classId) {
		case '?': // request-class
			switch (channel) {
				case 0:
					// use this for reading data from the txBuffer without putting new data to buffer
					break;
				case 'u': // update-packet
						spiCommData[2] = DSP_VERSION;
						spiCommData[3] = 0.0f;
						spiCommData[4] = 0.0f;

						spiDmaBegin((unsigned int*)&spiCommData[0], 5, false); // start DMA-transmission and transmit the first 5 elements of spiCommData
						// after this the DMA-chain will switch to the next spi_tcb

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
				switch (index) {
					case 0: // ChannelSend-TapPoint
						if (channel >= (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN)) {
							return;
						}
						dsp.channelSendMixbusTapPoint[intValues[0]][channel] = intValues[1];
						break;
					#if DEBUG_DISABLE_MATRIX == 0
					case 1: // MixbusSend-TapPoint
						if (channel >= (MAX_MIXBUS)) {
							return;
						}
						dsp.sendMatrixTapPoint[intValues[0]][channel] = intValues[1];
						break;
					case 2: // MainSend-TapPoint
						if (channel >= (MAX_MAIN)) {
							return;
						}
						dsp.sendMatrixTapPoint[intValues[0]][MAX_MIXBUS + channel] = intValues[1];
						break;
					#endif
				}
			}
			break;
		case 'v': // volume
			switch (index) {
				case DSP_SETTINGS_VOLUME: // Volume DSP-Channels / FX-Return / Mixbusses
					if (channel >= (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS)) {
						return;
					}

					if (valueCount == 5)
					{
						// Trim only on the 40 DSP-Channels
						if (channel < MAX_CHAN_FPGA)
						{
							dsp_settings.trim_pu[channel] = floatValues[0];
						}
						dsp.channelVolumeSet[channel] = floatValues[1];
						dsp.channelSendMainLeftVolume[channel] = floatValues[2];
						dsp.channelSendMainRightVolume[channel] = floatValues[3];
						dsp.channelSendMainSubVolume[channel] = floatValues[4];
					}
					break;

				case DSP_SETTINGS_MATRIX_VOLUME: // Matrix-Channels
					#if DEBUG_DISABLE_MATRIX == 0
					if (valueCount == 1) {
						dsp.matrixVolume[channel] = floatValues[0];
					}
					#endif
					break;
				case DSP_SETTINGS_MAIN_VOLUME: // Main-Channels
					if (valueCount == 3) {
						memcpy(&dsp.mainVolumeSet[0], &floatValues[0], 3 * sizeof(float));
					}
					break;
				case DSP_SETTINGS_MONITORING_VOLUME: // Monitoring
					if (valueCount == 1) {
						dsp.monitorVolume = floatValues[0];
					}
					break;

				case DSP_SETTINGS_SOLO: // Solo DSP-Channel / FX-Return / Mixbusses
					if (channel >= (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS)) {
						return;
					}

					if (valueCount == 2) {
						dsp.dspChannelSolo[channel] = (intValues[0] > 0);
						dsp.soloActive = (intValues[1] > 0);
					}

					break;

				case DSP_SETTINGS_MATRIX_SOLO: // Matrix Solo
					if (channel >= (MAX_MATRIX)) {
						return;
					}

					if (valueCount == 2) {
						dsp.matrixSolo[channel] = (intValues[0] > 0);
						dsp.soloActive = (intValues[1] > 0);
					}
					break;

				case DSP_SETTINGS_MAIN_SOLO: // Solo Main
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
		case 's': // sends to Mixbus
			if (valueCount == MAX_MIXBUS) {
				if (channel >= (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN)) {
					return;
				}

				for (int i = 0; i < MAX_MIXBUS; i++) {
					dsp.channelSendMixbusVolume[i][channel] = floatValues[i];
				}
			}
			break;
		case 'm': // sends to Matrix
			if (valueCount == (MAX_MIXBUS + MAX_MAIN)) {
				if (channel >= (MAX_MATRIX)) {
					return;
				}

				for (int i = 0; i < (MAX_MIXBUS + MAX_MAIN); i++) {
					dsp.sendMatrixVolume[channel][i] = floatValues[i];
				}
			}
			break;
		case 'g': // gate / expander
			if (channel >= (MAX_CHAN_FULLFEATURED)) {
				return;
			}

			if (valueCount == 9) {
				bool prevEnable = dsp.dspChannelGate[channel].value_enable;
				bool newEnable  = (floatValues[8] != 0.0f);
				dsp.dspChannelGate[channel].value_thresholdSq16 = floatValues[0];
				dsp.dspChannelGate[channel].value_gainmin       = floatValues[1];
				dsp.dspChannelGate[channel].value_coeff_attack  = floatValues[2];
				dsp.dspChannelGate[channel].value_hold_ticks    = floatValues[3];
				dsp.dspChannelGate[channel].value_coeff_release = floatValues[4];
				dsp.dspChannelGate[channel].value_thresholdDb   = floatValues[5];
				dsp.dspChannelGate[channel].value_negRangeDb    = floatValues[6];
				dsp.dspChannelGate[channel].value_expanderSlope = floatValues[7];
				dsp.dspChannelGate[channel].value_enable        = newEnable;
				if (!prevEnable && newEnable) {
					dsp.gateEnvelope[channel] = 1.0f;
					dsp.dspChannelGate[channel].holdTimer = 0;
				}
			}
			break;
		case 'e': // Equalizer/Filter
			switch (index) {
				case 'l': // LowCut (2 cascaded biquads = 10 interleaved coeffs + enable)
					if (channel >= (MAX_CHAN_FULLFEATURED)) {
						return;
					}

					if (valueCount == (5 * LOWCUT_SECTIONS) + 1) {
						bool prevEnable = dsp.lowcutEnable[channel];
						bool newEnable  = (floatValues[5 * LOWCUT_SECTIONS] != 0.0f);
						memcpy(&dsp.lowcutCoeffs[channel][0], &floatValues[0], (5 * LOWCUT_SECTIONS) * sizeof(float));
						dsp.lowcutEnable[channel] = newEnable;
						if (!prevEnable && newEnable) {
							memset(&dsp.lowcutStates[channel][0], 0, (2 * LOWCUT_SECTIONS) * sizeof(float));
						}
					}
					break;
				#if USE_HIGHCUT == 1
				case 'h': // HighCut (1 biquad = 5 coeffs + enable)
					if (channel >= (MAX_CHAN_FULLFEATURED)) {
						return;
					}

					if (valueCount == (5 * HIGHCUT_SECTIONS) + 1) {
						bool prevEnable = dsp.highcutEnable[channel];
						bool newEnable  = (floatValues[5 * HIGHCUT_SECTIONS] != 0.0f);
						memcpy(&dsp.highcutCoeffs[channel][0], &floatValues[0], (5 * HIGHCUT_SECTIONS) * sizeof(float));
						dsp.highcutEnable[channel] = newEnable;
						if (!prevEnable && newEnable) {
							memset(&dsp.highcutStates[channel][0], 0, (2 * HIGHCUT_SECTIONS) * sizeof(float));
						}
					}
					break;
				#endif
				case 'e': // EQ
					if (channel >= (CHANNELS_WITH_4BD_EQ)) {
						return;
					}

					if ((valueCount == (5 * EQ_4BD_BANDS) + 1) && (channel < CHANNELS_WITH_4BD_EQ)) {
						// copy biquad-coefficients
						memcpy(&dsp.peqCoeffs_4BD_EQ[channel][0], &floatValues[0], (valueCount - 1) * sizeof(float));
						dsp.eqActive[channel] = floatValues[5 * EQ_4BD_BANDS] != 0;
					}
					break;
				case 'r': // reset channel-parameters
					if (channel < MAX_CHAN_FULLFEATURED) {
						// reset lowcut biquad sections to passthrough
						dsp.lowcutEnable[channel] = false;
						memset(&dsp.lowcutCoeffs[channel][0], 0, (5 * LOWCUT_SECTIONS) * sizeof(float));
						dsp.lowcutCoeffs[channel][0] = 1.0f;
						dsp.lowcutCoeffs[channel][1] = 1.0f;
						memset(&dsp.lowcutStates[channel][0], 0, (2 * LOWCUT_SECTIONS) * sizeof(float));

						#if USE_HIGHCUT == 1
						// reset highcut biquad section to passthrough
						dsp.highcutEnable[channel] = false;
						memset(&dsp.highcutCoeffs[channel][0], 0, (5 * HIGHCUT_SECTIONS) * sizeof(float));
						dsp.highcutCoeffs[channel][0] = 1.0f;
						memset(&dsp.highcutStates[channel][0], 0, (2 * HIGHCUT_SECTIONS) * sizeof(float));
						#endif
					}

					if (channel < CHANNELS_WITH_4BD_EQ) {
						// initialize PEQs
						float coeffs[5] = {1, 0, 0, 0, 0}; // a0, a1, a2, b1, b2: direct passthrough
						for (int i_peq = 0; i_peq < EQ_4BD_BANDS; i_peq++) {
							fxSetPeqCoeffs(channel, i_peq, &coeffs[0]);
						}
						// reset biquad-integrators
						memset(&dsp.peqStates_4BD_EQ[channel][0], 0, 2 * EQ_4BD_BANDS * sizeof(float));
					}

					break;
			}
			break;
		case 'c': // Compressor
			if (channel >= (MAX_CHAN_FULLFEATURED)) {
				return;
			}

			if (valueCount == 9) {
				bool prevEnable = dsp.compressorEnable[channel];
				bool newEnable  = (floatValues[8] != 0.0f);
				dsp.dspChannelCompressor[channel].value_thresholdDb        = floatValues[0];
				dsp.dspChannelCompressor[channel].value_1_minus_1_by_ratio = floatValues[1]; // precalculated (1.0f - 1.0f/ratio)
				dsp.compressorMakeup[channel]                              = floatValues[2];
				dsp.dspChannelCompressor[channel].value_coeff_attack       = floatValues[3];
				dsp.dspChannelCompressor[channel].value_hold_ticks         = floatValues[4];
				dsp.dspChannelCompressor[channel].value_coeff_release      = floatValues[5];
				dsp.dspChannelCompressor[channel].value_kneeHalf           = floatValues[6];
				dsp.dspChannelCompressor[channel].value_kneeCoeff          = floatValues[7];
				dsp.compressorEnable[channel]                              = newEnable;
				if (!prevEnable && newEnable) {
					dsp.compressorEnvelope[channel] = 1.0f;
					dsp.dspChannelCompressor[channel].holdTimer = 0;
				}
			}
			break;
		default:
			break;
	}
}
