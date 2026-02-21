#pragma once

#include "base.h"
#include "defines.h"

class FxBase : public X32Base {

    protected:
        vector<MP_ID> parameters;
        //vector<float> parameter_value;

    public:
        FxBase(X32BaseParameter* basepar);

        uint8_t GetParameterCount();
        MP_ID GetParameterDefinition(uint8_t index);
        
        virtual String GetName();    
};