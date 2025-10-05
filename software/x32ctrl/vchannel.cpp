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

void VChannel::Init(int p_vChannelIndex, bool p_disabled){
    index = p_vChannelIndex;
    name = String("Kanal ") + String(p_vChannelIndex + 1);
    if (p_disabled){
        color = 0;
    } else {
        //p_vChannel->color = SURFACE_COLOR_WHITE;
        color = SURFACE_COLOR_YELLOW;
    }
    icon = 0;
    selected = false;
    solo = false;
    mute = false;
    volumeLR = VOLUME_MIN;
    volumeSub = VOLUME_MIN;
    balance = 0; // center
    vChannelType = 0; // normal channel

    // disable all audio
    dspChannel.inputSource = 0;
}

void VChannel::SetChanged(uint16_t p_flag){
    changed |= p_flag;
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