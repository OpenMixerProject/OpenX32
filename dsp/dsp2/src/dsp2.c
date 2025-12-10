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

  ControlSystem for DSP2 (FX DSP) v0.0.4, 11.12.2025

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

#include "dsp2.h"
#include "system.h"
#include "audio.h"
#include "fx.h"
#include "spi.h"
#include <cycles.h>

// global data
//static volatile uint32_t timerCounter;
static cycle_stats_t systemStats;
static uint32_t cyclesMain;

void openx32Init(void) {
	// initialize the default samplerate with 48kHz
	// other samplerates up to 192kHz are possible with AD and DA converters
	dsp.samplerate = 48000;

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
				case 'u': // update-packet
					data[0] = DSP_VERSION;
					memcpy(&data[1], &cyclesMain, sizeof(float));
					spiSendArray('s', 'u', 0, 2, &data);
					break;
				default:
					break;
			}
			break;
		case 'v':
			switch(index) {
				case 0: // FxReturnVolume
					if (valueCount == 1) {
						dsp.channelFxReturnVolume[channel] = floatValues[0];
						sysreg_bit_tgl(sysreg_FLAGS, FLG7);
					}
					break;
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

// ISR is called once on first falling edge of FrameSync
void misc0ISR(int sig) {
	systemSportInit();

	// make sure, that this ISR is called only once
	adi_int_UninstallHandler(ADI_CID_P0I);
}

int main() {
	// initialize all components
	adi_initComponents();
	systemPllInit();
	systemExternalMemoryInit();
	systemSruInit();

	// install interrupt handlers (see Processor Hardware Reference v2.2 page B-5)
	adi_int_InstallHandler(ADI_CID_P1I, (ADI_INT_HANDLER_PTR)spiISR, 0, true); // SPI Interrupt (called on new SPI-data)
	//adi_int_InstallHandler(ADI_CID_P3I, (ADI_INT_HANDLER_PTR)audioRxISR, 0, true); // SPORT1 Interrupt (called on new audio-data)
	adi_int_InstallHandler(ADI_CID_P4I, (ADI_INT_HANDLER_PTR)audioRxISR, 0, true); // SPORT3 Interrupt (called on new audio-data)
	adi_int_InstallHandler(ADI_CID_P16I, (ADI_INT_HANDLER_PTR)audioSpdifTxISR, 0, true); // SPORT6 Interrupt (called when new data for SPDIF is requested)
	//adi_int_InstallHandler(ADI_CID_TMZHI, timerIsr, (void *)&timerCounter, true); // iid - high priority core timer. Use "ADI_CID_TMZLI" for low priority
	adi_int_InstallHandler(ADI_CID_P0I, (ADI_INT_HANDLER_PTR)misc0ISR, 0, true); // MISCA0 Interrupt on P0I or P12I

	// initialize software-parts
	openx32Init();
	spiInit();
	audioInit();

	// initialize hardware-peripherals for AES3-output
	systemPcgInit();
	systemSpdifTxInit();

	// enable interrupts
	*pDAI_IRPTL_PRI |= DAI_INT_28; // DAI Interrupt Priority Register
	*pDAI_IRPTL_FE |= DAI_INT_28; // DAI Rising Edge Interrupt Latch Register
	*pDAI_IMASK_FE |= DAI_INT_28; // DAI Rising Edge Interrupt Latch Register

	// turn-off LED
	sysreg_bit_set(sysreg_FLAGS, FLG7);

	CYCLES_INIT(systemStats);

	// the main-loop
	while(1) {
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
