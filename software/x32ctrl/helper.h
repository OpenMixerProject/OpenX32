#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <string>
#include <vector>

// includes for reading IP-Address
#include <sys/stat.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "config.h"

using namespace std;

class Helper {
    private:
        uint32_t debug_;

    public:
        void Log(const char* format, ...);
        void Error(const char* format, ...);
        void DebugPrintMessageWithNullBytes(uint16_t debugFlag, char* message, uint16_t len);
        unsigned int Checksum(char* str);
        int ReadConfig(const char* filename, const char* key, char* value_buffer, size_t buffer_size);
        
        float Fadervalue2dBfs(uint16_t faderValue);
        uint16_t Oscvalue2Fadervalue(float oscValue);
        float Fadervalue2Oscvalue(uint16_t faderValue);
        uint16_t Dbfs2Fader(float dbfsValue);
        float Dbfs2Oscvalue(float dbfsValue);

        long GetFileSize(const char* filename);
        void ReverseBitOrderArray(uint8_t* data, uint32_t len);
        uint32_t ReverseBitOrder_uint32(uint32_t n);
        float Saturate(float value, float min, float max);

        String freq2String(float freq);
        String eqType2String(uint8_t type);

        String getIpAddress();

        bool IsInChannelBlock(uint8_t index, X32_VCHANNEL_BLOCK block);

        vector<string> split(string s, string delimiter);

        #define DEBUG_DEF(name, bitvalue) \
            void name(const char* format, ...) { \
                if ((debug_ & bitvalue) == bitvalue) { \
                    va_list args; \
                    va_start(args, format); \
                    vprintf((String(#name) + String(": ") + String(format) + String("\n")).c_str(), args); \
                    fflush(stdout); \
                    va_end(args); \
                } \
            } \
            \
            /* Check if this Debugflag is enabled */ \
            bool name() { \
                return ((debug_ & bitvalue) == bitvalue);   \
            } \
            \
            /* Enable or Disable Debugflag */ \
            void name(bool enabled) { \
                if(enabled){ \
                    debug_ |= bitvalue; \
                } else { \
                    debug_ &= ~bitvalue; \
                } \
            } 

        DEBUG_DEF(DEBUG_XREMOTE,  0b0000000000000001);
        DEBUG_DEF(DEBUG_SURFACE,  0b0000000000000010);
        DEBUG_DEF(DEBUG_VCHANNEL, 0b0000000000000100);
        DEBUG_DEF(DEBUG_X32CTRL,  0b0000000000001000);
        DEBUG_DEF(DEBUG_ADDA,     0b0000000000010000);
        DEBUG_DEF(DEBUG_MIXER,    0b0000000000100000);
        DEBUG_DEF(DEBUG_GUI,      0b0000000001000000);
        DEBUG_DEF(DEBUG_SPI,      0b0000000010000000);
        DEBUG_DEF(DEBUG_DSP1,     0b0000000100000000);
        DEBUG_DEF(DEBUG_DSP2,     0b0000001000000000);
        DEBUG_DEF(DEBUG_FPGA,     0b0000010000000000);
        DEBUG_DEF(DEBUG_UART,     0b0000100000000000);
        DEBUG_DEF(DEBUG_INI,      0b0001000000000000);

};