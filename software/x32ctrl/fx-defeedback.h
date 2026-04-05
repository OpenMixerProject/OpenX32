#pragma once

#include "fx-base.h"
#include "defines.h"

class FxDeFeedback : public FxBase
{
    using enum  MP_ID;

    public:
        FxDeFeedback(X32BaseParameter* basepar) : FxBase(basepar) {
        }

        String GetName() override{
            return "DeFeedback";
        }
};