#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "dsp1.h"

extern float audioBuffer[5][SAMPLES_IN_BUFFER][1 + MAX_CHAN_FPGA + MAX_CHAN_DSP2 + MAX_MIXBUS + MAX_MAIN + MAX_MATRIX + MAX_MONITOR];
extern volatile uint32_t audioGlitchCounter;

#if DEBUG_DISABLE_DELAYLINE == 0
	extern int delayLineTailOffsetInput[MAX_CHAN_FPGA]; // offset = (delayMs * sampleRate / 1000)
	extern int delayLineTailOffsetOutput[MAX_CHAN_FPGA]; // offset = (delayMs * sampleRate / 1000)
#endif

// function prototypes
void audioInit(void);
void audioSmoothVolume(void);
void audioProcessData(void);
void audioRxISR(uint32_t , void *);

#endif
