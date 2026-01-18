#pragma once

#include <stdint.h>
#include "WString.h"
#include "types.h"
#include "base.h"


class VChannel : public X32Base {
    private:
        // indicates, which data has changed and need to get synced
        x32_changeflag changed;
    
    public:
        String name;
        String nameIntern;
        uint8_t color;
        uint8_t icon;

        bool selected;
        float sends[16];

        X32_VCHANNELTYPE vChannelType;
        srDspChannel* rChannel; // runtime values
        sDspChannel* dspChannel; // values saved to/from config
        

        VChannel(X32BaseParameter* basepar);

        void SetChanged(x32_changeflag p_flag);
        void ResetVChannelChangeFlags();
        bool HasChanged(x32_changeflag p_flag);
        bool HasAnyChanged(void);

        void ChangeInput(int8_t amount);

        String ToString(void);
};