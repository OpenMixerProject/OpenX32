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
	  'v' = Volume
	  'e' = PEQ
	  'g' = Gate
	  'c' = Compressor
	  'a' = Auxiliary
	*/

	float* floatValues = (float*)values;
	unsigned int* intValues = (unsigned int*)values;
	float tmpValueFloat;

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
						spiCommData[2] = audioBuffer[TAP_POST_FADER][0][DSP_BUF_IDX_MAINLEFT];
						spiCommData[3] = audioBuffer[TAP_POST_FADER][0][DSP_BUF_IDX_MAINRIGHT];
						spiCommData[4] = audioBuffer[TAP_POST_FADER][0][DSP_BUF_IDX_MAINSUB];
						// VU-meters of all DSP input-channels and dynamics
						for (int i = 0; i < 40; i++) {
							//spiCommData[5 + i] = audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSPCHANNEL + i][0];
							spiCommData[5 + i] = audioBuffer[TAP_INPUT][0][dsp.inputRouting[i]];
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
						_valueCount = 45;
						parameter = (_valueCount << 24) + (_index << 16) + (_channel << 8) + _classId;
						memcpy(&spiCommData[1], &parameter, sizeof(uint32_t));
						parameter = 0x00000023; // #
						memcpy(&spiCommData[45 + 2], &parameter, sizeof(uint32_t));


						spiCommData[2] = DSP_VERSION;
						//spiCommData[0] = heap_space_unused(0); // returns free heap in 32-bit words. ID=0: internal RAM, ID=1: external SDRAM
						memcpy(&spiCommData[3], &cyclesTotal, sizeof(uint32_t));

						// VU-meters of main-channels
						spiCommData[4] = audioBuffer[TAP_POST_FADER][0][DSP_BUF_IDX_MAINLEFT];
						spiCommData[5] = audioBuffer[TAP_POST_FADER][0][DSP_BUF_IDX_MAINRIGHT];
						spiCommData[6] = audioBuffer[TAP_POST_FADER][0][DSP_BUF_IDX_MAINSUB];
						// VU-meters of all DSP input-channels and dynamics
						for (int i = 0; i < 40; i++) {
							//spiCommData[5 + i] = audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSPCHANNEL + i][0];
							spiCommData[7 + i] = audioBuffer[TAP_INPUT][0][dsp.inputRouting[i]];
							//spiCommData[45 + i] = dsp.compressorGain[i];
							//spiCommData[85 + i] = dsp.gateGain[i];
						}

						spiDmaBegin((unsigned int*)&spiCommData[0], _valueCount + 3, false);
					#elif USE_SPI_TXD_MODE == 2
						spiCommData[2] = DSP_VERSION;
						//spiCommData[3] = heap_space_unused(0); // returns free heap in 32-bit words. ID=0: internal RAM, ID=1: external SDRAM
						memcpy(&spiCommData[3], &cyclesTotal, sizeof(uint32_t));

						spiCommData[4] = audioBuffer[TAP_POST_FADER][0][DSP_BUF_IDX_MAINLEFT];
						spiCommData[5] = audioBuffer[TAP_POST_FADER][0][DSP_BUF_IDX_MAINRIGHT];
						spiCommData[6] = audioBuffer[TAP_POST_FADER][0][DSP_BUF_IDX_MAINSUB];

						spiDmaBegin((unsigned int*)&spiCommData[0], 4, false); // start DMA-transmission and transmit the first 4 elements of spiCommData
						// after this the DMA-chain will switch to the next spi_tcb
					#endif

					break;
				default:
					break;
			}
			break;
		case 'r': // DSP routing
			if (channel >= (MAX_CHAN_FPGA + MAX_CHAN_DSP2)) {
				return;
			}

			switch (index) {
				case 0:
					if (valueCount == 2) {
						dsp.inputRouting[channel] = intValues[0];
						dsp.inputTapPoint[channel] = intValues[1];
					}
					break;
				case 1:
					if (valueCount == 2) {
						dsp.outputRouting[channel] = intValues[0];
						dsp.outputTapPoint[channel] = intValues[1];
					}
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
				case 0: // DSP-Channels
					if (channel >= (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN)) {
						return;
					}

					if (valueCount == 4) {
						dsp.channelVolumeSet[channel] = floatValues[0];
						dsp.channelSendMainLeftVolume[channel] = floatValues[1];
						dsp.channelSendMainRightVolume[channel] = floatValues[2];
						dsp.channelSendMainSubVolume[channel] = floatValues[3];
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
				case 1: // Mixbus-Channels
					if (channel >= MAX_MIXBUS) {
						return;
					}

					if (valueCount == 4) {
						dsp.mixbusVolume[channel] = floatValues[0];
						dsp.mixbusSendMainLeftVolume[channel] = floatValues[1];
						dsp.mixbusSendMainRightVolume[channel] = floatValues[2];
						dsp.mixbusSendMainSubVolume[channel] = floatValues[3];
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
				case 2: // Matrix-Channels
/*

					if (valueCount == 1) {
						dsp.matrixVolume[channel] = floatValues[0];
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
*/
					break;
				case 3: // Main-Channels
					if (valueCount == 3) {
						memcpy(&dsp.mainVolumeSet[0], &floatValues[0], 3 * sizeof(float));
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
				case 4: // Monitoring
					if (valueCount == 1) {
						dsp.monitorVolume = floatValues[0];
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
				case 5: // FX-Send-Volume
					// Valid for DSP-Channels 1-40
					if (channel >= (MAX_CHAN_FPGA)) {
						return;
					}

					if (valueCount == 16) {
						for (int i = 0; i < 16; i++) {
							dsp.channelSendFxVolume[i][channel] = floatValues[i];
						}
					}
					break;
			}
			break;
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
				dsp.dspChannel[channel].gate.value_threshold = floatValues[0];
				dsp.dspChannel[channel].gate.value_gainmin = floatValues[1];
				dsp.dspChannel[channel].gate.value_coeff_attack = floatValues[2];
				dsp.dspChannel[channel].gate.value_hold_ticks = floatValues[3];
				dsp.dspChannel[channel].gate.value_coeff_release = floatValues[4];
				sysreg_bit_tgl(sysreg_FLAGS, FLG7);
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

						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
				case 'e': // EQ
					if (channel >= (CHANNELS_WITH_4BD_EQ)) {
						return;
					}

					if ((valueCount == (MAX_CHAN_EQS * 5)) && (channel < CHANNELS_WITH_4BD_EQ)) {
						// copy biquad-coefficients
						memcpy(&dsp.peqCoeffs[channel][0], &floatValues[0], valueCount * sizeof(float));

						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
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
				dsp.dspChannel[channel].compressor.value_threshold = floatValues[0];
				dsp.dspChannel[channel].compressor.value_ratio = floatValues[1];
				dsp.compressorMakeup[channel] = floatValues[2];
				dsp.dspChannel[channel].compressor.value_coeff_attack = floatValues[3];
				dsp.dspChannel[channel].compressor.value_hold_ticks = floatValues[4];
				dsp.dspChannel[channel].compressor.value_coeff_release = floatValues[5];
				sysreg_bit_tgl(sysreg_FLAGS, FLG7);
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
									sysreg_bit_clr(sysreg_FLAGS, FLG7);
									break;
								case 1:
									sysreg_bit_set(sysreg_FLAGS, FLG7);
									break;
								default:
									sysreg_bit_tgl(sysreg_FLAGS, FLG7);
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
