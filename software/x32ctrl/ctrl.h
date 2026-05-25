#pragma once

#include "external.h" // all external includes

#include "x32config.h"
#include "helper.h"
#include "state.h"
#include "mixer.h"
#include "xremote.h"
#include "wsm.h"

#if ENABLE_ARTNET
    #include "artnet.h"
#endif

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
#include "page-setup-surface.h"
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

class X32Ctrl : public X32Base
{
    using enum MP_ID;

    private:
        
        bool intialized = false; // is the whole mixer ready?

        Mixer* mixer;
        Surface* surface;
        XRemote* xremote;
        WSM* wsm;
        LcdMenu* lcdmenu;
        #if ENABLE_ARTNET
        Artnet* artnet;
        #endif

        // currently pressed button
        SurfaceElement* buttonPressed = 0;
        
        // second button pressed, while first button is also pressed
        SurfaceElement* secondbuttonPressed = 0;

        X32FaderBank* banks[(uint)X32BankId::__ELEMENT_COUNTER_DO_NOT_MOVE];

        X32FaderBank* bankLoadedInputsection;
        X32FaderBank* bankLoadedInputsection2;
        X32FaderBank* bankLoadedBussection;

        X32BankId preSpillLoadedBank = X32BankId::None;

        map<X32_PAGE, Page*> pages;
        X32_PAGE lastPage = X32_PAGE::HOME;

        sTouchControl touchcontrol;

        void my_handler(int s);

        void InitBanks();
        void InitBank_Channelstrip(X32FaderBank *bank, uint offset);
        void SetChannelstripBinding(X32FaderBank *bank, uint i, uint chanIndex);
        void InitBank_Channelstrip_DCA(X32FaderBank* bank, uint offset);
        void InitBank_Flex(X32FaderBank* bank);
        void InitBank_DMX(X32FaderBank* bank, uint offset);
        void LoadBank(X32BankTarget target, X32BankId id);
        void LoadAssignBank(X32AssignBankId id);
        void LoadDefaultSurfaceBinding();
        void LoadMainFaderSurfaceBinding();

        int surfacePacketCurrentIndex = 0;
        int surfacePacketCurrent = 0;
        uint8_t surfacePacketBuffer[SURFACE_MAX_PACKET_LENGTH][6];
        char surfaceBufferUart[256]; // buffer for UART-readings
        uint8_t receivedBoardId = 0; // BoardID from last received surface event, needed for short messages!
        void ProcessUartDataSurface();
        void ProcessUartDataAdda();
        void ProcessUartDataAES50();

        uint autosavewait = 0;
        void AutoSave();

    public:

        X32Ctrl(X32BaseParameter* basepar);
        void Init();
        void writeConfigEntry(Mixerparameter *const &parameter, uint index);
        void Tick10ms(void);
        void Tick50ms(void);
        void Tick100ms(void);
        void UdpHandleCommunication(void);
        void UdpHandleCommunication_WSM(void);

        void InitPagesAndGUI();
        bool ShowPrevPage();
        bool ShowNextPage();

        void syncGuiOrLcd(void);
        void syncSurface(bool fullSync);
        
        void SetLcdFromChannel(uint8_t p_boardId, uint8_t lcdIndex, uint8_t channelIndex);
        void SetLcdFromAssign(uint8_t p_boardId, uint8_t lcdIndex, SurfaceElementId element_id);
        void GetAssignLcdText(LcdData *data, SurfaceElementId encoder, SurfaceElementId upper_button, SurfaceElementId lower_button);
#if ENABLE_ARTNET
        void SetLcdFromArtnet(uint8_t p_boardId, uint8_t lcdIndex, uint8_t artnetIndex);
        #endif
        void SetLcdDark(uint8_t p_boardId, uint8_t lcdIndex);
        void UpdateMeters(void);
        void setLedChannelIndicator_Rack(void);        
        void setLedChannelIndicator_Core(void);        
        uint8_t CalcPreampMeter_FullOrCompact(uint8_t channel);
        uint8_t surfaceCalcDynamicMeter(uint8_t channel);
        void syncXRemote(bool syncAll);

        void ProcessSurface(X32_BOARD board, uint8_t classid, uint8_t index, uint16_t value);
        void SimulatorButton(uint key);
}; 