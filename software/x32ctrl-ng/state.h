#pragma once

#include <stdint.h>
#include "constants.h"

class State {
    private:
        // something was changed - sync surface/gui to mixer state
        uint16_t changed;
    public:
        void SetChangeFlags(uint16_t p_flag);
        bool HasChanged(uint16_t p_flag);
        bool HasAnyChanged(void);
        void ResetChangeFlags(void);
};