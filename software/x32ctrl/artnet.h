#pragma once



#include <stdint.h>

#include "defines.h"
#include "WString.h"
#include "base.h"

using namespace std;
using enum MP_ID;

#if ENABLE_ARTNET

#include "../libartnet/artnet/artnet.h"

class Artnet : public X32Base
{
    public:
        Artnet(X32BaseParameter* basepar);
        void Init();
        void Tick(); // called every 50ms
        void Sync();
        void SetChannel(uint16_t channel, float value, float timeMs);
        //float GetValue(uint16_t channel);

    private:
        artnet_node node;
        uint8_t dmxOutput[512];
        float dmx[512];
        float dmxDest[512];
        float dmxStep[512];
};

#endif