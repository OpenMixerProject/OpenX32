#ifndef __FX_H__
#define __FX_H__

#include "dsp1.h"

// function prototypes
float linearToDb(float linear);
float dbToLinear(float dB);
float linearToDb_fast(float linear);
void fxSetPeqCoeffs(int channel, int index, float coeffs[]);
void fxSmoothCoeffs(void);
#endif
