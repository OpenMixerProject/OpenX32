#pragma once

#include "fx-base.h"
#include "defines.h"

class FxMultibandCompressor : public FxBase {

    public:
        FxMultibandCompressor(X32BaseParameter* basepar) : FxBase(basepar) {

            // Define parameters of this FX

            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_FREQ1);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_FREQ2);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_FREQ3);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_FREQ4);

            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND1_THRESHOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND1_RATIO);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND1_ATTACK);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND1_HOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND1_RELEASE);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND1_MAKEUP);

            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND2_THRESHOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND2_RATIO);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND2_ATTACK);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND2_HOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND2_RELEASE);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND2_MAKEUP);

            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND3_THRESHOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND3_RATIO);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND3_ATTACK);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND3_HOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND3_RELEASE);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND3_MAKEUP);

            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND4_THRESHOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND4_RATIO);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND4_ATTACK);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND4_HOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND4_RELEASE);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND4_MAKEUP);

            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND5_THRESHOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND5_RATIO);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND5_ATTACK);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND5_HOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND5_RELEASE);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_L_BAND5_MAKEUP);



            
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_FREQ1);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_FREQ2);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_FREQ3);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_FREQ4);

            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND1_THRESHOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND1_RATIO);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND1_ATTACK);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND1_HOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND1_RELEASE);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND1_MAKEUP);

            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND2_THRESHOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND2_RATIO);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND2_ATTACK);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND2_HOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND2_RELEASE);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND2_MAKEUP);

            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND3_THRESHOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND3_RATIO);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND3_ATTACK);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND3_HOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND3_RELEASE);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND3_MAKEUP);

            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND4_THRESHOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND4_RATIO);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND4_ATTACK);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND4_HOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND4_RELEASE);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND4_MAKEUP);

            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND5_THRESHOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND5_RATIO);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND5_ATTACK);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND5_HOLD);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND5_RELEASE);
            parameters.push_back(MP_ID::FX_MULTIBANDCOMPRESOR_R_BAND5_MAKEUP);
        }

        String GetName() override{
            return "Combinator";
        }
};