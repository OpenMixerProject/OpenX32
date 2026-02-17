#pragma once

#include <map>
#include <vector>

#include "external.h"
#include "defines.h"
#include "config.h"
#include "types.h"
#include "base.h"

#include "helper.h"

#include "surface.h"
#include "surface-event.h"

#include "dsp1.h"
#include "fpga.h"
#include "adda.h"
#include "card.h"

using namespace std;

class Mixer : public X32Base
{
    using enum MP_ID;

    private:
        // solo is (somewhere) activated
        bool solo = false;

        void halSendGain(uint8_t dspChannel);
        void halSendPhantomPower(uint8_t dspChannel);
        void LoadVChannelLayout();

    public:
                  
        Fpga* fpga;
        Adda* adda;
        Card* card;
        DSP1* dsp;

        Mixer(X32BaseParameter* basepar);
        void Init();
        
        void SetBusSend(uint8_t vChannelIndex, uint8_t index, float value);
        
        void ChangeBusSend(uint8_t p_vChannelIndex, uint8_t encoderIndex, int8_t p_amount, uint8_t activeBusSend);


        float GetBusSend(uint8_t dspChannel, uint8_t index);
        String GetCardModelString();

        void ClearSolo(void);
        bool IsSoloActivated(void);

        void Sync(void);
        uint8_t halGetDspInputSource(uint8_t dspChannel);
};