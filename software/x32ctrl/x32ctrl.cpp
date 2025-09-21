/*
                               =#%@@@@@###%@@@@%-
                           =*###+               :#@*
                        +****.                      :%-
                      #++++             ############  :%-          @@@@@@@@@@@@
                    .+===             ###======++==*##  *+       @@@*#%%#****#@@@
                   -=-+               #+======+=.*===*#         @@#**@.*@******%@
                  -=-+                ##======*  #====+#.      @@****@  @******@@
                  +:-                  ##+====#  #***==+#+   =@@**@@@@  @****%@@
                 =*=*                   -#*===#  ## #===+## @@@***@ @@  @@#*@@@
      @@@@@@       ..                     ##+*#- ## #***==#= @@@@@@ @@   +@@@   @@@@@@@  @@@@@
    @@@    @@@                             ##= + ## ## #+==#- @@ @@ @@ = @@@    @@  @@  @@   @@
    @@      .@@#@@@@@@@  @@@ @@@ @@@@@@@@   .# # ## .= #-#++#= @ @  @@ * @*       @@*        @@
    @@       @@ @@    @@ @@@@@@@  @@   @@      # =#  = + *::=#   @  @+ *           -@@@   @@@=
    @@@    @@@  @@:   @@ @@       @@   @@      #  : .- : *::-#   @  +  #             @@ @@    @@
      @@@@@@    @@@@@@@   @@@@@@  @@   @@@  =# # ## :+ #-#++#+ @ @  @@.* @@     @@@@@@  @@@@@@@@
                @@                         ##+ * ## ## #+==#+ @@@@@ @@ = @@@
                @@                        ##+= = ## #***==#+ @@***@ @@   #@@@
                   :                    :#*==+#: ## #===+## @@@***@ @@  @@#*@@#
                  .%+                  ##+====#  #***==+#=   +@@**@@@@  @****%@@
                    %.                ##======*  #====*#  .*-  @@****@  @******@@
                     %=               #*======+: *===*#   +-=+  @@#**@ -@******@@
                      -@-             +##+=====+++=*##  ==-=-    @@@#%@@#****%@@@
                        *@*             =###########  -===*        @@@@@@@@@@@@
                           @@%.                   .::=++*
                             .#@@%%*-.    .:=+**##***+.
                                  .-+%%%%%%#***=-.

  Control software for Behringer X32 using OpenX32
  https://github.com/OpenMixerProject/OpenX32
  v0.0.7, 09.09.2025

  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  Parts of this software with kind permission of Music Tribe. Thank you!
*/

#include "x32ctrl.h"
#include "gui.h"
#include "uart.h"
#include "surface.h"
#include "adda.h"
#include "mixer.h"
#include "fpga.h"
#include "touchcontrol.h"
#include "xremote.h"
#include "spi.h"
#include "routing.h"
#include "dsp.h"
#include "hal.h"

// timer-raw-functions to be called either by linux-timer (X32core) or LVGL-timer (all other models)
void timer100msCallbackLvgl(_lv_timer_t* lv_timer) {timer100msCallback();}
void timer100msCallbackLinux(int timer) {timer100msCallback();}
void timer10msCallbackLvgl(_lv_timer_t* lv_timer) {ui_tick(); timer10msCallback(); syncAll();}
void timer10msCallbackLinux(int timer) {timer10msCallback();}

// called every 100ms
void timer100msCallback() {
    // surface wants to know the current state of all LED's and Meters
    surfaceKeepalive();
    touchcontrolTick();

    // update meters on XRemote-clients
    xremoteUpdateMeter();

    // toggle the LED on DSP1 to show some activity
    spiSendDspParameter_uint32(0, 'a', 42, 0, 2);

    // read the current DSP load
    if (!mixerIsModelX32Core()){
        uint32_t dspClockCycles = spiReadDspParameter_uint32(0, 1, 0);
        float dspLoad = (((float)dspClockCycles/264.0f) / (16.0f/0.048f)) * 100.0f;
        lv_label_set_text_fmt(objects.debugtext, "DSP1: %.2f %%", dspLoad); // DSP0, channel = 1, index = 0
    }
}

// called every 10ms
void timer10msCallback() {
    // read data from surface and calls surfaceCallback()
    surfaceProcessUartData(uartRx(&fdSurface, &surfaceBufferUart[0], sizeof(surfaceBufferUart)));

    // read data from ADDA-Boards (8ch input/output)
    addaProcessUartData(uartRx(&fdAdda, &addaBufferUart[0], sizeof(addaBufferUart)), false);

    // read data from FPGA
    fpgaProcessUartData(uartRx(&fdFpga, &fpgaBufferUart[0], sizeof(fpgaBufferUart)));

    // communication with XRemote-clients via UDP (X32-Edit, MixingStation, etc.)
    xremoteUdpHandleCommunication();
}

#if DEBUG
// Use Encoders on Display for some testing
uint8_t dbg1 = 0;
uint8_t dbg2 = 0;
uint8_t dbg3 = 0;
uint8_t dbg4 = 0;
uint8_t dbg5 = 0;
uint8_t dbg6 = 0;
#endif

// function will be called by uart-receive-function
void surfaceCallback(X32_BOARD boardId, uint8_t classId, uint8_t index, uint16_t value) {
    if (classId == 'f') {
        mixerSurfaceFaderMoved(boardId, index, value);
    } else if (classId == 'b') {

        // handle button press by surface business logic
        mixerSurfaceButtonPressed(boardId, index, value);

#if DEBUG
        //###################################################################
        //#
        //#       below buttons for debugging and development only!
        //#
        //###################################################################

            // calculate user-readable variables
            X32_BTN button = button2enum(((uint16_t)boardId << 8) + (uint16_t)(value & 0x7F));

            bool buttonPressed = (value >> 7) == 1;

            if (buttonPressed){
                switch (button) {
                case X32_BTN_MUTE_GROUP_1:
                    mixerDebugPrintvChannels();
                    break;
                case X32_BTN_MUTE_GROUP_2:
                    mixerDebugPrintBank(0);
                    mixerDebugPrintBusBank(0);
                    break;
                case X32_BTN_MUTE_GROUP_3:
                    addaSetGain(3, 1, 15.5, 1);
                    break;
                default:
                    x32debug("Button  : boardId = 0x%02X | classId = 0x%02X | index = 0x%02X | data = 0x%02X | X32_BTN = 0x%04X\n", boardId, classId, index, value, button);

                    if (!mixerIsModelX32Core()) {
                        (objects.debugtext, "Button  : boardId = 0x%02X | classId = 0x%02X | index = 0x%02X | data = 0x%02X | X32_BTN = 0x%04X\n", boardId, classId, index, value, button);
                    }

                    break;
                }
            }
#endif
            // inform LVGL about this new button-press
            //guiNewButtonPress(button, buttonPressed);  // TODO: needed?
    } else if (classId == 'e') {
        mixerSurfaceEncoderTurned(boardId, index, value);

#if DEBUG

        //###################################################################
        //#
        //#       below buttons for debugging and development only!
        //#
        //###################################################################

        X32_ENC encoder = encoder2enum(((uint16_t)boardId << 8) + (uint16_t)(index));
        int8_t amount = 0;

        if (value > 0 && value < 128){
            amount = (uint8_t)value;
        } else if (value > 128 && value < 256) {
            amount = -(256 - (uint8_t)(value));
        }
        //x32debug("Encoder: %d dbgColor=%d\n", amount, dbgColor);

        switch(encoder){
            case X32_ENC_ENCODER6:
                dbg6 += amount;
                break;
            case X32_ENC_ENCODER5:
                dbg5 += amount;
                break;
            case X32_ENC_ENCODER4:
                dbg4 += amount;
                break;
            case X32_ENC_ENCODER3:
                dbg3 += amount;
                break;
            case X32_ENC_ENCODER2:
                dbg2 += amount;
                break;
            case X32_ENC_ENCODER1:
                dbg1 += amount;
                break;
            default:
                x32debug("Encoder : boardId = 0x%02X | classId = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, classId, index, value);
                if (!mixerIsModelX32Core()) {
                    lv_label_set_text_fmt(objects.debugtext, "Encoder : boardId = 0x%02X | classId = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, classId, index, value);
                }
                break;
        }

        x32debug("DEBUG: dbg1=%d(0x%02X) dbg2=%d(0x%02X) dbg3=%d(0x%02X) dbg4=%d(0x%02X) dbg5=%d(0x%02X) dbg6=%d(0x%02X)\n",
                 dbg1, dbg1, dbg2, dbg2, dbg3, dbg3, dbg4, dbg4, dbg5, dbg5, dbg6, dbg6);

        switch(dbg1){
            case 1:
                // LED Test
                setLed(dbg2, dbg3, dbg4);
                break;
            case 2:
                // Meter Test
                //setMeterLed(dbg2, dbg3, dbg4);
                // boardId = 0, 1, 4, 5, 8
                messageBuilderInit(&message);

                messageBuilderAddRawByte(&message, 0xFE); // startbyte
                messageBuilderAddDataByte(&message, 0x80 + boardId); // start message for specific boardId
                messageBuilderAddDataByte(&message, dbg2); // classId: M = Meter
                messageBuilderAddDataByte(&message, dbg3); // index
                messageBuilderAddDataByte(&message, dbg4);
                messageBuilderAddRawByte(&message, 0xFE); // endbyte

                uartTx(&fdSurface, &message, true);
                break;
            case 3:
                // Meter Main Test
                //setMeterLedMain(dbg1, dbg2, ((uint16_t)dbg4 <<8) | dbg5, 0, 0);
                break;
        }

        // sLCDData* data;
        // data = malloc(sizeof(sLCDData));

        // data->boardId = X32_BOARD_R;
        // data->color = dbgColor;
        // data->index = 8; // Main Fader
        // data->icon.icon = 0x0;
        // data->icon.x = 0;
        // data->icon.y = 0;

        // // // 15,5dB 48V|Ø
        // // // L300Hz G|D|E
        // // // -30dB |---|---|
        // // // Saxophon1

        // // Gain / Lowcut
        // sprintf(data->texts[0].text, "48,5dB 300Hz");
        // data->texts[0].size = 0;
        // data->texts[0].x = 3;
        // data->texts[0].y = 0;

        // // Phanton / Invert / Gate / Dynamics / EQ active
        // sprintf(data->texts[1].text, "48V @ G D E");
        // data->texts[1].size = 0;
        // data->texts[1].x = 10;
        // data->texts[1].y = 15;

        // // Volume / Panorama
        // sprintf(data->texts[2].text, "-100dB ---|---");
        // data->texts[2].size = 0;
        // data->texts[2].x = 8;
        // data->texts[2].y = 30;

        // // vChannel Name
        // if (dbgx<0) {
        //     dbgx=1;
        // }
        // if (dbgy<0) {
        //     dbgy=1;
        // }
        // char* blubb = calloc(dbgx + 5, sizeof(char));

        // for (int t=0;t<dbgx; t++){
        //     blubb[t] = dbgy;
        // }
        // blubb[dbgx]='\0';
        // sprintf(data->texts[3].text, "%s", blubb);
        // data->texts[3].size = 0;
        // data->texts[3].x = 0;
        // data->texts[3].y = 48;

        // setLcdX(data, 4);
        // if (!mixerIsModelX32Core()) {
        //     lv_label_set_text_fmt(objects.debugtext, "dbcolor=%d dbgx=%d dbgy=%d text=%s", dbgColor, dbgx, dbgy, data->texts[3].text);
        // }

        // free(data);
        // data=NULL;

#endif

    } else {
        x32debug("unknown message: boardId = 0x%02X | classId = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, classId, index, value);
    }
}

void addaCallback(char* msg) {
    x32debug("Received message: %s\n", msg);
    if ((strlen(msg) == 4) && (msg[2] == 'Y')) {
        // we received acknowledge-message like *1Y# -> ignore it
    }else if ((strlen(msg) == 5) && ((msg[2] == 'B') && (msg[3] == 'E'))) {
        // we received keep-alive-message *8BE# from expansion-card -> ignore it
    }else{
        // we received other messages -> print them

        // check for *i8CHIN# or *i8CHOUT#
        if ((strlen(msg) >= 8) && ((msg[3] == 'C') && (msg[4] == 'H'))) {
            uint8_t boardId = (uint8_t)msg[1]-0x30;
            uint8_t chanCount = (uint8_t)msg[2]-0x30;

            if ((msg[5] == 'I') && (msg[6] == 'N')) {
                x32debug("Board %d is input-card with %d channels\n", boardId, chanCount);
            }else if ((msg[5] == 'O') && (msg[6] == 'U') && (msg[7] == 'T')) {
                x32debug("Board %d is output-card with %d channels\n", boardId, chanCount);
            }else{
                x32debug("Received message: %s\n", msg);
            }
        }
        if (!mixerIsModelX32Core()) {
            // caution: addaCallback() can be called before the GUI is ready!!!
            //lv_label_set_text_fmt(objects.debugtext, "Received Message: %s\n", msg);
        }
    }
}

void fpgaCallback(char* buf, uint8_t len) {
    // do something with the received data
    // at the moment the FPGA is sending "*X32 #" 5 times per second
    // later it is planned to receive information about audio-levels here
    //printf("Received: %s\n", buf);
    //lv_label_set_text_fmt(objects.debugtext, "Fpga Message: %s\n", buf);
}

// ####################################################################
// #
// #
// #        Sync
// #
// #
// ####################################################################

void syncAll(void) {
    if (mixerHasAnyChanged()){
        if (
            mixerHasChanged(X32_MIXER_CHANGED_PAGE)     ||
            mixerHasChanged(X32_MIXER_CHANGED_BANKING)  ||
            mixerHasChanged(X32_MIXER_CHANGED_SELECT)   ||
            mixerHasChanged(X32_MIXER_CHANGED_VCHANNEL) ||
            mixerHasChanged(X32_MIXER_CHANGED_GUI)
           ) {
            syncGui();
            syncSurface();
        }
        if (mixerHasChanged(X32_MIXER_CHANGED_ROUTING)) {
            routingSyncConfigFromMixer();
        }
        if (mixerHasChanged(X32_MIXER_CHANGED_VCHANNEL)) {
            halSyncVChannelConfigFromMixer();
        }

        mixerResetChangeFlags();
    }
}

// sync mixer state to GUI
void syncGui(void) {
    if (mixerIsModelX32Core()){
        return;
    }

    x32debug("Active Page: %d\n", mixer.activePage);

    s_vChannel* selected_vChannel = mixerGetSelectedvChannel();

    //####################################
    //#         General
    //####################################

    lv_color_t color;
    switch (selected_vChannel->color){
        case SURFACE_COLOR_BLACK:
            color = lv_color_make(0, 0, 0);
            break;
        case SURFACE_COLOR_RED:
            color = lv_color_make(255, 0, 0);
            break;
        case SURFACE_COLOR_GREEN:
            color = lv_color_make(0, 255, 0);
            break;
        case SURFACE_COLOR_YELLOW:
            color = lv_color_make(255, 255, 0);
            break;
        case SURFACE_COLOR_BLUE:
            color = lv_color_make(0, 0, 255);
            break;
        case SURFACE_COLOR_PINK:
            color = lv_color_make(255, 0, 255);
            break;
        case SURFACE_COLOR_CYAN:
            color = lv_color_make(0, 255, 255);
            break;
        case SURFACE_COLOR_WHITE:
            color = lv_color_make(255, 255, 255);
            break;
    }

    lv_label_set_text_fmt(objects.current_channel_number, "vCh%d", selected_vChannel->index);
    lv_label_set_text_fmt(objects.current_channel_name, "%s", selected_vChannel->name);
    lv_obj_set_style_bg_color(objects.current_channel_color, color, 0);


    // //set Encoders to default state
    // const char*  encoderTextMap[] = {"Input", " ", " "," "," ","Output", NULL};
    // lv_btnmatrix_set_map(objects.display_encoders, encoderTextMap);

    //####################################
    //#         Page Home
    //####################################



    //####################################
    //#         Page Config
    //####################################

    uint8_t group = 0;
    uint8_t channel = 0;
    if (mixer.activePage == X32_PAGE_CONFIG){
        char dspSourceName[5] = "";
        char inputSourceName[10] = "";
        routingGetDspSourceName(&dspSourceName[0], selected_vChannel->inputSource.dspChannel);
        sprintf(&inputSourceName[0], "%02d: %s", selected_vChannel->inputSource.dspChannel, dspSourceName);
        lv_label_set_text_fmt(objects.current_channel_source, inputSourceName);
        lv_label_set_text_fmt(objects.current_channel_gain, "%f", (double)selected_vChannel->inputSource.gain);
        lv_label_set_text_fmt(objects.current_channel_phantom, "%d", selected_vChannel->inputSource.phantomPower);
        lv_label_set_text_fmt(objects.current_channel_invert, "%d", selected_vChannel->inputSource.phaseInvert);

        //char outputDestinationName[10] = "";
        //routingGetOutputName(&outputDestinationName[0], mixerGetSelectedvChannel());
        //lv_label_set_text_fmt(objects.current_channel_destination, outputDestinationName);
    }else if (mixer.activePage == X32_PAGE_ROUTING) {
        char outputDestinationName[10] = "";
        char inputSourceName[10] = "";
        uint8_t routingIndex = 0;

        // read name of selected output-routing channel
        routingGetOutputNameByIndex(&outputDestinationName[0], mixer.selectedOutputChannelIndex);
        lv_label_set_text_fmt(objects.hardware_channel_output, outputDestinationName);

        // find name of currently set input-source
		routingIndex = routingGetOutputSourceByIndex(mixer.selectedOutputChannelIndex);
		routingGetSourceNameByIndex(&inputSourceName[0], routingIndex);
        lv_label_set_text_fmt(objects.hardware_channel_source, inputSourceName);
    }


    //####################################
    //#         Page Meters
    //####################################

    for(int i=0; i<=15; i++){
        s_vChannel *chan = &mixer.vChannel[i];

        if (chan->inputSource.phantomPower){
            lv_buttonmatrix_set_button_ctrl(objects.phantomindicators, i, LV_BUTTONMATRIX_CTRL_CHECKED);
        } else {
            lv_buttonmatrix_clear_button_ctrl(objects.phantomindicators, i, LV_BUTTONMATRIX_CTRL_CHECKED);
        }

        switch (i){
                case 0:
                    lv_slider_set_value(objects.slider01, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 1:
                    lv_slider_set_value(objects.slider02, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 2:
                    lv_slider_set_value(objects.slider03, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 3:
                    lv_slider_set_value(objects.slider04, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 4:
                    lv_slider_set_value(objects.slider05, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 5:
                    lv_slider_set_value(objects.slider06, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 6:
                    lv_slider_set_value(objects.slider07, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 7:
                    lv_slider_set_value(objects.slider08, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 8:
                    lv_slider_set_value(objects.slider09, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 9:
                    lv_slider_set_value(objects.slider10, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 10:
                    lv_slider_set_value(objects.slider11, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 11:
                    lv_slider_set_value(objects.slider12, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 12:
                    lv_slider_set_value(objects.slider13, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 13:
                    lv_slider_set_value(objects.slider14, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 14:
                    lv_slider_set_value(objects.slider15, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
                case 15:
                    lv_slider_set_value(objects.slider16, dBfs2fader(chan->volumeLR), LV_ANIM_OFF);
                    break;
        }
    }

    lv_label_set_text_fmt(objects.volumes, "%2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB", 
        (double)mixer.vChannel[0].volumeLR,
        (double)mixer.vChannel[1].volumeLR,
        (double)mixer.vChannel[2].volumeLR,
        (double)mixer.vChannel[3].volumeLR,
        (double)mixer.vChannel[4].volumeLR,
        (double)mixer.vChannel[5].volumeLR,
        (double)mixer.vChannel[6].volumeLR,
        (double)mixer.vChannel[7].volumeLR
    );

    //####################################
    //#         Page Setup
    //####################################

    // selected_vChannel->solo ?
    //     lv_imagebutton_set_state(objects.setup_solo, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED):
    //     lv_imagebutton_set_state(objects.setup_solo, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED);

    // selected_vChannel->mute ?
    //     lv_imagebutton_set_state(objects.setup_mute, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED):
    //     lv_imagebutton_set_state(objects.setup_mute, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED);

}

// sync mixer state to Surface
void syncSurface(void) {
    if ((mixer.activeMode == X32_SURFACE_MODE_BANKING_X32) || (mixer.activeMode == X32_SURFACE_MODE_BANKING_USER))
    {
        syncSurfaceBoardMain();

        if (mixerIsModelX32FullOrCompacrOrProducer()){   
            syncSurfaceBoard(X32_BOARD_L);
            if (mixerIsModelX32Full()){
                syncSurfaceBoard(X32_BOARD_M);
            }
            syncSurfaceBoard(X32_BOARD_R);
            syncSurfaceBankIndicator();
        }
    }
}

void syncSurfaceBoardMain() {
    bool needForSync = false;
    bool fullSync = false;
    s_vChannel* chan = mixerGetSelectedvChannel();

    if (mixerHasChanged(X32_MIXER_CHANGED_SELECT)){ 
        // vChannel selection has changed - do a full sync
        needForSync=true;
        fullSync=true; 
    }

    if (mixerHasChanged(X32_MIXER_CHANGED_VCHANNEL) && chan->changed != X32_VCHANNEL_CHANGED_NONE) {
        // the data in the currently selected vChannel has changed
        needForSync=true;
    }

    if (needForSync){
        if (mixerIsModelX32FullOrCompacrOrProducer()){

            // Channel section
            if (fullSync || mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_PHANTOM)){
                setLedByEnum(X32_BTN_PHANTOM_48V, chan->inputSource.phantomPower); 
            }
            if (fullSync || mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_PHANTOM)){
                setLedByEnum(X32_BTN_PHANTOM_48V, chan->inputSource.phantomPower); 
            }
            if (fullSync || mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_PHASE_INVERT)){
                setLedByEnum(X32_BTN_PHASE_INVERT, chan->inputSource.phaseInvert);
            }
        }

        if (mixerIsModelX32Rack()){
            // Channel section
            setLedChannelIndicator();
            chan = mixerGetSelectedvChannel();

            if (fullSync || mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_SOLO)){
                x32debug(" Solo");
                setLedByEnum(X32_BTN_CHANNEL_SOLO, chan->solo); 
            }
            if (fullSync || mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_MUTE)){
                x32debug(" Mute");
                setLedByEnum(X32_BTN_CHANNEL_MUTE, chan->mute); 
            }
            if (fullSync || mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_VOLUME)){
                // u_int16_t faderVolume = dBfs2fader(chan->volumeLR);
                // uint8_t pct = (faderVolume/VOLUME_MIN
                // setEncoderRing(X32_BOARD_MAIN, 0, 0, , 1);
            }
        }
    }

    // Clear Solo
    if (mixerHasChanged(X32_MIXER_CHANGED_VCHANNEL)){ setLedByEnum(X32_BTN_CLEAR_SOLO, mixerIsSoloActivated()); }
}

void syncSurfaceBoard(X32_BOARD p_board) {
    bool fullSync = false;

    if (mixerHasChanged(X32_MIXER_CHANGED_BANKING)) {
        fullSync=true;
    }

    uint8_t offset = 0;
    if (mixer.model == X32_MODEL_FULL){
        if (p_board == X32_BOARD_M){ offset=8; }
        if (p_board == X32_BOARD_R){ offset=16; }
    } else if ((mixer.model == X32_MODEL_COMPACT) || (mixer.model == X32_MODEL_PRODUCER)) {
        if (p_board == X32_BOARD_R){ offset=8; }
    }

    // update main-channel

    uint8_t maxChannel = 7;
    if ((p_board == X32_BOARD_R) && ((mixer.model == X32_MODEL_FULL) || (mixer.model == X32_MODEL_COMPACT) || (mixer.model == X32_MODEL_PRODUCER))) {
        maxChannel = 8; // include main-channel
    }
    for(int i=0; i<=maxChannel; i++){
        uint8_t vChannelIndex = mixerSurfaceChannel2vChannel(i+offset);

        if (vChannelIndex == VCHANNEL_NOT_SET) {

            // TODO: do only, wenn channel got unassigned

            setLed(p_board, 0x20+i, 0);
            setLed(p_board, 0x30+i, 0);
            setLed(p_board, 0x40+i, 0);
            setFader(p_board, i, 0);
            //  setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
            setLcd(p_board,     i, 0,     0,    0,    0,  0x00,  0,  0,          "",  0x00,  0, 0, "");

        } else {
            s_vChannel *chan = &mixer.vChannel[vChannelIndex];

            if (fullSync || mixerHasVChannelAnyChanged(chan)){
                x32debug("syncronize vChannel%d: %s -", vChannelIndex, chan->name);

                if (fullSync || mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_SELECT)){ 
                    x32debug(" Select");
                    setLed(p_board, 0x20+i, chan->selected);
                }
                if (fullSync || mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_SOLO)){
                    x32debug(" Solo");
                    setLed(p_board, 0x30+i, chan->solo); 
                }
                if (fullSync || mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_MUTE)){
                    x32debug(" Mute");
                    setLed(p_board, 0x40+i, chan->mute); 
                }

                if ((fullSync || mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_VOLUME)) && touchcontrolCanSetFader(p_board, i)){
                    x32debug(" Fader");
                    u_int16_t faderVolume = dBfs2fader(chan->volumeLR);
                    setFader(p_board, i, faderVolume);
                }

                if (
                    fullSync                                                           ||
                    mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_PHASE_INVERT )  ||
                    mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_VOLUME )        ||
                    mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_PHANTOM)        ||
                    mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_COLOR)          ||
                    mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_NAME)
                   )
                {
                    x32debug(" LCD");
                    setLcdFromVChannel(p_board, i, chan);

                    // char lcdText[20];
                    // sprintf(lcdText, "%2.1FdB %s", (double)chan->volumeLR, (chan->inputSource.phantomPower ? "(48V)" : ""));
                    // //  setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
                    // setLcd(p_board,     i, chan->color,     0,    12,    chan->icon,  0x00,  1,  1,          lcdText,  0x00,  1, 47, chan->name);
                }

                x32debug("\n");
            }
        }
    }

    if (p_board == X32_BOARD_R){
        // Clear Solo
        if (mixerHasChanged(X32_MIXER_CHANGED_VCHANNEL)){ setLedByEnum(X32_BTN_CLEAR_SOLO, mixerIsSoloActivated()); }
    }
}


void syncSurfaceBankIndicator(void) {
    if (mixerHasChanged(X32_MIXER_CHANGED_BANKING)){
        if (mixerIsModelX32Full()){
            setLedByEnum(X32_BTN_CH_1_16, 0);
            setLedByEnum(X32_BTN_CH_17_32, 0);
            setLedByEnum(X32_BTN_AUX_IN_EFFECTS, 0);
            setLedByEnum(X32_BTN_BUS_MASTER, 0);
            if (mixer.activeBank_inputFader == 0) { setLedByEnum(X32_BTN_CH_1_16, 1); }
            if (mixer.activeBank_inputFader == 1) { setLedByEnum(X32_BTN_CH_17_32, 1); }
            if (mixer.activeBank_inputFader == 2) { setLedByEnum(X32_BTN_AUX_IN_EFFECTS, 1); }
            if (mixer.activeBank_inputFader == 3) { setLedByEnum(X32_BTN_BUS_MASTER, 1); }
        }
        if (mixerIsModelX32CompacrOrProducer()) {
            setLedByEnum(X32_BTN_CH_1_8, 0);
            setLedByEnum(X32_BTN_CH_9_16, 0);
            setLedByEnum(X32_BTN_CH_17_24, 0);
            setLedByEnum(X32_BTN_CH_25_32, 0);
            setLedByEnum(X32_BTN_AUX_IN_1_6_USB_REC, 0);
            setLedByEnum(X32_BTN_EFFECTS_RETURNS, 0);
            setLedByEnum(X32_BTN_BUS_1_8_MASTER, 0);
            setLedByEnum(X32_BTN_BUS_9_16_MASTER, 0);
            if (mixer.activeBank_inputFader == 0) { setLedByEnum(X32_BTN_CH_1_8, 1); }
            if (mixer.activeBank_inputFader == 1) { setLedByEnum(X32_BTN_CH_9_16, 1); }
            if (mixer.activeBank_inputFader == 2) { setLedByEnum(X32_BTN_CH_17_24, 1); }
            if (mixer.activeBank_inputFader == 3) { setLedByEnum(X32_BTN_CH_25_32, 1); }
            if (mixer.activeBank_inputFader == 4) { setLedByEnum(X32_BTN_AUX_IN_1_6_USB_REC, 1); }
            if (mixer.activeBank_inputFader == 5) { setLedByEnum(X32_BTN_EFFECTS_RETURNS, 1); }
            if (mixer.activeBank_inputFader == 6) { setLedByEnum(X32_BTN_BUS_1_8_MASTER, 1); }
            if (mixer.activeBank_inputFader == 7) { setLedByEnum(X32_BTN_BUS_9_16_MASTER, 1); }
        }
        setLedByEnum(X32_BTN_GROUP_DCA_1_8, 0);
        setLedByEnum(X32_BTN_BUS_1_8, 0);
        setLedByEnum(X32_BTN_BUS_9_16, 0);
        setLedByEnum(X32_BTN_MATRIX_MAIN_C, 0);
        if (mixer.activeBank_busFader == 0) { setLedByEnum(X32_BTN_GROUP_DCA_1_8, 1); }
        if (mixer.activeBank_busFader == 1) { setLedByEnum(X32_BTN_BUS_1_8, 1); }
        if (mixer.activeBank_busFader == 2) { setLedByEnum(X32_BTN_BUS_9_16, 1); }
        if (mixer.activeBank_busFader == 3) { setLedByEnum(X32_BTN_MATRIX_MAIN_C, 1); }
    }
}

void x32log(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    vprintf(format, args);
    fflush(stdout); // immediately write to console!

    va_end(args);
}

void x32debug(const char* format, ...)
{
#if DEBUG == 1
    va_list args;
    va_start(args, format);

    vprintf(format, args);
    fflush(stdout); // immediately write to console!

    va_end(args);
#endif
}

void parseParams(int argc, char* argv[], int8_t* fpga, int8_t* dsp1, int8_t* dsp2, int8_t* noinit) {
    for (int8_t i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-fpga") == 0) {
            if (i + 1 < argc) {
                *fpga = i+1;
                i++;
            } else {
                *fpga = -1;
            }
        }
        else if (strcmp(argv[i], "-dsp1") == 0) {
            if (i + 1 < argc) {
                *dsp1 = i+1;
                i++;
            } else {
                *dsp1 = -1;
            }
        }
        else if (strcmp(argv[i], "-dsp2") == 0) {
            if (i + 1 < argc) {
                *dsp2 = i+1;
                i++;
            } else {
                *dsp2 = -1;
            }
        }
        else if (strcmp(argv[i], "-noinit") == 0) {
            if (i + 1 < argc) {
                *noinit = i+1;
                i++;
            } else {
                *noinit = -1;
            }
        }
        // handle unknown parameters
        else {
            printf("Unknown parameter: %s\n", argv[i]);
        }
    }
}

// the main-function - of course
int main(int argc, char* argv[]) {
    srand(time(NULL));

    x32log("  ____                  __   ______ ___  \n");
    x32log(" / __ \\                 \\ \\ / /___ \\__ \\ \n");
    x32log("| |  | |_ __   ___ _ __  \\ V /  __) | ) |\n");
    x32log("| |  | | '_ \\ / _ \\ '_ \\  > <  |__ < / / \n");
    x32log("| |__| | |_) |  __/ | | |/ . \\ ___) / /_ \n");
    x32log(" \\____/| .__/ \\___|_| |_/_/ \\_\\____/____|\n");
    x32log("       | |                               \n");
    x32log("       |_|                               \n");
    x32log("OpenX32 Main Control\n");
    x32log("v0.1.0, 19.09.2025\n");
    x32log("https://github.com/OpenMixerProject/OpenX32\n");

    // first try to find what we are: Fullsize, Compact, Producer, Rack or Core
    x32debug("Reading config...\n");
    char model[12];
    char serial[14];
    char date[16];
    readConfig("/etc/x32.conf", "MDL=", model, 12);
    readConfig("/etc/x32.conf", "SN=", serial, 14);
    readConfig("/etc/x32.conf", "DATE=", date, 16);
    x32log("Detected model: %s with Serial %s built on %s\n", model, serial, date);

    mixerInit(model);

    // check start-switches
    int8_t switchFpga = -1;
    int8_t switchDsp1 = -1;
    int8_t switchDsp2 = -1;
    int8_t switchNoinit = -1;
    parseParams(argc, argv, &switchFpga, &switchDsp1, &switchDsp2, &switchNoinit);
    // initializing DSPs and FPGA
    if (switchFpga > 0) { spiConfigureFpga(argv[switchFpga]); }
    if ((switchDsp1 > 0) && (switchDsp2 == -1)) { spiConfigureDsp(argv[switchDsp1], "", 1); }
    if ((switchDsp1 > 0) && (switchDsp2 > 0)) { spiConfigureDsp(argv[switchDsp1], argv[switchDsp2], 2); }

    x32log("Initializing communication...\n");
    //uartOpen("/dev/ttymxc0", 115200, &fdDebug); // this UART is not accessible from the outside
    uartOpen("/dev/ttymxc1", 115200, &fdSurface, true); // this UART is connected to the surface (Fader, LEDs, LCDs, Buttons) directly
    uartOpen("/dev/ttymxc2", 38400, &fdAdda, true); // this UART is connected to the FPGA and routed to the 8-channel AD/DA-boards and the Expansion Card
    uartOpen("/dev/ttymxc3", 115200, &fdFpga, true); // this UART is connected to the FPGA
    //uartOpen("/dev/ttymxc4", 115200, &fdMidi); // this UART is connected to the MIDI-connectors but is used by the Linux-console
    spiOpenDspConnections();
    xremoteInit();

    if (switchNoinit == -1) {
        x32log("Initializing X32 Surface...\n");
        //surfaceReset(); // resets all microcontrollers on the board (not necessary if no error occured)
        surfaceInit(); // initialize whole surface with default values

        x32log("Initializing X32 Audio...\n");
        addaInit(dsp.samplerate);
    }

    x32log("Setting up FPGA and DSPs...\n");
    routingInit();
    dspInit();
    dspSendAll();

    // unmute the local audio-boards
    addaSetMute(false);

    if (mixerIsModelX32Core()){
        // only necessary if LVGL is not used
        x32log("Starting Timer...\n");
        init100msTimer(); // start 100ms-timer only for Non-GUI systems

        mixerSetChangeFlags(X32_MIXER_CHANGED_ALL); // trigger first sync to gui/surface

        x32log("Wait for incoming data on /dev/ttymxc1...\n");
        x32log("Press Ctrl+C to terminate program.\n");

        while (1) {
            // run service-tasks
            timer10msCallback();

            // sleep for 1ms to lower CPU-load
            usleep(1000);
        }

    } else {
        x32log("Initializing GUI...\n");
        guiInit(); // initializes LVGL, FBDEV and starts endless loop
    }

    return 0;
}
