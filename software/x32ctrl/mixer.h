#pragma once

#include "external.h"
#include "constants.h"
#include "config.h"
#include "ctrl_types.h"
#include "base.h"

#include "helper.h"

#include "vchannel.h"
#include "surface.h"
#include "surface-event.h"

#include "dsp1.h"
#include "fpga.h"
#include "adda.h"

using namespace std;

// The string class
class Mixer : public X32Base
{
    private:
        sPreamps preamps;
        // solo is (somewhere) activated
        bool solo = false;

    //public:
        Adda* adda;

        void halSendGain(uint8_t dspChannel);
        void halSendPhantomPower(uint8_t dspChannel);

    public:
        // all virtual - channels / busses / matrix / etc.
        VChannel* vchannel[MAX_VCHANNELS];
        //uint8_t selectedOutputChannelIndex = 1;
        Fpga* fpga;
        DSP1* dsp;

        Mixer(X32BaseParameter* basepar);
        void Tick10ms(void);
        void Tick100ms(void);

        void SetVChannelChangeFlagsFromIndex(uint8_t vChannelIndex, uint16_t p_flag);
        void SetBalance(uint8_t vChannelIndex, float p_balance);
        void SetPhantom(uint8_t vChannelIndex, bool p_phantom);
        void SetPhaseInvert(uint8_t vChannelIndex, bool p_phaseInvert);
        void SetSolo(uint8_t vChannelIndex, bool solo);
        void SetMute(uint8_t vChannelIndex, bool mute);
        void SetVolume(uint8_t vChannelIndex, float p_volume);
        void SetVolumeFadervalue(uint8_t vChannelIndex, float volume);
        void SetVolumeOscvalue(uint8_t vChannelIndex, float volume);
        void SetPeq(uint8_t vChannelIndex, uint8_t eqIndex, char option, float value);
        void SetGain(uint8_t vChannelIndex, float gain);
        void SetBusSend(uint8_t vChannelIndex, uint8_t index, float value);
        void SetGate(uint8_t vChannelIndex, float threshold);
        void SetLowcut(uint8_t vChannelIndex, float lowCutFrequency);
        void SetDynamics(uint8_t vChannelIndex, float lowCutFrequency);

        void TogglePhantom(uint8_t vChannelIndex);
        void TogglePhaseInvert(uint8_t vChannelIndex);
        void ToggleSolo(uint8_t vChannelIndex);
        void ToggleMute(uint8_t vChannelIndex);

        void ChangeVChannel(int8_t amount);
        void ChangeGuiSelection(int8_t amount);
        void ChangeHardwareInput(int8_t amount);
	void ChangeDspInput(uint8_t vChannelIndex, int8_t amount);

        void ChangeBalance(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeBusSend(uint8_t p_vChannelIndex, uint8_t encoderIndex, int8_t p_amount, uint8_t activeBusSend);
        void ChangeGate(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeLowcut(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeDynamics(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangePeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, int8_t p_amount);
        void ChangeGain(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeVolume(uint8_t p_vChannelIndex, int8_t p_amount);

        VChannel* GetVChannel(uint8_t vCHannelIndex);
        float GetVolumeDbfs(uint8_t vChannelIndex);
        u_int16_t GetVolumeFadervalue(uint8_t vChannelIndex);
        float GetVolumeOscvalue(uint8_t vChannelIndex);
        bool GetMute(uint8_t dspChannel);
        bool GetSolo(uint8_t dspChannel);
        float GetBalance(uint8_t dspChannel);
        float GetGain(uint8_t dspChannel);
        bool GetPhantomPower(uint8_t dspChannel);
        bool GetPhaseInvert(uint8_t dspChannel);
        float GetBusSend(uint8_t dspChannel, uint8_t index);
        float GetGate(uint8_t vChannelIndex);
        float GetLowcut(uint8_t vChannelIndex);
        float GetDynamics(uint8_t vChannelIndex);

        void ResetVChannelChangeFlags(VChannel p_chan);

        void ClearSolo(void);

        bool IsSoloActivated(void);
        bool IsActiveModeOpenX32(void);
        bool IsActiveModeX32(void);

        void SyncVChannelsToHardware(void);
        uint8_t halGetDspInputSource(uint8_t dspChannel);

        void DebugPrintBank(uint8_t bank);
        void DebugPrintBusBank(uint8_t bank);
        void DebugPrintvChannels(void);
};
