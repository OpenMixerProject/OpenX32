#pragma once

#include "fx-base.h"
#include "defines.h"

class FxOverdrive : public FxBase {

    public:
        FxOverdrive(X32BaseParameter* basepar) : FxBase(basepar) {

            // Define parameters of this FX
            parameters.push_back(MP_ID::FX_OVERDRIVE_PREGAIN);
            parameters.push_back(MP_ID::FX_OVERDRIVE_Q);
            parameters.push_back(MP_ID::FX_OVERDRIVE_HPF_INPUTFREQ);
            parameters.push_back(MP_ID::FX_OVERDRIVE_LPF_INPUTFREQ);
            parameters.push_back(MP_ID::FX_OVERDRIVE_LPF_OUTPUTFREQ);
        }

        String GetName() override{
            return "Overdrive";
        }
};