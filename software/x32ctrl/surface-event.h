#pragma once

#include <stdint.h>
#include "constants.h"
#include "WString.h"

class SurfaceEvent{
    public:
        X32_BOARD boardId;
        uint8_t classId;
        uint8_t index;
        uint16_t value;

        SurfaceEvent(X32_BOARD boardId, uint8_t classId, uint8_t index, uint16_t value);
        String ToString(void);
};