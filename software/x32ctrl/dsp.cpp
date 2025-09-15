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

#include "dsp.h"
#include "mixer.h"

sDsp dsp;

void dspInit(void) {
    dsp.samplerate = 48000;

    for (uint8_t i = 0; i < 40; i++) {
        dsp.dspChannel[i].gate.threshold = -80; // dB -> no gate
        dsp.dspChannel[i].gate.range = 60; // full range
        dsp.dspChannel[i].gate.attackTime_ms = 10;
        dsp.dspChannel[i].gate.holdTime_ms = 50;
        dsp.dspChannel[i].gate.releaseTime_ms = 250;

        dsp.dspChannel[i].compressor.threshold = 0; // dB -> no compression
        dsp.dspChannel[i].compressor.ratio = 3; // 1:3
        dsp.dspChannel[i].compressor.makeup = 0; // dB
        dsp.dspChannel[i].compressor.attackTime_ms = 10;
        dsp.dspChannel[i].compressor.holdTime_ms = 10;
        dsp.dspChannel[i].compressor.releaseTime_ms = 150;

        for (uint8_t peq = 0; peq < MAX_CHAN_EQS; peq++) {
            dsp.dspChannel[i].peq[peq].type = 1;
            dsp.dspChannel[i].peq[peq].fc = 3000;
            dsp.dspChannel[i].peq[peq].Q = 2.0;
            dsp.dspChannel[i].peq[peq].gain = 0;
        }
    }
}
