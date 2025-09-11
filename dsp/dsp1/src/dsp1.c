/*
  OpenX32 - The Open Source Operating System for the Behringer X32 Audio Mixing Console
  ControlSystem for DSP1 (MainDSP) v0.0.2, 09.09.2025
  https://www.openx32.de
  https://github.com/OpenMixerProject/OpenX32


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

	TODO:
	===========================
	[ ] implement SPI communication in openx32Command() to receive parameters from X32ctrl
	[ ] implement external SD-RAM
	[ ] implement routing-functions (sends)
	[ ] implement bus-masters, solo, mute
	[ ] implement data-transmission to and from secondary DSP
	[ ] check implementation of DTS Neo:6 algorithm and surround-mixing
*/

#include "dsp1.h"
#include "system.h"
#include "audio.h"
#include "fx.h"
#include "spi.h"

// global data
static volatile uint32_t timerCounter;

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

void openx32Init(void) {
	openx32.samplerate = 48000; // other samplerates up to 192kHz are possible with AD and DA converters

	for (int ch = 0; ch < MAX_CHAN; ch++) {
		// initialize noisegate
		openx32.channel[ch].gate.threshold = -60.0; // dB
		openx32.channel[ch].gate.range = 60.0; // dB
		openx32.channel[ch].gate.attackTime_ms = 10.0; // ms
		openx32.channel[ch].gate.holdTime_ms = 50.0; // ms
		openx32.channel[ch].gate.releaseTime_ms = 258; // ms

		fxRecalcGate(&openx32.channel[ch].gate);

		// initialize Equalizers
		openx32.channel[ch].balance = 0; // -100 ... 0 ... 100
		openx32.channel[ch].volume = 0.0; // dB

		openx32.channel[ch].peqMax = 2;

		openx32.channel[ch].peq[0].Q = 2.0;
		openx32.channel[ch].peq[0].fc = 20; // Hz
		openx32.channel[ch].peq[0].gain = 0.0; // dB
		openx32.channel[ch].peq[0].type = 7; // LowCut / HighPass

		openx32.channel[ch].peq[1].Q = 2.0;
		openx32.channel[ch].peq[1].fc = 125; // Hz
		openx32.channel[ch].peq[1].gain = 0.0; // dB
		openx32.channel[ch].peq[1].type = 1; // Peak-Filter

		openx32.channel[ch].peq[2].Q = 2.0;
		openx32.channel[ch].peq[2].fc = 500; // Hz
		openx32.channel[ch].peq[2].gain = 0.0; // dB
		openx32.channel[ch].peq[2].type = 1; // Peak-Filter

		openx32.channel[ch].peq[3].Q = 2.0;
		openx32.channel[ch].peq[3].fc = 2000; // Hz
		openx32.channel[ch].peq[3].gain = 0.0; // dB
		openx32.channel[ch].peq[3].type = 1; // Peak-Filter

		openx32.channel[ch].peq[4].Q = 2.0;
		openx32.channel[ch].peq[4].fc = 10000; // Hz
		openx32.channel[ch].peq[4].gain = 0.0; // dB
		openx32.channel[ch].peq[4].type = 3; // High-Shelf

		for (int i = 0; i < openx32.channel[ch].peqMax; i++) {
			fxRecalcFilterCoefficients_PEQ(&openx32.channel[ch].peq[i]);
		}

		// initialize compressor
		openx32.channel[ch].compressor.threshold = -50.0; // dB
		openx32.channel[ch].compressor.ratio = 20.0; // 1:x
		openx32.channel[ch].compressor.makeup = 10.0; // dB
		openx32.channel[ch].compressor.attackTime_ms = 10.0; // ms
		openx32.channel[ch].compressor.holdTime_ms = 10.0; // ms
		openx32.channel[ch].compressor.releaseTime_ms = 150; // ms

		fxRecalcCompressor(&openx32.channel[ch].compressor);

		// initialize volumes
		openx32.channel[ch].volume = 1.0f;
	}
}

void openx32Command(unsigned int parameter, unsigned int value) {
	if ((parameter >= 10) && (parameter < (10 + 32))) {
		// volume for channels
		memcpy(&openx32.channel[parameter - 10].volume, &value, 4);
	}else if (parameter == 0x00000042) {
		// LED-control
		switch(value) {
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

	// turn-on LED
	sysreg_bit_set(sysreg_FLAGS, FLG7);

	spiInit();
	openx32Init();
	audioInit();
	systemSportInit();

	// install interrupt handlers (see Processor Hardware Reference v2.2 page B-5)
	adi_int_InstallHandler(ADI_CID_P1I, (ADI_INT_HANDLER_PTR)spiISR, 0, true); // SPI Interrupt (called on new SPI-data)
	adi_int_InstallHandler(ADI_CID_P3I, (ADI_INT_HANDLER_PTR)audioRxISR, 0, true); // SPORT1 Interrupt (called on new audio-data)
	adi_int_InstallHandler(ADI_CID_TMZHI, timerIsr, (void *)&timerCounter, true); // iid - high priority core timer. Use "ADI_CID_TMZLI" for low priority

	// t_timer = (t_periode + 1) * t_count / f_clk = (1001 * 1000)/266MHz = 0.0037631579 s
	timer_set(1000, 1000); // set period to 1000 and counter to 1000 -> count 1000 x 1000 -> 266MHz = 3.7594ns = 3.7594ms
	timer_on(); // start timer

	// the main-loop
	while(1) {
		if (timerCounter == 0) {
			// toggle LED controlled by timer
			//sysreg_bit_tgl(sysreg_FLAGS, FLG7); // alternative: sysreg_bit_clr() / sysreg_bit_set()
		}

		/*
		// toggle LED to show that we are receiving audio-data
		if (audioIsrCounter > (openx32.samplerate / SAMPLES_IN_BUFFER) / 2) {
			sysreg_bit_set(sysreg_FLAGS, FLG7);
		}else{
			sysreg_bit_clr(sysreg_FLAGS, FLG7);
		}
		*/

		// check for new audio-data to process
		if (audioReady) {
			audioProcessData();
		}

		if (spiNewRxDataReady) {
			spiProcessRxData();
		}
	}
}
