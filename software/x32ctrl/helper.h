#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <map>
#include <math.h>

// includes for reading IP-Address
#include <sys/stat.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "lib/plf_nanotimer.h"

#include "defines.h"
#include "enum.h"
#include "WString.h"


#define DEBUGLEVEL_OFF -1 // No Debug
#define DEBUGLEVEL_NORMAL 0 // General highlevel debug messages
#define DEBUGLEVEL_VERBOSE 1 // Function calls, Elements, ...
#define DEBUGLEVEL_TRACE 2 // Very verbose down to the last bit

using namespace std;

class Helper {
    private:
        uint32_t debug_;
        uint8_t debuglevel_;
        map<uint8_t, plf::nanotimer*> timers;

    public:

        void Log(const char* format, ...);
        void Error(const char* format, ...);
        unsigned int Checksum(char* str);
        int ReadConfig(const char* filename, const char* key, char* value_buffer, size_t buffer_size);
        
        float Fadervalue2dBfs(uint16_t faderValue);
        uint16_t Oscvalue2Fadervalue(float oscValue);
        float Fadervalue2Oscvalue(uint16_t faderValue);
        uint16_t Dbfs2Fader(float dbfsValue);
        float Dbfs2Oscvalue(float dbfsValue);
        float samplePu2Dbfs(float samplePu);
        float sample2Dbfs(uint32_t sample);

        uint8_t value2percent(float value, float value_min, float value_max);
        uint8_t value2percent(uint8_t value, uint8_t value_min, uint8_t value_max);
        uint8_t value2percent(int8_t value, int8_t value_min, int8_t value_max);
        
        long GetFileSize(const char* filename);
        void ReverseBitOrderArray(uint8_t* data, uint32_t len);
        uint32_t ReverseBitOrder_uint32(uint32_t n);
        float Saturate(float value, float min, float max);

        String freq2String(float freq);
        String eqType2String(uint8_t type);

        String getIpAddress();

        bool IsInChannelBlock(uint8_t index, X32_VCHANNEL_BLOCK block);

        vector<string> split(string s, string delimiter);

        void starttimer(uint8_t timer);
        void stoptimer(uint8_t timer, const char* description);


        // Only show debug messages up to DEBUGLEVEL_..., e.g. DEBUGLEVEL_NORMAL
        void SetDebugLevel(uint8_t debuglevel)
        {
            debuglevel_ = debuglevel;
        }

        void SetDebugAll()
        {
            debug_ = 0b1111111111111111;            
        }

        #if BUILD_DEBUG
        #define DEBUG_DEF(name, bitvalue) \
            void name(int debuglevel, const char* format, ...) { \
                if (debuglevel <= debuglevel_ && ((debug_ & bitvalue) == bitvalue)) { \
                    va_list args; \
                    va_start(args, format); \
                    vprintf((String(#name) + String(": ") + String(format) + String("\n")).c_str(), args); \
                    fflush(stdout); \
                    va_end(args); \
                } \
            } \
            \
            /* Check if this Debugflag is enabled */ \
            bool name(int debuglevel=0) { \
                return (debuglevel <= debuglevel_ && ((debug_ & bitvalue) == bitvalue));   \
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
        #else
        #define DEBUG_DEF(name, bitvalue) \
        void name(int debuglevel, const char* format, ...) { } \
        bool name(int debuglevel=0) { return false; } \
        void name(bool enabled) { } 
        #endif

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
        DEBUG_DEF(DEBUG_STATE,    0b0010000000000000);
        DEBUG_DEF(DEBUG_TIMER,    0b0100000000000000);
        DEBUG_DEF(DEBUG_FX,       0b1000000000000000);
};
