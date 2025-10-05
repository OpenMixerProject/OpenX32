#pragma once

#include "external.h"
#include "x32config.h"
#include "x32ctrl_types.h"
#include "x32message.h"

#include "vchannel.h"

#include "surface.h"


// The string class
class Mixer
{
    private:
        
        X32Config config;

        uint8_t selectedVChannel;
        uint8_t selectedOutputChannelIndex;


        // solo is (somewhere) activated
        bool solo = false;

        // something was changed - sync surface/gui to mixer state
        uint16_t changed;
        sBankMode modes[3];

        // all virtual - channels / busses / matrix / etc.
        VChannel vChannel[MAX_VCHANNELS];
        sMixerPage pages[MAX_PAGES];

        static X32Message* messages[100];


    public:
        X32_PAGE activePage;
        uint8_t activeBank_inputFader;
        uint8_t activeBank_busFader;
        uint8_t activeEQ;
        uint8_t activeBusSend;

        sTouchControl touchcontrol;

        Mixer(void);
        
        void SetVChannelDefaults(VChannel p_vChannel, uint8_t p_vChannelIndex, bool p_disabled);
        void SetChangeFlags(uint16_t p_flag);
        void SetVChannelChangeFlagsFromIndex(uint8_t p_chanIndex, uint16_t p_flag);
        void SetVChannelChangeFlags(VChannel p_chan, uint16_t p_flag);
        void SetSelect(uint8_t vChannelIndex, bool solo);
        void SetPhantom(uint8_t vChannelIndex, bool p_phantom);
        void SetPhaseInvert(uint8_t vChannelIndex, bool p_phaseInvert);
        void SetSolo(uint8_t vChannelIndex, bool solo);
        void SetMute(uint8_t vChannelIndex, bool solo);
        void SetVolume(uint8_t p_vChannelIndex, float p_volume);
        void SetPeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, float value);

        void ToggleSelect(uint8_t vChannelIndex);
        void TogglePhantom(uint8_t vChannelIndex);
        void TogglePhaseInvert(uint8_t vChannelIndex);
        void ToggleSolo(uint8_t vChannelIndex);
        void ToggleMute(uint8_t vChannelIndex);

        void ChangeVChannel(int8_t amount);
        void ChangeHardwareOutput(int8_t amount);
        void ChangeHardwareInput(int8_t amount);
        void ChangeSelect(uint8_t direction);
        void ChangePan(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeBusSend(uint8_t p_vChannelIndex, uint8_t encoderIndex, int8_t p_amount);
        void ChangeGate(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeLowcut(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeDynamics(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangePeq(uint8_t p_vChannelIndex, char option, int8_t p_amount);
        void ChangeGain(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangeVolume(uint8_t p_vChannelIndex, int8_t p_amount);
        void ChangePeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, int8_t p_amount);

        void ResetChangeFlags(void);
        void ResetVChannelChangeFlags(VChannel p_chan);

        void ClearSolo(void);

        bool IsSoloActivated(void);
        bool IsActiveModeOpenX32(void);
        bool IsActiveModeX32(void);

        bool HasChanged(uint16_t p_flag);
        bool HasAnyChanged(void);

        uint8_t GetSelectedvChannelIndex(void);
        VChannel GetSelectedvChannel(void);
        VChannel GetVChannel(uint8_t VChannelIndex);

        uint8_t SurfaceChannel2vChannel(uint8_t surfaceChannel);
        void SurfaceButtonPressed(X32_BOARD p_board, uint8_t p_index, uint16_t p_value);
        uint8_t GetvChannelIndexFromButtonOrFaderIndex(X32_BOARD p_board, uint16_t p_buttonIndex);
        void SurfaceFaderMoved(X32_BOARD p_board, uint8_t p_faderIndex, uint16_t p_faderValue);
        void SurfaceEncoderTurned(X32_BOARD p_board, uint8_t p_index, uint16_t p_value);
        
        void BankingSends(X32_BTN p_button);
        void BankingEQ(X32_BTN p_button);
        void Banking(X32_BTN p_button);

        void InitPages(void);
        void ShowPage(X32_PAGE page);
        void ShowPrevPage(void);
        void ShowNextPage(void);

        void syncAll(void);
        void guiSync(void);
        void surfaceSync(void);
        void surfaceSyncBoardMain();
        void surfaceSyncBoard(X32_BOARD board);
        void surfaceSyncBankIndicator(void);
        void surfaceUpdateMeter(void);
        void setLedChannelIndicator(void);
        uint8_t surfaceCalcPreampMeter(uint8_t channel);
        uint8_t surfaceCalcDynamicMeter(uint8_t channel);
        void halSyncChannelConfigFromMixer(void);

        void touchcontrolTick(void);
        bool touchcontrolCanSetFader(X32_BOARD p_board, uint8_t p_faderIndex);

        void DebugPrintBank(uint8_t bank);
        void DebugPrintBusBank(uint8_t bank);
        void DebugPrintvChannels(void);
};