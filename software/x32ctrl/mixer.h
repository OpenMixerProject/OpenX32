#pragma once

#include <map>
#include <vector>

#include "external.h"
#include "defines.h"
#include "config.h"
#include "types.h"
#include "base.h"

#include "helper.h"

#include "vchannel.h"
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

        sPreamps preamps;
        // solo is (somewhere) activated
        bool solo = false;

        void halSendGain(uint8_t dspChannel);
        void halSendPhantomPower(uint8_t dspChannel);
        void LoadVChannelLayout();

    public:
        // all virtual - channels / busses / matrix / etc.
        VChannel* vchannel[MAX_VCHANNELS];        
        Fpga* fpga;
        Adda* adda;
        Card* card;
        DSP1* dsp;

        Mixer(X32BaseParameter* basepar);
        void Init();
        
        void SetVChannelChangeFlagsFromIndex(uint8_t vChannelIndex, uint16_t p_flag);
        void SetPeq(uint8_t vChannelIndex, uint8_t eqIndex, char option, float value);
        void SetBusSend(uint8_t vChannelIndex, uint8_t index, float value);
        
        void ChangeHardwareInput(uint8_t outputIndex, int8_t amount);
	    void ChangeDspInput(uint8_t vChannelIndex, int8_t amount);
        void ChangeDspInputTapPoint(uint8_t vChannelIndex, int8_t amount);
        void ChangeDspOutput(uint8_t channel, int8_t amount);
        void ChangeDspOutputTapPoint(uint8_t channel, int8_t amount);
        void ChangeDspFxOutput(uint8_t channel, int8_t amount);
        void ChangeDspFxOutputTapPoint(uint8_t channel, int8_t amount);
        void ChangeDspAuxOutput(uint8_t channel, int8_t amount);
        void ChangeDspAuxOutputTapPoint(uint8_t channel, int8_t amount);
        void ChangeBusSend(uint8_t p_vChannelIndex, uint8_t encoderIndex, int8_t p_amount, uint8_t activeBusSend);
        void ChangePeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, int8_t p_amount);

        uint16_t GetSelectedVChannelIndex(void);
        VChannel* GetVChannel(uint8_t vCHannelIndex);
        float GetBusSend(uint8_t dspChannel, uint8_t index);
        String GetCardModelString();

        void ResetVChannelChangeFlags(VChannel p_chan);

        void ClearSolo(void);
        bool IsSoloActivated(void);

        void Sync(void);
        uint8_t halGetDspInputSource(uint8_t dspChannel);

        void DebugPrintvChannels(void);

        void ResetFxParameter(uint fxSlot, uint parIdx);  
        void ChangeFxParameter(uint fxSlot, uint parIdx, int8_t amount); 
};