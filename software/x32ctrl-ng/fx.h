#pragma once

//#include "x32ctrl.h"
#include "external.h"
#include "x32ctrl_types.h"
#include "x32config.h"

#define PI 3.14159265358979323846

// function prototypes
void fxRecalcFilterCoefficients_PEQ(sPEQ* peq);
float fxCalcFrequencyResponse_LC(float f, float fc, float fs);
float fxCalcFrequencyResponse_HC(float f, float fc, float fs);
float fxCalcFrequencyResponse_PEQ(float a0, float a1, float a2,  float b1, float b2, float f, float fs);
void fxRecalcFilterCoefficients_LR12(sLR12* LR12);
void fxRecalcFilterCoefficients_LR24(sLR24* LR24);
void fxRecalcGate(sGate* gate);
void fxRecalcCompressor(sCompressor* compressor);