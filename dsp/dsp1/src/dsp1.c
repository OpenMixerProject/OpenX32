/*
                               =#%@@@@@###%@@@@%-
                           =*###+               :#@*
                        +****.                      :%-
                      #++++             ############  :%-          @@@@@@@@@@@@
                    .+===             ###======++==*##  *+       @@@*#%%#****#@@@
                   -=-+               #+======+=.*===*#         @@#**@.*@******%@
                  -=-+                ##======*  #====+#.      @@****@  @******@@
                  +:-                  ##+====#  #***==+#+   =@@**@@@@  @****%@@
                 =*=*                   -#*===#  ## #===+## @@@***@ @@  @@#*@@@
      @@@@@@       ..                     ##+*#- ## #***==#= @@@@@@ @@   +@@@   @@@@@@@  @@@@@
    @@@    @@@                             ##= + ## ## #+==#- @@ @@ @@ = @@@    @@  @@  @@   @@
    @@      .@@#@@@@@@@  @@@ @@@ @@@@@@@@   .# # ## .= #-#++#= @ @  @@ * @*       @@*        @@
    @@       @@ @@    @@ @@@@@@@  @@   @@      # =#  = + *::=#   @  @+ *           -@@@   @@@=
    @@@    @@@  @@:   @@ @@       @@   @@      #  : .- : *::-#   @  +  #             @@ @@    @@
      @@@@@@    @@@@@@@   @@@@@@  @@   @@@  =# # ## :+ #-#++#+ @ @  @@.* @@     @@@@@@  @@@@@@@@
                @@                         ##+ * ## ## #+==#+ @@@@@ @@ = @@@
                @@                        ##+= = ## #***==#+ @@***@ @@   #@@@
                   :                    :#*==+#: ## #===+## @@@***@ @@  @@#*@@#
                  .%+                  ##+====#  #***==+#=   +@@**@@@@  @****%@@
                    %.                ##======*  #====*#  .*-  @@****@  @******@@
                     %=               #*======+: *===*#   +-=+  @@#**@ -@******@@
                      -@-             +##+=====+++=*##  ==-=-    @@@#%@@#****%@@@
                        *@*             =###########  -===*        @@@@@@@@@@@@
                           @@%.                   .::=++*
                             .#@@%%*-.    .:=+**##***+.
                                  .-+%%%%%%#***=-.

  ControlSystem for DSP1 (MainDSP) v0.1.5, 23.09.2025

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

#include "dsp1.h"
#include "system.h"
#include "audio.h"
#include "fx.h"
#include "spi.h"
#include <cycles.h>

// global data
//static volatile uint32_t timerCounter;
static cycle_stats_t systemStats;
static uint32_t cyclesMain;

/*
#pragma optimize_for_speed // interrupt handlers usually need to be optimized
#pragma section ("seg_int_code")  // handler functions perform better in internal memory
static void timerIsr(uint32_t iid, void* handlerArg) {
	volatile uint32_t *timer_counter = (uint32_t *) handlerArg;
	// Don't call standard I/O functions or update non-volatile global data in interrupt handlers

	// You can use the handler arguments to identify the interrupt being handled (iid)
	// and to access data via an interrupt specific callback pointer argument (handlerArg)
	assert(iid == ADI_CID_TMZHI || iid == ADI_CID_TMZLI);
	assert(handlerArg != NULL);

	// increment timer-counter up to 100000 and wrap around
	if (*timer_counter >= 100000) {
		*timer_counter = 0;
	}else{
		*timer_counter += 1;
	}
}
#pragma optimize_as_cmd_line // restore the optimizer settings to those for the build configuration

void delay(int i) {
    for (; i > 0; --i) {
    	NOP();
    }
}
*/

void openx32Init(void) {
	// initialize the default samplerate with 48kHz
	// other samplerates up to 192kHz are possible with AD and DA converters
	dsp.samplerate = 48000;

	// initialize states of dynamics
	for (int ch = 0; ch < MAX_CHAN; ch++) {
		dsp.dspChannel[ch].gate.state = GATE_CLOSED;
		dsp.dspChannel[ch].compressor.state = COMPRESSOR_IDLE;
	}
}

void openx32Command(unsigned short classId, unsigned short channel, unsigned short index, unsigned short valueCount, void* values) {
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
				case 'v': // version number
					tmpValueFloat = DSP_VERSION;
					spiSendValue('s', 'v', 0, tmpValueFloat); // classId='s'=Status, channel='v'=Version, index=0, value
					break;
				case 'c': // cpu load as "used cycles"
					spiSendValue_uint32('s', 'c', 0, cyclesMain); // classId='s'=Status, channel='c'=CPULoad, index=0, value
					break;
				case 'm': // meter
					for (int i = 0; i < 40; i++) {
						data[i] = audioBuffer[TAP_INPUT][BUF_IDX_DSPCHANNEL + i][0];
					}
					data[40] = audioBuffer[TAP_POST_FADER][BUF_IDX_MAINLEFT][0];
					data[41] = audioBuffer[TAP_POST_FADER][BUF_IDX_MAINRIGHT][0];
					data[42] = audioBuffer[TAP_POST_FADER][BUF_IDX_MAINSUB][0];
					spiSendArray('m', 0, 0, 43, &data); // classId='m'=Meter, channel=0=InputData, index=0, valueCount=40, value-Array
					break;
				case 'd': // dynamics (gate and compression)
					for (int i = 0; i < 40; i++) {
						data[i] = dsp.compressorGain[i];
						data[40 + i] = dsp.gateGain[i];
					}
					spiSendArray('d', 0, 0, 80, &data); // classId='d'=Dynamics, channel=0, index=0, valueCount=80, value-Array
					break;
				default:
					break;
			}
			break;
		case 'r': // DSP routing
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
				switch (index) {
					case 0: // ChannelSend-TapPoint
						dsp.channelSendMixbusTapPoint[channel][intValues[0]] = intValues[1];
						break;
					case 1: // MixbusSend-TapPoint
						dsp.mixbusSendMatrixTapPoint[channel][intValues[0]] = intValues[1];
						break;
					case 2: // MainSend-TapPoint
						dsp.mainSendMatrixTapPoint[intValues[0]] = intValues[1];
						break;
				}
			}
			break;
		case 'v': // volume
			switch (index) {
				case 0: // DSP-Channels
					if (valueCount == 4) {
						dsp.channelVolume[channel] = floatValues[0];
						dsp.channelSendMainLeftVolume[channel] = floatValues[1];
						dsp.channelSendMainRightVolume[channel] = floatValues[2];
						dsp.channelSendMainSubVolume[channel] = floatValues[3];
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
				case 1: // Mixbus-Channels
					if (valueCount == 4) {
						dsp.mixbusVolume[channel] = floatValues[0];
						dsp.mixbusSendMainLeftVolume[channel] = floatValues[1];
						dsp.mixbusSendMainRightVolume[channel] = floatValues[2];
						dsp.mixbusSendMainSubVolume[channel] = floatValues[3];
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
				case 2: // Matrix-Channels
					if (valueCount == 1) {
						dsp.matrixVolume[channel] = floatValues[0];
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
				case 3: // Main-Channels
					if (valueCount == 3) {
						dsp.mainLeftVolume = floatValues[0];
						dsp.mainRightVolume = floatValues[1];
						dsp.mainSubVolume = floatValues[2];
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
				case 4: // Monitoring
					if (valueCount == 1) {
						dsp.monitorVolume = floatValues[0];
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
			}
			break;
		case 'g': // gate
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
				case 0: // LowCut
					if (valueCount == 1) {
						dsp.lowcutCoeff[channel] = floatValues[0];
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
				case 1: // PEQ
					if (valueCount == (MAX_CHAN_EQS * 5)) {
						memcpy(&dsp.dspChannel[channel].peqCoeffs[0], &floatValues[0], valueCount * sizeof(float));
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
			}
			break;
		case 'c': // Compressor
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

int main() {
	// initialize all components
	adi_initComponents();
	systemPllInit();
	systemExternalMemoryInit();
	systemSruInit();

	openx32Init();
	spiInit();
	audioInit();
	systemSportInit();

	// install interrupt handlers (see Processor Hardware Reference v2.2 page B-5)
	adi_int_InstallHandler(ADI_CID_P1I, (ADI_INT_HANDLER_PTR)spiISR, 0, true); // SPI Interrupt (called on new SPI-data)
	adi_int_InstallHandler(ADI_CID_P3I, (ADI_INT_HANDLER_PTR)audioRxISR, 0, true); // SPORT1 Interrupt (called on new audio-data)
	//adi_int_InstallHandler(ADI_CID_TMZHI, timerIsr, (void *)&timerCounter, true); // iid - high priority core timer. Use "ADI_CID_TMZLI" for low priority

	// t_timer = (t_periode + 1) * t_count / f_clk = (1001 * 1000)/266MHz = 0.0037631579 s
	//timer_set(1000, 1000); // set period to 1000 and counter to 1000 -> count 1000 x 1000 -> 266MHz = 3.7594ns = 3.7594ms
	//timer_on(); // start timer

	// turn-off LED
	sysreg_bit_set(sysreg_FLAGS, FLG7);

	CYCLES_INIT(systemStats);

	// the main-loop
	while(1) {
		/*
		if (timerCounter == 0) {
			// toggle LED controlled by timer
			sysreg_bit_tgl(sysreg_FLAGS, FLG7); // alternative: sysreg_bit_clr() / sysreg_bit_set()
		}
		*/

		// check for new audio-data to process
		if (audioReady) {
			CYCLES_START(systemStats);

			audioProcessData();

			CYCLES_STOP(systemStats);
			cyclesMain = systemStats._cycles;
			CYCLES_RESET(systemStats);
		}

		// check for new SPI-data to process
		if (spiNewRxDataReady) {
			spiProcessRxData();
		}
	}
}
