#pragma once

#include "fx-base.h"
#include "defines.h"

class FxTransientshaper : public FxBase {

    public:
        FxTransientshaper(X32BaseParameter* basepar) : FxBase(basepar) {

            // Define parameters of this FX
            parameters.push_back(MP_ID::FX_TRANSIENTSHAPER_FAST);
            parameters.push_back(MP_ID::FX_TRANSIENTSHAPER_MEDIUM);
            parameters.push_back(MP_ID::FX_TRANSIENTSHAPER_SLOW);
            parameters.push_back(MP_ID::FX_TRANSIENTSHAPER_ATTACK);
            parameters.push_back(MP_ID::FX_TRANSIENTSHAPER_SUSTAIN);
            parameters.push_back(MP_ID::FX_TRANSIENTSHAPER_DELAY);
        }

        String GetName() override{
            return "TranShape";
        }
};