#pragma once

#include <stdint.h>
#include "constants.h"
#include "WString.h"

class State {
    private:
        // something was changed - sync surface/gui to mixer state
        uint16_t changed;

    public:
        int8_t switchFpga = -1;
        String switchFpgaPath;
        int8_t switchDsp1 = -1;
        String switchDsp1Path;
        int8_t switchDsp2 = -1;
        String switchDsp2Path;
        int8_t switchNoinit = -1;

        // DEBUG
        uint8_t debugvalue = 0;

        void SetChangeFlags(uint16_t p_flag);
        bool HasChanged(uint16_t p_flag);
        bool HasAnyChanged(void);
        void ResetChangeFlags(void);
};