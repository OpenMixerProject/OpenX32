#ifndef __FXTRANSIENTSHAPER_H_
#define __FXTRANSIENTSHAPER_H_

#include "dsp2.h"

#if FX_USE_TRANSIENTSHAPER == 1

#define FX_TRANSIENTSHAPER_DELAY_MS_MAX		50
#define FX_TRANSIENTSHAPER_BUFFER_SIZE 		((48000 * FX_TRANSIENTSHAPER_DELAY_MS_MAX) / 1000)

struct {
	float attack;
	float sustain;

	float kFast;
	float kSlow;

	int delayLineHead;
	int delayLineTailOffset;
	float envelopeFast;
	float envelopeSlow;
} transientshaper;

// function prototypes
void fxTransientshaperInit(void);
void fxTransientshaperSetParameter(float kFast, float kSlow, float attack, float sustain, float delayMs);
void fxTransientshaperProcess(float* bufIn, float* bufOut);

#endif

#endif /* FXTRANSIENTSHAPER_H_ */
