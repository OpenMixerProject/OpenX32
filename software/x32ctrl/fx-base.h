#pragma once

#include "base.h"
#include "defines.h"

class FxBase : public X32Base {

    protected:
        vector<MIXERPARAMETER> parameters;
        vector<float> parameter_value;

    public:
        FxBase(X32BaseParameter* basepar);

        uint8_t GetParameterCount();
        MIXERPARAMETER GetParameterDefinition(uint8_t index);

        void InitParameters();
        
        virtual String GetName();
        
        float GetParameter(uint8_t parIdx);
        bool SetParameter(uint8_t parIdx, float value);
        bool ChangeParameter(uint8_t parIdx, int8_t amount);
        bool ResetParameter(uint8_t parIdx);
};