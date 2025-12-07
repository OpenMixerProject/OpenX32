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

#include "vchannel.h"

 VChannel::VChannel(X32BaseParameter* basepar) : X32Base(basepar) {
    name = String();
    color = 0;
    icon = 0;
    selected = false;
    vChannelType = X32_VCHANNELTYPE_NONE;
}

void VChannel::SetChanged(uint16_t p_flag){
    changed |= p_flag;
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void VChannel::ResetVChannelChangeFlags(){
    changed = X32_VCHANNEL_CHANGED_NONE;
}

bool VChannel::HasChanged(uint16_t p_flag){
    return ((changed & p_flag) == p_flag); 
}

bool VChannel::HasAnyChanged(void){
    return changed != X32_VCHANNEL_CHANGED_NONE;
}

// change the input of the selected vchannel
void VChannel::ChangeInput(int8_t amount){

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

    helper->DEBUG_VCHANNEL(DEBUGLEVEL_NORMAL, "mixerChangeVChannel(): dspChannel=%d", dspChannel->inputSource);

    int16_t newValue = (int16_t)dspChannel->inputSource + amount;

  //  if (newValue > 68) {
    if (newValue > NUM_DSP_CHANNEL) {
        newValue = 0;
    }
    if (newValue < 0) {
        newValue = 68;
    }
    
   // dspSetInputRouting(chan->index);
    

    //mixer.dsp->dspChannel[chan->index].inputSource = newValue; // OFF / In1-32 / AUX 1-8 / BUS1-16
    dspChannel->inputSource = newValue; // 32 in + 8 aux + 8 FX return + 16 bus

    // if (chan.dspChannel.inputSource == 0) {
    //     // OFF: not supported yet
    // }else if ((chan.dspChannel.inputSource >= 1) && (chan.dspChannel.inputSource <= 40)) {
    //     // inputs
    //     dsp->dspChannel[chan.index] = 25 + chan.dspChannel.inputSource - 1;
    // }else if ((chan.dspChannel.inputSource >= 41) && (chan.dspChannel.inputSource <= 48)) {
    //     // FX1-FX8
    // }else if ((chan.dspChannel.inputSource >= 49) && (chan.dspChannel.inputSource <= 64)) {
    //     // MixBus 1-16
    //     dsp->dspChannel[chan.index] = 3 + chan.dspChannel.inputSource - 1;
    // }
    SetChanged(X32_VCHANNEL_CHANGED_INPUT);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

String VChannel::ToString(void){
    String s = String("Name: ") + name;
    s += String(" Color: ") + String(color);
    s += String(" Icon: ") + String(icon);
    s += String(" Selected: ") + String(selected);
    s += String(" vChannelType: ") + String(vChannelType);
    s += "\n";
    return s;
}