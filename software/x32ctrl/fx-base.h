#pragma once

#include "base.h"
#include "defines.h"

class FxBase : X32Base {

    protected:
        vector<MIXERPARAMETER> parameters;

    public:
        FxBase(X32BaseParameter* basepar);

        virtual String GetName();
        virtual void OnLoad();
        uint8_t GetParameterCount();
        MIXERPARAMETER GetParameterDefinition(uint8_t index);
        

};