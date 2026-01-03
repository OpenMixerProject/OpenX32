#pragma once

#include <math.h>

#include "base.h"
#include "defines.h"

class FX : X32Base {
    public:
        FX(X32BaseParameter* basepar);
        void RecalcFilterCoefficients_PEQ(sPEQ* peq);
        float CalcFrequencyResponse_LC(float f, float fc, float fs);
        float CalcFrequencyResponse_HC(float f, float fc, float fs);
        float CalcFrequencyResponse_PEQ(float a0, float a1, float a2,  float b1, float b2, float f, float fs);
        float CalcPhaseResponse_LC(float f, float fc);
        float CalcPhaseResponse_HC(float f, float fc);
        float CalcPhaseResponse_PEQ(float a0, float a1, float a2, float b0, float b1, float b2, float f, float fs);
        void RecalcFilterCoefficients_LR12(sLR12* LR12);
        void RecalcFilterCoefficients_LR24(sLR24* LR24);
        void RecalcGate(sGate* gate);
        void RecalcCompressor(sCompressor* compressor);
};




