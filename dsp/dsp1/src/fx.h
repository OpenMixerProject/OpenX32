#ifndef __FX_H__
#define __FX_H__

#include "dsp1.h"

// function prototypes
void fxRecalcFilterCoefficients_PEQ(sPEQ *peq);
void fxRecalcFilterCoefficients_LR12(sLR12 *LR12);
void fxRecalcFilterCoefficients_LR24(sLR24 *LR24);
void fxRecalcGate(sGate *gate);
void fxRecalcCompressor(sCompressor *Compressor);
float fxProcessGate(float input, sGate *gate);
float fxProcessEq(float input, sPEQ *peq);
float fxProcessCompressor(float input, sCompressor *compressor);

#endif
