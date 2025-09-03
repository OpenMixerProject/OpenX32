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

	// increment timer-counter up to 1000 and wrap around
	if (*timer_counter > 1000) {
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
	timer_set(1000, 1000); // set period and counter to 1000
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
	adi_int_InstallHandler(ADI_CID_P6I, (ADI_INT_HANDLER_PTR)audioISR, 0, true);
	systemSportInit();

	// the main-loop
	while(1) {
		/*
		if (timerCounter == 0) {
			// turn LED on
			//sysreg_bit_tgl(sysreg_FLAGS, FLG7);
			sysreg_bit_clr(sysreg_FLAGS, FLG7);
			spiRxTx(42); // send meaning of life to i.MX25
		}else if (timerCounter == 500) {
			// turn LED off
			sysreg_bit_set(sysreg_FLAGS, FLG7);
		}

		if (audioReady) {
			audioProcessData();
		}
		*/

		sysreg_bit_set(sysreg_FLAGS, FLG7);
		delay(100000000);
		sysreg_bit_clr(sysreg_FLAGS, FLG7);
		delay(100000000);
	}
}
