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


Mixer::Mixer(void){

    x32debug("############# InitMixer(model_index=%d) #############\n");
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

    if (config.IsModelX32Full()){
        // bank is 16 channels wide
        for (uint8_t bank=0;bank<4;bank++){
            for (int i = 0; i <=15; i++) {
                modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i] = i + (bank * 16);
                x32debug("Assing bank%d: surfaceChannel%d <-> vChannel%d\n", bank, i, i + (bank * 16));
            }
        }
    }

    if (config.IsModelX32CompactOrProducer()){
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
        if (config.IsModelX32Full()){
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
        if (config.IsModelX32CompactOrProducer()){
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
        if (config.IsModelX32Core()){
            // TODO
        }
    }

    return 0;
}

uint8_t Mixer::GetvChannelIndexFromButtonOrFaderIndex(X32_BOARD p_board, uint16_t p_buttonIndex) {
    uint8_t offset = 0;
    if (p_board == X32_BOARD_M) { offset=8; }
    if (config.IsModelX32Full()){
        if (p_board == X32_BOARD_R) { offset=16; }
    }
    if (config.IsModelX32CompactOrProducer()){
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
        if (config.IsModelX32Full()){
            if (p_board == X32_BOARD_R) { offset=16;}
        }
        if (config.IsModelX32CompactOrProducer()){
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
    if (config.IsModelX32FullOrCompactOrProducerOrRack()){
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
    int8_t newValue;

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
    if (config.IsModelX32FullOrCompactOrProducerOrRack()) {
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
                    newValue = activeEQ + amount;
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
                    break;
            }
        }else if (mixer.activePage == X32_PAGE_ROUTING_HWOUT) {
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
    if (config.IsModelX32Full()){
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
    if (config.IsModelX32CompactOrProducer()){
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

    mixer.pages[X32_PAGE_EQ].prevPage = X32_PAGE_CONFIG;
    mixer.pages[X32_PAGE_EQ].nextPage = X32_PAGE_NONE;

    mixer.pages[X32_PAGE_ROUTING].prevPage = X32_PAGE_NONE;
    mixer.pages[X32_PAGE_ROUTING].nextPage = X32_PAGE_ROUTING_HWOUT;

    mixer.pages[X32_PAGE_ROUTING_HWOUT].prevPage = X32_PAGE_ROUTING;
    mixer.pages[X32_PAGE_ROUTING_HWOUT].nextPage = X32_PAGE_NONE;
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
            //demoSurface();
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

// sync mixer state to GUI
void Mixer::guiSync(void) {
    if (config.IsModelX32Core()){
        return;
    }

    x32debug("Active Page: %d\n", activePage);

    VChannel pChannelSelected = GetSelectedvChannel();

    //####################################
    //#         General
    //####################################

    lv_color_t color;
    switch (pChannelSelected.color){
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

    lv_label_set_text_fmt(objects.current_channel_number, "vCh%d", pChannelSelected.index);
    lv_label_set_text_fmt(objects.current_channel_name, "%s", pChannelSelected.name);
    lv_obj_set_style_bg_color(objects.current_channel_color, color, 0);

    // //set Encoders to default state
    // const char*  encoderTextMap[] = {"Input", " ", " "," "," ","Output", NULL};
    // lv_btnmatrix_set_map(objects.display_encoders, encoderTextMap);

    //####################################
    //#         Page Home
    //####################################



    bool phantomPower = halGetPhantomPower(pChannelSelected.index);
    
    if (activePage == X32_PAGE_CONFIG){
    //####################################
    //#         Page Config
    //####################################
        char dspSourceName[5] = "";
        char inputSourceName[10] = "";
        // TODO dspGetSourceName(&dspSourceName[0], pChannelSelected.index);
        sprintf(&inputSourceName[0], "%02d: %s", (pChannelSelected.index + 1), dspSourceName);
        lv_label_set_text_fmt(objects.current_channel_source, inputSourceName);

        lv_label_set_text_fmt(objects.current_channel_gain, "%f", (double)halGetGain(pChannelSelected.index));
        lv_label_set_text_fmt(objects.current_channel_phantom, "%d", phantomPower);
        lv_label_set_text_fmt(objects.current_channel_invert, "%d", halGetPhaseInvert(pChannelSelected.index));
        lv_label_set_text_fmt(objects.current_channel_pan_bal, "%f", (double)halGetBalance(pChannelSelected.index));
        lv_label_set_text_fmt(objects.current_channel_volume, "%f", (double)halGetVolume(pChannelSelected.index));


        //char outputDestinationName[10] = "";
        //routingGetOutputName(&outputDestinationName[0], mixerGetSelectedChannel());
        //lv_label_set_text_fmt(objects.current_channel_destination, outputDestinationName);

        guiSetEncoderText("Source", "Gain", "-", "-", "-", "-");
    }else if (activePage == X32_PAGE_ROUTING) {
    //####################################
    //#         Page Routing
    //####################################
        // char outputDestinationName[10] = "";
        // char inputSourceName[10] = "";
        // uint8_t routingIndex = 0;

        // // read name of selected output-routing channel
        // fpgaRoutingGetOutputNameByIndex(&outputDestinationName[0], selectedOutputChannelIndex); // selectedOutputChannelIndex = 1..112
        // lv_label_set_text_fmt(objects.hardware_channel_output, outputDestinationName);

        // // find name of currently set input-source
		// routingIndex = fpgaRoutingGetOutputSourceByIndex(selectedOutputChannelIndex); // selectedOutputChannelIndex = 1..112
		// fpgaRoutingGetSourceNameByIndex(&inputSourceName[0], routingIndex); // routingIndex = 0..112
        // lv_label_set_text_fmt(objects.hardware_channel_source, inputSourceName);

        // guiSetEncoderText("Output", "Source", "-", "-", "-", "-");
    }else if (activePage == X32_PAGE_EQ) {
    //####################################
    //#         Page EQ
    //####################################
        // draw EQ-plot
        guiDrawEq(GetSelectedvChannelIndex());

        if (pChannelSelected.index < 40) {
            // support EQ-channel
            guiSetEncoderText("LC: " + freq2String(dsp.dspChannel[pChannelSelected.index].lowCutFrequency),
                "F: " + freq2String(dsp.dspChannel[pChannelSelected.index].peq[activeEQ].fc),
                "G: " + String(dsp.dspChannel[pChannelSelected.index].peq[activeEQ].gain, 1) + " dB",
                "Q: " + String(dsp.dspChannel[pChannelSelected.index].peq[activeEQ].Q, 1),
                "M: " + eqType2String(dsp.dspChannel[pChannelSelected.index].peq[activeEQ].type),
                "PEQ: " + String(activeEQ + 1)
            );
        }else{
            // unsupported at the moment
            guiSetEncoderText("-", "-", "-", "-", "-", "-");
        }
    }else if (activePage == X32_PAGE_METERS) {
    //####################################
    //#         Page Meters
    //####################################

        for(int i=0; i<=15; i++){
            VChannel chan = GetVChannel(i);

            if (phantomPower){
                lv_buttonmatrix_set_button_ctrl(objects.phantomindicators, i, LV_BUTTONMATRIX_CTRL_CHECKED);
            } else {
                lv_buttonmatrix_clear_button_ctrl(objects.phantomindicators, i, LV_BUTTONMATRIX_CTRL_CHECKED);
            }

            switch (i){
                case 0:
                    lv_slider_set_value(objects.slider01, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 1:
                    lv_slider_set_value(objects.slider02, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 2:
                    lv_slider_set_value(objects.slider03, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 3:
                    lv_slider_set_value(objects.slider04, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 4:
                    lv_slider_set_value(objects.slider05, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 5:
                    lv_slider_set_value(objects.slider06, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 6:
                    lv_slider_set_value(objects.slider07, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 7:
                    lv_slider_set_value(objects.slider08, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 8:
                    lv_slider_set_value(objects.slider09, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 9:
                    lv_slider_set_value(objects.slider10, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 10:
                    lv_slider_set_value(objects.slider11, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 11:
                    lv_slider_set_value(objects.slider12, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 12:
                    lv_slider_set_value(objects.slider13, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 13:
                    lv_slider_set_value(objects.slider14, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 14:
                    lv_slider_set_value(objects.slider15, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
                case 15:
                    lv_slider_set_value(objects.slider16, dBfs2fader(dsp.dspChannel[chan.index].volumeLR), LV_ANIM_OFF);
                    break;
            }
        }

        lv_label_set_text_fmt(objects.volumes, "%2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB", 
            (double)dsp.dspChannel[0].volumeLR,
            (double)dsp.dspChannel[1].volumeLR,
            (double)dsp.dspChannel[2].volumeLR,
            (double)dsp.dspChannel[3].volumeLR,
            (double)dsp.dspChannel[4].volumeLR,
            (double)dsp.dspChannel[5].volumeLR,
            (double)dsp.dspChannel[6].volumeLR,
            (double)dsp.dspChannel[7].volumeLR
        );
    }else if (activePage == X32_PAGE_SETUP) {
    //####################################
    //#         Page Setup
    //####################################

        // pChannelSelected.solo ?
        //     lv_imagebutton_set_state(objects.setup_solo, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED):
        //     lv_imagebutton_set_state(objects.setup_solo, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED);

        // pChannelSelected.mute ?
        //     lv_imagebutton_set_state(objects.setup_mute, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED):
        //     lv_imagebutton_set_state(objects.setup_mute, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED);
    }else{
    //####################################
    //#         All other pages
    //####################################
        guiSetEncoderText("-", "-", "-", "-", "-", "-");
    }
}

// ####################################################################
// #
// #
// #        Current state
// #
// #
// ###################################################################




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

void Mixer::syncAll(void) {
    if (HasAnyChanged()){
        if (
            HasChanged(X32_MIXER_CHANGED_PAGE)    ||
            HasChanged(X32_MIXER_CHANGED_BANKING) ||
            HasChanged(X32_MIXER_CHANGED_SELECT)  ||
            HasChanged(X32_MIXER_CHANGED_VCHANNEL) ||
            HasChanged(X32_MIXER_CHANGED_GUI)
           ) {
            guiSync();
            surfaceSync();
        }
        if (HasChanged(X32_MIXER_CHANGED_VCHANNEL)) {
            // TODO Maybe?: do not sync if just selection has changed
            halSyncChannelConfigFromMixer();
        }

        ResetChangeFlags();
    }
}


// sync mixer state to Surface
void Mixer::surfaceSync(void) {
    if ((activeBankMode == X32_SURFACE_MODE_BANKING_X32) || (activeBankMode == X32_SURFACE_MODE_BANKING_USER))
    {
        surfaceSyncBoardMain();

        if (config.IsModelX32FullOrCompactOrProducer()){   
            surfaceSyncBoard(X32_BOARD_L);
            if (config.IsModelX32Full()){
                surfaceSyncBoard(X32_BOARD_M);
            }
            surfaceSyncBoard(X32_BOARD_R);
            surfaceSyncBankIndicator();
        }
    }
}

void Mixer::surfaceSyncBoardMain() {
    bool needForSync = false;
    bool fullSync = false;
    VChannel chan = GetSelectedvChannel();

    if (HasChanged(X32_MIXER_CHANGED_SELECT)){ 
        // channel selection has changed - do a full sync
        needForSync=true;
        fullSync=true; 
    }

    if (HasChanged(X32_MIXER_CHANGED_VCHANNEL) && chan.HasAnyChanged()) {
        // the data in the currently selected channel has changed
        needForSync=true;
    }

    if (needForSync){
        if (config.IsModelX32FullOrCompactOrProducer()){
            // Channel section
            if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_PHANTOM)){
                setLedByEnum(X32_BTN_PHANTOM_48V, halGetPhantomPower(chan.index)); 
            }
            if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_PHASE_INVERT)){
                setLedByEnum(X32_BTN_PHASE_INVERT, halGetPhaseInvert(chan.index));
            }
            if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_GAIN)){
                // update gain-encoder
                setEncoderRing(enum2encoder(X32_ENC_GAIN) >> 8, enum2encoder(X32_ENC_GAIN) & 0xFF, 0, (halGetGain(chan.index) + 12.0f)/0.72f, 1);
            }
            if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_VOLUME)){
                // update pan-encoder
                setEncoderRing(enum2encoder(X32_ENC_PAN) >> 8, enum2encoder(X32_ENC_PAN) & 0xFF, 2, (halGetBalance(chan.index) + 100.0f)/2.0f, 1);
            }
            if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_SENDS)){
                // update pan-encoder
                setEncoderRing(enum2encoder(X32_ENC_BUS_SEND_1) >> 8, enum2encoder(X32_ENC_BUS_SEND_1) & 0xFF, 0, pow(10.0f, halGetBusSend(chan.index, activeBusSend * 4 + 0)/20.0f) * 100.0f, 1);
                setEncoderRing(enum2encoder(X32_ENC_BUS_SEND_2) >> 8, enum2encoder(X32_ENC_BUS_SEND_2) & 0xFF, 0, pow(10.0f, halGetBusSend(chan.index, activeBusSend * 4 + 1)/20.0f) * 100.0f, 1);
                setEncoderRing(enum2encoder(X32_ENC_BUS_SEND_3) >> 8, enum2encoder(X32_ENC_BUS_SEND_3) & 0xFF, 0, pow(10.0f, halGetBusSend(chan.index, activeBusSend * 4 + 2)/20.0f) * 100.0f, 1);
                setEncoderRing(enum2encoder(X32_ENC_BUS_SEND_4) >> 8, enum2encoder(X32_ENC_BUS_SEND_4) & 0xFF, 0, pow(10.0f, halGetBusSend(chan.index, activeBusSend * 4 + 3)/20.0f) * 100.0f, 1);
            }
            if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_GATE)){
                setEncoderRing(enum2encoder(X32_ENC_GATE) >> 8, enum2encoder(X32_ENC_GATE) & 0xFF, 4, 100.0f - ((dsp.dspChannel[chan.index].gate.threshold + 80.0f)/0.8f), 1);
            }
            if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_DYNAMIC)){
                setEncoderRing(enum2encoder(X32_ENC_DYNAMICS) >> 8, enum2encoder(X32_ENC_DYNAMICS) & 0xFF, 4, 100.0f - ((dsp.dspChannel[chan.index].compressor.threshold + 60.0f)/0.6f), 1);
            }
            if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_EQ)){
                // update EQ-encoder
                if (chan.index < 40) {
                    setEncoderRing(enum2encoder(X32_ENC_LOWCUT) >> 8, enum2encoder(X32_ENC_LOWCUT) & 0xFF, 1, (dsp.dspChannel[chan.index].lowCutFrequency - 20.0f)/3.8f, 1);
                    setEncoderRing(enum2encoder(X32_ENC_EQ_FREQ) >> 8, enum2encoder(X32_ENC_EQ_FREQ) & 0xFF, 1, (dsp.dspChannel[chan.index].peq[activeEQ].fc - 20.0f)/199.8f, 1);
                    setEncoderRing(enum2encoder(X32_ENC_EQ_GAIN) >> 8, enum2encoder(X32_ENC_EQ_GAIN) & 0xFF, 2, (dsp.dspChannel[chan.index].peq[activeEQ].gain + 15.0f)/0.3f, 1);
                    setEncoderRing(enum2encoder(X32_ENC_EQ_Q) >> 8, enum2encoder(X32_ENC_EQ_Q) & 0xFF, 3, ((10.0f - dsp.dspChannel[chan.index].peq[activeEQ].Q) + 0.3f)/0.097f, 1);
                    switch (dsp.dspChannel[chan.index].peq[activeEQ].type) {
                        case 0: // Allpass
                            break;
                        case 1: // Peak
                            break;
                        case 2: // LowShelf
                            break;
                        case 3: // HighShelf
                            break;
                        case 4: // Bandpass
                            break;
                        case 5: // Notch
                            break;
                        case 6: // LowPass
                            break;
                        case 7: // HighPass
                            break;
                    }
                }
            }
            
        }

        if (config.IsModelX32Rack()){
            // Channel section
            setLedChannelIndicator();
            chan = GetSelectedvChannel();

            if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_SOLO)){
                x32debug(" Solo");
                setLedByEnum(X32_BTN_CHANNEL_SOLO, halGetSolo(chan.index)); 
            }
            if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
                x32debug(" Mute");
                setLedByEnum(X32_BTN_CHANNEL_MUTE, halGetMute(chan.index)); 
            }
            if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_VOLUME)){
                // u_int16_t faderVolume = dBfs2fader(halGetVolume(chan.index));
                // uint8_t pct = (faderVolume/VOLUME_MIN
                // setEncoderRing(X32_BOARD_MAIN, 0, 0, , 1);
            }
        }
    }

    // Clear Solo
    if (HasChanged(X32_MIXER_CHANGED_VCHANNEL)){ setLedByEnum(X32_BTN_CLEAR_SOLO, IsSoloActivated()); }
}

void Mixer::surfaceSyncBoard(X32_BOARD p_board) {
    bool fullSync = false;

    if (HasChanged(X32_MIXER_CHANGED_BANKING)) {
        fullSync=true;
    }

    uint8_t offset = 0;
    if (config.IsModelX32Full()){
        if (p_board == X32_BOARD_M){ offset=8; }
        if (p_board == X32_BOARD_R){ offset=16; }
    } else if (config.IsModelX32CompactOrProducer()) {
        if (p_board == X32_BOARD_R){ offset=8; }
    }

    // update main-channel

    uint8_t maxChannel = 7;
    if ((p_board == X32_BOARD_R) && config.IsModelX32FullOrCompactOrProducer()) {
        maxChannel = 8; // include main-channel
    }
    for(int i=0; i<=maxChannel; i++){
        uint8_t channelIndex = SurfaceChannel2vChannel(i+offset);

        if (channelIndex == VCHANNEL_NOT_SET) {

            // TODO: do only, wenn channel got unassigned

            setLed(p_board, 0x20+i, 0);
            setLed(p_board, 0x30+i, 0);
            setLed(p_board, 0x40+i, 0);
            setFader(p_board, i, 0);
            //  setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
            setLcd(p_board,     i, 0,     0,    0,    0,  0x00,  0,  0,          "",  0x00,  0, 0, "");

        } else {
            VChannel chan = GetVChannel(channelIndex);

            if (fullSync || chan.HasAnyChanged()){
                x32debug("syncronize channel%d: %s -", channelIndex, chan.name);

                if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_SELECT)){ 
                    x32debug(" Select");
                    setLed(p_board, 0x20+i, chan.selected);
                }
                if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_SOLO)){
                    x32debug(" Solo");
                    setLed(p_board, 0x30+i, halGetSolo(chan.index)); 
                }
                if (fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
                    x32debug(" Mute");
                    setLed(p_board, 0x40+i, halGetMute(chan.index)); 
                }

                if ((fullSync || chan.HasChanged(X32_VCHANNEL_CHANGED_VOLUME)) && touchcontrolCanSetFader(p_board, i)){
                    x32debug(" Fader");
                    u_int16_t faderVolume = dBfs2fader(halGetVolume(chan.index));
                    setFader(p_board, i, faderVolume);
                }

                if (
                    fullSync                                                         ||
                    chan.HasChanged(X32_VCHANNEL_CHANGED_PHASE_INVERT )  ||
                    chan.HasChanged(X32_VCHANNEL_CHANGED_VOLUME )        ||
                    chan.HasChanged(X32_VCHANNEL_CHANGED_PHANTOM)        ||
                    chan.HasChanged(X32_VCHANNEL_CHANGED_COLOR)          ||
                    chan.HasChanged(X32_VCHANNEL_CHANGED_NAME)
                   )
                {
                    x32debug(" LCD");
                    setLcdFromVChannel(p_board, i, chan);

                    // char lcdText[20];
                    // sprintf(lcdText, "%2.1FdB %s", (double)halGetVolume(chan.index), (halGetPhantomPower(chan.index) ? "(48V)" : ""));
                    // //  setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
                    // setLcd(p_board,     i, chan->color,     0,    12,    chan->icon,  0x00,  1,  1,          lcdText,  0x00,  1, 47, chan->name);
                }

                x32debug("\n");
            }
        }
    }

    if (p_board == X32_BOARD_R){
        // Clear Solo
        if (HasChanged(X32_MIXER_CHANGED_VCHANNEL)) { setLedByEnum(X32_BTN_CLEAR_SOLO, IsSoloActivated()); }
    }
}

void Mixer::surfaceUpdateMeter(void) {
    if (config.IsModelX32Core()) {
        // no led-meters at all
        return;
    }

    // update preamp, dynamics, meterL, meterR, meterSolo
    // leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
    // leds = 32-bit bitwise (bit 0=-57dB ... 22=-2, 23=-1, 24=Clip)
    //setMeterLedMain(0b00000111, 0b11100001, 0x0FFFFF, 0x0FFFFF, 0x000FFF);
    uint8_t chanIdx = GetSelectedvChannelIndex();
    setMeterLedMain(surfaceCalcPreampMeter(chanIdx), surfaceCalcDynamicMeter(chanIdx), dsp.mainChannelLR.meterInfo[0], dsp.mainChannelLR.meterInfo[1], dsp.mainChannelSub.meterInfo[0]);


    if (config.IsModelX32Rack()) {
        // no led-meters per channel
        return;
    }

    // update channel-meters
    if (config.IsModelX32Full()) {
        // update meters on board L and M
        switch (activeBank_inputFader) {
            case 0: // Input 1-16
                for (uint8_t i = 0; i < 8; i++) {
                    setMeterLed(X32_BOARD_L, i, dsp.dspChannel[i].meterInfo);
                    setMeterLed(X32_BOARD_M, i, dsp.dspChannel[i + 8].meterInfo);
                }
                break;
            case 1: // Input 17-32
                for (uint8_t i = 0; i < 8; i++) {
                    setMeterLed(X32_BOARD_L, i, dsp.dspChannel[16 + i].meterInfo);
                    setMeterLed(X32_BOARD_M, i, dsp.dspChannel[16 + i + 8].meterInfo);
                }
                break;
            case 2: // Aux 1-8 / FX-Return
                for (uint8_t i = 0; i < 8; i++) {
                    setMeterLed(X32_BOARD_L, i, dsp.dspChannel[32 + i].meterInfo);
                    //setMeterLed(X32_BOARD_M, i, 0);
                }
                break;
            case 3: // Bus 1-16
                break;
        }
    }else{
        // update meters on board L
        switch (activeBank_inputFader) {
            case 0: // Input 1-8
                for (uint8_t i = 0; i < 8; i++) {
                    setMeterLed(X32_BOARD_L, i, dsp.dspChannel[i].meterInfo);
                }
                break;
            case 1: // Input 9-16
                for (uint8_t i = 0; i < 8; i++) {
                    setMeterLed(X32_BOARD_L, i, dsp.dspChannel[8 + i].meterInfo);
                }
                break;
            case 2: // Input 17-24
                for (uint8_t i = 0; i < 8; i++) {
                    setMeterLed(X32_BOARD_L, i, dsp.dspChannel[16 + i].meterInfo);
                }
                break;
            case 3: // Input 25-32
                for (uint8_t i = 0; i < 8; i++) {
                    setMeterLed(X32_BOARD_L, i, dsp.dspChannel[24 + i].meterInfo);
                }
                break;
            case 4: // Aux 1-8
                for (uint8_t i = 0; i < 8; i++) {
                    setMeterLed(X32_BOARD_L, i, dsp.dspChannel[32 + i].meterInfo);
                }
                break;
            case 5: // FX-Return
                break;
            case 6: // Bus 1-8
                break;
            case 7: // Bus 9-16
                break;
        }
    }

    // update meters on board R
    switch (activeBank_busFader) {
        case 0: // DCA1-8
            // no meter here
            break;
        case 1: // BUS 1-8
            break;
        case 2: // BUS 1-16
            break;
        case 3: // Matrix 1-6, Special, MainSub
            break;
    }
}

void Mixer::surfaceSyncBankIndicator(void) {
    if (HasChanged(X32_MIXER_CHANGED_BANKING)) {
        if (config.IsModelX32Full()){
            setLedByEnum(X32_BTN_CH_1_16, 0);
            setLedByEnum(X32_BTN_CH_17_32, 0);
            setLedByEnum(X32_BTN_AUX_IN_EFFECTS, 0);
            setLedByEnum(X32_BTN_BUS_MASTER, 0);
            if (activeBank_inputFader == 0) { setLedByEnum(X32_BTN_CH_1_16, 1); }
            if (activeBank_inputFader == 1) { setLedByEnum(X32_BTN_CH_17_32, 1); }
            if (activeBank_inputFader == 2) { setLedByEnum(X32_BTN_AUX_IN_EFFECTS, 1); }
            if (activeBank_inputFader == 3) { setLedByEnum(X32_BTN_BUS_MASTER, 1); }
        }
        if (config.IsModelX32CompactOrProducer()) {
            setLedByEnum(X32_BTN_CH_1_8, 0);
            setLedByEnum(X32_BTN_CH_9_16, 0);
            setLedByEnum(X32_BTN_CH_17_24, 0);
            setLedByEnum(X32_BTN_CH_25_32, 0);
            setLedByEnum(X32_BTN_AUX_IN_1_6_USB_REC, 0);
            setLedByEnum(X32_BTN_EFFECTS_RETURNS, 0);
            setLedByEnum(X32_BTN_BUS_1_8_MASTER, 0);
            setLedByEnum(X32_BTN_BUS_9_16_MASTER, 0);
            if (activeBank_inputFader == 0) { setLedByEnum(X32_BTN_CH_1_8, 1); }
            if (activeBank_inputFader == 1) { setLedByEnum(X32_BTN_CH_9_16, 1); }
            if (activeBank_inputFader == 2) { setLedByEnum(X32_BTN_CH_17_24, 1); }
            if (activeBank_inputFader == 3) { setLedByEnum(X32_BTN_CH_25_32, 1); }
            if (activeBank_inputFader == 4) { setLedByEnum(X32_BTN_AUX_IN_1_6_USB_REC, 1); }
            if (activeBank_inputFader == 5) { setLedByEnum(X32_BTN_EFFECTS_RETURNS, 1); }
            if (activeBank_inputFader == 6) { setLedByEnum(X32_BTN_BUS_1_8_MASTER, 1); }
            if (activeBank_inputFader == 7) { setLedByEnum(X32_BTN_BUS_9_16_MASTER, 1); }
        }
        setLedByEnum(X32_BTN_GROUP_DCA_1_8, 0);
        setLedByEnum(X32_BTN_BUS_1_8, 0);
        setLedByEnum(X32_BTN_BUS_9_16, 0);
        setLedByEnum(X32_BTN_MATRIX_MAIN_C, 0);
        if (activeBank_busFader == 0) { setLedByEnum(X32_BTN_GROUP_DCA_1_8, 1); }
        if (activeBank_busFader == 1) { setLedByEnum(X32_BTN_BUS_1_8, 1); }
        if (activeBank_busFader == 2) { setLedByEnum(X32_BTN_BUS_9_16, 1); }
        if (activeBank_busFader == 3) { setLedByEnum(X32_BTN_MATRIX_MAIN_C, 1); }
    }
}

// only X32 Rack and X32 Core
void Mixer::setLedChannelIndicator(void){
    if (config.IsModelX32Core() || config.IsModelX32Rack()){
        // Turn off all LEDS
        setLedByEnum(X32_LED_IN, 0);
        setLedByEnum(X32_LED_AUX, 0);
        setLedByEnum(X32_LED_BUS, 0);
        setLedByEnum(X32_LED_DCA, 0);
        setLedByEnum(X32_LED_MAIN, 0);
        setLedByEnum(X32_LED_MATRIX, 0);

        uint8_t chanIdx = GetSelectedvChannelIndex();

        if ((chanIdx >= 0)&&(chanIdx <= 31)){
            setLedByEnum(X32_LED_IN, 1);
        }
        if ((chanIdx >= 32)&&(chanIdx <= 47)){
            setLedByEnum(X32_LED_AUX, 1);
        }
        if ((chanIdx >= 48)&&(chanIdx <= 63)){
            setLedByEnum(X32_LED_BUS, 1);
        }
        if ((chanIdx >= 64)&&(chanIdx <= 69)){
            setLedByEnum(X32_LED_MATRIX, 1);
        }
        if ((chanIdx >= 70)&&(chanIdx <= 71)){
            setLedByEnum(X32_LED_MAIN, 1);
        }
        if ((chanIdx >= 72)&&(chanIdx <= 79)){
            setLedByEnum(X32_LED_DCA, 1);
        }

        // set 7-Segment Display
        setDisplay(chanIdx);        
    }
}


uint8_t Mixer::surfaceCalcPreampMeter(uint8_t channel) {
    if (channel >= 40) {
        return 0; // no preamps outside the 40 dsp-channels
    }

    float audiodata = dsp.dspChannel[channel].meterPu*2147483648.0f;
    uint32_t meterdata = 0;
    if (audiodata >= vuThresholds[0])  { meterdata |= 0b10000000; } // CLIP
    if (audiodata >= vuThresholds[3])  { meterdata |= 0b01000000; } // -3dBfs
    if (audiodata >= vuThresholds[5])  { meterdata |= 0b00100000; } // -6dBfs
    if (audiodata >= vuThresholds[7])  { meterdata |= 0b00010000; } // -9dBfs
    if (audiodata >= vuThresholds[8])  { meterdata |= 0b00001000; } // -12dBfs
    if (audiodata >= vuThresholds[10]) { meterdata |= 0b00000100; } // -18dBfs
    if (audiodata >= vuThresholds[14]) { meterdata |= 0b00000010; } // -30dBfs
    if (audiodata >= vuThresholds[24]) { meterdata |= 0b00000001; } // SIG

    return meterdata;
}

uint8_t Mixer::surfaceCalcDynamicMeter(uint8_t channel) {
    // leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
    if (channel < 40) {
        uint32_t meterdata = 0;

        if (dsp.dspChannel[channel].compressor.gain < 1.0f) { meterdata |= 0b10000000; };

        float gateValue = (1.0f - dsp.dspChannel[channel].gate.gain) * 80.0f;
        if (gateValue >= 2.0f)  { meterdata |= 0b00100000; }        
        if (gateValue >= 4.0f)  { meterdata |= 0b00010000; }        
        if (gateValue >= 6.0f)  { meterdata |= 0b00001000; }        
        if (gateValue >= 10.0f) { meterdata |= 0b00000100; }        
        if (gateValue >= 18.0f) { meterdata |= 0b00000010; }        
        if (gateValue >= 30.0f) { meterdata |= 0b00000001; }        

        if (dsp.dspChannel[channel].gate.gain < 1.0f) { meterdata |= 0b01000000; };

        return meterdata;
    }else{
        return 0; // no dynamic-data for other channels at the moment
    }
}

void Mixer::halSyncChannelConfigFromMixer(void){
    // loop trough all channels
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        VChannel chan = GetVChannel(i);

        if (i < 40) {
            // one of the 40 DSP-channels
            if (chan.HasChanged(X32_VCHANNEL_CHANGED_INPUT)) {
                //dspSetInputRouting(chan.index);
            }

            if (chan.HasChanged(X32_VCHANNEL_CHANGED_GAIN)){
                halSendGain(chan.index);
            }

            if (chan.HasChanged(X32_VCHANNEL_CHANGED_PHANTOM)){
                halSendPhantomPower(chan.index);
            }

            if ((chan.HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || (chan.HasChanged(X32_VCHANNEL_CHANGED_MUTE)))){
                dspSendChannelVolume(chan.index);
            }

            if (chan.HasChanged(X32_VCHANNEL_CHANGED_GATE)){
                dspSendGate(chan.index);
            }

            if (chan.HasChanged(X32_VCHANNEL_CHANGED_EQ)){
                dspSendEQ(chan.index);
                dspSendLowcut(chan.index);
            }

            if (chan.HasChanged(X32_VCHANNEL_CHANGED_DYNAMIC)){
                dspSendCompressor(chan.index);
            }

            if (chan.HasChanged(X32_VCHANNEL_CHANGED_SENDS)) {
                dspSendChannelSend(chan.index);
            }
        }else{
            // one of the other channels like Mixbus, DCA, Main, etc.
            uint8_t group;
            if ((i >= 40) && (i <= 47)) {
                // FX Returns 1-8
                group = 'f';
            }else if ((i >= 48) && (i <= 63)) {
                // Busmaster 1-16
                group = 'b';
            }else if ((i >= 64) && (i <= 69)) {
                // Matrix 1-6
                group = 'x';
            }else if (i == 70) {
                // "VERY SPECIAL CHANNEL"
                group = 'v';
            }else if (i == 71) {
                // Mono/Sub
                group = 's';
            }else if ((i >= 72) && (i <= 79)) {
                // DCA 1-8
                group = 'd';
            }else if (i == 80) {
                // main-LR
                group = 'm';
            }

            if ((chan.HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || (chan.HasChanged(X32_VCHANNEL_CHANGED_MUTE)))){
                switch (group) {
                    case 'b':
                        dspSendMixbusVolume(i - 48);
                        break;
                    case 'x':
                        dspSendMatrixVolume(i - 64);
                        break;
                    case 'm':
                        dspSendMainVolume();
                        break;
                    case 's':
                        dspSendMainVolume();
                        break;
                }
            }
        }
    }

    x32debug("Mixer gain to hardware synced\n");
}



// ####################################################################
// #
// #
// #        Touchcontrol
// #
// #
// ####################################################################

void Mixer::touchcontrolTick(void){
    if (touchcontrol.value > 0) {
        touchcontrol.value--;
        if (touchcontrol.value == 0)
        {
            // trigger sync for this channel
            SetVChannelChangeFlagsFromIndex(
                GetvChannelIndexFromButtonOrFaderIndex(touchcontrol.board, touchcontrol.faderIndex),
                X32_VCHANNEL_CHANGED_VOLUME
            );
        }
        x32debug("TouchControl=%d\n", touchcontrol.value);
    }
}

bool Mixer::touchcontrolCanSetFader(X32_BOARD p_board, uint8_t p_faderIndex) {
    if ((touchcontrol.board != p_board) && (touchcontrol.faderIndex != p_faderIndex)){
        return true;
    } 

    if (touchcontrol.value == 0){
        return true;
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
