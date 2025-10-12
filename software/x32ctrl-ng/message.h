#pragma once

#include <stdio.h>
#include <string.h>

#include "constants.h"

class Message{
    public:
        char buffer[MAX_MESSAGE_SIZE];
        size_t current_length;

        Message();

        int AddRawByte(char byte);
        int AddDataByte(char byte);
        int AddString(const char* str);
        int AddDataArray(const char* data, uint8_t len);
};