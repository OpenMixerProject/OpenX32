#pragma once

#include <stdint.h>

class SurfaceFaderblock{
    public:
        uint8_t _boardId;
        uint8_t _index;

        SurfaceFaderblock(uint8_t boardId, uint8_t index);
};
