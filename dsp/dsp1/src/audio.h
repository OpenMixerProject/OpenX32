#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "dsp1.h"

// function prototypes
void audioProcessData(void);
void audioISR(uint32_t , void *);

#endif
