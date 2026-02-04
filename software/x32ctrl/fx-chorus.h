#pragma once

#include "fx-base.h"
#include "defines.h"

class FxChorus : public FxBase {

    public:
        FxChorus(X32BaseParameter* basepar) : FxBase(basepar) {

            // Define parameters of this FX
            parameters.push_back(MIXERPARAMETER_FX_CHORUS_DEPTH_A);
            parameters.push_back(MIXERPARAMETER_FX_CHORUS_DEPTH_B);
            // TODO restliche Parameter
        }

        String GetName() override{
            return "Chorus";
        }
};