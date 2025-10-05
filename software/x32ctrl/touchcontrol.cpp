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

#include "touchcontrol.h"


// ####################################################################
// #
// #
// #        Touchcontrol
// #
// #
// ####################################################################

void touchcontrolTick(void){
    if (mixer->touchcontrol.value > 0) {
        mixer->touchcontrol.value--;
        if (mixer->touchcontrol.value == 0)
        {
            // trigger sync for this channel
            mixer->SetVChannelChangeFlagsFromIndex(
                mixer->GetvChannelIndexFromButtonOrFaderIndex(mixer->touchcontrol.board, mixer->touchcontrol.faderIndex),
                X32_VCHANNEL_CHANGED_VOLUME
            );
        }
        x32debug("TouchControl=%d\n", mixer->touchcontrol.value);
    }
}

bool touchcontrolCanSetFader(X32_BOARD p_board, uint8_t p_faderIndex) {
    if ((mixer->touchcontrol.board != p_board) && (mixer->touchcontrol.faderIndex != p_faderIndex)){
        return true;
    } 

    if (mixer->touchcontrol.value == 0){
        return true;
    }

    return false;
}
