#pragma once

// #include <iostream>

#include "external.h"
#include "config.h"
#include "helper.h"
#include "state.h"
#include "mixer.h"
#include "xremote.h"

// includes for timer
#include <time.h>
#include <signal.h>

// includes for lvgl
#include "lv_port_linux/lvgl/lvgl.h"
#include "lv_port_linux/src/lib/backends.h"
#include "lv_port_linux/src/lib/driver_backends.h"

// including the GUI built by EEZ-Studio
#include "eez/src/ui/actions.h"
#include "eez/src/ui/fonts.h"
#include "eez/src/ui/images.h"
#include "eez/src/ui/screens.h"
#include "eez/src/ui/structs.h"
#include "eez/src/ui/styles.h"
#include "eez/src/ui/ui.h"
#include "eez/src/ui/vars.h"

char displayEncoderText[6][30];
static const char* displayEncoderButtonMap[] = {
    displayEncoderText[0],
    displayEncoderText[1],
    displayEncoderText[2],
    displayEncoderText[3],
    displayEncoderText[4],
    displayEncoderText[5],
    NULL};

void parseParams(int argc, char* argv[], State* state);


class X32Ctrl : public X32Base {
    private:
        Mixer* mixer;
        Surface* surface;
        XRemote* xremote;

        sBankMode modes[3];

        sMixerPage pages[MAX_PAGES];  
        X32_PAGE activePage;
        uint8_t activeBank_inputFader;
        uint8_t activeBank_busFader;
        uint8_t activeEQ;
        uint8_t activeBusSend;

        uint8_t selectedVChannel;

        lv_chart_series_t* chartSeriesEQ;

        sTouchControl touchcontrol;

    public:
        X32Ctrl(X32BaseParameter* basepar);
        void Init();
        void Run();
        void Tick10ms(void);
        void Tick100ms(void);
        void ProcessEvents(void);
        void UdpHandleCommunication(void);

        void guiInit(void);
        void guiSetEncoderText(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6);
        void DrawEq(uint8_t selectedChannelIndex);

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

        void ChangeSelect(uint8_t direction);
        void SetSelect(uint8_t vChannelIndex, bool solo);
        void ToggleSelect(uint8_t vChannelIndex);
        uint8_t GetSelectedvChannelIndex(void);
        VChannel* GetSelectedvChannel(void);
        VChannel* GetVChannel(uint8_t VChannelIndex);
        uint8_t SurfaceChannel2vChannel(uint8_t surfaceChannel);
        uint8_t GetvChannelIndexFromButtonOrFaderIndex(X32_BOARD p_board, uint16_t p_buttonIndex);

        void FaderMoved(SurfaceEvent* event);
        void ButtonPressed(SurfaceEvent* event);
        void EncoderTurned(SurfaceEvent* event);

        void BankingSends(X32_BTN p_button);
        void BankingEQ(X32_BTN p_button);
        void Banking(X32_BTN p_button);

        void touchcontrolTick(void);
        bool touchcontrolCanSetFader(X32_BOARD p_board, uint8_t p_faderIndex);

        void DebugPrintBank(uint8_t bank);
        void DebugPrintBusBank(uint8_t bank);
        void DebugPrintvChannels(void);

};