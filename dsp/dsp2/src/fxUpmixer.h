#ifndef __FXUPMIXER_H_
#define __FXUPMIXER_H_

#include "dsp2.h"

#if FX_USE_UPMIXER == 1

#define FX_UPMIX_PHASEINVERTED_BACK_LR	1	// inverts the phase of the back-left and back-right speaker-channels
#define FX_UPMIX_CONTRAST_ENHANCEMENT	1	// 0=Regular Mask, 1=Mask with 1.5, 2=Mask with 2 or above this as a free value. The contrast between all channels is increased by using powf() for the mask-value
#define FX_UPMIX_ADD_AMBIENCE_TO_LR		1

// function prototypes
void fxUpmixerInit(void);
void fxUpmixerProcess(float* inBuf[2], float* outBuf[6], int samples);

#endif

#endif /* FXUPMIXER_H_ */
