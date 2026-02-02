#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "dsp2.h"

extern float pm audioBuffer[2][MAX_CHAN][SAMPLES_IN_BUFFER];

#ifdef __cplusplus
extern "C" {
#endif

// function prototypes
void audioInit(void);
void audioFxData(int fxSlot, float* data, int len);
void audioFxChangeSlot(int fxSlot, int newFxId, int channelMode);
void audioProcessData(void);
void audioRxISR(uint32_t , void *);
void audioSpdifTxISR(uint32_t, void*);

#ifdef __cplusplus
}
#endif

#endif
