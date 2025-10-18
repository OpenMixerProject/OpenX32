#pragma once

#include "external.h"
#include "config.h"
#include "x32ctrl_types.h"
#include "x32base.h"

#include "helper.h"

#include "vchannel.h"
#include "surface.h"
#include "surface-event.h"

#include "dsp1.h"
#include "fpga.h"
#include "adda.h"

using namespace std;

#define DEBUG 0

// The string class
class Mixer : public X32Base
{
    private:
        sPreamps preamps;
        uint8_t selectedOutputChannelIndex;
        // solo is (somewhere) activated
        bool solo = false;

    //public:
        Fpga* fpga;
        Adda* adda;

        void halSyncChannelsToMixer(void);
        void halSetVolume(uint8_t dspChannel, float volume);
        void halSetMute(uint8_t dspChannel, bool mute);
        void halSetSolo(uint8_t dspChannel, bool solo);
        void halSetBalance(uint8_t dspChannel, float balance);
        void halSetGain(uint8_t dspChannel, float gain);
        void halSetPhantomPower(uint8_t dspChannel, bool phantomPower);
        void halSetPhaseInversion(uint8_t dspChannel, bool phaseInverted);
        void halSetBusSend(uint8_t dspChannel, uint8_t index, float value);

        void halSendGain(uint8_t dspChannel);
        void halSendPhantomPower(uint8_t dspChannel);

    public:
        DSP1* dsp;
        // all virtual - channels / busses / matrix / etc.
        VChannel* vchannel[MAX_VCHANNELS];

        Mixer(Config* config, State* state);
        void ProcessUartData(void);

        void SetVChannelChangeFlagsFromIndex(uint8_t p_chanIndex, uint16_t p_flag);
        void SetBalance(uint8_t p_vChannelIndex, float p_balance);
        void SetPhantom(uint8_t vChannelIndex, bool p_phantom);
        void SetPhaseInvert(uint8_t vChannelIndex, bool p_phaseInvert);
        void SetSolo(uint8_t vChannelIndex, bool solo);
        void SetMute(uint8_t vChannelIndex, bool solo);
        void SetVolume(uint8_t p_vChannelIndex, float p_volume);
        void SetPeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, float value);

        void TogglePhantom(uint8_t vChannelIndex);
        void TogglePhaseInvert(uint8_t vChannelIndex);
        void ToggleSolo(uint8_t vChannelIndex);
        void ToggleMute(uint8_t vChannelIndex);

        void ChangeVChannel(int8_t amount);
        void ChangeHardwareOutput(int8_t amount);
        void ChangeHardwareInput(int8_t amount);

        void ChangeBalance(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeBusSend(uint8_t p_vChannelIndex, uint8_t encoderIndex, int8_t p_amount, uint8_t activeBusSend);
        void ChangeGate(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeLowcut(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeDynamics(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangePeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, int8_t p_amount);
        void ChangeGain(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeVolume(uint8_t p_vChannelIndex, int8_t p_amount);

        void ResetVChannelChangeFlags(VChannel p_chan);

        void ClearSolo(void);

        bool IsSoloActivated(void);
        bool IsActiveModeOpenX32(void);
        bool IsActiveModeX32(void);

        void SyncVChannelsToHardware(void);
        uint8_t halGetDspInputSource(uint8_t dspChannel);
        float halGetVolume(uint8_t dspChannel);
        bool halGetMute(uint8_t dspChannel);
        bool halGetSolo(uint8_t dspChannel);
        float halGetBalance(uint8_t dspChannel);
        float halGetGain(uint8_t dspChannel);
        bool halGetPhantomPower(uint8_t dspChannel);
        bool halGetPhaseInvert(uint8_t dspChannel);
        float halGetBusSend(uint8_t dspChannel, uint8_t index);


        void DebugPrintBank(uint8_t bank);
        void DebugPrintBusBank(uint8_t bank);
        void DebugPrintvChannels(void);
};