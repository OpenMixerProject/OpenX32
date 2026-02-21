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

#include "card.h"

Card::Card(X32BaseParameter* basepar, Adda* _adda) : X32Base(basepar) {
    this->adda = _adda;
}

void Card::Init() {
    // TODO - move from ADDA Class
}

void Card::Sync() {
    if (config->HasParameterChanged(MP_ID::CARD_NUMBER_OF_CHANNELS)) {
        XUSB_SetNumberOfChannels(config->GetUint(MP_ID::CARD_NUMBER_OF_CHANNELS));
    }
}


void Card::Card_SendCommand(String command){
	adda->SendReceive(command);
}

// probed on "X-USB:A:3" card
//
// channelparameter (in / out)
// 0 -> 32 / 32
// 1 -> 16 / 16
// 2 -> 32 / 8
// 3 -> 8 / 32
// 4 -> 8 / 8
// 5 -> 2 / 2
//
void Card::XUSB_SetNumberOfChannels(uint8_t channelparameter) {
	adda->SendReceive(String("*8C8") + String(channelparameter) + String("#"));
}

void Card::XLIVE_SetNumberOfChannelAndMode(uint8_t channelparameter, bool usb) {

	// "X-USB:A:3"
	//
	// channelparameter - in / out
	// 0 - 32 / 32
	// 1 - 16 / 16
	// 2 - 32 / 8
	// 3 - 8 / 32
	// 4 - 8 / 8
	// 5 - 2 / 2


	String command = "*8C8" + String(channelparameter);

	// only X-Live
	if (usb) {
		command += "U";
	} else {
		command += "C"; // Card
	}
	
	command += "#";

	adda->SendReceive(command);
}