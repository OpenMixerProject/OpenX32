#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "defines.h"

class MessageBase{
    public:
        MessageBase();

        char buffer[MAX_MESSAGE_SIZE];  // TODO: private
        size_t current_length;  // TODO: private

        int AddRawByte(char byte);
        int AddDataByte(char byte);
        int AddString(const char* str);
        int AddDataArray(const char* data, uint8_t len);
};