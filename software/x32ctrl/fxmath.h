#pragma once

#include <math.h>

#include "base.h"
#include "defines.h"

#include "fxslot.h"

class FxMath : X32Base {

    public:

        FxMath(X32BaseParameter* basepar);
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

        // functions for FX-Rack
        void fxCalcParameters_Reverb(float data[], float roomSizeMs, float rt60, float feedbackLowPassFreq, float dry, float wet);
        void fxCalcParameters_Chorus(float data[], float depth[2], float delayMs[2], float phase[2], float freq[2], float mix);
        void fxCalcParameters_TransientShaper(float data[], float tFastMs, float tMediumMs, float tSlowMs, float attack, float sustain, float delayMs);
        void fxCalcParameters_Overdrive(float data[], float preGain, float Q, float hpfInputFreq, float lpfInputFreq, float lpfOutputFreq);
        void fxCalcParameters_Delay(float data[], float delayMs[2]);
        void fxCalcParameters_MultibandCompressor(float data[], int channel, int band, float threshold, float ratio, float attack, float hold, float release, float makeup);
        void fxCalcParameters_MultibandCompressorFreq(float data[], int channel, float freq[4]);
        void fxCalcParameters_DynamicEQ(float data[], int band, int type, float frequency, float staticGain, float maxDynamicGain, float Q, float threshold, float ratio, float attack, float release);
};
