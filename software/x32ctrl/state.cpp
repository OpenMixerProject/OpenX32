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

State::State(Helper* h){
    helper = h;
}

void State::SetChangeFlags(x32_changeflag p_flag){
    helper->DEBUG_STATE(DEBUGLEVEL_TRACE, "SetChangeFlags(0x%.4X)", p_flag);
    changed |= p_flag;
}

bool State::HasAnyChanged(void){
    if (changed != X32_MIXER_CHANGED_NONE) {
        helper->DEBUG_STATE(DEBUGLEVEL_TRACE, "HasAnyChanged() -> 0x%.4X", changed);
        return (changed != X32_MIXER_CHANGED_NONE);
    }
    return false;
}

bool State::HasChanged(x32_changeflag p_flag){
    return ((changed & p_flag) == p_flag);
}

void State::ResetChangeFlags(void){
    helper->DEBUG_STATE(DEBUGLEVEL_TRACE, "ResetChangeFlags()");
    changed = X32_MIXER_CHANGED_NONE;
}