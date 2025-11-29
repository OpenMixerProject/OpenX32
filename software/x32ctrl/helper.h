#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <string>
#include <vector>

#include "config.h"

// includes for reading IP-Address
#include <sys/stat.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define DEBUG_MESSAGE(flag, ...) if (config->IsDebug() & config->HasDebugFlag(flag)) {helper->Debug(flag, __VA_ARGS__);}

#define DEBUG_ALL            0b1111111111111111
#define DEBUG_NONE           0b0000000000000000
#define DEBUG_XREMOTE        0b0000000000000001
#define DEBUG_SURFACE        0b0000000000000010
#define DEBUG_VCHANNEL       0b0000000000000100
#define DEBUG_X32CTRL        0b0000000000001000
#define DEBUG_ADDA           0b0000000000010000
#define DEBUG_MIXER          0b0000000000100000
#define DEBUG_GUI            0b0000000001000000
#define DEBUG_SPI            0b0000000010000000
#define DEBUG_DSP1           0b0000000100000000
#define DEBUG_DSP2           0b0000001000000000
#define DEBUG_FPGA           0b0000010000000000
#define DEBUG_UART           0b0000100000000000

class Helper{

    Config* config;

    public:
        Helper();
        Helper(Config* config);
        void Log(const char* format, ...);
        void Error(const char* format, ...);
        bool Debug(uint16_t debugFlag, const char* format, ...);
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
};