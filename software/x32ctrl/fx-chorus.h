#pragma once

#include "fx-base.h"
#include "defines.h"

class FxChorus : public FxBase
{
    using enum  MP_ID;

    public:
        FxChorus(X32BaseParameter* basepar) : FxBase(basepar) {

            // Define parameters of this FX
            parameters.push_back(FX_CHORUS_DEPTH_A);
            parameters.push_back(FX_CHORUS_DELAY_A);
            parameters.push_back(FX_CHORUS_PHASE_A);
            parameters.push_back(FX_CHORUS_FREQ_A);
            parameters.push_back(FX_CHORUS_DEPTH_B);
            parameters.push_back(FX_CHORUS_DELAY_B);
            parameters.push_back(FX_CHORUS_PHASE_B);
            parameters.push_back(FX_CHORUS_FREQ_B);
            parameters.push_back(FX_CHORUS_MIX);
        }

        String GetName() override{
            return "Chorus";
        }
};