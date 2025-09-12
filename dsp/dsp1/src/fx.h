#ifndef __FX_H__
#define __FX_H__

#include "dsp1.h"

// function prototypes
float fxProcessGate(float input, sGate *gate);
float fxProcessEq(float input, sPEQ *peq);
float fxProcessCompressor(float input, sCompressor *compressor);

#endif
