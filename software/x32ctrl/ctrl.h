#pragma once

// #include <iostream>

#include "external.h"
#include "config.h"
#include "helper.h"
#include "state.h"
#include "mixer.h"
#include "xremote.h"

#include "page.h"
#include "page-meters.h"
#include "page-home.h"
#include "page-config.h"
#include "page-gate.h"
#include "page-dynamics.h"
#include "page-eq.h"
#include "page-routing.h"
#include "page-routing-fpga.h"
#include "page-routing-channels.h"
#include "page-routing-dsp.h"
#include "page-library.h"
#include "page-effects.h"
#include "page-setup.h"
#include "page-setup-card.h"
#include "page-mutegroup.h"
#include "page-utility.h"

#include "lcd-menu.h"


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

#define PAGE_FUNC_BASE (bool pageInit, bool meterupdate, uint8_t chanIndex, VChannel* chan)
#define PAGE_FUNC_DEF(pagename) void pagename PAGE_FUNC_BASE
#define PAGE_FUNC_IMPL(pagename) void X32Ctrl:: pagename PAGE_FUNC_BASE

class X32Ctrl : public X32Base
{
    using enum MP_ID;

    private:
        ini::IniFile mixer_ini;

        Mixer* mixer;
        Surface* surface;
        XRemote* xremote;
        LcdMenu* lcdmenu;

        sBankMode modes[3];

        map<X32_PAGE, Page*> pages;
        X32_PAGE lastPage = X32_PAGE::HOME;

        sTouchControl touchcontrol;

        // currently pressed button
        X32_BTN buttonPressed = X32_BTN_NONE;
        
        // second button pressed, while first button is also pressed
        X32_BTN secondbuttonPressed = X32_BTN_NONE;

        void my_handler(int s);

        void ResetFaderBankLayout();
        void LoadFaderBankLayout(int layout);

        void LoadConfig();

        int surfacePacketCurrentIndex = 0;
        int surfacePacketCurrent = 0;
        uint8_t surfacePacketBuffer[SURFACE_MAX_PACKET_LENGTH][6];
        char surfaceBufferUart[256]; // buffer for UART-readings
        uint8_t receivedBoardId = 0; // BoardID from last received surface event, needed for short messages!
        void ProcessUartData();

        void Blink();

    public:
        X32Ctrl(X32BaseParameter* basepar);
        void Init();
        void SaveConfig();
        void writeConfigEntry(Mixerparameter *const &parameter, uint index);
        void Tick10ms(void);
        void Tick50ms(void);
        void Tick100ms(void);
        void ProcessEventsRaw(SurfaceEvent* event);
        void UdpHandleCommunication(void);

        void InitPagesAndGUI();
        void ShowPage(X32_PAGE page);
        void ShowPrevPage();
        void ShowNextPage();

        void syncGuiOrLcd(void);
        void syncSurface(void);
        
        void surfaceSyncBoardMain();
        void surfaceSyncBoard(X32_BOARD board);
        void surfaceSyncBoardExtra();
        void SetLcdFromVChannel(uint8_t p_boardId, uint8_t p_Index, uint8_t channelIndex);
        void surfaceSyncBankIndicator(void);
        void UpdateMeters(void);
        void setLedChannelIndicator_Rack(void);        
        void setLedChannelIndicator_Core(void);        
        uint8_t CalcPreampMeter_FullOrCompact(uint8_t channel);
        uint8_t surfaceCalcDynamicMeter(uint8_t channel);
        void syncXRemote(bool syncAll);

        void ChangeSelect(int8_t direction);
        uint8_t SurfaceChannel2vChannel(uint8_t surfaceChannel);
        uint8_t GetvChannelIndexFromButtonOrFaderIndex(X32_BOARD p_board, uint16_t p_buttonIndex);

        void SimulatorButton(uint key);
        void FaderMoved(SurfaceEvent* event);
        void ButtonPressedOrReleased(SurfaceEvent* event);
        void EncoderTurned(SurfaceEvent* event);

        void BankingSends(X32_BTN p_button);
        void BankingEQ(X32_BTN p_button);
        void Banking(X32_BTN p_button);
}; 