#pragma once

#include <stdint.h>

class SurfaceFader{
    public:
        uint16_t position_real = 0;
        uint16_t position_wanted = 0;
        uint16_t wait = 0;
        uint16_t setcheck_oldvalue = 0;
        uint16_t setcheck_newvalue = 0;
};
