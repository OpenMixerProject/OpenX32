#pragma once

#include "base.h"
#include "defines.h"

#include "fx-base.h"
#include "fx-reverb.h"
#include "fx-chorus.h"

class FxSlot : X32Base {

    private:
        X32BaseParameter* _basepar;

    public:

        FX_TYPE fxType = FX_TYPE_NONE;
        FxBase* fx;
        
        FxSlot(X32BaseParameter* basepar);

        void LoadFx(FX_TYPE fxToLoad);
        bool HasFx();
};