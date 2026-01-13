#ifndef __FXMATRIXUPMIXER_H_
#define __FXMATRIXUPMIXER_H_

#include "dsp2.h"

#if FX_USE_MATRIXUPMIXER == 1

#define FX_MATRIXUPMIXER_DELAYBACK_MS	17	// 17ms delay to back-surround
#define FX_MATRIXUPMIXER_SAMPLING_RATE 	48000
#define FX_MATRIXUPMIXER_DELAY_MS_MAX	25
#define FX_MATRIXUPMIXER_BUFFER_SIZE 	((FX_MATRIXUPMIXER_SAMPLING_RATE * FX_MATRIXUPMIXER_DELAY_MS_MAX) / 1000)

// function prototypes
void fxMatrixUpmixerInit(void);
void fxMatrixUpmixerProcess(float* inBuf[2], float* outBuf[6], int samples);

#endif

#endif /* FXUPMIXER_H_ */
