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
        uint16_t gui_old_selected_item = 0;
        uint16_t gui_selected_item = 0;

        // DEBUG
        uint8_t debugvalue = 0;
        bool dsp_disable_activity_light = false;
        bool dsp_disable_readout = false;

        void SetChangeFlags(uint16_t p_flag);
        bool HasChanged(uint16_t p_flag);
        bool HasAnyChanged(void);
        void ResetChangeFlags(void);
};
