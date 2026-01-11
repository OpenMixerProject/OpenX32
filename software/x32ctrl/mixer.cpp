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

Mixer::Mixer(X32BaseParameter* basepar): X32Base(basepar) {
    fpga = new Fpga(basepar);
    dsp = new DSP1(basepar);
    adda = new Adda(basepar);

    for(int v=0;v<MAX_VCHANNELS;v++) {
        vchannel[v] = new VChannel(basepar);
    }    
}

void Mixer::Init() {
    dsp->Init();
    dsp->SendAll();
    fpga->Init();
    adda->Init();

    LoadVChannelLayout();

    if(helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE)) {
		DebugPrintvChannels();
	}
}





void Mixer::LoadVChannelLayout() {
    //##################################################################################
    //#
    //#   create default vchannels (what the user is refering to as "mixer channel")
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
        VChannel* chan = vchannel[i];
        chan->dspChannel =  &dsp->Channel[i];
        chan->name = String("Kanal ") + String(i+1);
        chan->nameIntern = String("CH") + String(i+1);
        chan->color = SURFACE_COLOR_YELLOW;
        chan->vChannelType = X32_VCHANNELTYPE_NORMAL;
    }

    // AUX 1-6 / USB
    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up AUX");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = X32_VCHANNEL_BLOCK_AUX + i;

        VChannel* chan = vchannel[index];
        chan->dspChannel = &dsp->Channel[index];

        if(i <=5){
            chan->name = String("AUX") + String(i+1);
            chan->nameIntern = chan->name;
            chan->color = SURFACE_COLOR_GREEN;
        } else {
            chan->name = String("USB");
            chan->nameIntern = chan->name;
            chan->color = SURFACE_COLOR_YELLOW;
        }
        chan->vChannelType = X32_VCHANNELTYPE_AUX;
    }

    // FX Returns 1-8
    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up FX Returns");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = X32_VCHANNEL_BLOCK_FXRET + i;
        VChannel* chan = vchannel[index];
        chan->name = String("FX RET") + String(i+1);
        chan->nameIntern = chan->name;
        chan->color = SURFACE_COLOR_BLUE;
        chan->vChannelType = X32_VCHANNELTYPE_FXRET;
    }

    // Bus 1-16
    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up Busses");
    for (uint8_t i=0; i<=15;i++){
        uint8_t index = X32_VCHANNEL_BLOCK_BUS + i;
        VChannel* chan = vchannel[index];
        chan->name = String("BUS") + String(i+1);
        chan->nameIntern = chan->name;
        chan->color = SURFACE_COLOR_CYAN;
        chan->vChannelType = X32_VCHANNELTYPE_BUS;
    }

    // Matrix 1-6 / Special / SUB
    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up Matrix / SPECIAL / SUB");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = X32_VCHANNEL_BLOCK_MATRIX + i;
        VChannel* chan = vchannel[index];
        if(i <=5){
            chan->name = String("MATRIX") + String(i+1);
            chan->nameIntern = chan->name;
            chan->color =  SURFACE_COLOR_PINK;
            chan->vChannelType = X32_VCHANNELTYPE_MATRIX;
        } else if (i == 6){
            chan->name = String("SPECIAL");
            chan->nameIntern = chan->name;
            chan->color =  SURFACE_COLOR_RED;
            chan->vChannelType = X32_VCHANNELTYPE_SPECIAL;
        } else if (i == 7){
            chan->name = String("M/C");
            chan->nameIntern = chan->name;
            chan->color =  SURFACE_COLOR_WHITE;
            chan->vChannelType = X32_VCHANNELTYPE_MAINSUB;
        };
    }

    // DCA 1-8
    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up DCA");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = X32_VCHANNEL_BLOCK_DCA + i;
        VChannel* chan = vchannel[index];
        chan->name = String("DCA") + String(i+1);
        chan->nameIntern = chan->name;
        chan->color = SURFACE_COLOR_PINK;
        chan->vChannelType = X32_VCHANNELTYPE_DCA;;
    }

    // Main Channel
    {
        VChannel* chan = vchannel[X32_VCHANNEL_BLOCK_MAIN];
        chan->name = String("MAIN");
        chan->nameIntern = chan->name;
        chan->color = SURFACE_COLOR_WHITE;
        chan->vChannelType = X32_VCHANNELTYPE_MAIN;
    }
}


// ####################################################################
// #
// #
// #        Routing + Hardware channel assignment
// #
// #
// ###################################################################



void Mixer::ChangeGuiSelection(int8_t amount) {
    int16_t newValue = (int16_t)state->gui_old_selected_item + amount;
    
    state->gui_selected_item = newValue;
	state->SetChangeFlags(X32_MIXER_CHANGED_GUI_SELECT);
}

void Mixer::ChangeHardwareInput(uint8_t outputIndex, int8_t amount) {
    // get current routingIndex
    uint8_t currentRouting = fpga->GetOutputByIndex(outputIndex + 1);
    int16_t newValue = currentRouting + amount;

    if (newValue > NUM_INPUT_CHANNEL) {
        newValue -= (NUM_INPUT_CHANNEL + 1);
    }
    if (newValue < 0) {
        newValue += (NUM_INPUT_CHANNEL + 1); // we allow 208 input channels plus a single "OFF" channel
    }

    helper->DEBUG_MIXER(DEBUGLEVEL_NORMAL, "Change! %d -> %d", currentRouting, newValue);

    fpga->ConnectByIndex(newValue, outputIndex + 1);
    fpga->SendRoutingToFpga(outputIndex);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspInput(uint8_t vChannelIndex, int8_t amount) {
    int16_t newValue = dsp->Channel[vChannelIndex].input + amount;

    if (newValue >= DSP_MAX_INTERNAL_CHANNELS) {
        newValue -= DSP_MAX_INTERNAL_CHANNELS;
    }
    if (newValue < 0) {
        newValue += DSP_MAX_INTERNAL_CHANNELS;
    }

    dsp->Channel[vChannelIndex].input = newValue;
    dsp->SetInputRouting(vChannelIndex);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspInputTapPoint(uint8_t vChannelIndex, int8_t amount) {
    int16_t newValue = dsp->Channel[vChannelIndex].inputTapPoint + amount;

    if (newValue > 4) {
        newValue = 0;
    }
    if (newValue < 0) {
        newValue = 4;
    }

    dsp->Channel[vChannelIndex].inputTapPoint = newValue;
    dsp->SetInputRouting(vChannelIndex);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspOutput(uint8_t channel, int8_t amount) {
    int16_t newValue = dsp->Dsp1toFpga[channel].input + amount;

    if (newValue >= DSP_MAX_INTERNAL_CHANNELS) {
        newValue -= DSP_MAX_INTERNAL_CHANNELS;
    }
    if (newValue < 0) {
        newValue += DSP_MAX_INTERNAL_CHANNELS;
    }

    dsp->Dsp1toFpga[channel].input = newValue;
    dsp->SetOutputRouting(channel);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspOutputTapPoint(uint8_t channel, int8_t amount) {
    int16_t newValue = dsp->Dsp1toFpga[channel].tapPoint + amount;

    if (newValue > 4) {
        newValue = 0;
    }
    if (newValue < 0) {
        newValue = 4;
    }

    dsp->Dsp1toFpga[channel].tapPoint = newValue;
    dsp->SetOutputRouting(channel);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspFxOutput(uint8_t channel, int8_t amount) {
    int16_t newValue = dsp->Dsp1toDsp2Routing[channel].input + amount;

    if (newValue >= DSP_MAX_INTERNAL_CHANNELS) {
        newValue -= DSP_MAX_INTERNAL_CHANNELS;
    }
    if (newValue < 0) {
        newValue += DSP_MAX_INTERNAL_CHANNELS;
    }

    dsp->Dsp1toDsp2Routing[channel].input = newValue;
    dsp->SetFxOutputRouting(channel);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspFxOutputTapPoint(uint8_t channel, int8_t amount) {
    int16_t newValue = dsp->Dsp1toDsp2Routing[channel].tapPoint + amount;

    if (newValue > 4) {
        newValue = 0;
    }
    if (newValue < 0) {
        newValue = 4;
    }

    dsp->Dsp1toDsp2Routing[channel].tapPoint = newValue;
    dsp->SetFxOutputRouting(channel);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::SetVChannelChangeFlagsFromIndex(uint8_t p_chanIndex, uint16_t p_flag){
    vchannel[p_chanIndex]->SetChanged(p_flag);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}


void Mixer::SetSolo(uint8_t channelIndex, bool solo){
    VChannel* chan = GetVChannel(channelIndex);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL:
        case X32_VCHANNELTYPE_AUX: {
            chan->dspChannel->solo = solo;
            chan->SetChanged(X32_VCHANNEL_CHANGED_SOLO);
            break;
        }
        case X32_VCHANNELTYPE_BUS: {
            dsp->Bus[channelIndex - X32_VCHANNEL_BLOCK_BUS].solo = solo;
            chan->SetChanged(X32_VCHANNEL_CHANGED_SOLO);
            break;
        }
        case X32_VCHANNELTYPE_MATRIX: {
            dsp->Matrix[channelIndex - X32_VCHANNEL_BLOCK_MATRIX].solo = solo;
            chan->SetChanged(X32_VCHANNEL_CHANGED_SOLO);
            break;
        }
        case X32_VCHANNELTYPE_MAINSUB: {
            dsp->MainChannelSub.solo = solo;
            chan->SetChanged(X32_VCHANNEL_CHANGED_SOLO);
            break;
        }
        case X32_VCHANNELTYPE_MAIN: {
            dsp->MainChannelLR.solo = solo;
            chan->SetChanged(X32_VCHANNEL_CHANGED_SOLO);
            break;
        }
    }

    if (chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO)){
        helper->DEBUG_MIXER(DEBUGLEVEL_NORMAL, "Solo of channel %s \"%s\" changed to %s\n", chan->nameIntern.c_str(), chan->name.c_str(), solo ? "ON" : "OFF"); 
    }

    //TODOs
    // - switch monitor source
}

void Mixer::ToggleSolo(uint8_t vChannelIndex){
    SetSolo(vChannelIndex, !GetSolo(vChannelIndex));
}

void Mixer::ClearSolo(void){
    if (IsSoloActivated()){
        for (int i=0; i<MAX_VCHANNELS; i++){
            SetSolo(i, false);
        }
    }
}

void Mixer::SetPhantom(uint8_t p_vChannelIndex, bool p_phantom){
    VChannel* chan = GetVChannel(p_vChannelIndex);
    if (chan->vChannelType == X32_VCHANNELTYPE_NORMAL) {
        uint8_t channelInputSource = chan->dspChannel->input;

        // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
        if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
            // we are connected to one of the DSP-inputs

            // check if we are connected to a channel with gain
            uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];

            if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
                // XLR-input
                preamps.phantomPowerXlr[dspInputSource - 1] = p_phantom;
            }
            else if ((dspInputSource >= 113) && (dspInputSource <= 160))
            {
                preamps.phantomPowerAes50a[dspInputSource - 1] = p_phantom;
                // AES50A input
            }
            else if ((dspInputSource >= 161) && (dspInputSource <= 208)) 
            {
                preamps.phantomPowerAes50b[dspInputSource - 1] = p_phantom;
                // AES50B input
            }
        }
        chan->SetChanged(X32_VCHANNEL_CHANGED_PHANTOM);
    }
}

void Mixer::TogglePhantom(uint8_t p_vChannelIndex){
    SetPhantom(p_vChannelIndex, !GetPhantomPower(p_vChannelIndex));
}

void Mixer::SetPhaseInvert(uint8_t p_vChannelIndex, bool p_phaseInvert){
    VChannel* chan = GetVChannel(p_vChannelIndex);
    if (chan->vChannelType == X32_VCHANNELTYPE_NORMAL) {
        uint8_t channelInputSource = chan->dspChannel->input;

        // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
        if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
            // we are connected to one of the DSP-inputs

            // check if we are connected to a channel with gain
            uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];

            if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
                // XLR-input
                preamps.phaseInvertXlr[dspInputSource - 1] = p_phaseInvert;
            }
            else if ((dspInputSource >= 113) && (dspInputSource <= 160))
            {
                preamps.phaseAes50a[dspInputSource - 1] = p_phaseInvert;
                // AES50A input
            }
            else if ((dspInputSource >= 161) && (dspInputSource <= 208)) 
            {
                preamps.phaseAes50b[dspInputSource - 1] = p_phaseInvert;
                // AES50B input
            }
        }
        
        chan->SetChanged(X32_VCHANNEL_CHANGED_PHASE_INVERT);
    }
}

void Mixer::TogglePhaseInvert(uint8_t vChannelIndex){
    SetPhaseInvert(vChannelIndex, !GetPhaseInvert(vChannelIndex));
}

void Mixer::SetMute(uint8_t channelIndex, bool mute){
    VChannel* chan = GetVChannel(channelIndex);

    helper->DEBUG_MIXER(DEBUGLEVEL_NORMAL, "mute channelIndex=%d %s", channelIndex, mute ? "ON": "OFF");

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL:
        case X32_VCHANNELTYPE_AUX: {
            chan->dspChannel->muted = mute; 
            chan->SetChanged(X32_VCHANNEL_CHANGED_MUTE);
            break;
        }
        case X32_VCHANNELTYPE_BUS: {
            dsp->Bus[channelIndex - X32_VCHANNEL_BLOCK_BUS].muted = mute;
            chan->SetChanged(X32_VCHANNEL_CHANGED_MUTE);
            break;
        }
        case X32_VCHANNELTYPE_MATRIX: {
            dsp->Matrix[channelIndex - X32_VCHANNEL_BLOCK_MATRIX].muted = mute;
            chan->SetChanged(X32_VCHANNEL_CHANGED_MUTE);
            break;
        }
        case X32_VCHANNELTYPE_MAINSUB: {
            dsp->MainChannelSub.muted = mute;
            chan->SetChanged(X32_VCHANNEL_CHANGED_MUTE);
            break;
        }
        case X32_VCHANNELTYPE_MAIN: {
            dsp->MainChannelLR.muted = mute;
            chan->SetChanged(X32_VCHANNEL_CHANGED_MUTE);
            break;
        }
    }
}

void Mixer::ToggleMute(uint8_t vChannelIndex){
    SetMute(vChannelIndex, !GetMute(vChannelIndex));
}

void Mixer::SetGain(uint8_t p_vChannelIndex, float gain) {
    VChannel* chan = GetVChannel(p_vChannelIndex);

    if(chan->vChannelType == X32_VCHANNELTYPE_NORMAL || chan->vChannelType == X32_VCHANNELTYPE_AUX) {

        uint8_t channelInputSource = dsp->Channel[p_vChannelIndex].input;

        // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
        if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
            // we are connected to one of the DSP-inputs

            // check if we are connected to a channel with gain
            uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
            if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
                // XLR-input
                preamps.gainXlr[dspInputSource - 1] = gain;
            }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
                // AES50A input
                preamps.gainAes50a[dspInputSource - 1] = gain;
            }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
                preamps.gainAes50b[dspInputSource - 1] = gain;
                // AES50B input
            }
        }

        chan->SetChanged(X32_VCHANNEL_CHANGED_GAIN);
    }   
}

// volume in dBfs
void Mixer::ChangeGain(uint8_t vChannelIndex, int8_t p_amount){
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    float newValue = GetGain(vChannelIndex) + (((float)p_amount) * 0.3 );
    if (newValue > 60) {
        newValue = 60;
    }else if (newValue < -12) {
        newValue = -12;
    }
    SetGain(vChannelIndex, newValue);
}

// volume in dBfs
void Mixer::ChangeVolume(uint8_t vChannelIndex, int8_t p_amount){
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    float newValue = GetVolumeDbfs(vChannelIndex) + (((float)p_amount) * 0.3 );
    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }
    SetVolume(vChannelIndex, newValue);
}

// volume in dBfs
void Mixer::SetVolume(uint8_t vChannelIndex, float volume){
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    float newValue = volume;
    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }

    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "SetVolume -> %f dbFS", newValue);

    VChannel* chan = GetVChannel(vChannelIndex);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL:
        case X32_VCHANNELTYPE_AUX: {
            dsp->Channel[vChannelIndex].volumeLR = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_FXRET: {
            dsp->volumeFxReturn[vChannelIndex - X32_VCHANNEL_BLOCK_FXRET] = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_BUS: {
            dsp->Bus[vChannelIndex - X32_VCHANNEL_BLOCK_BUS].volumeLR = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_MATRIX: {
            dsp->Matrix[vChannelIndex - X32_VCHANNEL_BLOCK_MATRIX].volume = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_SPECIAL: {
            dsp->Matrix[vChannelIndex - X32_VCHANNEL_BLOCK_SPECIAL].volume = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_MAINSUB: {
            dsp->MainChannelSub.volume = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_MAIN: {
            dsp->MainChannelLR.volume = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_DCA: {
            dsp->volumeDca[vChannelIndex - X32_VCHANNEL_BLOCK_DCA] = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
    }    
}    

// Volume in Fadervalue 0x000...0x0FFF
void Mixer::SetVolumeFadervalue(uint8_t vChannelIndex, float volume){
    SetVolume(vChannelIndex, helper->Fadervalue2dBfs(volume));
}

// Volume in Oscvalue 0.000~...1.0
void Mixer::SetVolumeOscvalue(uint8_t vChannelIndex, float volume){
    SetVolumeFadervalue(vChannelIndex, helper->Oscvalue2Fadervalue(volume));
}

void Mixer::ChangeBalance(uint8_t vChannelIndex, int8_t p_amount){
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    float newValue = GetBalance(vChannelIndex) + pow((float)p_amount, 3.0f);
    if (newValue > 100) {
        newValue = 100;
    }else if (newValue < -100) {
        newValue = -100;
    }
    SetBalance(vChannelIndex, newValue);
}

void Mixer::SetBalance(uint8_t vChannelIndex, float p_balance){
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    float newValue = p_balance;
    if (newValue > 100) {
        newValue = 100;
    }else if (newValue < -100) {
        newValue = -100;
    }

    VChannel* chan = GetVChannel(vChannelIndex);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL:
        case X32_VCHANNELTYPE_AUX: {
            dsp->Channel[vChannelIndex].balance = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            chan->SetChanged(X32_VCHANNEL_CHANGED_BALANCE); // TODO: check if this is necessary
            break;
        }
        case X32_VCHANNELTYPE_BUS: {
            dsp->Bus[vChannelIndex - X32_VCHANNEL_BLOCK_BUS].balance = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_MAINSUB: {
            dsp->MainChannelSub.balance = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_MAIN: {
            dsp->MainChannelLR.balance = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
    }
}

void Mixer::SetBusSend(uint8_t vChannelIndex, uint8_t index, float value) {
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    
    float newValue = value;
    if (value > 10) {
        newValue = 10;
    }else if (value < -100) {
        newValue = -100;
    }

    VChannel* chan = GetVChannel(vChannelIndex);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL:
        case X32_VCHANNELTYPE_AUX: {
            dsp->Channel[vChannelIndex].sendMixbus[index] = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_BUS: {
            // we have only 6 matrices -> check it
            if (index < 6) {
                dsp->Bus[vChannelIndex].sendMatrix[index] = newValue;
            }
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_MAINSUB: {
            // we have only 6 matrices -> check it
            if (index < 6) {
                dsp->MainChannelSub.sendMatrix[index] = newValue;
            }
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE_MAIN: {
            // we have only 6 matrices -> check it
            if (index < 6) {
                dsp->MainChannelLR.sendMatrix[index] = newValue;
            }
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
    }
}

void Mixer::ChangeBusSend(uint8_t vChannelIndex, uint8_t encoderIndex, int8_t p_amount, uint8_t activeBusSend){
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    
    float newValue = GetBusSend(vChannelIndex, encoderIndex)  + pow((float)p_amount, 3.0f);

    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }
    SetBusSend(vChannelIndex, encoderIndex, newValue);
}

void Mixer::SetGate(uint8_t vChannelIndex, char option, float value) {
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    VChannel* chan = GetVChannel(vChannelIndex);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL: {
            sGate* gate = &dsp->Channel[vChannelIndex].gate;
            switch (option) {
                case 'T': // Threshold
                    if ((value >= -80) && (value <= 0)) {
                        gate->threshold = value;
                    }
                    break;
                case 'R': // Range
                    if ((value >= 3) && (value <= 60)) {
                        gate->range = value;
                    }
                    break;
                case 'A': // Attack
                    if ((value >= 0) && (value <= 120)) {
                        gate->attackTime_ms = value;
                    }
                    break;
                case 'H': // Hold
                    if ((value >= 0.02) && (value <= 2000)) {
                        gate->holdTime_ms = value;
                    }
                    break;
                case 'r': // Release
                    if ((value >= 5) && (value <= 4000)) {
                        gate->releaseTime_ms = value;
                    }
                    break;
            }
            chan->SetChanged(X32_VCHANNEL_CHANGED_GATE);
            break;
        }
    }    
}

float Mixer::GetGate(uint8_t vChannelIndex, char option) {
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return -80;
    }

    VChannel* chan = GetVChannel(vChannelIndex);

    if (chan->vChannelType == X32_VCHANNELTYPE_NORMAL){
        sGate* gate = &dsp->Channel[vChannelIndex].gate;
        switch (option) {
            case 'T': // Threshold
                return gate->threshold;
            case 'R': // Range
                return gate->range;
            case 'A': // Attack
                return gate->attackTime_ms;
            case 'H': // Hold
                return gate->holdTime_ms;
            case 'r': // Release
                return gate->releaseTime_ms;
        }
    }
    
    return 0;
}

void Mixer::ChangeGate(uint8_t vChannelIndex, char option, int8_t p_amount) {
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    float newValue = GetGate(vChannelIndex, option);

    switch (option) {
        case 'T': // Threshold
            newValue += ((float)p_amount * abs((float)p_amount)) * 1.0f;
            break;
        case 'R': // Range
            newValue += ((float)p_amount * abs((float)p_amount)) * 1.5f;
            break;
        case 'A': // Attack
            newValue += ((float)p_amount * abs((float)p_amount)) * 2.0f;
            break;
        case 'H': // Hold
            newValue += ((float)p_amount * abs((float)p_amount)) * 2.0f;
            break;
        case 'r': // Release
            newValue += ((float)p_amount * abs((float)p_amount)) * 2.0f;
            break;
    }

    SetGate(vChannelIndex, option, newValue);
}

void Mixer::SetLowcut(uint8_t vChannelIndex, float lowCutFrequency){
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    float newValue = lowCutFrequency;
    if (newValue > 24000) {
        newValue = 24000;
    } else if (newValue < 20) {
        newValue = 20;
    }

    VChannel* chan = GetVChannel(vChannelIndex);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL:
        case X32_VCHANNELTYPE_AUX: {
            dsp->Channel[vChannelIndex].lowCutFrequency = lowCutFrequency;
            chan->SetChanged(X32_VCHANNEL_CHANGED_EQ);
            break;
        }
    }
}

float Mixer::GetLowcut(uint8_t vChannelIndex){
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return 20;
    }

    VChannel* chan = GetVChannel(vChannelIndex);

    if (chan->vChannelType == X32_VCHANNELTYPE_NORMAL || chan->vChannelType == X32_VCHANNELTYPE_AUX){
        return dsp->Channel[vChannelIndex].lowCutFrequency;
    }
    
    return 20;
}

void Mixer::ChangeLowcut(uint8_t vChannelIndex, int8_t amount){
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    float newValue = GetLowcut(vChannelIndex) * (1 + (float)amount/20.0f);
    SetLowcut(vChannelIndex, newValue);
}

void Mixer::SetDynamics(uint8_t vChannelIndex, char option, float value) {
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    VChannel* chan = GetVChannel(vChannelIndex);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL:
        case X32_VCHANNELTYPE_AUX: {
            sCompressor* comp = &dsp->Channel[vChannelIndex].compressor;
            switch (option) {
                case 'T': // Threshold
                    if ((value >= -60) && (value <= 0)) {
                        comp->threshold = value;
                        helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Channel %s: Compressor threshold set to %f", chan->name.c_str(), value);
                    }
                    break;
                case 'R': // Ratio
                    if (value < 1.1) {
                        comp->ratio = 1.1;
                    }else if (value > 100) {
                        comp->ratio = 100.0f;
                    }else{
                        comp->ratio = value;
                    }
                    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Channel %s: Compressor ratio set to %f", chan->name.c_str(), value);
                    break;
                case 'M': // Makeup
                    if ((value >= 0) && (value <= 24)) {
                        comp->makeup = value;
                        helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Channel %s: Compressor makeup-gain set to %f", chan->name.c_str(), value);
                    }
                    break;
                case 'A': // Attack
                    if ((value >= 0) && (value <= 120)) {
                        comp->attackTime_ms = value;
                        helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Channel %s: Compressor attack set to %f", chan->name.c_str(), value);
                    }
                    break;
                case 'H': // Hold
                    if ((value >= 0.02) && (value <= 2000)) {
                        comp->holdTime_ms = value;
                        helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Channel %s: Compressor hold set to %f", chan->name.c_str(), value);
                    }
                    break;
                case 'r': // Release
                    if ((value >= 5) && (value <= 4000)) {
                        comp->releaseTime_ms = value;
                        helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Channel %s: Compressor release set to %f", chan->name.c_str(), value);
                    }
                    break;
            }
            chan->SetChanged(X32_VCHANNEL_CHANGED_DYNAMIC);
            break;
        }
    }    
}

float Mixer::GetDynamics(uint8_t vChannelIndex, char option) {
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return 0;
    }

    VChannel* chan = GetVChannel(vChannelIndex);

    if (chan->vChannelType == X32_VCHANNELTYPE_NORMAL || chan->vChannelType == X32_VCHANNELTYPE_AUX){
        sCompressor* comp = &dsp->Channel[vChannelIndex].compressor;
        switch (option) {
            case 'T': // Threshold
                return comp->threshold;
                break;
            case 'R': // Ratio
                return comp->ratio;
                break;
            case 'M': // Makeup
                return comp->makeup;
                break;
            case 'A': // Attack
                return comp->attackTime_ms;
                break;
            case 'H': // Hold
                return comp->holdTime_ms;
                break;
            case 'r': // Release
                return comp->releaseTime_ms;
                break;
        }
    }

    return 0;
}

void Mixer::ChangeDynamics(uint8_t vChannelIndex, char option, int8_t p_amount) {
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    float newValue = GetDynamics(vChannelIndex, option);

    switch (option) {
        case 'T': // Threshold
            newValue += (float)p_amount/2.0f;
            break;
        case 'R': // Ratio
            newValue = newValue + (newValue * (float)p_amount)/10.0f; // take into account that we have to set small ratios and very large values
            break;
        case 'M': // Makeup
            newValue += ((float)p_amount * abs((float)p_amount)) * 1.0f;
            break;
        case 'A': // Attack
            newValue += ((float)p_amount * abs((float)p_amount)) * 2.0f;
            break;
        case 'H': // Hold
            newValue += ((float)p_amount * abs((float)p_amount)) * 2.0f;
            break;
        case 'r': // Release
            newValue += ((float)p_amount * abs((float)p_amount)) * 2.0f;
            break;
    }

    SetDynamics(vChannelIndex, option, newValue);
}

void Mixer::SetPeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, float value){
    if (pChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    if (eqIndex >= MAX_CHAN_EQS) {
        return;
    }

    VChannel* chan = GetVChannel(pChannelIndex);

    sPEQ* peq = nullptr;

    if (pChannelIndex < 40) {
        peq = &dsp->Channel[pChannelIndex].peq[eqIndex];
    }else if ((pChannelIndex >= 48) && (pChannelIndex <= 63)) {
        peq = &dsp->Bus[pChannelIndex - 48].peq[eqIndex];
    }else if ((pChannelIndex >= 64) && (pChannelIndex <= 69)) {
        peq = &dsp->Channel[pChannelIndex - 64].peq[eqIndex];
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
        dsp->fx->RecalcFilterCoefficients_PEQ(peq);

        chan->SetChanged(X32_VCHANNEL_CHANGED_EQ);
    }
}

void Mixer::ChangePeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, int8_t p_amount){
    if (pChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    if (eqIndex >= MAX_CHAN_EQS) {
        return;
    }

    sPEQ* peq = nullptr;

    if (pChannelIndex < 40) {
        peq = &dsp->Channel[pChannelIndex].peq[eqIndex];
    }else if ((pChannelIndex >= 48) && (pChannelIndex <= 63)) {
        peq = &dsp->Bus[pChannelIndex - 48].peq[eqIndex];
    }else if ((pChannelIndex >= 64) && (pChannelIndex <= 69)) {
        peq = &dsp->Channel[pChannelIndex - 64].peq[eqIndex];
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

bool Mixer::IsSoloActivated(void){
    for (uint8_t i=0; i < MAX_VCHANNELS; i++)
    {
        if (GetSolo(i)){
            return true;
        }
    }
    return false;
}

void Mixer::Sync(void){
    // loop trough all vchannels and sync to hardware
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        VChannel* chan = vchannel[i];

        if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK_NORMAL) ||
            helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK_AUX)) {

            // one of the 40 DSP-channels
            if (chan->HasChanged(X32_VCHANNEL_CHANGED_INPUT)) {
                dsp->SetInputRouting(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_GAIN)){
                halSendGain(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_PHANTOM)){
                halSendPhantomPower(i);
            }

            if ((
                chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) ||
                chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE) ||
                chan->HasChanged(X32_VCHANNEL_CHANGED_BALANCE)
            )){
                dsp->SendChannelVolume(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_GATE)){
                dsp->SendGate(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_EQ)){
                dsp->SendEQ(i);
                dsp->SendLowcut(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_DYNAMIC)){
                dsp->SendCompressor(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_SENDS)) {
                dsp->SendChannelSend(i);
            }
        } else {
            // one of the other channels like Mixbus, DCA, Main, etc.
            uint8_t group = ' ';
            if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK_FXRET)) {
                // FX Returns 1-8
                group = 'f';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK_BUS)) {
                // Busmaster 1-16
                group = 'b';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK_MATRIX)) {
                // Matrix 1-6
                group = 'x';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK_SPECIAL)) {
                // "VERY SPECIAL CHANNEL"
                group = 'v';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK_MAINSUB)) {
                // Mono/Sub
                group = 's';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK_DCA)) {
                // DCA 1-8
                group = 'd';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK_MAIN)) {
                // main-LR
                group = 'm';
            }

            if ((
                chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) ||
                chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)
            )){
                switch (group) {
                    case 'b':
                        dsp->SendMixbusVolume(i - 48);
                        break;
                    case 'x':
                        dsp->SendMatrixVolume(i - 64);
                        break;
                    case 'm':
                        dsp->SendMainVolume();
                        break;
                    case 's':
                        dsp->SendMainVolume();
                        break;
                }
            }
        }
    }

    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Mixer to Hardware synced");
}

// set the gain of the local XLR head-amp-control
void Mixer::halSendGain(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].input;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input

            // send value to adda-board
            uint8_t boardId = adda->GetXlrInBoardId(dspInputSource);
            if (boardId >= 0 && boardId < 4) {
                uint8_t addaChannel = dspInputSource;
                while (addaChannel > 8) {
                    addaChannel -= 8;
                }
                adda->SetGain(boardId, addaChannel, preamps.gainXlr[dspInputSource - 1], preamps.phantomPowerXlr[dspInputSource - 1]);
            }
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
        }
    }
}


// enable or disable phatom-power of local XLR-inputs
void Mixer::halSendPhantomPower(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].input;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input

            // send value to adda-board
            uint8_t boardId = adda->GetXlrInBoardId(dspInputSource);
            if (boardId >= 0 && boardId < 4) {
                uint8_t addaChannel = dspInputSource;
                while (addaChannel > 8) {
                    addaChannel -= 8;
                }
                adda->SetGain(boardId, addaChannel, preamps.gainXlr[dspInputSource - 1], preamps.phantomPowerXlr[dspInputSource - 1]);
            }
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
        }
    }
}

uint8_t Mixer::halGetDspInputSource(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].input;

    // check if we are using one of the FPGA-routed channels
    if ((channelInputSource >= 1) && (channelInputSource < 40)) {
        return fpga->fpgaRouting.dsp[channelInputSource - 1];
    }else{
        // DSP is not using one of the FPGA-routed channels
        return 0;
    }
}

uint16_t Mixer::GetSelectedVChannelIndex(void) {
    return config->selectedVChannel;
}

VChannel* Mixer::GetVChannel(uint8_t vCHannelIndex){
    return vchannel[vCHannelIndex];
}

float Mixer::GetVolumeDbfs(uint8_t vChannelIndex) {
    VChannel* chan = GetVChannel(vChannelIndex);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL:
        case X32_VCHANNELTYPE_AUX: {
            // Normal- and AUX-Channel must always return the volume-level of the
            // current DSP-Channel regardless of the internal source of the DSP
            return dsp->Channel[vChannelIndex].volumeLR;
        }
        case X32_VCHANNELTYPE_FXRET: {
            // FX return
            return dsp->volumeFxReturn[vChannelIndex - 40];
        }
        case X32_VCHANNELTYPE_BUS: {
            return dsp->Bus[vChannelIndex - 48].volumeLR;
        }
        case X32_VCHANNELTYPE_MATRIX: {
            return dsp->Matrix[vChannelIndex - 64].volume;
        }
        case X32_VCHANNELTYPE_SPECIAL: {
            return dsp->volumeSpecial; 
        }
        case X32_VCHANNELTYPE_MAINSUB: {
            return dsp->MainChannelSub.volume;
        }
        case X32_VCHANNELTYPE_MAIN: {
            return dsp->MainChannelLR.volume;
        }
        case X32_VCHANNELTYPE_DCA: {
            return dsp->volumeDca[vChannelIndex - 72];
        }
        default: {
            return VOLUME_MIN;
        }
    }
}

u_int16_t Mixer::GetVolumeFadervalue(uint8_t vChannelIndex){
    return helper->Dbfs2Fader(GetVolumeDbfs(vChannelIndex));
}

float Mixer::GetVolumeOscvalue(uint8_t vChannelIndex){
    return helper->Dbfs2Oscvalue(GetVolumeDbfs(vChannelIndex));
}

bool Mixer::GetMute(uint8_t dspChannel) {
    if (dspChannel <= 39) {
        return dsp->Channel[dspChannel].muted;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX-Return
        return false;
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        return dsp->Bus[dspChannel - 48].muted;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        return dsp->Matrix[dspChannel - 64].muted;
    }else if (dspChannel == 70) {
        // special
        return false;
    }else if (dspChannel == 71) {
        return dsp->MainChannelSub.muted;
    }else if ((dspChannel >= 72) && (dspChannel <= 79)) {
        // DCA
        return false;
    }else if (dspChannel == 80) {
        return dsp->MainChannelLR.muted;
    }

    return false;
}

bool Mixer::GetSolo(uint8_t dspChannel) {
    VChannel* chan = GetVChannel(dspChannel);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL:
        case X32_VCHANNELTYPE_AUX: {
            return chan->dspChannel->solo;
        }
        case X32_VCHANNELTYPE_BUS: {
            return dsp->Bus[dspChannel - X32_VCHANNEL_BLOCK_BUS].solo;
        }
        case X32_VCHANNELTYPE_MATRIX: {
            return dsp->Matrix[dspChannel - X32_VCHANNEL_BLOCK_MATRIX].solo;
        }
        case X32_VCHANNELTYPE_MAINSUB: {
            return dsp->MainChannelSub.solo;
        }
        case X32_VCHANNELTYPE_MAIN: {
            return dsp->MainChannelLR.solo;
        }
    }

    return false;
}

float Mixer::GetBalance(uint8_t vChannelIndex) {
    VChannel* chan = GetVChannel(vChannelIndex);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL:
        case X32_VCHANNELTYPE_AUX: {
            return dsp->Channel[vChannelIndex].balance;
        }
        case X32_VCHANNELTYPE_BUS: {
            return dsp->Bus[vChannelIndex - X32_VCHANNEL_BLOCK_BUS].balance;
        }
        case X32_VCHANNELTYPE_MAINSUB: {
            return dsp->MainChannelSub.balance;
        }
        case X32_VCHANNELTYPE_MAIN: {
            return dsp->MainChannelLR.balance;
        }
    }
    
    return 0;
}

float Mixer::GetGain(uint8_t vChannelIndex) {
    VChannel* chan = GetVChannel(vChannelIndex);

    if(chan->vChannelType == X32_VCHANNELTYPE_NORMAL || chan->vChannelType == X32_VCHANNELTYPE_AUX) {

        uint8_t channelInputSource = dsp->Channel[vChannelIndex].input;

        // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
        if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
            // we are connected to one of the DSP-inputs

            // check if we are connected to a channel with gain
            uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
            if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
                // XLR-input
                return preamps.gainXlr[dspInputSource - 1];
            }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
                // AES50A input
                return preamps.gainAes50a[dspInputSource - 113];
            }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
                // AES50B input
                return preamps.gainAes50b[dspInputSource - 161];
            }
        }
    }
    
    // we are connected to an internal DSP-signal OR not connected at all
    return 0;
}

bool Mixer::GetPhantomPower(uint8_t vChannelIndex) {
    
    VChannel* chan = GetVChannel(vChannelIndex);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE_NORMAL: {

            uint8_t channelInputSource = dsp->Channel[vChannelIndex].input;

            // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
            if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
                // we are connected to one of the DSP-inputs

                // check if we are connected to a channel with gain
                uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
                if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
                    // XLR-input
                    return preamps.phantomPowerXlr[dspInputSource - 1];
                }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
                    // AES50A input
                    return preamps.phantomPowerAes50a[dspInputSource - 113];
                }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
                    // AES50B input
                    return preamps.phantomPowerAes50b[dspInputSource - 161];
                }
            }
        }
    }

    // we are connected to an internal DSP-signal OR not connected at all
    return 0;
}

bool Mixer::GetPhaseInvert(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].input;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input
            return preamps.phaseInvertXlr[dspInputSource - 1];
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
            return preamps.phaseAes50a[dspInputSource - 113];
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
            return preamps.phaseAes50b[dspInputSource - 161];
        }
    }

    // we are connected to an internal DSP-signal OR not connected at all
    return 0;
}

float Mixer::GetBusSend(uint8_t dspChannel, uint8_t index) {
    if (dspChannel < 40) {
        return dsp->Channel[dspChannel].sendMixbus[index];
    }else if ((dspChannel >= 48) && (dspChannel < 63)) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            return dsp->Bus[dspChannel].sendMatrix[index];
        }
    }else if (dspChannel == 71) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            return dsp->MainChannelSub.sendMatrix[index];
        }
    }else if (dspChannel == 80) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            return dsp->MainChannelLR.sendMatrix[index];
        }
    }

    return 0;
}

void Mixer::Card_SetChannels(uint8_t channelparamter) {
    adda->SetCard_XUSB_NumberOfChannels(channelparamter);
}

void Mixer::Card_SendCommand(String command) {
    adda->Card_SendCommand(command);
}

void Mixer::DebugPrintvChannels(void){
	for (int i = 0; i < MAX_VCHANNELS; i++)
	{
	   printf(GetVChannel(i)->ToString().c_str());
	}
}
