#pragma once

#include <chrono>
#include <stdint.h>
#include "defines.h"
#include "WString.h"
#include "helper.h"

// Class for holding only "temporary" state data during runtime
class State {

    public:

        bool bodyless;

        // Surface

        // currently pressed button
        X32_BTN buttonPressed = X32_BTN_NONE;
        // second button pressed, while first button is also pressed
        X32_BTN secondbuttonPressed = X32_BTN_NONE;

        uint8_t activeBank_inputFader = 0;
        uint8_t activeBank_busFader = 0;
        uint8_t activeBusSend = 0;

        uint8_t activeEQ = 0;

        // DSPs

        float dspLoad[2];
        float dspFreeHeapWords[2];
        float dspVersion[2];

        X32_PAGE lastPage = X32_PAGE::HOME;


        bool x32core_lcdmode_setup;
        uint8_t ledbrightness = LED_BRIGHTNESS_4;

        // DEBUG
        uint8_t debugvalue = 0;
        uint8_t debugvalue2 = 0;
        bool dsp_disable_activity_light = false;
        bool dsp_disable_readout = false;
        bool surface_disable_lcd_update = false;

        uint simulator_last_key = 0;
        // \DEBUG

        uint32_t fpga_spi_speed = 0;
        uint32_t dsp_spi_config_speed = 0;
        uint32_t dsp_spi_speed = 0;


        // old - only dummy, so that code compiles ;-)

        void SetChangeFlags(uint16_t p_flag){};
};
