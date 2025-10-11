/*
    ____                  __   ______ ___  
   / __ \                 \ \ / /___ \__ \ 
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
  | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |                               
         |_|                               
  
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
*/

#include "mixer.h"
#include "demo.h"
#include "surface.h"
#include "dsp.h"
#include "fpga.h"
#include "hal.h"
#include "gui.h"
#include "fx.h"

sMixer mixer;

// ####################################################################
// #
// #
// #        Init
// #
// #
// ####################################################################

void mixerInit(char model[]) {
    if (strcmp(model, "X32Core") == 0) {
        mixer.model = X32_MODEL_CORE;
    }else if (strcmp(model, "X32RACK") == 0) {
        mixer.model = X32_MODEL_RACK;
    }else if (strcmp(model, "X32Producer") == 0) {
        mixer.model = X32_MODEL_PRODUCER;
    }else if (strcmp(model, "X32C") == 0) {
        mixer.model =  X32_MODEL_COMPACT;
    }else if (strcmp(model, "X32") == 0) {
        mixer.model = X32_MODEL_FULL;
    }else{
        x32log("ERROR: No model detected!\n");
        mixer.model = X32_MODEL_NONE;
    }

    x32debug("############# InitMixer(model_index=%d) #############\n", mixer.model);
    mixerInitPages();
    initDefinitions();

    // disable all channels
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        mixerSetChannelDefaults(&mixer.channel[i], i, true);
    }

    // empty all banks
    for (uint8_t b = 0; b < 8; b++){
        for (uint8_t sCh = 0; sCh < 16; sCh++){
            mixer.modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[b].surfaceChannel2Channel[sCh] = CHANNEL_NOT_SET;
            mixer.modes[X32_SURFACE_MODE_BANKING_USER].inputBanks[b].surfaceChannel2Channel[sCh] = CHANNEL_NOT_SET;
        }
    }
    for (uint8_t b = 0; b < 4; b++){
        for (uint8_t sCh = 0; sCh < 8; sCh++){
            mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[b].surfaceChannel2Channel[sCh] = CHANNEL_NOT_SET;
            mixer.modes[X32_SURFACE_MODE_BANKING_USER].busBanks[b].surfaceChannel2Channel[sCh] = CHANNEL_NOT_SET;
        }
    }



    //##################################################################################
    //#
    //#   create default channels (what the user is refering to as "mixer channel")
    //#
    //#   0   -  31  Main DSP-Channels
    //#   32  -  39  AUX 1-6 / USB
    //#   40  -  47  FX Returns
    //#   48  -  63  Bus 1-16
    //#
    //#   64  -  71  Matrix 1-6 / Special / Main Center
    //#   72  -  79  DCA
    //#   80         Main Left/Right
    //#
    //##################################################################################
    for (int i = 0; i <= 31; i++) {
        sChannel *channel = &(mixer.channel[i]);
        mixerSetChannelDefaults(channel, i, false);
        mixer.dsp.dspChannel[i].solo = false;
        mixer.dsp.dspChannel[i].muted = false;
        mixer.dsp.dspChannel[i].volumeLR = VOLUME_MIN;
        mixer.dsp.dspChannel[i].volumeSub = VOLUME_MIN;
        mixer.dsp.dspChannel[i].balance = 0; // center
        sprintf(&(channel->name[0]), "Kanal %d", i+1); // german label :-)
    }
    mixerDebugPrintChannels();

    // AUX 1-6 / USB
    x32debug("Setting up AUX\n");
    for (uint8_t i = 0; i <= 7;i++){
        uint8_t index = 32 + i;
        sChannel *channel = &mixer.channel[index];
        mixerSetChannelDefaults(channel, index, false);
        mixer.dsp.dspChannel[index].solo = false;
        mixer.dsp.dspChannel[index].muted = false;
        mixer.dsp.dspChannel[index].volumeLR = VOLUME_MIN;
        mixer.dsp.dspChannel[index].volumeSub = VOLUME_MIN;
        mixer.dsp.dspChannel[index].balance = 0; // center

        if(i <=5){
            sprintf(&channel->name[0], "AUX%d", i+1);
            channel->color = SURFACE_COLOR_GREEN;
        } else {
            sprintf(&channel->name[0], "USB");
            channel->color = SURFACE_COLOR_YELLOW;
        }
    }

    // FX Returns 1-8
    x32debug("Setting up FX Returns\n");
    for (uint8_t i = 0; i <= 7; i++){
        uint8_t index = 40 + i;
        sChannel *channel = &mixer.channel[index];
        mixerSetChannelDefaults(channel, index, false);
        mixer.dsp.volumeFxReturn[i] = VOLUME_MIN;
        sprintf(&channel->name[0], "FX Ret%d", i+1);
        channel->color = SURFACE_COLOR_BLUE;
    }

    // Bus 1-16
    x32debug("Setting up Busses\n");
    for (uint8_t i=0; i<=15; i++){
        uint8_t index = 48 + i;
        sChannel *channel = &mixer.channel[index];
        mixerSetChannelDefaults(channel, index, false);
        mixer.dsp.mixbusChannel[i].solo = false;
        mixer.dsp.mixbusChannel[i].muted = false;
        mixer.dsp.mixbusChannel[i].volumeLR = VOLUME_MIN;
        mixer.dsp.mixbusChannel[i].volumeSub = VOLUME_MIN;
        mixer.dsp.mixbusChannel[i].balance = 0; // center
        sprintf(&channel->name[0], "BUS%d", i+1);
        channel->color = SURFACE_COLOR_CYAN;
    }

    // Matrix 1-6 / Special / SUB
    x32debug("Setting up Matrix\n");
    for (uint8_t i = 0; i <= 5; i++){
        uint8_t index = 64 + i;
        sChannel *channel = &(mixer.channel[index]);
        mixerSetChannelDefaults(channel, index, false);
        mixer.dsp.matrixChannel[i].muted = false;
        mixer.dsp.matrixChannel[i].solo = false;
        mixer.dsp.matrixChannel[i].volume = VOLUME_MIN;
        sprintf(&channel->name[0], "MATRIX%d", i+1);
        channel->color = SURFACE_COLOR_PINK;
    }
    // SPECIAL
    {
        sChannel *channel = &(mixer.channel[70]);
        mixerSetChannelDefaults(channel, 70, false);
        sprintf(&channel->name[0], "SPECIAL");
        mixer.dsp.volumeSpecial = VOLUME_MIN;
        channel->color = SURFACE_COLOR_RED;
    }
    // MainSub
    {
        sChannel *channel = &(mixer.channel[71]);
        mixerSetChannelDefaults(channel, 71, false);
        sprintf(&channel->name[0], "M/C");
        mixer.dsp.mainChannelSub.volume = VOLUME_MIN;
        mixer.dsp.mainChannelSub.balance = 0; // center
        mixer.dsp.mainChannelSub.muted = false;
        channel->color = SURFACE_COLOR_WHITE;
    }

    // DCA 1-8
    x32debug("Setting up DCA\n");
    for (uint8_t i = 0; i <= 7; i++){
        uint8_t index = 72 + i;
        sChannel *channel = &mixer.channel[index];
        mixerSetChannelDefaults(channel, index, false);
        mixer.dsp.volumeDca[i] = VOLUME_MIN;
        sprintf(&channel->name[0], "DCA%d", i+1);
        channel->color = SURFACE_COLOR_PINK;
    }


    // main-channel
    {
        sChannel *channel = &mixer.channel[80];
        mixerSetChannelDefaults(channel, 80, false);
        mixer.dsp.mainChannelLR.volume = VOLUME_MIN;
        mixer.dsp.mainChannelLR.balance = 0; // center
        mixer.dsp.mainChannelLR.muted = false;
        sprintf(&channel->name[0], "Main");
        channel->color = SURFACE_COLOR_WHITE;
        channel->channelType = 1; // main channel
    }

    // channel0 selected by default
    mixerSetSelect(0, true);

    //##################################################################################
    //#
    //#   assign channels to input fader bank - X32 Default Layout
    //#
    //##################################################################################

    if (mixerIsModelX32Full()){
        // bank is 16 channels wide
        for (uint8_t bank=0;bank<4;bank++){
            for (int i = 0; i <=15; i++) {
                mixer.modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2Channel[i] = i + (bank * 16);
                x32debug("Assing bank%d: surfaceChannel%d <-> channel%d\n", bank, i, i + (bank * 16));
            }
        }
    }

    if (mixerIsModelX32CompactOrProducer()){
        // bank is 8 channels wide
        for (uint8_t bank=0;bank<8;bank++){
            for (int i = 0; i <=7; i++) {
                mixer.modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2Channel[i] = i + (bank * 8);
                x32debug("Assing bank%d: surfaceChannel%d <-> channel%d\n", bank, i, i + (bank * 8));
            }
        }
    }

    //##################################################################################
    //#
    //#   assign channels to bus fader bank - X32 Default Layout
    //#
    //##################################################################################

    // DCA - starts at channel 72
    for (int i = 0; i <=7; i++) {
        mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[0].surfaceChannel2Channel[i] = i + 72;
    }

    // Bus 1-8 - starts at channel 48
    for (int i = 0; i <=7; i++) {
        mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[1].surfaceChannel2Channel[i] = i + 48;
    }

    // Bus 9-16 - starts at channel 56
    for (int i = 0; i <=7; i++) {
        mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[2].surfaceChannel2Channel[i] = i + 56;
    }

    // Matrix / SPECIAL / SUB - starts at channel 64
    for (int i = 0; i <=9; i++) {
        mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[3].surfaceChannel2Channel[i] = i + 64;
    }

    mixer.activeMode = X32_SURFACE_MODE_BANKING_X32;
    mixer.activeBank_inputFader = 0;
    mixer.activeBank_busFader = 0;
    mixer.activeEQ = 0;
    mixer.activeBusSend = 0;

    x32debug("END ############# InitMixer() ############# END \n\n");
}

void mixerSetChannelDefaults(sChannel* pChannel, uint8_t pChannelIndex, bool p_disabled){
    pChannel->index = pChannelIndex;
    if (p_disabled){
        pChannel->color = 0;
    } else {
        //pChannel->color = SURFACE_COLOR_WHITE;
        pChannel->color = SURFACE_COLOR_YELLOW;
    }
    pChannel->icon = 0;
    pChannel->selected = false;
    pChannel->channelType = 0; // normal channel
}

// ####################################################################
// #
// #
// #        Routing + Hardware channel assignment
// #
// #
// ###################################################################


// change the input of the selected channel
void mixerChangeDspInput(int8_t amount){
    // each channel is routed within the DSP and has access to either the 40 input-channels or one of the 16 busses or FX-returns

    sChannel *chan = mixerGetSelectedChannel();
    int16_t newValue = (int16_t)mixer.dsp.dspChannel[chan->index].inputSource + amount;

    /*
         0  OFF
     1..32  Input 1-32
    33..40  AUX 1-8
    41..56  Mixbus 1-16
    57..62  Matrix 1-6
    63..65  Main L/R/S
    66..68  Monitor L/R and Talkback
    69..84: FX Return 1-16
    85..92: DSP2 Aux-Channel 1-8
    */
    if (newValue > 68) {
        newValue = 0;
    }
    if (newValue < 0) {
        newValue = 68;
    }
    mixer.dsp.dspChannel[chan->index].inputSource = newValue; // OFF / In1-32 / AUX 1-8 / BUS1-16
    dspSetInputRouting(chan->index);
    mixerSetChannelChangeFlags(chan, X32_CHANNEL_CHANGED_INPUT);
    mixerSetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void mixerChangeHardwareOutput(int8_t amount) {
    // output-taps
    // 1-16 = XLR-outputs
    // 17-32 = UltraNet/P16-outputs
    // 33-64 = Card-outputs
    // 65-72 = AUX-outputs
    // 73-112 = DSP-inputs
    // 113-160 = AES50A-outputs
    // 161-208 = AES50B-outputs

    int16_t newValue = (int16_t)mixer.selectedOutputChannelIndex + amount;

    if (newValue > NUM_OUTPUT_CHANNEL) {
        newValue = 1;
    }
    if (newValue < 1) {
        newValue = NUM_OUTPUT_CHANNEL;
    }
    mixer.selectedOutputChannelIndex = newValue;
    // no sending to FPGA as we are not changing the hardware-routing here
    mixerSetChangeFlags(X32_MIXER_CHANGED_GUI);
}

void mixerChangeHardwareInput(int8_t amount) {
    // get current routingIndex
    int16_t newValue = fpgaRoutingGetOutputSourceByIndex(mixer.selectedOutputChannelIndex) + amount;

    if (newValue > NUM_INPUT_CHANNEL) {
        newValue = 0;
    }
    if (newValue < 0) {
        newValue = NUM_INPUT_CHANNEL;
    }
    fpgaRoutingSetOutputSourceByIndex(mixer.selectedOutputChannelIndex, newValue); // 1..112
    fpgaRoutingSendConfigToFpga();
    mixerSetChangeFlags(X32_MIXER_CHANGED_GUI);
}

// ####################################################################
// #
// #
// #        Changeflags
// #
// #
// ####################################################################

void mixerSetChangeFlags(uint16_t p_flag){
    mixer.changed |= p_flag;
}

bool mixerHasAnyChanged(void){
    return (mixer.changed != X32_MIXER_CHANGED_NONE);
}

bool mixerHasChanged(uint16_t p_flag){
    return ((mixer.changed & p_flag) == p_flag);
}

void mixerResetChangeFlags(void){
    mixer.changed = X32_MIXER_CHANGED_NONE;
    for (uint8_t i=0;i<MAX_CHANNELS;i++){
        mixerResetChannelChangeFlags(&(mixer.channel[i]));
    }
}

void mixerSetChannelChangeFlagsFromIndex(uint8_t p_chanIndex, uint16_t p_flag){
    mixer.channel[p_chanIndex].changed |= p_flag;
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
}

void mixerSetChannelChangeFlags(sChannel* p_chan, uint16_t p_flag){
    p_chan->changed |= p_flag;
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
}

bool mixerHasChannelChanged(sChannel* p_chan, uint16_t p_flag){
    return ((p_chan->changed & p_flag) == p_flag);
}

bool mixerHasChannelAnyChanged(sChannel* p_chan){
    return ((p_chan->changed) != X32_CHANNEL_CHANGED_NONE);
}

void mixerResetChannelChangeFlags(sChannel* p_chan){
    p_chan->changed = X32_CHANNEL_CHANGED_NONE;
}

// ####################################################################
// #
// #
// #        channel Assigment
// #
// #
// ####################################################################

uint8_t mixerGetSelectedChannelIndex(void){
    return  mixer.selectedChannel;
}

sChannel* mixerGetSelectedChannel(void){
    return  &(mixer.channel[mixerGetSelectedChannelIndex()]);
}

uint8_t mixerSurfaceChannel2Channel(uint8_t surfaceChannel){
    if ((mixer.activeMode == X32_SURFACE_MODE_BANKING_X32) || (mixer.activeMode == X32_SURFACE_MODE_BANKING_USER)){
        if (mixerIsModelX32Full()){
            if (surfaceChannel <= 15){
                // input-section
                return mixer.modes[mixer.activeMode].inputBanks[mixer.activeBank_inputFader].surfaceChannel2Channel[surfaceChannel];
            } else if (surfaceChannel == 24) {
                // main-channel
                return 80;
            } else {
                // bus-section and mainfader
                return mixer.modes[mixer.activeMode].busBanks[mixer.activeBank_busFader].surfaceChannel2Channel[surfaceChannel-16];
            }
        }
        if (mixerIsModelX32CompactOrProducer()){
            if (surfaceChannel <= 7){
                // input-section
                return mixer.modes[mixer.activeMode].inputBanks[mixer.activeBank_inputFader].surfaceChannel2Channel[surfaceChannel];
            } else if (surfaceChannel == 16) {
                // main-channel
                return 80;
            } else {
                // bus-section and mainfader
                return mixer.modes[mixer.activeMode].busBanks[mixer.activeBank_busFader].surfaceChannel2Channel[surfaceChannel-8];
            }
            return 0;
        }
        if (mixerIsModelX32Core()){
            // TODO
        }
    }

    return 0;
}

uint8_t mixerGetChannelIndexFromButtonOrFaderIndex(X32_BOARD p_board, uint16_t p_buttonIndex) {
    uint8_t offset = 0;
    if (p_board == X32_BOARD_M) { offset=8; }
    if (mixerIsModelX32Full()){
        if (p_board == X32_BOARD_R) { offset=16; }
    }
    if (mixerIsModelX32CompactOrProducer()){
        if (p_board == X32_BOARD_R) { offset=8; }
    }
    return mixerSurfaceChannel2Channel(p_buttonIndex + offset);
}

// ####################################################################
// #
// #
// #        Fader moved, button pressed, encoder turned and such
// #
// #
// ####################################################################

void mixerSurfaceFaderMoved(X32_BOARD p_board, uint8_t p_faderIndex, uint16_t p_faderValue){
    uint8_t channelIndex = CHANNEL_NOT_SET;

    if (mixer.activeMode == X32_SURFACE_MODE_BANKING_X32) {
        uint8_t offset = 0;
        if (p_board == X32_BOARD_M) { offset=8;}
        if (mixerIsModelX32Full()){
            if (p_board == X32_BOARD_R) { offset=16;}
        }
        if (mixerIsModelX32CompactOrProducer()){
            if (p_board == X32_BOARD_R) { offset=8;}
        }
        channelIndex = mixerSurfaceChannel2Channel(p_faderIndex + offset);
        mixerSetVolume(channelIndex, fader2dBfs(p_faderValue));

        mixer.touchcontrol.board = p_board;
        mixer.touchcontrol.faderIndex = p_faderIndex;
        mixer.touchcontrol.value = 5;

        x32debug("mixerSurfaceFaderMoved(p_board=%d, p_faderIndex=%d, p_faderValue=0x%02X): channel%d TouchControl=%d\n", p_board, p_faderIndex, p_faderValue, channelIndex, mixer.touchcontrol.value);
    }
}

void mixerSurfaceButtonPressed(X32_BOARD p_board, uint8_t p_index, uint16_t p_value) {
    X32_BTN button = button2enum(((uint16_t)p_board << 8) + (uint16_t)(p_value & 0x7F));
    bool buttonPressed = (p_value >> 7) == 1;

    if (mixer.activeMode == X32_SURFACE_MODE_BANKING_X32) {
        if (buttonPressed){
            switch (button) {
                case X32_BTN_LEFT:
                    mixerShowPrevPage();
                    break;
                case X32_BTN_RIGHT:
                    mixerShowNextPage();
                    break;
                case X32_BTN_HOME:
                    mixerShowPage(X32_PAGE_HOME);
                    break;
                case X32_BTN_VIEW_CONFIG:
                    mixerShowPage(X32_PAGE_CONFIG);
                    break;
                case X32_BTN_VIEW_EQ:
                    mixerShowPage(X32_PAGE_EQ);
                    break;
                case X32_BTN_METERS:
                    mixerShowPage(X32_PAGE_METERS);
                    break;
                case X32_BTN_ROUTING:
                    mixerShowPage(X32_PAGE_ROUTING);
                    break;
                case X32_BTN_SETUP:
                    mixerShowPage(X32_PAGE_SETUP);
                    break;
                case X32_BTN_LIBRARY:
                    mixerShowPage(X32_PAGE_LIBRARY);
                    break;
                case X32_BTN_EFFECTS:
                    mixerShowPage(X32_PAGE_EFFECTS);
                    break;
                case X32_BTN_MUTE_GRP:
                    mixerShowPage(X32_PAGE_MUTE_GRP);
                    break;
                case X32_BTN_UTILITY:
                    mixerShowPage(X32_PAGE_UTILITY);
                    break;
                case X32_BTN_EQ_LOW:
                case X32_BTN_EQ_LOW_MID:
                case X32_BTN_EQ_HIGH_MID:
                case X32_BTN_EQ_HIGH:
                    mixerBankingEQ(button);
                    break;
                case X32_BTN_BUS_SEND_1_4:
                case X32_BTN_BUS_SEND_5_8:
                case X32_BTN_BUS_SEND_9_12:
                case X32_BTN_BUS_SEND_13_16:
                    mixerBankingSends(button);
                    break;
                case X32_BTN_CH_1_16:
                case X32_BTN_CH_17_32:
                case X32_BTN_AUX_IN_EFFECTS:
                case X32_BTN_BUS_MASTER:
                case X32_BTN_EFFECTS_RETURNS:
                case X32_BTN_CH_1_8:
                case X32_BTN_CH_9_16:
                case X32_BTN_CH_17_24:
                case X32_BTN_CH_25_32:
                case X32_BTN_AUX_IN_1_6_USB_REC:
                case X32_BTN_BUS_1_8_MASTER:
                case X32_BTN_BUS_9_16_MASTER:
                case X32_BTN_GROUP_DCA_1_8:
                case X32_BTN_BUS_1_8:
                case X32_BTN_BUS_9_16:
                case X32_BTN_MATRIX_MAIN_C:
                    mixerBanking(button);
                    break;
                case X32_BTN_BOARD_L_CH_1_SELECT:
                case X32_BTN_BOARD_L_CH_2_SELECT:
                case X32_BTN_BOARD_L_CH_3_SELECT:
                case X32_BTN_BOARD_L_CH_4_SELECT:
                case X32_BTN_BOARD_L_CH_5_SELECT:
                case X32_BTN_BOARD_L_CH_6_SELECT:
                case X32_BTN_BOARD_L_CH_7_SELECT:
                case X32_BTN_BOARD_L_CH_8_SELECT:
                    mixerToggleSelect(mixerGetChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_L_CH_1_SELECT));
                    break;
                case X32_BTN_BOARD_M_CH_1_SELECT:
                case X32_BTN_BOARD_M_CH_2_SELECT:
                case X32_BTN_BOARD_M_CH_3_SELECT:
                case X32_BTN_BOARD_M_CH_4_SELECT:
                case X32_BTN_BOARD_M_CH_5_SELECT:
                case X32_BTN_BOARD_M_CH_6_SELECT:
                case X32_BTN_BOARD_M_CH_7_SELECT:
                case X32_BTN_BOARD_M_CH_8_SELECT:
                    mixerToggleSelect(mixerGetChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_M_CH_1_SELECT));
                    break;
                case X32_BTN_BOARD_R_CH_1_SELECT:
                case X32_BTN_BOARD_R_CH_2_SELECT:
                case X32_BTN_BOARD_R_CH_3_SELECT:
                case X32_BTN_BOARD_R_CH_4_SELECT:
                case X32_BTN_BOARD_R_CH_5_SELECT:
                case X32_BTN_BOARD_R_CH_6_SELECT:
                case X32_BTN_BOARD_R_CH_7_SELECT:
                case X32_BTN_BOARD_R_CH_8_SELECT:
                case X32_BTN_MAIN_SELECT:
                    mixerToggleSelect(mixerGetChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_R_CH_1_SELECT));
                    break;
                case X32_BTN_BOARD_L_CH_1_SOLO:
                case X32_BTN_BOARD_L_CH_2_SOLO:
                case X32_BTN_BOARD_L_CH_3_SOLO:
                case X32_BTN_BOARD_L_CH_4_SOLO:
                case X32_BTN_BOARD_L_CH_5_SOLO:
                case X32_BTN_BOARD_L_CH_6_SOLO:
                case X32_BTN_BOARD_L_CH_7_SOLO:
                case X32_BTN_BOARD_L_CH_8_SOLO:
                    mixerToggleSolo(mixerGetChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_L_CH_1_SOLO));
                    break;
                case X32_BTN_BOARD_M_CH_1_SOLO:
                case X32_BTN_BOARD_M_CH_2_SOLO:
                case X32_BTN_BOARD_M_CH_3_SOLO:
                case X32_BTN_BOARD_M_CH_4_SOLO:
                case X32_BTN_BOARD_M_CH_5_SOLO:
                case X32_BTN_BOARD_M_CH_6_SOLO:
                case X32_BTN_BOARD_M_CH_7_SOLO:
                case X32_BTN_BOARD_M_CH_8_SOLO:
                    mixerToggleSolo(mixerGetChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_M_CH_1_SOLO));
                    break;
                case X32_BTN_BOARD_R_CH_1_SOLO:
                case X32_BTN_BOARD_R_CH_2_SOLO:
                case X32_BTN_BOARD_R_CH_3_SOLO:
                case X32_BTN_BOARD_R_CH_4_SOLO:
                case X32_BTN_BOARD_R_CH_5_SOLO:
                case X32_BTN_BOARD_R_CH_6_SOLO:
                case X32_BTN_BOARD_R_CH_7_SOLO:
                case X32_BTN_BOARD_R_CH_8_SOLO:
                case X32_BTN_MAIN_SOLO:
                    mixerToggleSolo(mixerGetChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_R_CH_1_SOLO));
                    break;
                case X32_BTN_BOARD_L_CH_1_MUTE:
                case X32_BTN_BOARD_L_CH_2_MUTE:
                case X32_BTN_BOARD_L_CH_3_MUTE:
                case X32_BTN_BOARD_L_CH_4_MUTE:
                case X32_BTN_BOARD_L_CH_5_MUTE:
                case X32_BTN_BOARD_L_CH_6_MUTE:
                case X32_BTN_BOARD_L_CH_7_MUTE:
                case X32_BTN_BOARD_L_CH_8_MUTE:
                    mixerToggleMute(mixerGetChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_L_CH_1_MUTE));
                    break;
                case X32_BTN_BOARD_M_CH_1_MUTE:
                case X32_BTN_BOARD_M_CH_2_MUTE:
                case X32_BTN_BOARD_M_CH_3_MUTE:
                case X32_BTN_BOARD_M_CH_4_MUTE:
                case X32_BTN_BOARD_M_CH_5_MUTE:
                case X32_BTN_BOARD_M_CH_6_MUTE:
                case X32_BTN_BOARD_M_CH_7_MUTE:
                case X32_BTN_BOARD_M_CH_8_MUTE:
                    mixerToggleMute(mixerGetChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_M_CH_1_MUTE));
                    break;
                case X32_BTN_BOARD_R_CH_1_MUTE:
                case X32_BTN_BOARD_R_CH_2_MUTE:
                case X32_BTN_BOARD_R_CH_3_MUTE:
                case X32_BTN_BOARD_R_CH_4_MUTE:
                case X32_BTN_BOARD_R_CH_5_MUTE:
                case X32_BTN_BOARD_R_CH_6_MUTE:
                case X32_BTN_BOARD_R_CH_7_MUTE:
                case X32_BTN_BOARD_R_CH_8_MUTE:
                case X32_BTN_MAIN_MUTE:
                    mixerToggleMute(mixerGetChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_R_CH_1_MUTE));
                    break;
                case X32_BTN_CLEAR_SOLO:
                    mixerClearSolo();
                    break;
                case X32_BTN_PHANTOM_48V:
                    mixerTogglePhantom(mixerGetSelectedChannelIndex());
                    break;
                case X32_BTN_PHASE_INVERT:
                    mixerTogglePhaseInvert(mixerGetSelectedChannelIndex());
                    break;
                case X32_BTN_CHANNEL_SOLO: // only X32 Rack
                    mixerToggleSolo(mixerGetSelectedChannelIndex());
                    break;
                case X32_BTN_CHANNEL_MUTE: // only X32 Rack
                    mixerToggleMute(mixerGetSelectedChannelIndex());
                    break;
                default:
                    // TODO: Callback to x32ctrl if needed
                    x32debug("Unhandled button detected.\n");
                    break;
            }
        }
    }

    // Display Encoders
    // - are independent from Surface Modes!
    if (mixerIsModelX32FullOrCompactOrProducerOrRack()){
        if (mixer.activePage == X32_PAGE_CONFIG){
            if (buttonPressed){
                switch (button){
                    case X32_BTN_ENCODER1:
                        break;
                    case X32_BTN_ENCODER2:
                        mixerTogglePhantom(mixerGetSelectedChannelIndex());
                        break;
                    case X32_BTN_ENCODER3:
                        break;
                    case X32_BTN_ENCODER4:
                        break;
                    case X32_BTN_ENCODER5:
                        break;
                    case X32_BTN_ENCODER6:
                        break;
                }
            }
        }else if (mixer.activePage == X32_PAGE_EQ){
            if (buttonPressed){
                switch (button){
                    case X32_BTN_ENCODER1:
                        mixerChangeLowcut(mixerGetSelectedChannelIndex(), -10000); // will be limited to 20 Hz
                        break;
                    case X32_BTN_ENCODER2:
                        mixerSetPeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'F', 3000);
                        break;
                    case X32_BTN_ENCODER3:
                        mixerSetPeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'G', 0);
                        break;
                    case X32_BTN_ENCODER4:
                        mixerSetPeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'Q', 2);
                        break;
                    case X32_BTN_ENCODER5:
                        mixerSetPeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'T', 1);
                        break;
                    case X32_BTN_ENCODER6:
                        dspResetEq(mixerGetSelectedChannelIndex());
                        break;
                }
            }
        }
    }
}

void mixerSurfaceEncoderTurned(X32_BOARD p_board, uint8_t p_index, uint16_t p_value) {
    X32_ENC encoder = encoder2enum(((uint16_t)p_board << 8) + (uint16_t)(p_index));
    int8_t amount = 0;

    if (p_value > 0 && p_value < 128){
        amount = (uint8_t)p_value;
    } else if (p_value > 128 && p_value < 256) {
        amount = -(256 - (uint8_t)(p_value));
    }
    x32debug("Encoder: %d\n", amount);

    if (mixer.activeMode == X32_SURFACE_MODE_BANKING_X32) {
        switch (encoder){
            case X32_ENC_CHANNEL_SELECT:  // only X32 Rack and Core - Channel Select    TODO: Implement on Core
                mixerChangeSelect(amount);
                break;
            case X32_ENC_CHANNEL_LEVEL:
                mixerChangeVolume(mixerGetSelectedChannelIndex(), amount);
                break;
            case X32_ENC_GAIN:
                mixerChangeGain(mixerGetSelectedChannelIndex(), amount);
                break;
            case X32_ENC_GATE:
                mixerChangeGate(mixerGetSelectedChannelIndex(), amount);
                break;
            case X32_ENC_LOWCUT:
                mixerChangeLowcut(mixerGetSelectedChannelIndex(), amount);
                break;
            case X32_ENC_DYNAMICS:
                mixerChangeDynamics(mixerGetSelectedChannelIndex(), amount);
                break;
            case X32_ENC_EQ_Q:
                mixerChangePeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'Q', amount);
                break;
            case X32_ENC_EQ_FREQ:
                mixerChangePeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'F', amount);
                break;
            case X32_ENC_EQ_GAIN:
                mixerChangePeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'G', amount);
                break;
            case X32_BTN_EQ_MODE:
                mixerChangePeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'T', amount);
                break;
            case X32_ENC_PAN:
                mixerChangePan(mixerGetSelectedChannelIndex(), amount);
                break;
            case X32_ENC_BUS_SEND_1:
                mixerChangeBusSend(mixerGetSelectedChannelIndex(), 0, amount);
                break;
            case X32_ENC_BUS_SEND_2:
                mixerChangeBusSend(mixerGetSelectedChannelIndex(), 1, amount);
                break;
            case X32_ENC_BUS_SEND_3:
                mixerChangeBusSend(mixerGetSelectedChannelIndex(), 2, amount);
                break;
            case X32_ENC_BUS_SEND_4:
                mixerChangeBusSend(mixerGetSelectedChannelIndex(), 3, amount);
                break;
            default:
                // TODO: Callback to x32ctrl if needed
                x32debug("Unhandled encoder detected.\n");
                break;
        }
    }

    // Display Encoders
    // - are independent from Surface Modes!
    if (mixerIsModelX32FullOrCompactOrProducerOrRack()) {
        if (mixer.activePage == X32_PAGE_CONFIG){
            switch (encoder){
                case X32_ENC_ENCODER1:
                    mixerChangeDspInput(amount);
                    break;
                case X32_ENC_ENCODER2:
                    mixerChangeGain(mixerGetSelectedChannelIndex(), amount);
                    break;
                case X32_ENC_ENCODER3:
                    break;
                case X32_ENC_ENCODER4:
                    break;
                case X32_ENC_ENCODER5:
                    break;
                case X32_ENC_ENCODER6:
                    break;
            }
        }else if (mixer.activePage == X32_PAGE_EQ) {
            switch (encoder){
                case X32_ENC_ENCODER1:
                    mixerChangeLowcut(mixerGetSelectedChannelIndex(), amount);
                    break;
                case X32_ENC_ENCODER2:
                    mixerChangePeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'F', amount);
                    break;
                case X32_ENC_ENCODER3:
                    mixerChangePeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'G', amount);
                    break;
                case X32_ENC_ENCODER4:
                    mixerChangePeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'Q', amount);
                    break;
                case X32_ENC_ENCODER5:
                    mixerChangePeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'T', amount);
                    break;
                case X32_ENC_ENCODER6:
                    int8_t newValue = mixer.activeEQ + amount;
                    if (newValue < 0) {
                        mixer.activeEQ = 0;
                    }else if(newValue >= MAX_CHAN_EQS) {
                        mixer.activeEQ = MAX_CHAN_EQS - 1;
                    }else{
                        mixer.activeEQ = newValue;
                    }
                    mixerChangePeq(mixerGetSelectedChannelIndex(), mixer.activeEQ, 'T', 0);
                    break;
            }
        }else if (mixer.activePage == X32_PAGE_ROUTING_HWOUT) {
            switch (encoder){
                case X32_ENC_ENCODER1:
                    mixerChangeHardwareOutput(amount);
                    break;
                case X32_ENC_ENCODER2:
                    mixerChangeHardwareInput(amount);
                    break;
                case X32_ENC_ENCODER3:
                    break;
                case X32_ENC_ENCODER4:
                    break;
                case X32_ENC_ENCODER5:
                    break;
                case X32_ENC_ENCODER6:
                    break;
            }
        }
    }
}

// direction - positive or negative integer value
void mixerChangeSelect(uint8_t direction){
    int16_t newselectedChannel = mixer.selectedChannel += direction;
    if (newselectedChannel < 0) {
        newselectedChannel = 0;
    } else if (newselectedChannel >= MAX_CHANNELS){
        newselectedChannel = MAX_CHANNELS - 1;
    }

    mixerSetSelect(newselectedChannel, true);
}

void mixerSetSelect(uint8_t channelIndex, bool select){
    for(uint8_t i=0; i<MAX_CHANNELS;i++){
        mixer.channel[i].selected = false;
        mixer.channel[i].changed |= X32_CHANNEL_CHANGED_SELECT;
    }
    mixer.channel[channelIndex].selected = select;
    mixer.channel[channelIndex].changed |= X32_CHANNEL_CHANGED_SELECT;

    mixer.selectedChannel = channelIndex;
    mixerSetChangeFlags(X32_MIXER_CHANGED_SELECT);
}

void mixerToggleSelect(uint8_t channelIndex){
    mixerSetSelect(channelIndex, !mixer.channel[channelIndex].selected);
}

void mixerSetSolo(uint8_t pChannelIndex, bool solo){
    halSetSolo(pChannelIndex, solo);
    mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_SOLO);
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);

    //TODOs
    // - switch monitor source
}

void mixerToggleSolo(uint8_t channelIndex){
    mixerSetSolo(channelIndex, !halGetSolo(channelIndex));
}

void mixerClearSolo(void){
    if (mixerIsSoloActivated()){
        for (int i=0; i<MAX_CHANNELS; i++){
            mixerSetSolo(i, false);
        }
    }
}

void mixerSetPhantom(uint8_t pChannelIndex, bool p_phantom){
    halSetPhantomPower(pChannelIndex, p_phantom);
    mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_PHANTOM);
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
}

void mixerTogglePhantom(uint8_t pChannelIndex){
    mixerSetPhantom(pChannelIndex, !halGetPhantomPower(pChannelIndex));
}

void mixerSetPhaseInvert(uint8_t pChannelIndex, bool p_phaseInvert){
    halSetPhaseInversion(pChannelIndex, p_phaseInvert);
    mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_PHASE_INVERT);
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
}

void mixerTogglePhaseInvert(uint8_t channelIndex){
    mixerSetPhaseInvert(channelIndex, !halGetPhaseInvert(channelIndex));
}

void mixerSetMute(uint8_t pChannelIndex, bool mute){
    halSetMute(pChannelIndex, mute);
    mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_MUTE);
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
}

void mixerToggleMute(uint8_t channelIndex){
    mixerSetMute(channelIndex, !halGetMute(channelIndex));
}

void mixerBankingSends(X32_BTN p_button) {
    setLedByEnum(X32_BTN_BUS_SEND_1_4, 0);
    setLedByEnum(X32_BTN_BUS_SEND_5_8, 0);
    setLedByEnum(X32_BTN_BUS_SEND_9_12, 0);
    setLedByEnum(X32_BTN_BUS_SEND_13_16, 0);

    switch (p_button){
        case X32_BTN_BUS_SEND_1_4:
            mixer.activeBusSend = 0;
            setLedByEnum(X32_BTN_BUS_SEND_1_4, 1);
            break;
        case X32_BTN_BUS_SEND_5_8:
            mixer.activeBusSend = 1;
            setLedByEnum(X32_BTN_BUS_SEND_5_8, 1);
            break;
        case X32_BTN_BUS_SEND_9_12:
            mixer.activeBusSend = 2;
            setLedByEnum(X32_BTN_BUS_SEND_9_12, 1);
            break;
        case X32_BTN_BUS_SEND_13_16:
            mixer.activeBusSend = 3;
            setLedByEnum(X32_BTN_BUS_SEND_13_16, 1);
            break;
    }

    mixerSetChangeFlags(X32_CHANNEL_CHANGED_SENDS);
}

void mixerBankingEQ(X32_BTN p_button){
    setLedByEnum(X32_BTN_EQ_LOW, 0);
    setLedByEnum(X32_BTN_EQ_LOW_MID, 0);
    setLedByEnum(X32_BTN_EQ_HIGH_MID, 0);
    setLedByEnum(X32_BTN_EQ_HIGH, 0);

    switch (p_button){
        case X32_BTN_EQ_LOW:
            mixer.activeEQ = 0;
            setLedByEnum(X32_BTN_EQ_LOW, 1);
            break;
        case X32_BTN_EQ_LOW_MID:
            mixer.activeEQ = 1;
            setLedByEnum(X32_BTN_EQ_LOW_MID, 1);
            break;
        case X32_BTN_EQ_HIGH_MID:
            mixer.activeEQ = 2;
            setLedByEnum(X32_BTN_EQ_HIGH_MID, 1);
            break;
        case X32_BTN_EQ_HIGH:
            mixer.activeEQ = 3;
            setLedByEnum(X32_BTN_EQ_HIGH, 1);
            break;
        // TODO: implement LOW2 and HIGH2
    }

    mixerSetChannelChangeFlagsFromIndex(mixer.selectedChannel, X32_CHANNEL_CHANGED_EQ);
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
}

void mixerBanking(X32_BTN p_button){
    if (mixerIsModelX32Full()){
        switch (p_button){
            case X32_BTN_CH_1_16:
                mixer.activeBank_inputFader = 0;
                break;
            case X32_BTN_CH_17_32:
                mixer.activeBank_inputFader = 1;
                break;
            case X32_BTN_AUX_IN_EFFECTS:
                mixer.activeBank_inputFader = 2;
                break;
            case X32_BTN_BUS_MASTER:
                mixer.activeBank_inputFader = 3;
                break;
            case X32_BTN_GROUP_DCA_1_8:
                mixer.activeBank_busFader = 0;
                break;
            case X32_BTN_BUS_1_8:
                mixer.activeBank_busFader = 1;
                break;
            case X32_BTN_BUS_9_16:
                mixer.activeBank_busFader = 2;
                break;
            case X32_BTN_MATRIX_MAIN_C:
                mixer.activeBank_busFader = 3;
                break;
            default:
                break;
        }
    }
    if (mixerIsModelX32CompactOrProducer()){
        switch (p_button){
            case X32_BTN_CH_1_8:
                mixer.activeBank_inputFader = 0;
                break;
            case X32_BTN_CH_9_16:
                mixer.activeBank_inputFader = 1;
                break;
            case X32_BTN_CH_17_24:
                mixer.activeBank_inputFader = 2;
                break;
            case X32_BTN_CH_25_32:
                mixer.activeBank_inputFader = 3;
                break;
            case X32_BTN_AUX_IN_1_6_USB_REC:
                mixer.activeBank_inputFader = 4;
                break;
            case X32_BTN_EFFECTS_RETURNS:
                mixer.activeBank_inputFader = 5;
                break;
            case X32_BTN_BUS_1_8_MASTER:
                mixer.activeBank_inputFader = 6;
                break;
            case X32_BTN_BUS_9_16_MASTER:
                mixer.activeBank_inputFader = 7;
                break;
            case X32_BTN_GROUP_DCA_1_8:
                mixer.activeBank_busFader = 0;
                break;
            case X32_BTN_BUS_1_8:
                mixer.activeBank_busFader = 1;
                break;
            case X32_BTN_BUS_9_16:
                mixer.activeBank_busFader = 2;
                break;
            case X32_BTN_MATRIX_MAIN_C:
                mixer.activeBank_busFader = 3;
                break;
            default:
                break;
        }
    }

    mixerSetChangeFlags(X32_MIXER_CHANGED_BANKING);
}

// volume in dBfs
void mixerChangeGain(uint8_t pChannelIndex, int8_t p_amount){
    if (pChannelIndex == CHANNEL_NOT_SET) {
        return;
    }

    float newValue = halGetGain(pChannelIndex) + (2.5f * p_amount);
    if (newValue > 60) {
        newValue = 60;
    }else if (newValue < -12) {
        newValue = -12;
    }
    halSetGain(pChannelIndex, newValue);

    mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_GAIN);
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
}

// volume in dBfs
void mixerChangeVolume(uint8_t pChannelIndex, int8_t p_amount){
    if (pChannelIndex == CHANNEL_NOT_SET) {
        return;
    }

    float newValue = halGetVolume(pChannelIndex) + ((float)p_amount * abs((float)p_amount));
    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }
    halSetVolume(pChannelIndex, newValue);

    mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_VOLUME);
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
}

// volume in dBfs
void mixerSetVolume(uint8_t pChannelIndex, float p_volume){
    if (pChannelIndex == CHANNEL_NOT_SET) {
        return;
    }

    float newValue = p_volume;
    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }
    halSetVolume(pChannelIndex, newValue);

    mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_VOLUME);
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
}

void mixerChangePan(uint8_t pChannelIndex, int8_t p_amount){
    if (pChannelIndex == CHANNEL_NOT_SET) {
        return;
    }

    float newValue = halGetBalance(pChannelIndex) + ((float)p_amount * abs((float)p_amount));
    if (newValue > 100) {
        newValue = 100;
    }else if (newValue < -100) {
        newValue = -100;
    }
    halSetBalance(pChannelIndex, newValue);

    mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_VOLUME);
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
}

void mixerChangeBusSend(uint8_t pChannelIndex, uint8_t encoderIndex, int8_t p_amount){
    float newValue;
    if (pChannelIndex == CHANNEL_NOT_SET) {
        return;
    }
    
    halSetBusSend(pChannelIndex, (mixer.activeBusSend * 4) + encoderIndex, halGetBusSend(pChannelIndex, (mixer.activeBusSend * 4) + encoderIndex) + ((float)p_amount * abs((float)p_amount)));
    mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_SENDS);
    mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
}
void mixerChangeGate(uint8_t pChannelIndex, int8_t p_amount){
    float newValue;
    if (pChannelIndex == CHANNEL_NOT_SET) {
        return;
    }
    
    if ((pChannelIndex >= 0) && (pChannelIndex < 40)) {
        newValue = mixer.dsp.dspChannel[pChannelIndex].gate.threshold + ((float)p_amount * abs((float)p_amount)) * 0.4f;
        if (newValue > 0) {
            newValue = 0;
        }else if (newValue < -80) {
            newValue = -80;
        }
        mixer.dsp.dspChannel[pChannelIndex].gate.threshold = newValue;

        mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_GATE);
        mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
    }else{
        // no support for gate at mixbus
    }
}

void mixerChangeLowcut(uint8_t pChannelIndex, int8_t p_amount){
    float newValue;
    if (pChannelIndex == CHANNEL_NOT_SET) {
        return;
    }

    if ((pChannelIndex >= 0) && (pChannelIndex < 40)) {
        newValue = mixer.dsp.dspChannel[pChannelIndex].lowCutFrequency * (1 + (float)p_amount/20.0f);
        if (newValue > 400) {
            newValue = 400;
        }else if (newValue < 20) {
            newValue = 20;
        }
        mixer.dsp.dspChannel[pChannelIndex].lowCutFrequency = newValue;
        mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_EQ);
        mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
    }else{
        // no support for lowcut
    }
}

void mixerChangeDynamics(uint8_t pChannelIndex, int8_t p_amount){
    float newValue;
    if (pChannelIndex == CHANNEL_NOT_SET) {
        return;
    }
    if ((pChannelIndex >= 0) && (pChannelIndex < 40)) {
        newValue = mixer.dsp.dspChannel[pChannelIndex].compressor.threshold + ((float)p_amount * abs((float)p_amount)) * 0.4f;
        if (newValue > 0) {
            newValue = 0;
        }else if (newValue < -60) {
            newValue = -60;
        }
        mixer.dsp.dspChannel[pChannelIndex].compressor.threshold = newValue;
        mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_DYNAMIC);
        mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
    }else{
        // no support for lowcut
    }
}

void mixerSetPeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, float value){
    if (pChannelIndex == CHANNEL_NOT_SET) {
        return;
    }
    if (eqIndex >= MAX_CHAN_EQS) {
        return;
    }

    sPEQ* peq;

    if ((pChannelIndex >= 0) && (pChannelIndex < 40)) {
        peq = &mixer.dsp.dspChannel[pChannelIndex].peq[eqIndex];
    }else if ((pChannelIndex >= 48) && (pChannelIndex <= 63)) {
        peq = &mixer.dsp.mixbusChannel[pChannelIndex - 48].peq[eqIndex];
    }else if ((pChannelIndex >= 64) && (pChannelIndex <= 69)) {
        peq = &mixer.dsp.dspChannel[pChannelIndex - 64].peq[eqIndex];
    }

    if (peq != nullptr) {
        switch (option) {
            case 'Q':
                if (value < 0.3) {
                    peq->Q = 0.3;
                }else if (value > 10) {
                    peq->Q = 10;
                }else{
                    peq->Q = value;
                }
                break;
            case 'F':
                if (value < 20) {
                    peq->fc = 20;
                }else if (value > 20000) {
                    peq->fc = 20000;
                }else{
                    peq->fc = value;
                }
                break;
            case 'G':
                if (value < -15) {
                    peq->gain = -15;
                }else if (value > 15) {
                    peq->gain = 15;
                }else{
                    peq->gain = value;
                }
                break;
            case 'T':
                if (value > 7) {
                    peq->type = 7;
                }else if (value < 0){
                    peq->type = 0;
                }else{
                    peq->type = value;
                }
                break;
        }
        fxRecalcFilterCoefficients_PEQ(peq);
        mixerSetChannelChangeFlagsFromIndex(pChannelIndex, X32_CHANNEL_CHANGED_EQ);
        mixerSetChangeFlags(X32_MIXER_CHANGED_CHANNEL);
    }
}

void mixerChangePeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, int8_t p_amount){
    if (pChannelIndex == CHANNEL_NOT_SET) {
        return;
    }
    if (eqIndex >= MAX_CHAN_EQS) {
        return;
    }

    sPEQ* peq;

    if ((pChannelIndex >= 0) && (pChannelIndex < 40)) {
        peq = &mixer.dsp.dspChannel[pChannelIndex].peq[eqIndex];
    }else if ((pChannelIndex >= 48) && (pChannelIndex <= 63)) {
        peq = &mixer.dsp.mixbusChannel[pChannelIndex - 48].peq[eqIndex];
    }else if ((pChannelIndex >= 64) && (pChannelIndex <= 69)) {
        peq = &mixer.dsp.dspChannel[pChannelIndex - 64].peq[eqIndex];
    }

    if (peq != nullptr) {
        switch (option) {
            case 'Q':
                mixerSetPeq(pChannelIndex, eqIndex, option, peq->Q + ((float)p_amount * abs((float)p_amount))/10.0f);
                break;
            case 'F':
                mixerSetPeq(pChannelIndex, eqIndex, option, pow(10.0f, log10(peq->fc) + (float)p_amount * 0.01f));
                break;
            case 'G':
                mixerSetPeq(pChannelIndex, eqIndex, option, peq->gain + ((float)p_amount * abs((float)p_amount)) * 0.1f);
                break;
            case 'T':
                mixerSetPeq(pChannelIndex, eqIndex, option, peq->type + p_amount);
                break;
        }
    }
}

// ####################################################################
// #
// #
// #        Pages
// #
// #
// ###################################################################


void mixerInitPages(){
    // Init Pages
    mixer.pages[X32_PAGE_HOME].prevPage = X32_PAGE_NONE;
    mixer.pages[X32_PAGE_HOME].nextPage = X32_PAGE_CONFIG;

    mixer.pages[X32_PAGE_CONFIG].prevPage = X32_PAGE_HOME;
    mixer.pages[X32_PAGE_CONFIG].nextPage = X32_PAGE_EQ;

    mixer.pages[X32_PAGE_EQ].prevPage = X32_PAGE_CONFIG;
    mixer.pages[X32_PAGE_EQ].nextPage = X32_PAGE_NONE;

    mixer.pages[X32_PAGE_ROUTING].prevPage = X32_PAGE_NONE;
    mixer.pages[X32_PAGE_ROUTING].nextPage = X32_PAGE_ROUTING_HWOUT;

    mixer.pages[X32_PAGE_ROUTING_HWOUT].prevPage = X32_PAGE_ROUTING;
    mixer.pages[X32_PAGE_ROUTING_HWOUT].nextPage = X32_PAGE_NONE;
}

void mixerShowNextPage(void){
    if (mixer.pages[mixer.activePage].nextPage != X32_PAGE_NONE){
        mixerShowPage(mixer.pages[mixer.activePage].nextPage);
    }
}

void mixerShowPrevPage(void){
    if (mixer.pages[mixer.activePage].prevPage != X32_PAGE_NONE){
        mixerShowPage(mixer.pages[mixer.activePage].prevPage);
    }
}


void mixerShowPage(X32_PAGE p_page) {  // TODO: move to GUI Update section

    // first turn all page LEDs off
    setLedByEnum(X32_BTN_HOME, 0);
    setLedByEnum(X32_BTN_METERS, 0);
    setLedByEnum(X32_BTN_ROUTING, 0);
    setLedByEnum(X32_BTN_SETUP, 0);
    setLedByEnum(X32_BTN_LIBRARY, 0);
    setLedByEnum(X32_BTN_EFFECTS, 0);
    setLedByEnum(X32_BTN_MUTE_GRP, 0);
    setLedByEnum(X32_BTN_UTILITY, 0);
    // turn all view LEDs of
    setLedByEnum(X32_BTN_VIEW_CONFIG, 0);
    setLedByEnum(X32_BTN_VIEW_EQ, 0);

    mixer.activePage = p_page;

    switch (mixer.activePage)
    {
        case X32_PAGE_HOME:
            lv_tabview_set_active(objects.maintab, 1, LV_ANIM_OFF);
            lv_tabview_set_active(objects.hometab, 0, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_HOME, 1);
            break;
        case X32_PAGE_CONFIG:
            lv_tabview_set_active(objects.maintab, 1, LV_ANIM_OFF);
            lv_tabview_set_active(objects.hometab, 1, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_VIEW_CONFIG, 1);
            break;

        case X32_PAGE_EQ:
            lv_tabview_set_active(objects.maintab, 1, LV_ANIM_OFF);
            lv_tabview_set_active(objects.hometab, 4, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_VIEW_EQ, 1);
            break;

        case X32_PAGE_METERS:
            lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_METERS, 1);
            break;
        case X32_PAGE_ROUTING:
            lv_tabview_set_active(objects.maintab, 3, LV_ANIM_OFF);
            lv_tabview_set_active(objects.routingtab, 0, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_ROUTING, 1);
            break;
        case X32_PAGE_ROUTING_HWOUT:
            lv_tabview_set_active(objects.maintab, 3, LV_ANIM_OFF);
            lv_tabview_set_active(objects.routingtab, 5, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_ROUTING, 1);
            break;
        case X32_PAGE_SETUP:
            lv_tabview_set_active(objects.maintab, 4, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_SETUP, 1);
            break;
        case X32_PAGE_LIBRARY:
            lv_tabview_set_active(objects.maintab, 5, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_LIBRARY, 1);
            break;
        case X32_PAGE_EFFECTS:
            demoSurface();
            lv_tabview_set_active(objects.maintab, 6, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_EFFECTS, 1);
            break;
        case X32_PAGE_MUTE_GRP:
            lv_tabview_set_active(objects.maintab, 7, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_MUTE_GRP, 1);
            break;
        case X32_PAGE_UTILITY:
            lv_tabview_set_active(objects.maintab, 8, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_UTILITY, 1);
            break;
        default:
            mixer.activePage = X32_PAGE_NONE;
            x32debug("ERROR: Page not found! Show welcome page instead.\n");
    }

    mixerSetChangeFlags(X32_MIXER_CHANGED_PAGE);
}

// ####################################################################
// #
// #
// #        Current state
// #
// #
// ###################################################################

bool mixerIsSoloActivated(void){
    for (int i=0; i<40; i++) {
        if (mixer.dsp.dspChannel[i].solo){ return true; }
    } 
    for (int i=0; i<16; i++) {
        if (mixer.dsp.mixbusChannel[i].solo){ return true; }
    } 
    for (int i=0; i<8; i++) {
        if (mixer.dsp.matrixChannel[i].solo){ return true; }
    }
    return false;
}



bool mixerIsModelX32Full(void){
    return (mixer.model == X32_MODEL_FULL);
}
bool mixerIsModelX32FullOrCompactOrProducer(void){
    return ((mixer.model == X32_MODEL_FULL) || (mixer.model == X32_MODEL_COMPACT) || (mixer.model == X32_MODEL_PRODUCER));
}
bool mixerIsModelX32FullOrCompactOrProducerOrRack(void){
    return (mixerIsModelX32FullOrCompactOrProducer() || (mixer.model == X32_MODEL_RACK));
}
bool mixerIsModelX32CompactOrProducer(void){
    return ((mixer.model == X32_MODEL_COMPACT) || (mixer.model == X32_MODEL_PRODUCER));
}
bool mixerIsModelX32Core(void){
    return (mixer.model == X32_MODEL_CORE);
}
bool mixerIsModelX32Rack(void){
    return (mixer.model == X32_MODEL_RACK);
}

// ####################################################################
// #
// #
// #        Debug
// #
// #
// ###################################################################

void mixerDebugPrintBank(uint8_t bank)
{
    x32debug("################# BANK%d ###################\n", bank);
    for (uint8_t i=0;i<35;i++){
        x32debug("surfaceChannel%d -> channel%d\n", i, mixer.modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2Channel[i]);
    }
    x32debug("END ############# BANK%d ############### END\n", bank);
}

void mixerDebugPrintBusBank(uint8_t bank)
{
    x32debug("################# BUS BANK%d ###################\n", bank);
    for (uint8_t i=0;i<35;i++){
        x32debug("surfaceChannel%d -> channel%d\n", i, mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[bank].surfaceChannel2Channel[i]);
    }
    x32debug("END ############# BUS BANK%d ############### END\n", bank);
}

void mixerDebugPrintChannels(void){
    for (int i = 0; i < 40; i++)
    {
        sChannel* channel = &(mixer.channel[i]);
        x32debug("channel%-3d %-32s color=%d icon=%d selected=%d solo=%d mute=%d volume=%2.1f input=dspCh%d\n",
                 i, channel->name[0], channel->color, channel->icon, channel->selected, mixer.dsp.dspChannel[channel->index].solo, mixer.dsp.dspChannel[channel->index].muted, (double)mixer.dsp.dspChannel[channel->index].volumeLR, mixer.dsp.dspChannel[channel->index].inputSource);
    }
}
