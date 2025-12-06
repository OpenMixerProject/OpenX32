#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "dsp1.h"

extern float audioBuffer[5][SAMPLES_IN_BUFFER][1 + MAX_CHAN_FPGA + MAX_CHAN_DSP2 + MAX_MIXBUS + MAX_MAIN + MAX_MATRIX + MAX_MONITOR];

// function prototypes
void audioInit(void);
void audioProcessData(void);
void audioRxISR(uint32_t , void *);

#endif
