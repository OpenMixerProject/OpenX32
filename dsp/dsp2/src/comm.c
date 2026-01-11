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
	float data[80];
	float tmpValueFloat;

	switch (classId) {
		case '?': // request-class
			switch (channel) {
				case 0:
					// use this for reading data from the txBuffer without putting new data to buffer
					break;
				case 'u': // update-packet
					data[0] = DSP_VERSION;
					memcpy(&data[1], &cyclesTotal, sizeof(float));
					spiSendArray('s', 'u', 0, 2, &data);
					break;
				default:
					break;
			}
			break;
		case 'v':
			switch(index) {
				default:
					break;
			}
			break;
		case 'e': // Equalizer/Filter
			switch (index) {
			case 'e': // EQ
				if (valueCount == (MAX_CHAN_EQS * 5)) {
					// copy biquad-coefficients
					//memcpy(&dsp.dspChannel[channel].peqCoeffsSet[0], &floatValues[0], valueCount * sizeof(float));
					memcpy(&dsp.dspChannel[channel].peqCoeffs[0], &floatValues[0], valueCount * sizeof(float));

					// reset biquad-integrators
					memset(&dsp.dspChannel[channel].peqStates[0], 0, MAX_CHAN_EQS * 2 * sizeof(float));

					sysreg_bit_tgl(sysreg_FLAGS, FLG7);
				}
				break;
			}
			break;
		case 'a': // Auxiliary
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
		default:
			break;
	}

	// for later use: enable DMA-transmission via SPI
	// read data via DMA
	// spiDmaBegin(true, 20);
	// send data via DMA
	// spiDmaBegin(false, 20);
}
