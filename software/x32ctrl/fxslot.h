#pragma once

#include "base.h"
#include "defines.h"

#include "fx-base.h"
#include "fx-reverb.h"
#include "fx-chorus.h"
#include "fx-transientshaper.h"
#include "fx-delay.h"

class FxSlot : X32Base {

    private:
        X32BaseParameter* _basepar;

    public:

        FX_TYPE fxType = FX_TYPE_NONE;
        FxBase* fx;
        
        FxSlot(X32BaseParameter* basepar);

        void LoadFx(FX_TYPE fxToLoad);
        void ClearFx();
        bool HasFx();
};