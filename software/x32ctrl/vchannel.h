#ifndef VCHANNEL_H_
#define VCHANNEL_H_

#include "x32ctrl.h"

class VChannel{
    public:
        uint8_t index;
        // indicates, which data has changes and need to get synced
        uint16_t changed;
        String name;
        uint8_t color;
        uint8_t icon;
        bool mute;
        bool solo;
        bool selected;
        float sends[16];

        float volumeLR; // volume in dBfs
        float volumeSub; //volume in dBfs
        float balance; // balance between -100 .. 0 .. +100

        // 0 - normal channel
        // 1 - main channel
        uint8_t vChannelType;
        sInputSource inputSource;

        void Init(int p_vChannelIndex, bool p_disabled);
};

#endif