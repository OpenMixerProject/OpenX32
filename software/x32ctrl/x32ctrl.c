/*
  Control software for Behringer X32 using OpenX32
  https://github.com/xn--nding-jua/OpenX32
  v0.0.3, 02.08.2025

  This software uses LVGL and the LVGL Linux Port to display a nice user-interface
  To compile the software just call "make" and a 32-bit ARM-binary should be created
  in the build/bin-folder

  Parts of this software with kind permission of Music Tribe. Thank you!
*/

#include "x32ctrl.h"



// called every 100ms
void timer100msCallback(struct _lv_timer_t *) {

    mixerTouchControllTick();
    
    if (mixerIsDirty())
    {
        syncGui();
        syncSurface();
        mixingSyncRoutingConfigFromMixer();

        mixerSetDirtClean();
    }
}

void timer10msCallback(struct _lv_timer_t *) {
  // call EEZ-GUI tick
  ui_tick();

  // read data from surface and calls surfaceCallback()
  surfaceProcessUartData(uartRx(&fdSurface, &uartBufferSurface[0], sizeof(uartBufferSurface)));

  // read data from ADDA-Boards (8ch input/output)
  addaProcessUartData(uartRx(&fdAdda, &uartBufferAdda[0], sizeof(uartBufferAdda)));

  // read data from FPGA
  fpgaProcessUartData(uartRx(&fdFpga, &uartBufferFpga[0], sizeof(uartBufferFpga)));
}

void surfaceDemo(void) {
    printf("  Setting Button-LEDs for boards 0 and 1...\n");
    setLedByEnum(X32_BTN_TALK_B, 1);
    
    setLedByEnum(X32_BTN_EQ, 1);
    setLedByEnum(X32_BTN_EQ_HIGH_MID, 1);
    setLedByEnum(X32_BTN_VIEW_EQ, 1);

    printf("  Setting Button-LEDs for boards 4, 5 and 8...\n");
    for (uint8_t i=0; i<=8; i++) {
      // select-LEDs
      setLed(4, 0x20+i, 1); // boardId 4 = faderL
      setLed(5, 0x20+i, 1); // boardId 5 = faderM
      setLed(8, 0x20+i, 1); // boardId 8 = faderR

      // cue leds
      setLed(4, 0x30+i, 1); // boardId 4 = faderL
      setLed(5, 0x30+i, 1); // boardId 5 = faderM
      setLed(8, 0x30+i, 1); // boardId 8 = faderR

      // mute leds
      setLed(4, 0x40+i, 1); // boardId 4 = faderL
      setLed(5, 0x40+i, 1); // boardId 5 = faderM
      setLed(8, 0x40+i, 1); // boardId 8 = faderR

      // random leds
//      setLed(4, 0x00+i, 1); // boardId 4 = faderL
//      setLed(5, 0x00+i, 1); // boardId 5 = faderM
//      setLed(8, 0x00+i, 1); // boardId 8 = faderR
    }

    // fader ranges from 0...8
    printf("  Setting Faders for boards 4, 5 and 8...\n");
    for (uint8_t i=0; i<=8; i++) {
      setFader(4, i, 0x000 + 0xAA*i);
      setFader(5, i, 0x555 + 0xAA*i);
      setFader(8, i, 0xAAA + 0xAA*i);
    }

    // meterLED ranges from 1...9
    printf("  Setting VU-Meters for boards 1, 4, 5 and 8...\n");
    setMeterLed(1, 0, 0b00000011);
    for (uint8_t i=0; i<=7; i++) {
      setMeterLed(4, i, 0b00000011);
      setMeterLed(5, i, 0b00001111);
      setMeterLed(8, i, 0b01111111);
    }

    // preamp, dynamics, meterL, meterR, meterSolo
    setMeterLedMain(0b00000111, 0b11100001, 0x0FFFFF, 0x0FFFFF, 0x000FFF);

    // set some encoders with different options
    for (uint8_t i=0; i<=100; i++) {
      setEncoderRing(0, 0, 0, i, true); // boardId, index, ledMode, ledPct, backlight
      setEncoderRing(0, 1, 1, i, true); // boardId, index, ledMode, ledPct, backlight
      setEncoderRing(0, 2, 2, i, true); // boardId, index, ledMode, ledPct, backlight
      setEncoderRing(0, 3, 3, i, true); // boardId, index, ledMode, ledPct, backlight
      usleep(5000);
    }

    // set display
    printf("  Setting Displays for board 0...\n");
    for (uint8_t i=0; i<4; i++) {
      setLcd(0, i, 7, 70, 8, 0xE2, 0x20, 0, 0, "Board 0", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
    }

    printf("  Setting Displays for boards 4, 5 and 8...\n");
    for (uint8_t i=0; i<=8; i++) {
      setLcd(4, i, 1, 70, 8, 0xE2, 0x20, 0, 0, "Board 1", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
      setLcd(5, i, 2, 70, 8, 0xE2, 0x20, 0, 0, "Board 2", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
      setLcd(8, i, 4, 70, 8, 0xE2, 0x20, 0, 0, "Board 3", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
    }
}

// function will be called by uart-receive-function
void surfaceCallback(uint8_t boardId, uint8_t class, uint8_t index, uint16_t value) {
    if (class == 'f') {
        mixerSurfaceFaderMoved(boardId, index, value);
    } else if (class == 'b') {

        // handle button press by surface business logic
        bool processed = mixerSurfaceButtonPressed(boardId, index, value);

        //###################################################################
        //#
        //#       below buttons for debugging and development only!
        //#
        //###################################################################
        if (!processed)
        {
            // calculate user-readable variables
            X32_BTN button = button2enum(((uint16_t)boardId << 8) + (uint16_t)(value & 0x7F));

            bool buttonPressed = (value >> 7) == 1;
        
            if (buttonPressed){
                switch (button) {
                    case X32_BTN_LEFT:
                    // set routing to DSP-channels 1-8 which is just an 8-channel tone-generator at the moment
                    for (uint8_t ch=1; ch<=8; ch++) {
                        mixingSetRouting('x', ch, mixingGetInputSource('d', ch)); // set xlr-output 1-8
                        mixingSetRouting('x', ch+8, mixingGetInputSource('d', ch)); // set xlr-output 9-16

                        mixingSetRouting('a', ch, mixingGetInputSource('d', ch)); // set aux-channel 1-8

                        mixingSetRouting('c', ch, mixingGetInputSource('d', ch)); // set card-output 1-8
                        mixingSetRouting('c', ch+8, mixingGetInputSource('d', ch)); // set card-output 9-16
                        mixingSetRouting('c', ch+16, mixingGetInputSource('d', ch)); // set card-output 17-24
                        mixingSetRouting('c', ch+24, mixingGetInputSource('d', ch)); // set card-output 25-32

                        mixingSetRouting('p', ch, mixingGetInputSource('d', ch)); // set P16-output 1-8
                        mixingSetRouting('p', ch+8, mixingGetInputSource('d', ch)); // set P16-output 9-16
                    }
                    // transmit routing-configuration to FPGA
                    mixingTxRoutingConfig();

                    break;
                case X32_BTN_RIGHT:
                    // set routing to XLR-inputs
                    for (uint8_t ch=1; ch<=8; ch++) {
                        mixingSetRouting('x', ch, mixingGetInputSource('x', ch)); // set xlr-output 1-8 to xlr-inputs 1-8
                        mixingSetRouting('x', ch+8, mixingGetInputSource('x', ch+8)); // set xlr-output 9-16 to xlr-inputs 9-16

                        mixingSetRouting('a', ch, mixingGetInputSource('x', ch)); // set aux-channel 1-8 to aux-inputs 1-8

                        mixingSetRouting('c', ch, mixingGetInputSource('x', ch)); // set card-output 1-8 to xlr-inputs 1-8
                        mixingSetRouting('c', ch+8, mixingGetInputSource('x', ch+8)); // set card-output 9-16 to xlr-inputs 9-16
                        mixingSetRouting('c', ch+16, mixingGetInputSource('x', ch+16)); // set card-output 17-24 to xlr-inputs 17-24
                        mixingSetRouting('c', ch+24, mixingGetInputSource('x', ch+24)); // set card-output 25-32 to xlr-inputs 25-32

                        mixingSetRouting('p', ch, mixingGetInputSource('x', ch)); // set P16-output 1-8 to xlr-inputs 1-8
                        mixingSetRouting('p', ch+8, mixingGetInputSource('x', ch+8)); // set P16-output 9-16 to xlr-inputs 9-16
                    }
                    // transmit routing-configuration to FPGA
                    mixingTxRoutingConfig();

                    break;
                case X32_BTN_UP:
                    // set routing to AUX-inputs
                    for (uint8_t ch=1; ch<=8; ch++) {
                        mixingSetRouting('x', ch, mixingGetInputSource('a', ch)); // set xlr-output 1-8 to xlr-inputs 1-8
                        mixingSetRouting('x', ch+8, mixingGetInputSource('a', ch)); // set xlr-output 9-16 to xlr-inputs 9-16

                        mixingSetRouting('a', ch, mixingGetInputSource('a', ch)); // set aux-channel 1-8 to aux-inputs 1-8

                        mixingSetRouting('c', ch, mixingGetInputSource('a', ch)); // set card-output 1-8 to xlr-inputs 1-8
                        mixingSetRouting('c', ch+8, mixingGetInputSource('a', ch)); // set card-output 9-16 to xlr-inputs 9-16
                        mixingSetRouting('c', ch+16, mixingGetInputSource('a', ch)); // set card-output 17-24 to xlr-inputs 17-24
                        mixingSetRouting('c', ch+24, mixingGetInputSource('a', ch)); // set card-output 25-32 to xlr-inputs 25-32

                        mixingSetRouting('p', ch, mixingGetInputSource('a', ch)); // set P16-output 1-8 to xlr-inputs 1-8
                        mixingSetRouting('p', ch+8, mixingGetInputSource('a', ch)); // set P16-output 9-16 to xlr-inputs 9-16
                    }
                    // transmit routing-configuration to FPGA
                    mixingTxRoutingConfig();

                    break;
                case X32_BTN_DOWN:
                    // set routing to Card-inputs
                    for (uint8_t ch=1; ch<=8; ch++) {
                        mixingSetRouting('x', ch, mixingGetInputSource('c', ch)); // set xlr-output 1-8 to xlr-inputs 1-8
                        mixingSetRouting('x', ch+8, mixingGetInputSource('c', ch+8)); // set xlr-output 9-16 to xlr-inputs 9-16

                        mixingSetRouting('a', ch, mixingGetInputSource('c', ch)); // set aux-channel 1-8 to aux-inputs 1-8

                        mixingSetRouting('c', ch, mixingGetInputSource('c', ch)); // set card-output 1-8 to xlr-inputs 1-8
                        mixingSetRouting('c', ch+8, mixingGetInputSource('c', ch+8)); // set card-output 9-16 to xlr-inputs 9-16
                        mixingSetRouting('c', ch+16, mixingGetInputSource('c', ch+16)); // set card-output 17-24 to xlr-inputs 17-24
                        mixingSetRouting('c', ch+24, mixingGetInputSource('c', ch+24)); // set card-output 25-32 to xlr-inputs 25-32

                        mixingSetRouting('p', ch, mixingGetInputSource('c', ch)); // set P16-output 1-8 to xlr-inputs 1-8
                        mixingSetRouting('p', ch+8, mixingGetInputSource('c', ch+8)); // set P16-output 9-16 to xlr-inputs 9-16
                    }
                    // transmit routing-configuration to FPGA
                    mixingTxRoutingConfig();

                    addaSendCmd("*8C80U#"); // switch Card to 48kHz and USB-input

                    break;
                case X32_BTN_MUTE_GROUP_1:
                    mixerDebugPrintvChannels();
                    break;
                case X32_BTN_MUTE_GROUP_2:
                    mixerDebugPrintBank(0);
                    mixerDebugPrintBank(1);
                    mixerDebugPrintBank(2);
                    mixerDebugPrintBank(3);
                    break;
                default:

                    x32debug("Button  : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
                    lv_label_set_text_fmt(objects.debugtext, "Button  : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);

                    break;
                }
            }
            
            // inform LVGL about this new button-press
            guiNewButtonPress(button, buttonPressed);  // TODO: needed?
        }      
    } else if (class == 'e') {
      // calculate user-readable variables
      //uint16_t encoderNr = ((uint16_t)boardId << 8) + (uint16_t)index;
      //int16_t velocity = (int16_t)value;

      x32debug("Encoder : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
      lv_label_set_text_fmt(objects.debugtext, "Encoder : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
    } else {
        x32debug("unknown message: boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
    }
}

void addaCallback(char *msg) {
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

        lv_label_set_text_fmt(objects.debugtext, "Received Message: %s\n", msg);
    }
}

void fpgaCallback(char *buf, uint8_t len) {
    // do something with the received data
    // at the moment the FPGA is sending "*X32 #" 5 times per second
    // later it is planned to receive information about audio-levels here
    //printf("Received: %s\n", buf);
    //lv_label_set_text_fmt(objects.debugtext, "Fpga Message: %s\n", buf);
}

// sync mixer state to GUI
void syncGui(void) {

    s_vChannel* selected_vChannel = &mixer.vChannel[mixerGetSelectedvChannel()];

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
    
    lv_label_set_text_fmt(objects.current_channel_number, "CH%d", selected_vChannel->number);
    lv_label_set_text_fmt(objects.current_channel_name, "%s", selected_vChannel->name);
    lv_obj_set_style_bg_color(objects.current_channel_color, color, 0);

    //####################################
    //#         Page Home
    //####################################

    char inputSourceName[10] = "";
    mixingGetInputName(&inputSourceName, selected_vChannel->inputSource.group, selected_vChannel->inputSource.hardwareChannel);
    lv_label_set_text_fmt(objects.current_channel_source, inputSourceName);
    lv_label_set_text_fmt(objects.current_channel_gain, "%f", (double)selected_vChannel->inputSource.gain);
    lv_label_set_text_fmt(objects.current_channel_phantom, "%d", selected_vChannel->inputSource.phantomPower);
    lv_label_set_text_fmt(objects.current_channel_invert, "%d", selected_vChannel->inputSource.phaseInvert);


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
                    lv_slider_set_value(objects.slider01, dBfs2fader(chan->volume), LV_ANIM_OFF);                     
                    break;
                case 1:
                    lv_slider_set_value(objects.slider02, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 2:
                    lv_slider_set_value(objects.slider03, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 3:
                    lv_slider_set_value(objects.slider04, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 4:
                    lv_slider_set_value(objects.slider05, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 5:
                    lv_slider_set_value(objects.slider06, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 6:
                    lv_slider_set_value(objects.slider07, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 7:
                    lv_slider_set_value(objects.slider08, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 8:
                    lv_slider_set_value(objects.slider09, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 9:
                    lv_slider_set_value(objects.slider10, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 10:
                    lv_slider_set_value(objects.slider11, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 11:
                    lv_slider_set_value(objects.slider12, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 12:
                    lv_slider_set_value(objects.slider13, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 13:
                    lv_slider_set_value(objects.slider14, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 14:
                    lv_slider_set_value(objects.slider15, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
                case 15:
                    lv_slider_set_value(objects.slider16, dBfs2fader(chan->volume), LV_ANIM_OFF);
                    break;
        }
    }

    lv_label_set_text_fmt(objects.volumes, "%2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB", 
        (double)mixer.vChannel[0].volume,
        (double)mixer.vChannel[1].volume,
        (double)mixer.vChannel[2].volume,
        (double)mixer.vChannel[3].volume,
        (double)mixer.vChannel[4].volume,
        (double)mixer.vChannel[5].volume,
        (double)mixer.vChannel[6].volume,
        (double)mixer.vChannel[7].volume
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
    syncSurfaceBoardMain();
    syncSurfaceBoard(X32_BOARD_L);
    if (mixer.model == X32_MODEL_FULL){
        syncSurfaceBoard(X32_BOARD_M);
    }
    syncSurfaceBoard(X32_BOARD_R);
    syncSurfaceBankIndicator();
}

void syncSurfaceBoardMain() {
    // Phantom
    if (mixerIsPhantomDirty()) { setLedByEnum(X32_BTN_PHANTOM_48V, mixer.vChannel[mixer.selectedVChannel].inputSource.phantomPower); }
    // Phase Invert
    if (mixerIsPhaseInvertDirty()) { setLedByEnum(X32_BTN_PHASE_INVERT, mixer.vChannel[mixer.selectedVChannel].inputSource.phaseInvert); }
}

void syncSurfaceBoard(X32_BOARD p_board) {
    uint8_t offset = 0;
    if (mixer.model == X32_MODEL_FULL){
        if (p_board == X32_BOARD_M){ offset=8; }
        if (p_board == X32_BOARD_R){ offset=16; }
    } else if ((mixer.model == X32_MODEL_COMPACT) || (mixer.model == X32_MODEL_PRODUCER)) {
        if (p_board == X32_BOARD_R){ offset=8; }
    }

    for(int i=0; i<=7; i++){

        uint8_t vChannelIndex = mixerSurfaceChannel2vChannel(i+offset);

        if (vChannelIndex == VCHANNEL_NOT_SET) {
            
            setLed(p_board, 0x20+i, 0);
            setLed(p_board, 0x30+i, 0);
            setLed(p_board, 0x40+i, 0);
            setFader(p_board, i, 0);
            //  setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
            setLcd(p_board,     i, 0,     0,    0,    0,  0x00,  0,  0,          "",  0x00,  0, 0, "");

        } else {
            s_vChannel *chan = &mixer.vChannel[vChannelIndex];

            x32debug("syncronize vChannel%d: %s\n", vChannelIndex, chan->name);

            if (mixerIsSelectDirty()){ setLed(p_board, 0x20+i, chan->selected); }
            if (mixerIsSoloDirty()){ setLed(p_board, 0x30+i, chan->solo); }
            if (mixerIsMuteDirty()){ setLed(p_board, 0x40+i, chan->mute); }
             
            if (mixerIsVolumeDirty() && mixerTouchcontrolCanSetFader(p_board, i)){
                u_int16_t faderVolume = dBfs2fader(chan->volume);
                setFader(p_board, i, faderVolume);
            }

            if (mixerIsLCDDirty()){
                char lcdText[20];
                sprintf(lcdText, "%2.1FdB %s", (double)chan->volume, (chan->inputSource.phantomPower ? "(48V)" : ""));
                //  setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
                setLcd(p_board,     i, chan->color,     0,    12,    chan->icon,  0x00,  0,  0,          lcdText,  0x00,  20, 48, chan->name);
            }
        }
    }

    if (p_board == X32_BOARD_R){
        // Clear Solo
        if (mixerIsSoloDirty()){ setLedByEnum(X32_BTN_CLEAR_SOLO, mixerIsSoloActivated()); }
    }    
}

void syncSurfaceBankIndicator(void) {
    if (mixerIsBankingDirty()){
        if (mixerIsModelX32Full()){
            setLedByEnum(X32_BTN_CH_1_16, 0);
            setLedByEnum(X32_BTN_CH_17_32, 0);
            if (mixer.activeBank == 0) { setLedByEnum(X32_BTN_CH_1_16, 1); }
            if (mixer.activeBank == 1) { setLedByEnum(X32_BTN_CH_17_32, 1); }
        }
        if (mixerIsModelX32CompacrOrProducer()) {
            setLedByEnum(X32_BTN_CH_1_8, 0);
            setLedByEnum(X32_BTN_CH_9_16, 0);
            setLedByEnum(X32_BTN_CH_17_24, 0);
            setLedByEnum(X32_BTN_CH_25_32, 0);
            if (mixer.activeBank == 0) { setLedByEnum(X32_BTN_CH_1_8, 1); }
            if (mixer.activeBank == 1) { setLedByEnum(X32_BTN_CH_9_16, 1); }
            if (mixer.activeBank == 2) { setLedByEnum(X32_BTN_CH_17_24, 1); }
            if (mixer.activeBank == 3) { setLedByEnum(X32_BTN_CH_25_32, 1); }
        }
    }
}

void x32log(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    vprintf(format, args);
    fflush(stdout); // immediately write to console!

    va_end(args);
}

void x32debug(const char *format, ...)
{
#if DEBUG == 1
    va_list args;
    va_start(args, format);

    vprintf(format, args);
    fflush(stdout); // immediately write to console!

    va_end(args);
#endif
}


// the main-function - of course
int main() {
    srand(time(NULL));
    x32log("OpenX32 UserInterface\n");
    x32log("v0.0.6, 17.08.2025\n");
    x32log("https://github.com/xn--nding-jua/OpenX32\n");

    x32log("Reading config...\n");
    char model[12];
    char serial[12];
    char date[16];
    readConfig("/etc/x32.conf", "MDL=", model, 12);
    readConfig("/etc/x32.conf", "SN=", serial, 12);
    readConfig("/etc/x32.conf", "DATE=", date, 16);
    if (strcmp(model, "X32Core") == 0) {
        initButtonDefinition(X32_MODEL_CORE);
        initMixer(X32_MODEL_CORE);
    }else if (strcmp(model, "X32Rack") == 0) {
        initButtonDefinition(X32_MODEL_RACK);
        initMixer(X32_MODEL_RACK);
    }else if (strcmp(model, "X32Producer") == 0) {
        initButtonDefinition(X32_MODEL_PRODUCER);
        initMixer(X32_MODEL_PRODUCER);
    }else if (strcmp(model, "X32C") == 0) {
        initButtonDefinition(X32_MODEL_COMPACT);
        initMixer(X32_MODEL_COMPACT);
    }else if (strcmp(model, "X32") == 0) {
        initButtonDefinition(X32_MODEL_FULL);
        initMixer(X32_MODEL_FULL);
    }else{
        #if DEBUG == 0
        x32log("ERROR: No model detected - assume X32 Fullsize!\n");
        initButtonDefinition(X32_MODEL_NONE);
        initMixer(X32_MODEL_NONE);
        #else
        // (for development without inserted sd card)
        //
        //x32debug("ERROR: No model detected - assume X32 Fullsize!\n");
        //initButtonDefinition(X32_MODEL_FULL);
        //initMixer(X32_MODEL_FULL);
        //
        x32debug("ERROR: No model detected - assume X32 Compact!\n");
        initButtonDefinition(X32_MODEL_COMPACT);
        initMixer(X32_MODEL_COMPACT);
        #endif
    }

    mixerDebugPrintvChannels();

    mixerDebugPrintBank(0);
    mixerDebugPrintBank(1);
    mixerDebugPrintBank(2);
    mixerDebugPrintBank(3);

    x32log(" Detected model: %s with Serial %s built on %s\n", model, serial, date);

    //x32log("Connecting to UART1 (Debug)...");
    //uartOpen("/dev/ttymxc0", 115200, &fdDebug); // this UART is not accessible from the outside

    x32log("Connecting to UART2 (Surface)...\n");
    uartOpen("/dev/ttymxc1", 115200, &fdSurface); // this UART is connected to the surface (Fader, LEDs, LCDs, Buttons) directly

    x32log("Connecting to UART3 (ADDA-Boards)...\n");
    uartOpen("/dev/ttymxc2", 38400, &fdAdda); // this UART is connected to the FPGA and routed to the 8-channel AD/DA-boards and the Expansion Card

    x32log("Connecting to UART4 (FPGA)...\n");
    uartOpen("/dev/ttymxc3", 115200, &fdFpga); // this UART is connected to the FPGA

    //x32log("Connecting to UART5 (MIDI)...\n");
    //uartOpen("/dev/ttymxc4", 115200, &fdMidi); // this UART is connected to the MIDI-connectors but is used by the Linux-console


    x32log("Initializing X32 Surface...\n");
    surfaceReset(); // resets all microcontrollers on the board (not necessary)
    surfaceInit(); // initialize whole surface with default values

    x32log("Initializing X32 Audio...\n");
    addaInit(48000);
    //mixingDefaultRoutingConfig(); // set default routing configuration

    // **************** TESTING ****************
    // set gain for all local xlr-input-channels to +15.5dB for testing with microphone
    for (uint8_t ch=1; ch<=32; ch++) {
        mixingSetGain('x', ch, 15.5);
    }

    // set routing to DSP-channels 1-8 which is just an 8-channel tone-generator at the moment
    for (uint8_t ch=1; ch<=8; ch++) {
        mixingSetRouting('x', ch, mixingGetInputSource('d', ch)); // set xlr-output 1-8
        mixingSetRouting('x', ch+8, mixingGetInputSource('d', ch)); // set xlr-output 9-16

        mixingSetRouting('a', ch, mixingGetInputSource('d', ch)); // set aux-channel 1-8

        mixingSetRouting('c', ch, mixingGetInputSource('d', ch)); // set card-output 1-8
        mixingSetRouting('c', ch+8, mixingGetInputSource('d', ch)); // set card-output 9-16
        mixingSetRouting('c', ch+16, mixingGetInputSource('d', ch)); // set card-output 17-24
        mixingSetRouting('c', ch+24, mixingGetInputSource('d', ch)); // set card-output 25-32

        mixingSetRouting('p', ch, mixingGetInputSource('d', ch)); // set P16-output 1-8
        mixingSetRouting('p', ch+8, mixingGetInputSource('d', ch)); // set P16-output 9-16
    }
    // transmit routing-configuration to FPGA
    mixingTxRoutingConfig();

    // turn on Phantom-Power on channels 1, 10, 19 and 27 for testing (LED identifies board)
    mixingSetPhantomPower('x', 1+0, true); // first channel on board 1 (1-8)
    mixingSetPhantomPower('x', 9+1, true); // second channel on board 3 (9-16)
    mixingSetPhantomPower('x', 17+2, true); // third channel on board 0 (17-24)
    mixingSetPhantomPower('x', 25+3, true); // fourth channel on board 2 (25-32)

    // set X-LIVE-Card to USB-Routing
/*
    addaSendCmd("*8I#"); // identify card
    usleep(10000);
    addaSendCmd("*8R#"); // identify card
    usleep(10000);
*/
    // **************** END OF TESTING ****************

    // unmute the local audio-boards
    addaSetMute(false);

/*
    // only necessary if LVGL is not used
    x32printf("Starting Timer...\n");
    initTimer();

    x32printf("Wait for incoming data on /dev/ttymxc1...\n");
    x32printf("Press Ctrl+C to terminate program.\n");
    while (1) {
      // read data from surface and calls surfaceCallback()
      surfaceProcessUartData(uartRx(&fdSurface, &uartBufferSurface[0], sizeof(uartBufferSurface)));

      // read data from ADDA-Boards (8ch input/output)
      addaProcessUartData(uartRx(&fdAdda, &uartBufferAdda[0], sizeof(uartBufferAdda)));

      // read data from FPGA
      fpgaProcessUartData(uartRx(&fdFpga, &uartBufferFpga[0], sizeof(uartBufferFpga)));

      // sleep for 1ms to lower CPU-load
      usleep(1000);
    }
*/

    x32log("Initializing GUI...\n");
    guiInit(); // initializes LVGL, FBDEV and starts endless loop

    return 0;
}
