#ifndef __FX_H__
#define __FX_H__

#include "dsp1.h"

// function prototypes
void fxProcessGateLogic(int channel, float samples[]);
void fxSetPeqCoeffs(int channel, int index, float coeffs[]);
void fxProcessCompressorLogic(int channel, float samples[]);
#endif
