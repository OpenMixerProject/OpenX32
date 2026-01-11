#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "dsp2.h"

extern float audioBuffer[5][MAX_CHAN][SAMPLES_IN_BUFFER];

// function prototypes
void audioInit(void);
void audioProcessData(void);
void audioRxISR(uint32_t , void *);
void audioSpdifTxISR(uint32_t, void*);

#endif
