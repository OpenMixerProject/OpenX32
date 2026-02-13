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

  ControlSystem for DSP2 (FX DSP) v0.1.0, 19.01.2026

  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025-2026 OpenMixerProject
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
#include "spi.h"
#include <cycles.h>

// global data
//static volatile uint32_t timerCounter;
static cycle_stats_t systemStats; // static limits this global variable to this file
static uint32_t cyclesAudio; // static limits this global variable to this file
static uint32_t cyclesMain; // static limits this global variable to this file
uint32_t cyclesTotal;
sDsp dsp;
float pm peqCoeffs[5 * 4]; // storage for IIR-coefficients in PM for biquad_trans
uint32_t spiTimeoutCounter = 0;

#pragma section("seg_int_code")
void openx32Init(void) {
	// initialize the default samplerate with 48kHz
	// other samplerates up to 192kHz are possible with AD and DA converters
	dsp.samplerate = 48000;
}

// ISR is called once on first falling edge of FrameSync
#pragma section("seg_int_code")
void misc0ISR(int sig) {
	systemSportInit();

	// make sure, that this ISR is called only once
	adi_int_UninstallHandler(ADI_CID_P0I);
}

#pragma section("seg_int_code")
int main() {
	// initialize all components
	adi_initComponents();
	systemPllInit();
	systemExternalMemoryInit();
	systemSruInit();
	openx32Init();
	spiInit();
	audioInit();

	// initialize hardware-peripherals for AES3-output
	systemPcgInit();
	systemSpdifTxInit();

	// install interrupt handlers (see Processor Hardware Reference v2.2 page B-5)
	adi_int_InstallHandler(ADI_CID_P1I, (ADI_INT_HANDLER_PTR)spiISR, 0, true); // SPI Interrupt (called on new SPI-data)
	//adi_int_InstallHandler(ADI_CID_P3I, (ADI_INT_HANDLER_PTR)audioRxISR, 0, true); // SPORT1 Interrupt (called on new audio-data)
	adi_int_InstallHandler(ADI_CID_P4I, (ADI_INT_HANDLER_PTR)audioRxISR, 0, true); // SPORT3 Interrupt (called on new audio-data)
	adi_int_InstallHandler(ADI_CID_P16I, (ADI_INT_HANDLER_PTR)audioSpdifTxISR, 0, true); // SPORT6 Interrupt (called when new data for SPDIF is requested)
	//adi_int_InstallHandler(ADI_CID_TMZHI, timerIsr, (void *)&timerCounter, true); // iid - high priority core timer. Use "ADI_CID_TMZLI" for low priority
	adi_int_InstallHandler(ADI_CID_P0I, (ADI_INT_HANDLER_PTR)misc0ISR, 0, true); // MISCA0 Interrupt on P0I or P12I

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
			cyclesAudio = systemStats._cycles;
			cyclesTotal = cyclesAudio + cyclesMain;
			CYCLES_RESET(systemStats);

			spiTimeoutCounter++; // will be incremented every 333 microseconds
		}

		// check for new SPI-data to process
		if (spiNewRxDataReady) {
			spiTimeoutCounter = 0; // reset SPI counter

			CYCLES_START(systemStats);

			spiProcessRxData();

			CYCLES_STOP(systemStats);
			cyclesMain = systemStats._cycles;
			CYCLES_RESET(systemStats);
		}

		// check if we have received some data over SPI within the last 250ms
		// we are receiving audio every 333 microseconds. 750 * 0.333us = 250ms
		if (spiTimeoutCounter >= 750) {
			// we ran into a SPI-timeout -> reset SPI system
			spiCoreRxBegin();

			spiTimeoutCounter = 0;
		}
	}
}
