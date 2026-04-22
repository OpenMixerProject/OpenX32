#pragma once

#include <stdint.h>
#include "WString.h"
#include "base.h"
#include <artnet/artnet.h>

class Artnet : public X32Base
{
    public:
        Artnet(X32BaseParameter* basepar);
        void Init();
        void Tick(); // called every 50ms
        void setChannel(uint16_t channel, float value, float timeMs);

    private:
        artnet_node node;
        uint8_t dmxOutput[512];
        float dmx[512];
        float dmxDest[512];
        float dmxStep[512];
};