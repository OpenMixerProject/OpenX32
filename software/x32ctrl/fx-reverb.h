#pragma once

#include "fx-base.h"
#include "defines.h"

class FxReverb : public FxBase {

    public:
        FxReverb(X32BaseParameter* basepar) : FxBase(basepar) {

            // Define parameters of this FX
            parameters.push_back(MP_TYPE::FX_REVERB_ROOMSIZE);
            parameters.push_back(MP_TYPE::FX_REVERB_RT60);
            parameters.push_back(MP_TYPE::FX_REVERB_LPFREQ);
            parameters.push_back(MP_TYPE::FX_REVERB_DRY);
            parameters.push_back(MP_TYPE::FX_REVERB_WET);
        }

        String GetName() override{
            return "Reeeverb";
        }
};