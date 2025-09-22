#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "dsp1.h"

extern float audioBuffer[5][1 + MAX_CHAN + MAX_MIXBUS + 6 + 3 + 3][SAMPLES_IN_BUFFER];

// function prototypes
void audioInit(void);
void audioProcessData(void);
void audioRxISR(uint32_t , void *);

#endif
