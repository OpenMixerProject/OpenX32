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

  ControlSystem for DSP1 (MainDSP) in the Behringer X32/M32 devices

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

#include "dsp1.h"
#include "system.h"
#include "audio.h"
#include "fx.h"
#include "spi.h"

// global data
//static volatile uint32_t timerCounter;
//cycle_stats_t systemStats;
uint32_t spiTimeoutCounter = 0;
volatile uint32_t cyclemap[17];

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
	for (int ch = 0; ch < MAX_CHAN_FULLFEATURED; ch++) {
		dsp.gateEnvelope[ch] = 1.0f;
		dsp.compressorEnvelope[ch] = 1.0f;
		dsp.dspChannelCompressor[ch].holdTimer = 0;
	}
}

// ISR is called once on first falling edge of FrameSync
void misc0ISR(int sig) {
	systemSportInit();

	// make sure, that this ISR is called only once
	adi_int_UninstallHandler(ADI_CID_P0I);
}

void led(void) {

	static uint32_t counter = 0;

	if (counter > 0xFFFFF) {
		//			 toggle LED controlled by timer
		sysreg_bit_tgl(sysreg_FLAGS, FLG7); // alternative: sysreg_bit_clr() / sysreg_bit_set()
		counter = 0;
	}
	counter++;
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

	// install interrupt handlers (see Processor Hardware Reference v2.2 page B-5)
	adi_int_InstallHandler(ADI_CID_P1I, (ADI_INT_HANDLER_PTR)spiISR, 0, true); // SPI Interrupt (called on new SPI-data)
	//adi_int_InstallHandler(ADI_CID_P3I, (ADI_INT_HANDLER_PTR)audioRxISR, 0, true); // SPORT1 Interrupt (called on new audio-data)
	adi_int_InstallHandler(ADI_CID_P11I, (ADI_INT_HANDLER_PTR)audioRxISR, 0, true); // SPORT7 Interrupt (called on new audio-data)
	//adi_int_InstallHandler(ADI_CID_TMZHI, timerIsr, (void *)&timerCounter, true); // iid - high priority core timer. Use "ADI_CID_TMZLI" for low priority
	adi_int_InstallHandler(ADI_CID_P0I, (ADI_INT_HANDLER_PTR)misc0ISR, 0, true); // MISCA0 Interrupt on P0I or P12I

	// enable interrupts
	*pDAI_IRPTL_PRI |= DAI_INT_28; // DAI Interrupt Priority Register
	*pDAI_IRPTL_FE |= DAI_INT_28; // DAI Rising Edge Interrupt Latch Register
	*pDAI_IMASK_FE |= DAI_INT_28; // DAI Rising Edge Interrupt Latch Register

	// t_timer = (t_periode + 1) * t_count / f_clk = (1001 * 1000)/266MHz = 0.0037631579 s
	//timer_set(1000, 1000); // set period to 1000 and counter to 1000 -> count 1000 x 1000 -> 266MHz = 3.7594ns = 3.7594ms
	//timer_on(); // start timer

	// turn-off LED
	sysreg_bit_set(sysreg_FLAGS, FLG7);

	cyclemap[0] = 0; // cycles in audioProcessData()
	cyclemap[1] = 0; // DataInput
	cyclemap[2] = 0; // InputDelay/Routing
	cyclemap[3] = 0; // Lowcut
	cyclemap[4] = 0; // Noisegate
	cyclemap[5] = 0; // EQ
	cyclemap[6] = 0; // Dynamics
	cyclemap[7] = 0; // Channel/Fader
	cyclemap[8] = 0; // Mixbus/Main-Out


	cycle_t cycletemp;

	// the main-loop
	while(1) {


		led();



		// check for new audio-data to process
		if (audioReady) {

			START_CYCLE_COUNT(cycletemp);
			audioProcessData();
			STOP_CYCLE_COUNT(cyclemap[0], cycletemp);

			spiTimeoutCounter++; // will be incremented every 333 microseconds
		}

		// check for new SPI-data to process
		if (spiNewRxDataReady) {
			spiTimeoutCounter = 0; // reset SPI counter

			spiProcessRxData();

		}

		// check if we have received some data over SPI within the last 250ms
		// we are receiving audio every 333 microseconds. 750 * 0.333us = 250ms
		if (spiTimeoutCounter >= 750) {
			// we ran into a SPI-timeout -> reset SPI system
			spiCoreRxBegin();

			spiTimeoutCounter = 0;
		}

		spiCallback();


	}
}
