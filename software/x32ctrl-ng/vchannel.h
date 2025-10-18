#pragma once

#include <stdint.h>
#include "WString.h"
#include "x32ctrl_types.h"
#include "x32base.h"


class VChannel : public X32Base {
    private:
        // indicates, which data has changed and need to get synced
        uint16_t changed;
    
    public:
        String name;
        String nameIntern;
        uint8_t color;
        uint8_t icon;

        bool selected;
        float sends[16];

        X32_VCHANNELTYPE vChannelType;
        sDspChannel* dspChannel;

        VChannel(X32BaseParameter* basepar);

        void SetChanged(uint16_t p_flag);
        void ResetVChannelChangeFlags();
        bool HasChanged(uint16_t p_flag);
        bool HasAnyChanged(void);

        void ChangeInput(int8_t amount);

        String ToString(void);
};