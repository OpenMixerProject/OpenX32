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
  v0.1.5, 30.09.2025

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
#include "dsp.h"
#include "hal.h"

Mixer* mixer = NULL;

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
    surfaceUpdateMeter();

    // update meters on XRemote-clients
    xremoteUpdateMeter();

    // toggle the LED on DSP1 and DSP2 to show some activity
    spiSendDspParameter_uint32(0, 'a', 42, 0, 2);
    spiSendDspParameter_uint32(1, 'a', 42, 0, 2);

    // read meter- and dynamics-information from DSP
    if (mixerIsModelX32FullOrCompactOrProducer()) {
        spiSendDspParameter_uint32(0, '?', 'm', 0, 0); // non-blocking request of meter-data
        //spiSendDspParameter_uint32(0, '?', 'd', 0, 0); // non-blocking request of gate- and compression
    }

    // read the current DSP load
    if (!mixerIsModelX32Core()) {
        spiSendDspParameter_uint32(0, '?', 'c', 0, 0); // non-blocking request of DSP-Load-parameter
        spiSendDspParameter_uint32(1, '?', 'c', 0, 0); // non-blocking request of DSP-Load-parameter
        lv_label_set_text_fmt(objects.debugtext, "DSP1: %.2f %% [v%.2f] | DSP2: %.2f %% [v%.2f]", mixer.dsp.dspLoad[0], mixer.dsp.dspVersion[0], mixer.dsp.dspLoad[1], mixer.dsp.dspVersion[1]); // show the received value (could be a bit older than the request)
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

    // continuously read data from both DSPs if we expect data
    spiSendDspParameterArray(0, '?', 0, 0, mixer.dsp.dataToRead[0], NULL); // dummy-command just for reading without adding data to TxBuffer
    spiSendDspParameterArray(1, '?', 0, 0, mixer.dsp.dataToRead[1], NULL); // dummy-command just for reading without adding data to TxBuffer

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
void callbackSurface(X32_BOARD boardId, uint8_t classId, uint8_t index, uint16_t value) {
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
                    mixerDebugPrintChannels();
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

        // // channel Name
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

void callbackAdda(char* msg) {
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
            // caution: callbackAdda() can be called before the GUI is ready!!!
            //lv_label_set_text_fmt(objects.debugtext, "Received Message: %s\n", msg);
        }
    }
}

void callbackFpga(char* buf, uint8_t len) {
    // do something with the received data
    // at the moment the FPGA is sending "*X32 #" 5 times per second
    // later it is planned to receive information about audio-levels here
    //printf("Received: %s\n", buf);
    //lv_label_set_text_fmt(objects.debugtext, "Fpga Message: %s\n", buf);
}

void callbackDsp1(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values) {
    float* floatValues = (float*)values;
    uint32_t* intValues = (uint32_t*)values;

    switch (classId) {
        case 's': // status-feedback
            switch (channel) {
                case 'v': // DSP-Version
                    if (valueCount == 1) {
                        mixer.dsp.dspVersion[0] = floatValues[0];
                    }
                    break;
                case 'c': // DSP-Load in dspClockCycles
                    if (valueCount == 1) {
                        mixer.dsp.dspLoad[0] = (((float)intValues[0]/264.0f) / (16.0f/0.048f)) * 100.0f;
                    }
                    break;
            }
            break;
        case 'm': // meter information
            // copy meter-info to individual channels
            // leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
            if (valueCount == 43) {
                for (int i = 0; i < 40; i++) {
                    mixer.dsp.dspChannel[i].meterPu = abs(floatValues[i])/2147483648.0f; // convert 32-bit value to p.u.
                    uint32_t data = (uint32_t)abs(floatValues[i]); // convert received float-value to unsigned integer
                    // data contains a 32-bit sample-value
                    // lets check the threshold and set meterInfo
                    mixer.dsp.dspChannel[i].meterInfo = 0;
                    if (data >= vuThresholds[0])  { mixer.dsp.dspChannel[i].meterInfo |= 0b00100000; } // CLIP
                    if (data >= vuThresholds[5])  { mixer.dsp.dspChannel[i].meterInfo |= 0b00010000; } // -6dBfs
                    if (data >= vuThresholds[8])  { mixer.dsp.dspChannel[i].meterInfo |= 0b00001000; } // -12dBfs
                    if (data >= vuThresholds[10]) { mixer.dsp.dspChannel[i].meterInfo |= 0b00000100; } // -18dBfs
                    if (data >= vuThresholds[14]) { mixer.dsp.dspChannel[i].meterInfo |= 0b00000010; } // -30dBfs
                    if (data >= vuThresholds[24]) { mixer.dsp.dspChannel[i].meterInfo |= 0b00000001; } // -60dBfs

                    // the dynamic-information is received with the 'd' information, but we will store them here
                    if (mixer.dsp.dspChannel[i].gate.gain < 1.0f) { mixer.dsp.dspChannel[i].meterInfo |= 0b01000000; }
                    if (mixer.dsp.dspChannel[i].compressor.gain < 1.0f) { mixer.dsp.dspChannel[i].meterInfo |= 0b10000000; }
                }
                mixer.dsp.mainChannelLR.meterPu[0] = abs(floatValues[40])/2147483648.0f; // convert 32-bit value to p.u.
                mixer.dsp.mainChannelLR.meterPu[1] = abs(floatValues[41])/2147483648.0f; // convert 32-bit value to p.u.
                mixer.dsp.mainChannelSub.meterPu[0] = abs(floatValues[42])/2147483648.0f; // convert 32-bit value to p.u.

                // leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
                // leds = 32-bit bitwise (bit 0=-57dB ... 22=-2, 23=-1, 24=Clip)
                mixer.dsp.mainChannelLR.meterInfo[0] = 0;
                mixer.dsp.mainChannelLR.meterInfo[1] = 0;
                mixer.dsp.mainChannelSub.meterInfo[0] = 0;
                uint32_t data[3];
                data[0] = abs(floatValues[40]);
                data[1] = abs(floatValues[41]);
                data[2] = abs(floatValues[42]);
                for (int i = 0; i < 24; i++) {
                    if (data[0] >= vuThresholds[i]) { mixer.dsp.mainChannelLR.meterInfo[0]  |= (1U << (23 - i)); }
                    if (data[1] >= vuThresholds[i]) { mixer.dsp.mainChannelLR.meterInfo[1]  |= (1U << (23 - i)); }
                    if (data[2] >= vuThresholds[i]) { mixer.dsp.mainChannelSub.meterInfo[0] |= (1U << (23 - i)); }
                }
            }
            break;
        case 'd': // dynamics-information
            if (valueCount == 80) {
                // first copy the compression-information
                for (int i = 0; i < 40; i++) {
                    mixer.dsp.dspChannel[i].compressor.gain = floatValues[i];
                    mixer.dsp.dspChannel[i].gate.gain = floatValues[40 + i];
                }
            }
        default:
            break;
    }
}

void callbackDsp2(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values) {
    float* floatValues = (float*)values;
    uint32_t* intValues = (uint32_t*)values;

    switch (classId) {
        case 's': // status-feedback
            switch (channel) {
                case 'v': // DSP-Version
                    if (valueCount == 1) {
                        mixer.dsp.dspVersion[1] = floatValues[0];
                    }
                    break;
                case 'c': // DSP-Load in dspClockCycles
                    if (valueCount == 1) {
                        mixer.dsp.dspLoad[1] = (((float)intValues[0]/264.0f) / (16.0f/0.048f)) * 100.0f;
                    }
                    break;
            }
            break;
        default:
            break;
    }
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
            mixerHasChanged(X32_MIXER_CHANGED_PAGE)    ||
            mixerHasChanged(X32_MIXER_CHANGED_BANKING) ||
            mixerHasChanged(X32_MIXER_CHANGED_SELECT)  ||
            mixerHasChanged(X32_MIXER_CHANGED_CHANNEL) ||
            mixerHasChanged(X32_MIXER_CHANGED_GUI)
           ) {
            guiSync();
            surfaceSync();
        }
        if (mixerHasChanged(X32_MIXER_CHANGED_CHANNEL)) {
            halSyncChannelConfigFromMixer();
        }

        mixerResetChangeFlags();
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
    x32log("v0.1.5, 30.09.2025\n");
    x32log("https://github.com/OpenMixerProject/OpenX32\n");

    // first try to find what we are: Fullsize, Compact, Producer, Rack or Core
    x32debug("Reading config...\n");
    char model[12];
    char serial[15];
    char date[16];
    readConfig("/etc/x32.conf", "MDL=", model, 12);
    readConfig("/etc/x32.conf", "SN=", serial, 15);
    readConfig("/etc/x32.conf", "DATE=", date, 16);
    x32log("Detected model: %s with Serial %s built on %s\n", model, serial, date);

    mixer = &Mixer(model);

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
    // request versions from both DSP
    spiSendDspParameter_uint32(0, '?', 'v', 0, 0); // non-blocking request of DSP-Load-parameter
    spiSendDspParameter_uint32(1, '?', 'v', 0, 0); // non-blocking request of DSP-Load-parameter

    if (switchNoinit == -1) {
        x32log("Initializing X32 Surface...\n");
        //surfaceReset(); // resets all microcontrollers on the board (not necessary if no error occured)
        surfaceInit(); // initialize whole surface with default values

        x32log("Initializing X32 Audio...\n");
        addaInit(mixer.dsp.samplerate);
    }

    x32log("Setting up FPGA and DSPs...\n");
    fpgaRoutingInit();
    dspInit();
    dspSendAll();

    // unmute the local audio-boards
    addaSetMute(false);

    if (mixer->IsModelX32Core()){
        // only necessary if LVGL is not used
        x32log("Starting Timer...\n");
        init100msTimer(); // start 100ms-timer only for Non-GUI systems

        mixer->SetChangeFlags(X32_MIXER_CHANGED_ALL); // trigger first sync to gui/surface

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
