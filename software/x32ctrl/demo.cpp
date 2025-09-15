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

#include "x32ctrl.h"
#include "surface.h"
#include "mixer.h"

void demoSurface(void) {
    printf("  Setting Button-LEDs for boards 0 and 1...\n");
    setLedByEnum(X32_BTN_TALK_B, 1);
    setLedByEnum(X32_BTN_EQ, 1);
    setLedByEnum(X32_BTN_EQ_HIGH_MID, 1);
    setLedByEnum(X32_BTN_VIEW_EQ, 1);

    printf("  Setting Button-LEDs for boards 4, 5 and 8...\n");
    for (uint8_t i=0; i<=8; i++) {
      // select-LEDs
      setLed(4, 0x20+i, 1); // boardId 4 = faderL
      setLed(5, 0x20+i, 1); // boardId 5 = faderM
      setLed(8, 0x20+i, 1); // boardId 8 = faderR

      // cue leds
      setLed(4, 0x30+i, 1); // boardId 4 = faderL
      setLed(5, 0x30+i, 1); // boardId 5 = faderM
      setLed(8, 0x30+i, 1); // boardId 8 = faderR

      // mute leds
      setLed(4, 0x40+i, 1); // boardId 4 = faderL
      setLed(5, 0x40+i, 1); // boardId 5 = faderM
      setLed(8, 0x40+i, 1); // boardId 8 = faderR

      // random leds
//      setLed(4, 0x00+i, 1); // boardId 4 = faderL
//      setLed(5, 0x00+i, 1); // boardId 5 = faderM
//      setLed(8, 0x00+i, 1); // boardId 8 = faderR
    }

    // fader ranges from 0...8
    printf("  Setting Faders for boards 4, 5 and 8...\n");
    for (uint8_t i=0; i<=8; i++) {
      setFader(4, i, 0x000 + 0xAA*i);
      setFader(5, i, 0x555 + 0xAA*i);
      setFader(8, i, 0xAAA + 0xAA*i);
    }

    // meterLED ranges from 1...9
    printf("  Setting VU-Meters for boards 1, 4, 5 and 8...\n");
    setMeterLed(1, 0, 0b00000011);
    for (uint8_t i=0; i<=7; i++) {
      setMeterLed(4, i, 0b00000011);
      setMeterLed(5, i, 0b00001111);
      setMeterLed(8, i, 0b01111111);
    }

    // preamp, dynamics, meterL, meterR, meterSolo
    setMeterLedMain(0b00000111, 0b11100001, 0x0FFFFF, 0x0FFFFF, 0x000FFF);

    // set some encoders with different options
    for (uint8_t i=0; i<=100; i++) {
      setEncoderRing(0, 0, 0, i, true); // boardId, index, ledMode, ledPct, backlight
      setEncoderRing(0, 1, 1, i, true); // boardId, index, ledMode, ledPct, backlight
      setEncoderRing(0, 2, 2, i, true); // boardId, index, ledMode, ledPct, backlight
      setEncoderRing(0, 3, 3, i, true); // boardId, index, ledMode, ledPct, backlight
      usleep(5000);
    }

    if (!mixerIsModelX32Core()){
        // set display
        printf("  Setting Displays for board 0...\n");
        for (uint8_t i=0; i<4; i++) {
        setLcd(0, i, 7, 70, 8, 0xE2, 0x20, 0, 0, "Board 0", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
        }

        printf("  Setting Displays for boards 4, 5 and 8...\n");
        for (uint8_t i=0; i<=8; i++) {
        setLcd(4, i, 1, 70, 8, 0xE2, 0x20, 0, 0, "Board 1", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
        setLcd(5, i, 2, 70, 8, 0xE2, 0x20, 0, 0, "Board 2", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
        setLcd(8, i, 4, 70, 8, 0xE2, 0x20, 0, 0, "Board 3", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
        }
    }
}
