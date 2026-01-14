#pragma once

#include <stdint.h>
#include "WString.h"

class LcdText {
    public:
        // sizeA/B = 0x00 (small) or 0x20 (large)
        uint8_t size;
        // 0-125
        uint8_t x;
        // 0-63
        uint8_t y;
        String text;
};

class LcdIcon{
    public:
        // 0-125
        uint8_t x;
        // 0-63
        uint8_t y;
        // icon = 0xA0 (none), 0xA1 ... 0xE9
        uint8_t icon;
};

class LcdData {
    public:
        uint8_t boardId;
        uint8_t lcdIndex;
        // 4 bits (bit 0=R, 1=G, 2=B, 3=Inverted)
        // 
        uint8_t color;
        LcdIcon icon;
        // text(s) to display, max 7
        LcdText texts[7];
};



