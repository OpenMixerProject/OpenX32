#ifndef FX_H_
#define FX_H_

#include "x32ctrl.h"
#include "dsp.h"

#define PI 3.14159265358979323846

// function prototypes
void fxRecalcFilterCoefficients_PEQ(sPEQ *peq);
void fxRecalcFilterCoefficients_LR12(sLR12 *LR12);
void fxRecalcFilterCoefficients_LR24(sLR24 *LR24);
void fxRecalcGate(sGate *gate);
void fxRecalcCompressor(sCompressor *Compressor);

#endif
