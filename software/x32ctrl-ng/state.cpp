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

#include "state.h"


// ####################################################################
// #
// #
// #        Changeflags
// #
// #
// ####################################################################


void State::SetChangeFlags(uint16_t p_flag){
    changed |= p_flag;
}

bool State::HasAnyChanged(void){
    return (changed != X32_MIXER_CHANGED_NONE);
}

bool State::HasChanged(uint16_t p_flag){
    return ((changed & p_flag) == p_flag);
}

void State::ResetChangeFlags(void){
    changed = X32_MIXER_CHANGED_NONE;
    // for (uint8_t i=0;i<MAX_VCHANNELS;i++){
    //     ResetVChannelChangeFlags(vchannel[i]);
    // }
}