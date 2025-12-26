#pragma once

// #include <iostream>

#include "external.h"
#include "config.h"
#include "helper.h"
#include "state.h"
#include "mixer.h"
#include "xremote.h"

#include "page.h"

// Commandline and config file parser CLI11 (https://github.com/CLIUtils/CLI11)
#include "CLI11.hpp"

// ini parser
#include "inicpp.h"

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

#include "tinyosc.h"

using namespace std;

#define FADER_BANK_LAYOUT_X32 0 // fader bank layout like X32 (Channels, Busses, etc.)
#define FADER_BANK_LAYOUT_USER 1

class X32Ctrl : public X32Base {
    private:
        ini::IniFile mixer_ini;

        Mixer* mixer;
        Surface* surface;
        XRemote* xremote;

        sBankMode modes[3];

        map<X32_PAGE, Page*> pages;

        uint8_t activeBank_inputFader;
        uint8_t activeBank_busFader;
        uint8_t activeEQ;
        uint8_t activeBusSend;

        uint8_t selectedVChannel;

        sTouchControl touchcontrol;

        void my_handler(int s);

        void ResetFaderBankLayout();
        void LoadFaderBankLayout(int layout);

        void LoadConfig();

        int surfacePacketCurrentIndex = 0;
        int surfacePacketCurrent = 0;
        char surfacePacketBuffer[SURFACE_MAX_PACKET_LENGTH][6];
        char surfaceBufferUart[256]; // buffer for UART-readings
        uint8_t receivedBoardId = 0; // BoardID from last received surface event, needed for short messages!
        void ProcessUartData();

        void Blink();

    public:
        lv_chart_series_t* chartSeriesEQ;
        lv_chart_series_t* chartSeriesCompressor;
        lv_chart_series_t* chartSeriesCompressorAudio;
        lv_chart_series_t* chartSeriesGate;
        lv_chart_series_t* chartSeriesGateAudio;

        X32Ctrl(X32BaseParameter* basepar);
        void Init();
        void SaveConfig();
        void Tick10ms(void);
        void Tick100ms(void);
        void ProcessEventsRaw(SurfaceEvent* event);
        void UdpHandleCommunication(void);

        void guiFastRefresh();
        void guiSetEncoderText(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6);
        void DrawGate(uint8_t selectedChannelIndex);
        void DrawDynamics(uint8_t selectedChannelIndex);
        void DrawEq(uint8_t selectedChannelIndex);

        void InitPages();
        void ShowPage(X32_PAGE page);
        void ShowPrevPage();
        void ShowNextPage();

        void syncAll(void);
        void guiSync(void);
        void surfaceSync(void);
        void surfaceSyncBoardMain();
        void surfaceSyncBoard(X32_BOARD board);
        void surfaceSyncBoardExtra();
        void SetLcdFromVChannel(uint8_t p_boardId, uint8_t p_Index, uint8_t channelIndex);
        void surfaceSyncBankIndicator(void);
        void surfaceUpdateMeter(void);
        void setLedChannelIndicator(void);
        uint8_t surfaceCalcPreampMeter(uint8_t channel);
        uint8_t surfaceCalcDynamicMeter(uint8_t channel);
        void xremoteSync(bool syncAll);

        void ChangeSelect(int8_t direction);
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

        void DebugPrintBank(uint8_t bank);
        void DebugPrintBusBank(uint8_t bank);
};