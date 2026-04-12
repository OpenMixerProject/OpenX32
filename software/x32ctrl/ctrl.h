#pragma once

// #include <iostream>

#include "external.h"
#include "x32config.h"
#include "helper.h"
#include "state.h"
#include "mixer.h"
#include "xremote.h"

#include "page.h"
#include "page-meters.h"
#include "page-rta.h"
#include "page-home.h"
#include "page-config.h"
#include "page-gate.h"
#include "page-dynamics.h"
#include "page-eq.h"
#include "page-sends.h"
#include "page-main.h"
#include "page-routing.h"
#include "page-routing-fpga.h"
#include "page-routing-channels.h"
#include "page-routing-dsp.h"
#include "page-library.h"
#include "page-effects.h"
#include "page-setup.h"
#include "page-setup-card.h"
#include "page-mutegroup.h"
#include "page-debug.h"
#include "page-about.h"
#include "page-scenes.h"
#include "page-prototypegui.h"
#include "lcd-menu.h"

#include "surfaceelement.h"
#include "surfacebindingparameter.h"

// Commandline and config file parser CLI11 (https://github.com/CLIUtils/CLI11)
#include "CLI11.hpp"

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
        
        Mixer* mixer;
        Surface* surface;
        XRemote* xremote;
        LcdMenu* lcdmenu;

        // surface binding
        map<SurfaceElementId, SurfaceBindingParameter*>* surface_binding;
 
        // 4 banks on X32 Full, 8 banks on X32 Compact/Producer
        sBank inputBanks[8];
        sBank busBanks[4];

        X32Bank* banks[10];

        map<X32_PAGE, Page*> pages;
        X32_PAGE lastPage = X32_PAGE::HOME;

        sTouchControl touchcontrol;

        // currently pressed button
        SurfaceElement* buttonPressed = 0;
        
        // second button pressed, while first button is also pressed
        SurfaceElement* secondbuttonPressed = 0;

        void my_handler(int s);

        void InitBanks();
        void InitBank_Channelstrip(X32Bank* bank, uint offset);
        void LoadBank(X32BankTarget target, X32BankId id);
        void InitSurfaceBinding();
        void SurfaceBind(SurfaceElementId surfaceelement_id, SurfaceBindingParameter* binding_parameter);
        void SurfaceBind_MixerParameter(SurfaceElementId surfaceelement_id, SurfaceBindingAction action, MP_ID mixerparaemter_id, uint mixerparameter_index);
        void SurfaceBind_Bank(SurfaceElementId surfaceelement_id, X32BankId bank_id, X32BankTarget target);

        void ResetFaderBankLayout();
        void LoadFaderBankLayout(int layout);

        int surfacePacketCurrentIndex = 0;
        int surfacePacketCurrent = 0;
        uint8_t surfacePacketBuffer[SURFACE_MAX_PACKET_LENGTH][6];
        char surfaceBufferUart[256]; // buffer for UART-readings
        uint8_t receivedBoardId = 0; // BoardID from last received surface event, needed for short messages!
        void ProcessUartDataSurface();
        void ProcessUartDataAdda();
        void ProcessUartDataAES50();

        void ProcessSurface(X32_BOARD board, uint8_t classid, uint8_t index, uint16_t value);

    public:

        X32Ctrl(X32BaseParameter* basepar);
        void Init();
        void writeConfigEntry(Mixerparameter *const &parameter, uint index);
        void Tick10ms(void);
        void Tick50ms(void);
        void Tick100ms(void);
        void ProcessSurfaceEventsRaw(SurfaceEvent* event);
        void UdpHandleCommunication(void);

        void InitPagesAndGUI();
        void ShowPage(X32_PAGE page);
        void ShowPrevPage();
        void ShowNextPage();

        void syncGuiOrLcd(void);
        void syncSurface(bool fullSync);
        
        void surfaceSyncBoardMain(bool fullSync);
        void surfaceSyncBoard(X32_BOARD board, bool fullSync);
        void surfaceSyncBoardExtra(bool fullSync);
        void SetLcdFromChannel(uint8_t p_boardId, uint8_t p_Index, uint8_t channelIndex);
        void SetLcdFromVChannel(uint8_t p_boardId, uint8_t p_Index, uint8_t channelIndex);
        void surfaceSyncBankIndicator(bool fullSync);
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
        void ButtonPressedOrReleased(SurfaceEvent* event);
        void EncoderTurned(SurfaceEvent* event);

        void BankingSends(X32_BTN p_button);
        void BankingEQ(X32_BTN p_button);
        void Banking(X32_BTN p_button);
}; 