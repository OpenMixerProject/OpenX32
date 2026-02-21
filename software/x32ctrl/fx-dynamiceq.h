#pragma once

#include "fx-base.h"
#include "defines.h"

class FxDynamicEQ : public FxBase {

    public:
        FxDynamicEQ(X32BaseParameter* basepar) : FxBase(basepar) {

            // Define parameters of this FX
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND1_TYPE);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND1_FREQ);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND1_STATICGAIN);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND1_MAXDYNGAIN);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND1_Q);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND1_THRESHOLD);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND1_RATIO);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND1_ATTACK);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND1_RELEASE);
        
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND2_TYPE);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND2_FREQ);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND2_STATICGAIN);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND2_MAXDYNGAIN);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND2_Q);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND2_THRESHOLD);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND2_RATIO);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND2_ATTACK);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND2_RELEASE);

            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND3_TYPE);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND3_FREQ);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND3_STATICGAIN);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND3_MAXDYNGAIN);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND3_Q);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND3_THRESHOLD);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND3_RATIO);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND3_ATTACK);
            parameters.push_back(MP_ID::FX_DYNAMICEQ_BAND3_RELEASE);
        }

        String GetName() override{
            return "DEQ";
        }
};