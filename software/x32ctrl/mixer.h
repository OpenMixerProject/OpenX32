#ifndef MIXER_H_
#define MIXER_H_

#include "x32ctrl.h"

#include "demo.h"
#include "surface.h"
#include "dsp.h"
#include "routing.h"
#include "vchannel.h"

// The string class
class Mixer
{
    private:
        X32_MODEL model;
        X32_SURFACE_MODE activeMode;
        uint8_t activeBank_inputFader;
        uint8_t activeBank_busFader;
        uint8_t activeEQ;
        uint8_t activeBusSend;
        uint8_t selectedVChannel;
        uint8_t selectedOutputChannelIndex;
        X32_PAGE activePage;

        // solo is (somewhere) activated
        bool solo;

        // something was changed - sync surface/gui to mixer state
        uint16_t changed;

        sTouchControl touchcontrol;

        sBankMode modes[3];

        // all virtual - channels / busses / matrix / etc.
        VChannel vChannel[MAX_VCHANNELS];
        sMixerPage pages[MAX_PAGES];


        // specific hardware-components
    sFpgaRouting fpgaRouting;
    sDsp dsp;
    sPreamps preamps;

    
    char displayEncoderText[6][30];
    lv_chart_series_t* chartSeriesEQ;

       
   

    public:
        Mixer(String model);
        void SetVChannelDefaults(VChannel p_vChannel, uint8_t p_vChannelIndex, bool p_disabled);
        void ChangeVChannel(int8_t amount);
        void ChangeHardwareOutput(int8_t amount);
        void ChangeHardwareInput(int8_t amount);
        void SetChangeFlags(uint16_t p_flag);

        bool IsModelX32Full(void);
        bool IsModelX32FullOrCompacrOrProducer(void);
        bool IsModelX32FullOrCompacrOrProducerOrRack(void);
        bool IsModelX32CompacrOrProducer(void);
        bool IsModelX32Core(void);
        bool IsModelX32Rack(void);
        bool IsModelX32Compact(void);

        bool HasChanged(uint16_t p_flag);
        bool HasAnyChanged(void);
        void ResetChangeFlags(void);
        void SetVChannelChangeFlagsFromIndex(uint8_t p_chanIndex, uint16_t p_flag);
        void SetVChannelChangeFlags(VChannel p_chan, uint16_t p_flag);
        bool HasVChannelChanged(VChannel p_chan, uint16_t p_flag);
        bool HasVChannelAnyChanged(VChannel p_chan);
        void ResetVChannelChangeFlags(VChannel p_chan);

        uint8_t SurfaceChannel2vChannel(uint8_t surfaceChannel);

        void SurfaceButtonPressed(X32_BOARD p_board, uint8_t p_index, uint16_t p_value);
        uint8_t GetvChannelIndexFromButtonOrFaderIndex(X32_BOARD p_board, uint16_t p_buttonIndex);
        void SurfaceFaderMoved(X32_BOARD p_board, uint8_t p_faderIndex, uint16_t p_faderValue);
        void SurfaceEncoderTurned(X32_BOARD p_board, uint8_t p_index, uint16_t p_value);

        void ChangeSelect(uint8_t direction);
        void SetSelect(uint8_t vChannelIndex, bool solo);
        void ToggleSelect(uint8_t vChannelIndex);
        bool IsSoloActivated(void);
        void ClearSolo(void);

        void SetPhantom(uint8_t vChannelIndex, bool p_phantom);
        void TogglePhantom(uint8_t vChannelIndex);
        void SetPhaseInvert(uint8_t vChannelIndex, bool p_phaseInvert);
        void TogglePhaseInvert(uint8_t vChannelIndex);

        uint8_t GetSelectedvChannelIndex(void);
        VChannel GetSelectedvChannel(void);

        void SetSolo(uint8_t vChannelIndex, bool solo);
        void ToggleSolo(uint8_t vChannelIndex);
        void SetMute(uint8_t vChannelIndex, bool solo);
        void ToggleMute(uint8_t vChannelIndex);
        void BankingSends(X32_BTN p_button);
        void BankingEQ(X32_BTN p_button);
        void Banking(X32_BTN p_button);

        void ChangeGain(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeVolume(uint8_t p_vChannelIndex, int8_t p_amount);
        void SetVolume(uint8_t p_vChannelIndex, float p_volume);
        void ChangePan(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeBusSend(uint8_t p_vChannelIndex, uint8_t encoderIndex, int8_t p_amount);
        void ChangeGate(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeLowcut(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeDynamics(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangePeq(uint8_t p_vChannelIndex, char option, int8_t p_amount);

        void InitPages();
        void ShowPage(X32_PAGE page);
        void ShowPrevPage(void);
        void ShowNextPage(void);

        void DebugPrintBank(uint8_t bank);
        void DebugPrintBusBank(uint8_t bank);
        void DebugPrintvChannels(void);
};

#endif