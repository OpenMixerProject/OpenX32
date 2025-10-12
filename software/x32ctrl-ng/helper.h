#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "config.h"

class Helper{

    Config* config;

    public:
        Helper(Config* config);
        void Log(const char* format, ...);
        void SetDebug(int value);
        void Debug(const char* format, ...);
        unsigned int Checksum(char* str);
        int ReadConfig(const char* filename, const char* key, char* value_buffer, size_t buffer_size);
};