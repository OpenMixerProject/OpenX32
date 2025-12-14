#pragma once

#include <stdint.h>
#include "defines.h"
#include "WString.h"
#include "helper.h"

// Class for holding only "temporary" state data during runtime
class State {
    private:
        // something was changed - sync surface/gui to mixer state
        uint16_t changed;
        Helper* helper;

    public:
        State(Helper* h);

        float dspLoad[2];
        float dspVersion[2];

        bool page_routing_fpga_table_drawn = false;
        bool page_routing_dsp1_table_drawn = false;
        bool page_routing_dsp2_table_drawn = false;
        int16_t gui_old_selected_item = 0;
        int16_t gui_selected_item = 0;

        X32_PAGE activePage;

        // DEBUG
        uint8_t debugvalue = 0;
        bool dsp_disable_activity_light = false;
        bool dsp_disable_readout = false;
        bool surface_disable_lcd_update = false;

        uint32_t fpga_spi_speed = 0;
        uint32_t dsp_spi_config_speed = 0;
        uint32_t dsp_spi_speed = 0;

        void SetChangeFlags(uint16_t p_flag);
        bool HasChanged(uint16_t p_flag);
        bool HasAnyChanged(void);
        void ResetChangeFlags(void);
};
