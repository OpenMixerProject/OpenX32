#pragma once

#include <stdint.h>
#include "base.h"
#include "spi.h"

#define FPGA_MAX_PACKET_LENGTH 32
#define FPGA_PACKET_LENGTH 8 // *xxxxSS#
#define FPGA_IDX_ROUTING   0    // reserve 200 slots
#define FPGA_IDX_CH_VOL    200  // here we need 40 values here

class Fpga : public X32Base {
    public:
        sFpgaRouting fpgaRouting;

        SPI* spi;

        Fpga(X32BaseParameter* basepar);

        void Init(void);
        void RoutingDefaultConfig(void);

        void RoutingSetOutputSource(uint8_t group, uint8_t channel, uint8_t inputsource);
        void RoutingSetOutputSourceByIndex(uint8_t outputIndex, uint8_t inputsource);
        uint8_t RoutingGetOutputSource(uint8_t group, uint8_t channel);
        uint8_t RoutingGetOutputSourceByIndex(uint8_t outputIndex);

        uint8_t RoutingGetSourceIndex(uint8_t group, uint8_t channel);
        uint8_t RoutingGetOutputIndex(uint8_t group, uint8_t channel);
        void RoutingGetSourceGroupAndChannelByIndex(uint8_t sourceIndex, uint8_t* group, uint8_t* channel);
        void RoutingGetOutputGroupAndChannelByIndex(uint8_t outputIndex, uint8_t* group, uint8_t* channel);
        void RoutingGetSourceGroupAndChannelByDspChannel(uint8_t dspChannel, uint8_t* group, uint8_t* channel);

        void RoutingGetSourceName(char* p_nameBuffer, uint8_t group, uint8_t channel);
        void RoutingGetSourceNameByIndex(char* p_nameBuffer, uint8_t sourceIndex);
        void RoutingGetOutputName(char* p_nameBuffer, uint8_t group, uint8_t p_chan);
        void RoutingGetOutputNameByIndex(char* p_nameBuffer, uint8_t index);

        void RoutingSendConfigToFpga(int16_t channel);
};