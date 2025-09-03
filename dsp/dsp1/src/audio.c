#include "audio.h"

volatile int audioReady = 0;
volatile int audioProcessing = 0;

/*
	Used Audio-Mapping:
	TDM IN0   -> SPORT0A
	TDM IN1   -> SPORT0B
	TDM IN2   -> SPORT1A
	TDM IN3   -> SPORT1B
	TDM INAUX -> SPORT2A

	SPORT2B   -> TDM OUT0
	SPORT3A   -> TDM OUT1
	SPORT3B   -> TDM OUT2
	SPORT4A   -> TDM OUT3
	SPORT4B   -> TDM OUTAUX
*/

void audioProcessData(void) {
	audioReady = 0; // clear global flag that audio is not ready anymore
	audioProcessing = 1; // set global flag that we are processing now

	for (uint32_t i = 0; i < NUM_SAMPLES; i++) {
		// do something with the received samples
	}

	audioProcessing = 0; // clear global flag that processing is done
}

void audioISR(uint32_t iid, void *handlerarg) {
	// we received new audio-data
	// check if we are still processing the data, which means >100% CPU Load -> Crash System
    if (audioProcessing) {
    	systemCrash();
    }

    audioReady = 1; // set flag, that we have new data to process
}
