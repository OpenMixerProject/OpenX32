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
  Copyright 2025-2026 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "fxBase.h"

fx::fx(int fxSlot, int channelMode) {
	// take the memory-slot-address (we have 8 of them)
	_fxSlot = fxSlot;
	_channelMode = channelMode;

	// calculate the base-address in memory for this effect. We have 1MB for each channel
	_memoryAddress = (SDRAM_AUDIO_START + (_fxSlot * (SDRAM_AUDIO_SIZE_BYTE / 8))); // we are using 8 fx-slots so divide by 8

	_sampleRate = 48000.0f;
}

fx::~fx() { }

void fx::setSampleRate(float sampleRate) {
	_sampleRate = sampleRate;
}

/*
void fx::clearMemory() {
	// initialize the memory with zeros
    float* ptr = (float*)_memoryAddress;
    for (int i = 0; i < ((SDRAM_AUDIO_SIZE_BYTE / 8) / sizeof(float)); i++) {
        ptr[i] = 0.0f;
    }
}
*/
