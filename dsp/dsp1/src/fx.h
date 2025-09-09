#ifndef __FX_H__
#define __FX_H__

#include "dsp1.h"

// function prototypes
void fxRecalcFilterCoefficients_PEQ(sPEQ *peq);
void fxRecalcFilterCoefficients_LR12(sLR12 *LR12);
void fxRecalcFilterCoefficients_LR24(sLR24 *LR24);
void fxRecalcNoiseGate(sNoisegate *Noisegate);
void fxRecalcCompressor(sCompressor *Compressor);
void fxBiquad(int sample, int* in[], int* out[], double coeffA[], double coeffB[]);

#endif
