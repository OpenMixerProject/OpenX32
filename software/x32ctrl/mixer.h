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

// ini parser
#include "inicpp.h"

using namespace std;
using enum MP_ID;

class Mixer : public X32Base
{
    private:

        // solo is (somewhere) activated
        bool solo = false;

        void halSendGain(uint8_t dspChannel);
        void halSendPhantomPower(uint8_t dspChannel);
        void LoadRoutingDefault();
        void LoadVChannelLayout();

    public:
                  
        Fpga* fpga;
        Adda* adda;
        Card* card;
        DSP1* dsp;

        Mixer(X32BaseParameter* basepar);
        void Init();

        bool LoadConfig(uint scene);
        void SaveConfig(uint scene);
        
        void SetBusSend(uint8_t vChannelIndex, uint8_t index, float value);        
        String GetCardModelString();

        void ClearSolo(void);
        bool IsSoloActivated(void);

        void Sync(void);
        uint8_t halGetDspInputSource(uint8_t dspChannel);
};