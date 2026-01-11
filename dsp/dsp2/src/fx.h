#ifndef __FX_H__
#define __FX_H__

#include "dsp2.h"

// function prototypes
void fxInit(void);
void fxDecompositingAndUpmixing(float* inBuf[2], float* outBuf[5], int samples);
#endif
