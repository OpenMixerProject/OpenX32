#pragma once

#include <math.h>

#include "x32base.h"
#include "constants.h"

class FX : X32Base {
    public:
        FX(Config* config, State* state);
        void RecalcFilterCoefficients_PEQ(sPEQ* peq);
        float CalcFrequencyResponse_LC(float f, float fc, float fs);
        float CalcFrequencyResponse_HC(float f, float fc, float fs);
        float CalcFrequencyResponse_PEQ(float a0, float a1, float a2,  float b1, float b2, float f, float fs);
        void RecalcFilterCoefficients_LR12(sLR12* LR12);
        void RecalcFilterCoefficients_LR24(sLR24* LR24);
        void RecalcGate(sGate* gate);
        void RecalcCompressor(sCompressor* compressor);
};




