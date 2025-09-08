#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "dsp1.h"

// function prototypes
void audioInit(void);
void audioProcessData(void);
void audioTxISR(uint32_t , void *);
void audioRxISR(uint32_t , void *);


#endif
