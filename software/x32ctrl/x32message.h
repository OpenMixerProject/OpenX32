#pragma once

#include "x32ctrl_types.h"

class X32Message{

};

class X32SurfaceMessage : public X32Message{
    public: 
        X32_BOARD receivedBoardId;
        uint8_t receivedClass;
        uint8_t receivedIndex;
        uint8_t receivedValue;
};

class X32AddaMessage : public X32Message{

};