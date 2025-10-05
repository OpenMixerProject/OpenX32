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


Mixer::Mixer(String p_model){

    if (p_model == "X32Core") {
        model = X32_MODEL_CORE;
    }else if (p_model == "X32RACK") {
        model = X32_MODEL_RACK;
    }else if (p_model == "X32Producer") {
        model = X32_MODEL_PRODUCER;
    }else if (p_model == "X32C") {
        model =  X32_MODEL_COMPACT;
    }else if (p_model == "X32") {
        model = X32_MODEL_FULL;
    }else{
        x32log("ERROR: No model detected!\n");
        model = X32_MODEL_NONE;
    }

    x32debug("############# InitMixer(model_index=%d) #############\n", model);
    InitPages();
    initDefinitions();

    // disable all channels
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        SetVChannelDefaults(vChannel[i], i, true);
    }

    // empty all banks
    // "Input Channels"
    for (uint8_t b = 0; b < 8; b++){
        for (uint8_t sCh = 0; sCh < 16; sCh++){
            modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
            modes[X32_SURFACE_MODE_BANKING_USER].inputBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
        }
    }
    // "Busses"
    for (uint8_t b = 0; b < 4; b++){
        for (uint8_t sCh = 0; sCh < 8; sCh++){
            modes[X32_SURFACE_MODE_BANKING_X32].busBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
            modes[X32_SURFACE_MODE_BANKING_USER].busBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
        }
    }

    //##################################################################################
    //#
    //#   create default vChannels (what the user is refering to as "mixer channel")
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
    for (int i=0; i<=31; i++) {
        vChannel[i].Init(i, false);
        
        // TODO set phantom, Phase and inputChannel

        // s_inputSource *input = &(vChannel[i].inputSource);
        // input->phantomPower = false;
        // input->phaseInvert = false;
        // input->dspChannel = i+1;

        // TODO: assign vChannel to "bus vChannel"
// #if DEBUG
//         // route input straight to output - just for testing
//         if (i<=7) {
//             s_outputDestination *output = &vChannel->outputDestination;
//             output->hardwareGroup = 'x';
//             output->hardwareChannel = i+1;       
//         }
// #endif
    }

// #if DEBUG
//     // set color on first 8 channels for testing
//     // set input to Off for channelindex 5
//     vChannel[0].color = SURFACE_COLOR_WHITE;
//     vChannel[0].volume = 0;
//     sprintf(vChannel[0].name, "White");
//     vChannel[1].color = SURFACE_COLOR_YELLOW;
//     vChannel[1].volume = 0;
//     sprintf(vChannel[1].name, "Yellow");
//     vChannel[2].color = SURFACE_COLOR_BLUE;
//     vChannel[2].volume = 0;
//     sprintf(vChannel[2].name, "Blue");
//     vChannel[3].color = SURFACE_COLOR_CYAN;
//     vChannel[3].volume = 0;
//     sprintf(vChannel[3].name, "Cyan has 30 characters - long.");
//     vChannel[4].color = SURFACE_COLOR_GREEN;
//     vChannel[4].volume = 0;
//     sprintf(vChannel[4].name, "Green");
//     vChannel[5].color = SURFACE_COLOR_PINK;
//     vChannel[5].volume = 0;
//     sprintf(vChannel[5].name, "Pink (Off)");
//     vChannel[6].color = SURFACE_COLOR_RED;
//     vChannel[6].volume = 0;
//     sprintf(vChannel[6].name, "Red");
//     vChannel[7].color = SURFACE_COLOR_BLACK;
//     vChannel[7].volume = 0;
//     sprintf(vChannel[7].name, "Black");

//     // set inverted color on next 8 channels for testing
//     vChannel[8].color = SURFACE_COLOR_WHITE | SURFACE_COLOR_INVERTED;
//     vChannel[8].volume = -20;
//     sprintf(vChannel[8].name, "Inverted");
//     vChannel[9].color = SURFACE_COLOR_YELLOW | SURFACE_COLOR_INVERTED;
//     vChannel[9].volume = -20;
//     sprintf(vChannel[9].name, "Inverted");
//     vChannel[10].color = SURFACE_COLOR_BLUE | SURFACE_COLOR_INVERTED;
//     vChannel[10].volume = -20;
//     sprintf(vChannel[10].name, "Inverted");
//     vChannel[11].color = SURFACE_COLOR_CYAN | SURFACE_COLOR_INVERTED;
//     vChannel[11].volume = -20;
//     sprintf(vChannel[11].name, "Inverted");
//     vChannel[12].color = SURFACE_COLOR_GREEN | SURFACE_COLOR_INVERTED;
//     vChannel[12].volume = -20;
//     sprintf(vChannel[12].name, "Inverted");
//     vChannel[13].color = SURFACE_COLOR_PINK | SURFACE_COLOR_INVERTED;
//     vChannel[13].volume = -20;
//     sprintf(vChannel[13].name, "Inverted");
//     vChannel[14].color = SURFACE_COLOR_RED | SURFACE_COLOR_INVERTED;
//     vChannel[14].volume = -20;
//     sprintf(vChannel[14].name, "Inverted");
//     vChannel[15].color = SURFACE_COLOR_BLACK | SURFACE_COLOR_INVERTED;
//     vChannel[15].volume = -20;
//     sprintf(vChannel[15].name, "Inverted");
// #endif

    DebugPrintvChannels();

    // // AUX 1-6 / USB
    // x32debug("Setting up AUX\n");
    // for (uint8_t i=0; i<=7;i++){
    //     uint8_t index = 32 + i;
    //     vChannel[index].Init(i, false);
        
    //     s_inputSource *input = &vChannel->inputSource;
    //     input->phantomPower = false;
    //     input->phaseInvert = false;
    //     input->dspChannel = index + 1;

    //     if(i <=5){
    //         sprintf(&vChannel->name[0], "AUX%d", i+1);
    //         vChannel->color = SURFACE_COLOR_GREEN;
    //     } else {
    //         sprintf(&vChannel->name[0], "USB");
    //         vChannel->color = SURFACE_COLOR_YELLOW;
    //     }
    // }

    // // FX Returns 1-8
    // x32debug("Setting up FX Returns\n");
    // for (uint8_t i=0; i<=7;i++){
    //     uint8_t index = 40 + i;
    //     s_vChannel *vChannel = &(vChannel[index]);
    //     SetVChannelDefaults(vChannel, index, false);
    //     //sprintf(&vChannel->name[0], "FX Ret%d", i+1);
    //     //vChannel->color = SURFACE_COLOR_BLUE;
    // }

    // // Bus 1-16
    // x32debug("Setting up Busses\n");
    // for (uint8_t i=0; i<=15;i++){
    //     uint8_t index = 48 + i;
    //     //s_vChannel *vChannel = &mixer.vChannel[index];
    //     //SetVChannelDefaults(vChannel, index, false);
    //     //sprintf(&vChannel->name[0], "BUS%d", i+1);
    //     //vChannel->color = SURFACE_COLOR_CYAN;
    // }

    // // Matrix 1-6 / Special / SUB
    // x32debug("Setting up Matrix / SPECIAL / SUB\n");
    // for (uint8_t i=0; i<=7;i++){
    //     uint8_t index = 64 + i;
    //     s_vChannel *vChannel = &mixer.vChannel[index];
    //     SetVChannelDefaults(vChannel, index, false);
    //     if(i <=5){
    //         sprintf(&vChannel->name[0], "MATRIX%d", i+1);
    //         vChannel->color = SURFACE_COLOR_PINK;
    //     } else if (i == 6){
    //         sprintf(&vChannel->name[0], "SPECIAL");
    //         vChannel->color = SURFACE_COLOR_RED;
    //     } else if (i == 7){
    //         sprintf(&vChannel->name[0], "M/C");
    //         vChannel->color = SURFACE_COLOR_WHITE;
    //     }
    // }

    // // DCA 1-8
    // x32debug("Setting up DCA\n");
    // for (uint8_t i=0; i<=7;i++){
    //     uint8_t index = 72 + i;
    //     s_vChannel *vChannel = &mixer.vChannel[index];
    //     SetVChannelDefaults(vChannel, index, false);
    //     sprintf(&vChannel->name[0], "DCA%d", i+1);
    //     vChannel->color = SURFACE_COLOR_PINK;
    // }


    // // main-channel
    // {
    //     s_vChannel *vChannel = &mixer.vChannel[80];
    //     SetVChannelDefaults(vChannel, 80, false);
    //     sprintf(&vChannel->name[0], "Main");
    //     vChannel->color = SURFACE_COLOR_WHITE;
    //     vChannel->vChannelType = 1; // main channel
    // }

    // vChannel0 selected by default
    SetSelect(0, true);

    //##################################################################################
    //#
    //#   assign channels to input fader bank - X32 Default Layout
    //#
    //##################################################################################

    if (IsModelX32Full()){
        // bank is 16 channels wide
        for (uint8_t bank=0;bank<4;bank++){
            for (int i = 0; i <=15; i++) {
                modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i] = i + (bank * 16);
                x32debug("Assing bank%d: surfaceChannel%d <-> vChannel%d\n", bank, i, i + (bank * 16));
            }
        }
    }

    if (IsModelX32CompactOrProducer()){
        // bank is 8 channels wide
        for (uint8_t bank=0;bank<8;bank++){
            for (int i = 0; i <=7; i++) {
                modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i] = i + (bank * 8);
                x32debug("Assing bank%d: surfaceChannel%d <-> vChannel%d\n", bank, i, i + (bank * 8));
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
        modes[X32_SURFACE_MODE_BANKING_X32].busBanks[0].surfaceChannel2VChannel[i] = i + 72;
    }

    // Bus 1-8 - starts at channel 48
    for (int i = 0; i <=7; i++) {
        modes[X32_SURFACE_MODE_BANKING_X32].busBanks[1].surfaceChannel2VChannel[i] = i + 48;
    }

    // Bus 9-16 - starts at channel 56
    for (int i = 0; i <=7; i++) {
        modes[X32_SURFACE_MODE_BANKING_X32].busBanks[2].surfaceChannel2VChannel[i] = i + 56;
    }

    // Matrix / SPECIAL / SUB - starts at channel 64
    for (int i = 0; i <=9; i++) {
        modes[X32_SURFACE_MODE_BANKING_X32].busBanks[3].surfaceChannel2VChannel[i] = i + 64;
    }

    activeBank_inputFader = 0;
    activeBank_busFader = 0;
    activeEQ = 0;
    activeBusSend = 0;

    x32debug("END ############# InitMixer() ############# END \n\n");
};


// ####################################################################
// #
// #
// #        Routing + Hardware channel assignment
// #
// #
// ###################################################################

// change the input of the selected vChannel
void Mixer::ChangeVChannel(int8_t amount){

//// change the input of the selected channel
//void mixerChangeDspInput(int8_t amount){
    // each channel is routed within the DSP and has access to either the 40 input-channels or one of the 16 busses or FX-returns
 
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

    VChannel chan = GetSelectedvChannel();

    x32debug("mixerChangeVChannel(): dspChannel=%d\n", chan.dspChannel.inputSource);

    int16_t newValue = (int16_t)chan.dspChannel.inputSource + amount;

  //  if (newValue > 68) {
    if (newValue > NUM_DSP_CHANNEL) {
        newValue = 0;
    }
    if (newValue < 0) {
        newValue = 68;
    }
    
   // dspSetInputRouting(chan->index);
    

    //mixer.dsp.dspChannel[chan->index].inputSource = newValue; // OFF / In1-32 / AUX 1-8 / BUS1-16
    chan.dspChannel.inputSource = newValue; // 32 in + 8 aux + 8 FX return + 16 bus

    // if (chan.dspChannel.inputSource == 0) {
    //     // OFF: not supported yet
    // }else if ((chan.dspChannel.inputSource >= 1) && (chan.dspChannel.inputSource <= 40)) {
    //     // inputs
    //     dsp.dspChannel[chan.index] = 25 + chan.dspChannel.inputSource - 1;
    // }else if ((chan.dspChannel.inputSource >= 41) && (chan.dspChannel.inputSource <= 48)) {
    //     // FX1-FX8
    // }else if ((chan.dspChannel.inputSource >= 49) && (chan.dspChannel.inputSource <= 64)) {
    //     // MixBus 1-16
    //     dsp.dspChannel[chan.index] = 3 + chan.dspChannel.inputSource - 1;
    // }
    SetVChannelChangeFlags(chan, X32_VCHANNEL_CHANGED_INPUT);
    SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

// void Mixer::ChangeHardwareOutput(int8_t amount) {
//     // output-taps
//     // 1-16 = XLR-outputs
//     // 17-32 = UltraNet/P16-outputs
//     // 33-64 = Card-outputs
//     // 65-72 = AUX-outputs
//     // 73-112 = DSP-inputs
//     // 113-160 = AES50A-outputs
//     // 161-208 = AES50B-outputs

//     int16_t newValue = (int16_t)selectedOutputChannelIndex + amount;

//     if (newValue > NUM_OUTPUT_CHANNEL) {
//         newValue = 1;
//     }
//     if (newValue < 1) {
//         newValue = NUM_OUTPUT_CHANNEL;
//     }
//     selectedOutputChannelIndex = newValue;
//     // no sending to FPGA as we are not changing the hardware-routing here
//     SetChangeFlags(X32_MIXER_CHANGED_GUI);
// }

// void Mixer::ChangeHardwareInput(int8_t amount) {
//     // get current routingIndex
//     int16_t newValue = routingGetOutputSourceByIndex(selectedOutputChannelIndex) + amount;

//     if (newValue > NUM_INPUT_CHANNEL) {
//         newValue = 0;
//     }
//     if (newValue < 0) {
//         newValue = NUM_INPUT_CHANNEL;
//     }
//     routingSetOutputSourceByIndex(selectedOutputChannelIndex, newValue);
//     routingSendConfigToFpga();
//     SetChangeFlags(X32_MIXER_CHANGED_GUI);
// }

// ####################################################################
// #
// #
// #        Changeflags
// #
// #
// ####################################################################


void Mixer::SetChangeFlags(uint16_t p_flag){
    changed |= p_flag;
}

bool Mixer::HasAnyChanged(void){
    return (changed != X32_MIXER_CHANGED_NONE);
}

bool Mixer::HasChanged(uint16_t p_flag){
    return ((changed & p_flag) == p_flag);
}

void Mixer::ResetChangeFlags(void){
    changed = X32_MIXER_CHANGED_NONE;
    for (uint8_t i=0;i<MAX_VCHANNELS;i++){
        ResetVChannelChangeFlags(vChannel[i]);
    }
}

void Mixer::SetVChannelChangeFlagsFromIndex(uint8_t p_chanIndex, uint16_t p_flag){
    vChannel[p_chanIndex].SetChanged(p_flag);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::SetVChannelChangeFlags(VChannel p_chan, uint16_t p_flag){
    p_chan.SetChanged(p_flag);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

bool Mixer::HasVChannelChanged(VChannel p_chan, uint16_t p_flag){
    return p_chan.HasChanged(p_flag);
}

bool Mixer::HasVChannelAnyChanged(VChannel p_chan){
    return ((p_chan.changed) != X32_VCHANNEL_CHANGED_NONE);
}

void Mixer::ResetVChannelChangeFlags(VChannel p_chan){
    p_chan.changed = X32_VCHANNEL_CHANGED_NONE;
}

// ####################################################################
// #
// #
// #        channel Assigment
// #
// #
// ####################################################################

uint8_t Mixer::GetSelectedvChannelIndex(void){
    return selectedVChannel;
}

VChannel Mixer::GetSelectedvChannel(void){
    return vChannel[GetSelectedvChannelIndex()];
}

VChannel Mixer::GetVChannel(uint8_t VChannelIndex){
    return vChannel[VChannelIndex];
}

uint8_t Mixer::SurfaceChannel2vChannel(uint8_t surfaceChannel){
    if ((activeBankMode == X32_SURFACE_MODE_BANKING_X32) || (activeBankMode == X32_SURFACE_MODE_BANKING_USER)){
        if (IsModelX32Full()){
            if (surfaceChannel <= 15){
                // input-section
                return modes[activeBankMode].inputBanks[activeBank_inputFader].surfaceChannel2VChannel[surfaceChannel];
            } else if (surfaceChannel == 24) {
                // main-channel
                return 80;
            } else {
                // bus-section and mainfader
                return modes[activeBankMode].busBanks[activeBank_busFader].surfaceChannel2VChannel[surfaceChannel-16];
            }
        }
        if (IsModelX32CompactOrProducer()){
            if (surfaceChannel <= 7){
                // input-section
                return modes[activeBankMode].inputBanks[activeBank_inputFader].surfaceChannel2VChannel[surfaceChannel];
            } else if (surfaceChannel == 16) {
                // main-channel
                return 80;
            } else {
                // bus-section and mainfader
                return modes[activeBankMode].busBanks[activeBank_busFader].surfaceChannel2VChannel[surfaceChannel-8];
            }
            return 0;
        }
        if (IsModelX32Core()){
            // TODO
        }
    }

    return 0;
}

uint8_t Mixer::GetvChannelIndexFromButtonOrFaderIndex(X32_BOARD p_board, uint16_t p_buttonIndex) {
    uint8_t offset = 0;
    if (p_board == X32_BOARD_M) { offset=8; }
    if (IsModelX32Full()){
        if (p_board == X32_BOARD_R) { offset=16; }
    }
    if (IsModelX32CompactOrProducer()){
        if (p_board == X32_BOARD_R) { offset=8; }
    }
    return SurfaceChannel2vChannel(p_buttonIndex + offset);
}

// ####################################################################
// #
// #
// #        Fader moved, button pressed, encoder turned and such
// #
// #
// ####################################################################

void Mixer::SurfaceFaderMoved(X32_BOARD p_board, uint8_t p_faderIndex, uint16_t p_faderValue){
    uint8_t vChannelIndex = VCHANNEL_NOT_SET;

    if (activeBankMode == X32_SURFACE_MODE_BANKING_X32) {
        uint8_t offset = 0;
        if (p_board == X32_BOARD_M) { offset=8;}
        if (IsModelX32Full()){
            if (p_board == X32_BOARD_R) { offset=16;}
        }
        if (IsModelX32CompactOrProducer()){
            if (p_board == X32_BOARD_R) { offset=8;}
        }
        vChannelIndex = SurfaceChannel2vChannel(p_faderIndex + offset);
        SetVolume(vChannelIndex, fader2dBfs(p_faderValue));

        touchcontrol.board = p_board;
        touchcontrol.faderIndex = p_faderIndex;
        touchcontrol.value = 5;

        x32debug("mixerSurfaceFaderMoved(p_board=%d, p_faderIndex=%d, p_faderValue=0x%02X): vChannel%d TouchControl=%d\n", p_board, p_faderIndex, p_faderValue, vChannelIndex, touchcontrol.value);
    }
}


void Mixer::SurfaceButtonPressed(X32_BOARD p_board, uint8_t p_index, uint16_t p_value) {
    X32_BTN button = button2enum(((uint16_t)p_board << 8) + (uint16_t)(p_value & 0x7F));
    bool buttonPressed = (p_value >> 7) == 1;

    if (activeBankMode == X32_SURFACE_MODE_BANKING_X32) {
        if (buttonPressed){
            switch (button) {
                case X32_BTN_LEFT:
                    ShowPrevPage();
                    break;
                case X32_BTN_RIGHT:
                    ShowNextPage();
                    break;
                case X32_BTN_HOME:
                    ShowPage(X32_PAGE_HOME);
                    break;
                case X32_BTN_VIEW_CONFIG:
                    ShowPage(X32_PAGE_CONFIG);
                    break;
                case X32_BTN_VIEW_EQ:
                    ShowPage(X32_PAGE_EQ);
                    break;
                case X32_BTN_METERS:
                    ShowPage(X32_PAGE_METERS);
                    break;
                case X32_BTN_ROUTING:
                    ShowPage(X32_PAGE_ROUTING);
                    break;
                case X32_BTN_SETUP:
                    ShowPage(X32_PAGE_SETUP);
                    break;
                case X32_BTN_LIBRARY:
                    ShowPage(X32_PAGE_LIBRARY);
                    break;
                case X32_BTN_EFFECTS:
                    ShowPage(X32_PAGE_EFFECTS);
                    break;
                case X32_BTN_MUTE_GRP:
                    ShowPage(X32_PAGE_MUTE_GRP);
                    break;
                case X32_BTN_UTILITY:
                    ShowPage(X32_PAGE_UTILITY);
                    break;
                case X32_BTN_EQ_LOW:
                case X32_BTN_EQ_LOW_MID:
                case X32_BTN_EQ_HIGH_MID:
                case X32_BTN_EQ_HIGH:
                    BankingEQ(button);
                    break;
                case X32_BTN_BUS_SEND_1_4:
                case X32_BTN_BUS_SEND_5_8:
                case X32_BTN_BUS_SEND_9_12:
                case X32_BTN_BUS_SEND_13_16:
                    BankingSends(button);
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
                    Banking(button);
                    break;
                case X32_BTN_BOARD_L_CH_1_SELECT:
                case X32_BTN_BOARD_L_CH_2_SELECT:
                case X32_BTN_BOARD_L_CH_3_SELECT:
                case X32_BTN_BOARD_L_CH_4_SELECT:
                case X32_BTN_BOARD_L_CH_5_SELECT:
                case X32_BTN_BOARD_L_CH_6_SELECT:
                case X32_BTN_BOARD_L_CH_7_SELECT:
                case X32_BTN_BOARD_L_CH_8_SELECT:
                    ToggleSelect(GetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_L_CH_1_SELECT));
                    break;
                case X32_BTN_BOARD_M_CH_1_SELECT:
                case X32_BTN_BOARD_M_CH_2_SELECT:
                case X32_BTN_BOARD_M_CH_3_SELECT:
                case X32_BTN_BOARD_M_CH_4_SELECT:
                case X32_BTN_BOARD_M_CH_5_SELECT:
                case X32_BTN_BOARD_M_CH_6_SELECT:
                case X32_BTN_BOARD_M_CH_7_SELECT:
                case X32_BTN_BOARD_M_CH_8_SELECT:
                    ToggleSelect(GetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_M_CH_1_SELECT));
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
                    ToggleSelect(GetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_R_CH_1_SELECT));
                    break;
                case X32_BTN_BOARD_L_CH_1_SOLO:
                case X32_BTN_BOARD_L_CH_2_SOLO:
                case X32_BTN_BOARD_L_CH_3_SOLO:
                case X32_BTN_BOARD_L_CH_4_SOLO:
                case X32_BTN_BOARD_L_CH_5_SOLO:
                case X32_BTN_BOARD_L_CH_6_SOLO:
                case X32_BTN_BOARD_L_CH_7_SOLO:
                case X32_BTN_BOARD_L_CH_8_SOLO:
                    ToggleSolo(GetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_L_CH_1_SOLO));
                    break;
                case X32_BTN_BOARD_M_CH_1_SOLO:
                case X32_BTN_BOARD_M_CH_2_SOLO:
                case X32_BTN_BOARD_M_CH_3_SOLO:
                case X32_BTN_BOARD_M_CH_4_SOLO:
                case X32_BTN_BOARD_M_CH_5_SOLO:
                case X32_BTN_BOARD_M_CH_6_SOLO:
                case X32_BTN_BOARD_M_CH_7_SOLO:
                case X32_BTN_BOARD_M_CH_8_SOLO:
                    ToggleSolo(GetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_M_CH_1_SOLO));
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
                    ToggleSolo(GetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_R_CH_1_SOLO));
                    break;
                case X32_BTN_BOARD_L_CH_1_MUTE:
                case X32_BTN_BOARD_L_CH_2_MUTE:
                case X32_BTN_BOARD_L_CH_3_MUTE:
                case X32_BTN_BOARD_L_CH_4_MUTE:
                case X32_BTN_BOARD_L_CH_5_MUTE:
                case X32_BTN_BOARD_L_CH_6_MUTE:
                case X32_BTN_BOARD_L_CH_7_MUTE:
                case X32_BTN_BOARD_L_CH_8_MUTE:
                    ToggleMute(GetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_L_CH_1_MUTE));
                    break;
                case X32_BTN_BOARD_M_CH_1_MUTE:
                case X32_BTN_BOARD_M_CH_2_MUTE:
                case X32_BTN_BOARD_M_CH_3_MUTE:
                case X32_BTN_BOARD_M_CH_4_MUTE:
                case X32_BTN_BOARD_M_CH_5_MUTE:
                case X32_BTN_BOARD_M_CH_6_MUTE:
                case X32_BTN_BOARD_M_CH_7_MUTE:
                case X32_BTN_BOARD_M_CH_8_MUTE:
                    ToggleMute(GetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_M_CH_1_MUTE));
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
                    ToggleMute(GetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_R_CH_1_MUTE));
                    break;
                case X32_BTN_CLEAR_SOLO:
                    ClearSolo();
                    break;
                case X32_BTN_PHANTOM_48V:
                    TogglePhantom(GetSelectedvChannelIndex());
                    break;
                case X32_BTN_PHASE_INVERT:
                    TogglePhaseInvert(GetSelectedvChannelIndex());
                    break;
                case X32_BTN_CHANNEL_SOLO: // only X32 Rack
                    ToggleSolo(GetSelectedvChannelIndex());
                    break;
                case X32_BTN_CHANNEL_MUTE: // only X32 Rack
                    ToggleMute(GetSelectedvChannelIndex());
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
    if (IsModelX32FullOrCompactOrProducerOrRack()){
        if (activePage == X32_PAGE_CONFIG){
            if (buttonPressed){
                switch (button){
                    case X32_BTN_ENCODER1:
                        break;
                    case X32_BTN_ENCODER2:
                        TogglePhantom(GetSelectedvChannelIndex());
                        break;
                    case X32_BTN_ENCODER3:
                        break;
                    case X32_BTN_ENCODER4:
                        break;
                    case X32_BTN_ENCODER5:
                        break;
                    case X32_BTN_ENCODER6:
                        break;
                    default:
                        break;
                }
            }
        }else if (activePage == X32_PAGE_EQ){
            if (buttonPressed){
                switch (button){
                    case X32_BTN_ENCODER1:
                        ChangeLowcut(GetSelectedvChannelIndex(), -10000); // will be limited to 20 Hz
                        break;
                    case X32_BTN_ENCODER2:
                        SetPeq(GetSelectedvChannelIndex(), activeEQ, 'F', 3000);
                        break;
                    case X32_BTN_ENCODER3:
                        SetPeq(GetSelectedvChannelIndex(), activeEQ, 'G', 0);
                        break;
                    case X32_BTN_ENCODER4:
                        SetPeq(GetSelectedvChannelIndex(), activeEQ, 'Q', 2);
                        break;
                    case X32_BTN_ENCODER5:
                        SetPeq(GetSelectedvChannelIndex(), activeEQ, 'T', 1);
                        break;
                    case X32_BTN_ENCODER6:
                        dspResetEq(GetSelectedvChannelIndex());
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void Mixer::SurfaceEncoderTurned(X32_BOARD p_board, uint8_t p_index, uint16_t p_value) {
    X32_ENC encoder = encoder2enum(((uint16_t)p_board << 8) + (uint16_t)(p_index));
    int8_t amount = 0;

    if (p_value > 0 && p_value < 128){
        amount = (uint8_t)p_value;
    } else if (p_value > 128 && p_value < 256) {
        amount = -(256 - (uint8_t)(p_value));
    }
    x32debug("Encoder: %d\n", amount);

    if (activeBankMode == X32_SURFACE_MODE_BANKING_X32) {
        switch (encoder){
            case X32_ENC_CHANNEL_SELECT:  // only X32 Rack and Core - Channel Select    TODO: Implement on Core
                ChangeSelect(amount);
                break;
            case X32_ENC_CHANNEL_LEVEL:
                ChangeVolume(GetSelectedvChannelIndex(), amount);
                break;
            case X32_BTN_EQ_MODE:
                ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'T', amount);
                break;
            case X32_ENC_GAIN:
                ChangeGain(GetSelectedvChannelIndex(), amount);
                break;
            case X32_ENC_GATE:
                ChangeGate(GetSelectedvChannelIndex(), amount);
                break;
            case X32_ENC_LOWCUT:
                ChangeLowcut(GetSelectedvChannelIndex(), amount);
                break;
            case X32_ENC_DYNAMICS:
                ChangeDynamics(GetSelectedvChannelIndex(), amount);
                break;
            case X32_ENC_EQ_Q:
                ChangePeq(GetSelectedvChannelIndex(), 'Q', amount);
                break;
            case X32_ENC_EQ_FREQ:
                ChangePeq(GetSelectedvChannelIndex(), 'F', amount);
                break;
            case X32_ENC_EQ_GAIN:
                ChangePeq(GetSelectedvChannelIndex(), 'G', amount);
                break;
            case X32_ENC_PAN:
                ChangePan(GetSelectedvChannelIndex(), amount);
                break;
            case X32_ENC_BUS_SEND_1:
                ChangeBusSend(GetSelectedvChannelIndex(), 0, amount);
                break;
            case X32_ENC_BUS_SEND_2:
                ChangeBusSend(GetSelectedvChannelIndex(), 1, amount);
                break;
            case X32_ENC_BUS_SEND_3:
                ChangeBusSend(GetSelectedvChannelIndex(), 2, amount);
                break;
            case X32_ENC_BUS_SEND_4:
                ChangeBusSend(GetSelectedvChannelIndex(), 3, amount);
                break;
            default:
                // TODO: Callback to x32ctrl if needed
                x32debug("Unhandled encoder detected.\n");
                break;
        }
    }

    // Display Encoders
    // - are independent from Surface Modes!
    if (IsModelX32FullOrCompactOrProducerOrRack()) {
        if (activePage == X32_PAGE_CONFIG){
            switch (encoder){
                case X32_ENC_ENCODER1:
                    // TODO ?
                    //ChangeDspInput(amount);
                    break;
                case X32_ENC_ENCODER2:
                    ChangeGain(GetSelectedvChannelIndex(), amount);
                    break;
                case X32_ENC_ENCODER3:                   
                    break;
                case X32_ENC_ENCODER4:
                    break;
                case X32_ENC_ENCODER5:
                    break;
                case X32_ENC_ENCODER6:
                    break;
                default:  
                    // just here to avoid compiler warnings                  
                    break;
            }
        }else if (activePage == X32_PAGE_EQ) {
            switch (encoder){
                case X32_ENC_ENCODER1:
                    ChangeLowcut(GetSelectedvChannelIndex(), amount);
                    break;
                case X32_ENC_ENCODER2:
                    ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'F', amount);
                    break;
                case X32_ENC_ENCODER3:
                    ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'G', amount);
                    break;
                case X32_ENC_ENCODER4:
                    ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'Q', amount);
                    break;
                case X32_ENC_ENCODER5:
                    ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'T', amount);
                    break;
                case X32_ENC_ENCODER6:
                    int8_t newValue = activeEQ + amount;
                    if (newValue < 0) {
                        activeEQ = 0;
                    }else if(newValue >= MAX_CHAN_EQS) {
                        activeEQ = MAX_CHAN_EQS - 1;
                    }else{
                        activeEQ = newValue;
                    }
                    ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'T', 0);
                    break;
                default:
                    // just here to avoid compiler warnings
                    break;
            }
        }else if (activePage == X32_PAGE_ROUTING) {
            switch (encoder){
                case X32_ENC_ENCODER1:
                    ChangeHardwareOutput(amount);
                    break;
                case X32_ENC_ENCODER2:
                    ChangeHardwareInput(amount);
                    break;
                case X32_ENC_ENCODER3:
                    break;
                case X32_ENC_ENCODER4:
                    break;
                case X32_ENC_ENCODER5:
                    break;
                case X32_ENC_ENCODER6:
                    break;
                default:  
                    // just here to avoid compiler warnings                  
                    break;
            }
        }
    }
}

// direction - positive or negative integer value
void Mixer::ChangeSelect(uint8_t direction){
    int16_t newSelectedVChannel = selectedVChannel += direction;
    if (newSelectedVChannel < 0) {
        newSelectedVChannel = 0;
    } else if (newSelectedVChannel >= MAX_VCHANNELS){
        newSelectedVChannel = MAX_VCHANNELS - 1;
    }

    SetSelect(newSelectedVChannel, true);
}

void Mixer::SetSelect(uint8_t vChannelIndex, bool select){
    for(uint8_t i=0; i<MAX_VCHANNELS;i++){
        vChannel[i].selected = false;
        vChannel[i].SetChanged(X32_VCHANNEL_CHANGED_SELECT);
    }
    vChannel[vChannelIndex].selected = select;
    vChannel[vChannelIndex].SetChanged(X32_VCHANNEL_CHANGED_SELECT);

    selectedVChannel = vChannelIndex;
    SetChangeFlags(X32_MIXER_CHANGED_SELECT);
}

void Mixer::ToggleSelect(uint8_t vChannelIndex){
    SetSelect(vChannelIndex, !vChannel[vChannelIndex].selected);
}

void Mixer::SetSolo(uint8_t p_vChannelIndex, bool solo){
    vChannel[p_vChannelIndex].solo = solo;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_SOLO);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);

    //TODOs
    // - switch monitor source
}

void Mixer::ToggleSolo(uint8_t vChannelIndex){
    SetSolo(vChannelIndex, !vChannel[vChannelIndex].solo);
}

void Mixer::ClearSolo(void){
    if (IsSoloActivated()){
        for (int i=0; i<MAX_VCHANNELS; i++){
            SetSolo(i, false);
        }
    }
}

void Mixer::SetPhantom(uint8_t p_vChannelIndex, bool p_phantom){
    // TODO vChannel[p_vChannelIndex].inputSource.phantomPower = p_phantom;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_PHANTOM);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::TogglePhantom(uint8_t p_vChannelIndex){
    // TODO SetPhantom(selectedVChannel, !vChannel[p_vChannelIndex].inputSource.phantomPower);
}

void Mixer::SetPhaseInvert(uint8_t p_vChannelIndex, bool p_phaseInvert){
    // TODO vChannel[p_vChannelIndex].inputSource.phaseInvert = p_phaseInvert;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_PHASE_INVERT);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::TogglePhaseInvert(uint8_t vChannelIndex){
    // TODO SetPhaseInvert(selectedVChannel, !vChannel[vChannelIndex].inputSource.phaseInvert);
}

void Mixer::SetMute(uint8_t p_vChannelIndex, bool mute){
    vChannel[p_vChannelIndex].mute = mute;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_MUTE);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::ToggleMute(uint8_t vChannelIndex){
    SetMute(vChannelIndex, !vChannel[vChannelIndex].mute);
}

void Mixer::BankingSends(X32_BTN p_button) {
    setLedByEnum(X32_BTN_BUS_SEND_1_4, 0);
    setLedByEnum(X32_BTN_BUS_SEND_5_8, 0);
    setLedByEnum(X32_BTN_BUS_SEND_9_12, 0);
    setLedByEnum(X32_BTN_BUS_SEND_13_16, 0);

    switch (p_button){
        case X32_BTN_BUS_SEND_1_4:
            activeBusSend = 0;
            setLedByEnum(X32_BTN_BUS_SEND_1_4, 1);
            break;
        case X32_BTN_BUS_SEND_5_8:
            activeBusSend = 1;
            setLedByEnum(X32_BTN_BUS_SEND_5_8, 1);
            break;
        case X32_BTN_BUS_SEND_9_12:
            activeBusSend = 2;
            setLedByEnum(X32_BTN_BUS_SEND_9_12, 1);
            break;
        case X32_BTN_BUS_SEND_13_16:
            activeBusSend = 3;
            setLedByEnum(X32_BTN_BUS_SEND_13_16, 1);
            break;
        default:
            break;
    }

    SetChangeFlags(X32_VCHANNEL_CHANGED_SENDS);
}

void Mixer::BankingEQ(X32_BTN p_button){
    setLedByEnum(X32_BTN_EQ_LOW, 0);
    setLedByEnum(X32_BTN_EQ_LOW_MID, 0);
    setLedByEnum(X32_BTN_EQ_HIGH_MID, 0);
    setLedByEnum(X32_BTN_EQ_HIGH, 0);

    switch (p_button){
        case X32_BTN_EQ_LOW:
            activeEQ = 0;
            setLedByEnum(X32_BTN_EQ_LOW, 1);
            break;
        case X32_BTN_EQ_LOW_MID:
            activeEQ = 1;
            setLedByEnum(X32_BTN_EQ_LOW_MID, 1);
            break;
        case X32_BTN_EQ_HIGH_MID:
            activeEQ = 2;
            setLedByEnum(X32_BTN_EQ_HIGH_MID, 1);
            break;
        case X32_BTN_EQ_HIGH:
            activeEQ = 3;
            setLedByEnum(X32_BTN_EQ_HIGH, 1);
            break;
        // TODO: implement LOW2 and HIGH2
        default:
            break;
    }

    SetChangeFlags(X32_VCHANNEL_CHANGED_EQ);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::Banking(X32_BTN p_button){
    if (IsModelX32Full()){
        switch (p_button){
            case X32_BTN_CH_1_16:
                activeBank_inputFader = 0;
                break;
            case X32_BTN_CH_17_32:
                activeBank_inputFader = 1;
                break;
            case X32_BTN_AUX_IN_EFFECTS:
                activeBank_inputFader = 2;
                break;
            case X32_BTN_BUS_MASTER:
                activeBank_inputFader = 3;
                break;
            case X32_BTN_GROUP_DCA_1_8:
                activeBank_busFader = 0;
                break;
            case X32_BTN_BUS_1_8:
                activeBank_busFader = 1;
                break;
            case X32_BTN_BUS_9_16:
                activeBank_busFader = 2;
                break;
            case X32_BTN_MATRIX_MAIN_C:
                activeBank_busFader = 3;
                break;
            default:
                break;
        }
    }
    if (IsModelX32CompactOrProducer()){
        switch (p_button){
            case X32_BTN_CH_1_8:
                activeBank_inputFader = 0;
                break;
            case X32_BTN_CH_9_16:
                activeBank_inputFader = 1;
                break;
            case X32_BTN_CH_17_24:
                activeBank_inputFader = 2;
                break;
            case X32_BTN_CH_25_32:
                activeBank_inputFader = 3;
                break;
            case X32_BTN_AUX_IN_1_6_USB_REC:
                activeBank_inputFader = 4;
                break;
            case X32_BTN_EFFECTS_RETURNS:
                activeBank_inputFader = 5;
                break;
            case X32_BTN_BUS_1_8_MASTER:
                activeBank_inputFader = 6;
                break;
            case X32_BTN_BUS_9_16_MASTER:
                activeBank_inputFader = 7;
                break;
            case X32_BTN_GROUP_DCA_1_8:
                activeBank_busFader = 0;
                break;
            case X32_BTN_BUS_1_8:
                activeBank_busFader = 1;
                break;
            case X32_BTN_BUS_9_16:
                activeBank_busFader = 2;
                break;
            case X32_BTN_MATRIX_MAIN_C:
                activeBank_busFader = 3;
                break;
            default:
                break;
        }
    }

    SetChangeFlags(X32_MIXER_CHANGED_BANKING);
}

// volume in dBfs
void Mixer::ChangeGain(uint8_t p_vChannelIndex, int8_t p_amount){
    float newValue;
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    // TODO newValue = vChannel[p_vChannelIndex].inputSource.gain + (2.5f * p_amount);
    if (newValue > 60) {
        newValue = 60;
    }else if (newValue < -12) {
        newValue = -12;
    }
    // TODO vChannel[p_vChannelIndex].inputSource.gain = newValue;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_GAIN);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

// volume in dBfs
void Mixer::ChangeVolume(uint8_t p_vChannelIndex, int8_t p_amount){
    float newValue;
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    newValue = vChannel[p_vChannelIndex].volumeLR + ((float)p_amount * abs((float)p_amount));
    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }
    vChannel[p_vChannelIndex].volumeLR = newValue;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_VOLUME);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

// volume in dBfs
void Mixer::SetVolume(uint8_t p_vChannelIndex, float p_volume){
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    float newValue = p_volume;
    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }
    vChannel[p_vChannelIndex].volumeLR = newValue;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_VOLUME);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::ChangePan(uint8_t p_vChannelIndex, int8_t p_amount){
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    float newValue = vChannel[p_vChannelIndex].balance + pow((float)p_amount, 3.0f);
    if (newValue > 100) {
        newValue = 100;
    }else if (newValue < -100) {
        newValue = -100;
    }
    vChannel[p_vChannelIndex].balance = newValue;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_VOLUME);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::ChangeBusSend(uint8_t p_vChannelIndex, uint8_t encoderIndex, int8_t p_amount){
    float newValue;
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    //halSetBusSend(pChannelIndex, (mixer.activeBusSend * 4) + encoderIndex, halGetBusSend(pChannelIndex, (mixer.activeBusSend * 4) + encoderIndex) + ((float)p_amount * abs((float)p_amount)));
    newValue = vChannel[p_vChannelIndex].sends[(activeBusSend * 4) + encoderIndex] + pow((float)p_amount, 3.0f);
    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }
    vChannel[p_vChannelIndex].sends[(activeBusSend * 4) + encoderIndex] = newValue;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_SENDS);
    SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}
void Mixer::ChangeGate(uint8_t p_vChannelIndex, int8_t p_amount){
    float newValue;
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    
    if ((p_vChannelIndex >= 0) && (p_vChannelIndex < 40)) {
        newValue = dsp.dspChannel[p_vChannelIndex].gate.threshold + ((float)p_amount * abs((float)p_amount)) * 0.4f;
        if (newValue > 0) {
            newValue = 0;
        }else if (newValue < -80) {
            newValue = -80;
        }
        
        //mixer.dsp.dspChannel[pChannelIndex].gate.threshold = newValue;
        // TODO dsp.dspChannel[vChannel[p_vChannelIndex].inputSource.dspChannel-1].gate.threshold = newValue;

        SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_GATE);
        SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
    }else{
        // no support for gate at mixbus
    }
}
void Mixer::ChangeLowcut(uint8_t p_vChannelIndex, int8_t p_amount){
    float newValue;
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    if ((p_vChannelIndex >= 0) && (p_vChannelIndex < 40)) {
        newValue = dsp.dspChannel[p_vChannelIndex].lowCutFrequency * (1 + (float)p_amount/20.0f);
        if (newValue > 400) {
            newValue = 400;
        }else if (newValue < 20) {
            newValue = 20;
        }
        //mixer.dsp.dspChannel[pChannelIndex].lowCutFrequency = newValue;
        // TODO dsp.dspChannel[vChannel[p_vChannelIndex].inputSource.dspChannel-1].lowCutFrequency = newValue;
        SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_EQ);
        SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
    }else{
        // no support for lowcut
    }
}

void Mixer::ChangeDynamics(uint8_t pChannelIndex, int8_t p_amount){
    float newValue;
    if (pChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    if ((pChannelIndex >= 0) && (pChannelIndex < 40)) {
        newValue = dsp.dspChannel[pChannelIndex].compressor.threshold + ((float)p_amount * abs((float)p_amount)) * 0.4f;
        if (newValue > 0) {
            newValue = 0;
        }else if (newValue < -60) {
            newValue = -60;
        }
        dsp.dspChannel[pChannelIndex].compressor.threshold = newValue;
        SetVChannelChangeFlagsFromIndex(pChannelIndex, X32_VCHANNEL_CHANGED_DYNAMIC);
        SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
    }else{
        // no support for lowcut
    }
}

void Mixer::SetPeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, float value){
    if (pChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    if (eqIndex >= MAX_CHAN_EQS) {
        return;
    }

    sPEQ* peq;

    if ((pChannelIndex >= 0) && (pChannelIndex < 40)) {
        peq = &dsp.dspChannel[pChannelIndex].peq[eqIndex];
    }else if ((pChannelIndex >= 48) && (pChannelIndex <= 63)) {
        peq = &dsp.mixbusChannel[pChannelIndex - 48].peq[eqIndex];
    }else if ((pChannelIndex >= 64) && (pChannelIndex <= 69)) {
        peq = &dsp.dspChannel[pChannelIndex - 64].peq[eqIndex];
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
        SetVChannelChangeFlagsFromIndex(pChannelIndex, X32_VCHANNEL_CHANGED_EQ);
        SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
    }
}

void Mixer::ChangePeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, int8_t p_amount){
    if (pChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    if (eqIndex >= MAX_CHAN_EQS) {
        return;
    }

    sPEQ* peq;

    if ((pChannelIndex >= 0) && (pChannelIndex < 40)) {
        peq = &dsp.dspChannel[pChannelIndex].peq[eqIndex];
    }else if ((pChannelIndex >= 48) && (pChannelIndex <= 63)) {
        peq = &dsp.mixbusChannel[pChannelIndex - 48].peq[eqIndex];
    }else if ((pChannelIndex >= 64) && (pChannelIndex <= 69)) {
        peq = &dsp.dspChannel[pChannelIndex - 64].peq[eqIndex];
    }

    if (peq != nullptr) {
        switch (option) {
            case 'Q':
                SetPeq(pChannelIndex, eqIndex, option, peq->Q + ((float)p_amount * abs((float)p_amount))/10.0f);
                break;
            case 'F':
                SetPeq(pChannelIndex, eqIndex, option, pow(10.0f, log10(peq->fc) + (float)p_amount * 0.01f));
                break;
            case 'G':
                SetPeq(pChannelIndex, eqIndex, option, peq->gain + ((float)p_amount * abs((float)p_amount)) * 0.1f);
                break;
            case 'T':
                SetPeq(pChannelIndex, eqIndex, option, peq->type + p_amount);
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


void Mixer::InitPages(void){
    // Init Pages
    pages[X32_PAGE_HOME].prevPage = X32_PAGE_NONE;
    pages[X32_PAGE_HOME].nextPage = X32_PAGE_CONFIG;

    pages[X32_PAGE_CONFIG].prevPage = X32_PAGE_HOME;
    pages[X32_PAGE_CONFIG].nextPage = X32_PAGE_EQ;

    pages[X32_PAGE_EQ].prevPage = X32_PAGE_CONFIG;
    pages[X32_PAGE_EQ].nextPage = X32_PAGE_NONE;
}

void Mixer::ShowNextPage(void){
    if (pages[activePage].nextPage != X32_PAGE_NONE){
        ShowPage(pages[activePage].nextPage);
    }
}

void Mixer::ShowPrevPage(void){
    if (pages[activePage].prevPage != X32_PAGE_NONE){
        ShowPage(pages[activePage].prevPage);
    }
}


void Mixer::ShowPage(X32_PAGE p_page) {  // TODO: move to GUI Update section

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

    activePage = p_page;

    switch (activePage)
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
            activePage = X32_PAGE_NONE;
            x32debug("ERROR: Page not found! Show welcome page instead.\n");
    }

    Mixer::SetChangeFlags(X32_MIXER_CHANGED_PAGE);
}

// ####################################################################
// #
// #
// #        Current state
// #
// #
// ###################################################################

bool Mixer::IsModelX32Full(void){
    return (model == X32_MODEL_FULL);
}
bool Mixer::IsModelX32FullOrCompactOrProducer(void){
    return ((model == X32_MODEL_FULL) || (model == X32_MODEL_COMPACT) || (model == X32_MODEL_PRODUCER));
}
bool Mixer::IsModelX32FullOrCompactOrProducerOrRack(void){
    return (IsModelX32FullOrCompactOrProducer() || (model == X32_MODEL_RACK));
}
bool Mixer::IsModelX32CompactOrProducer(void){
    return ((model == X32_MODEL_COMPACT) || (model == X32_MODEL_PRODUCER));
}
bool Mixer::IsModelX32Core(void){
    return (model == X32_MODEL_CORE);
}
bool Mixer::IsModelX32Rack(void){
    return (model == X32_MODEL_RACK);
}
bool Mixer::IsModelX32Compact(void){
    return (model == X32_MODEL_COMPACT);
}


bool Mixer::IsSoloActivated(void){
    //for (int i=0; i<40; i++) {
    for (int i=0; i<MAX_VCHANNELS; i++)
    {
        if (vChannel[i].solo){
            return true;
        }
    } 
    for (int i=0; i<16; i++) {
        if (dsp.mixbusChannel[i].solo){ return true; }
    } 
    for (int i=0; i<8; i++) {
        if (dsp.matrixChannel[i].solo){ return true; }
    }
    return false;
}


// ####################################################################
// #
// #
// #        Debug
// #
// #
// ###################################################################

void Mixer::DebugPrintBank(uint8_t bank)
{
    x32debug("################# BANK%d ###################\n", bank);
    for (uint8_t i=0;i<35;i++){
        x32debug("surfaceChannel%d -> vChannel%d\n", i, modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i]);
    }
    x32debug("END ############# BANK%d ############### END\n", bank);
}

void Mixer::DebugPrintBusBank(uint8_t bank)
{
    x32debug("################# BUS BANK%d ###################\n", bank);
    for (uint8_t i=0;i<35;i++){
        x32debug("surfaceChannel%d -> vChannel%d\n", i, modes[X32_SURFACE_MODE_BANKING_X32].busBanks[bank].surfaceChannel2VChannel[i]);
    }
    x32debug("END ############# BUS BANK%d ############### END\n", bank);
}

void Mixer::DebugPrintvChannels(void){
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
       // x32debug("vChannel%-3d %-32s color=%d icon=%d selected=%d solo=%d mute=%d volume=%2.1f input=dspCh%d\n",
       //         i, vChannel[i].name, vChannel[i].color, vChannel[i].icon, vChannel[i].selected, vChannel[i].solo, vChannel[i].mute, (double)vChannel[i].volumeLR, vChannel[i].inputSource.dspChannel);
    }
}
