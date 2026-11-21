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
volatile uint32_t cyclemap[CYCLEMAP_SIZE];


void openx32Init(void)
{
	// initialize the default samplerate with 48kHz
	// other samplerates up to 192kHz are possible with AD and DA converters
	dsp_settings.samplerate = 48000;

	// initialize states
	for (int ch = 0; ch < MAX_CHAN_FULLFEATURED; ch++) {

		dsp_settings.trim_pu[ch] = 1.0f;

		dsp.lowcutEnable[ch] = false;
		dsp.dspChannelGate[ch].value_enable = false;
		dsp.gateEnvelope[ch] = 1.0f;

		dsp.compressorEnable[ch] = false;
		dsp.compressorEnvelope[ch] = 1.0f;
		dsp.dspChannelCompressor[ch].holdTimer = 0;
	}

	for(int ch = 0; ch < CHANNELS_WITH_4BD_EQ; ch++)
	{
		dsp.eqActive[ch] = false;
	}
}

// ISR is called once on first falling edge of FrameSync
void misc0ISR(int sig)
{
	systemSportInit();

	// make sure, that this ISR is called only once
	adi_int_UninstallHandler(ADI_CID_P0I);
}

int main()
{
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
	adi_int_InstallHandler(ADI_CID_P11I, (ADI_INT_HANDLER_PTR)audioRxISR, 0, true); // SPORT7 Interrupt (called on new audio-data)
	adi_int_InstallHandler(ADI_CID_P0I, (ADI_INT_HANDLER_PTR)misc0ISR, 0, true); // MISCA0 Interrupt on P0I or P12I

	// enable interrupts
	*pDAI_IRPTL_PRI |= DAI_INT_28; // DAI Interrupt Priority Register
	*pDAI_IRPTL_FE |= DAI_INT_28; // DAI Rising Edge Interrupt Latch Register
	*pDAI_IMASK_FE |= DAI_INT_28; // DAI Rising Edge Interrupt Latch Register

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
	cyclemap[8] = 0; // Mixbus
	cyclemap[9] = 0; // Main-Out
	cyclemap[10] = 0; // EQMain
	cyclemap[11] = 0; // Main Volume
	cyclemap[12] = 0; // Matrix
	cyclemap[13] = 0; // Monitor
	cyclemap[14] = 0; // Routing/OutputDelay
	cyclemap[15] = 0; // copy VU-Data

	memset((uint32_t*)cyclemap, 0, sizeof(cyclemap));

	uint32_t spiTimeoutCounter = 0;

	// the main-loop
	while(1)
	{
		cycle_t cycletemp;

		if (audioReady)
		{
			START_CYCLE_COUNT(cycletemp);

			audioReady = false;
			audioProcessData(); // process audio
			spiTimeoutCounter++; // will be incremented every 333 microseconds

			STOP_CYCLE_COUNT(cyclemap[0], cycletemp);
		}

		// check for new SPI-data to process
		if (spiNewRxDataReady)
		{
			START_CYCLE_COUNT(cycletemp);

			spiNewRxDataReady = false;

			spiTimeoutCounter = 0; // reset SPI counter

			// copy cyclemap to spiCommData

			for (int i = 0; i < CYCLEMAP_SIZE; i++)
			{
				memcpy(&spiCommData[SPI_DATA_CYCLE_MAP_STARTINDEX + i], (uint32_t*)&cyclemap[i], sizeof(uint32_t));
			}

			STOP_CYCLE_COUNT(cyclemap[23], cycletemp);
			START_CYCLE_COUNT(cycletemp);

			spiProcessRxData();

			STOP_CYCLE_COUNT(cyclemap[24], cycletemp);
		}

		// Glitch detection counter
		if (audioGlitchDetected)
		{
			audioGlitchDetected = false;
			cyclemap[26]++;
		}

		// check if we have received some data over SPI within the last 250ms
		// we are receiving audio every 333 microseconds. 750 * 0.333us = 250ms
		if (spiTimeoutCounter >= 750)
		{
			spiTimeoutCounter = 0;

			// we ran into a SPI-timeout -> reset SPI system
			spiCoreRxBegin();
		}

		spiCallback();
	}
}
