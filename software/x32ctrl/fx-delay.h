#pragma once

#include "fx-base.h"
#include "defines.h"

class FxDelay : public FxBase {

    public:
        FxDelay(X32BaseParameter* basepar) : FxBase(basepar) {

            // Define parameters of this FX
            parameters.push_back(MP_TYPE::FX_DELAY_DELAY_A);
            parameters.push_back(MP_TYPE::FX_DELAY_DELAY_B);
        }

        String GetName() override{
            return "Delay";
        }
};