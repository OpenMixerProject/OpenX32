/*
  OpenX32 - The Open Source Operating System for the Behringer X32 Audio Mixing Console
  ControlSystem for DSP1 (MainDSP) v0.0.1, 05.09.2025
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
*/

#include "dsp1.h"

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

void timerInit() {
	adi_int_InstallHandler(ADI_CID_TMZHI,        	// iid - high priority core timer. Use "ADI_CID_TMZLI" for low priority
							timerIsr,        		// handler
							(void *)&timerCounter,	// handler parameter
							true					// enable this timer
	);
	timer_set(1000, 1000); // set period to 1000 and counter to 1000 (= 1us)
	timer_on(); // start timer
}

int main() {
	// init adi components
	adi_initComponents();

	// init our own things
	systemPllInit();
	systemExternalMemoryInit();
	systemSruInit();
	timerInit();
	spiInit();
	audioInit();
	systemSportInit();

	// install interrupt handlers (see Processor Hardware Reference v2.2 page B-5)
	adi_int_InstallHandler(ADI_CID_P1I, (ADI_INT_HANDLER_PTR)spiISR, 0, true); // SPI Interrupt
	adi_int_InstallHandler(ADI_CID_P6I, (ADI_INT_HANDLER_PTR)audioTxISR, 0, true); // SPORT0 Interrupt (transmitting of TDM channels 1-8)
	adi_int_InstallHandler(ADI_CID_P3I, (ADI_INT_HANDLER_PTR)audioRxISR, 0, true); // SPORT1 Interrupt (receiving of TDM channels 1-8)

	// the main-loop
	while(1) {
		if (timerCounter == 0) {
			// toggle LED
			//sysreg_bit_tgl(sysreg_FLAGS, FLG7); // alternative: sysreg_bit_clr() / sysreg_bit_set()
		}

		// toggle LED to show that we are receiving audio-data
		if (audioIsrCounter > (SAMPLERATE / SAMPLES_IN_BUFFER) / 2) {
			sysreg_bit_set(sysreg_FLAGS, FLG7);
		}else{
			sysreg_bit_clr(sysreg_FLAGS, FLG7);
		}

		if (audioReady) {
			audioProcessData();
		}
	}
}
