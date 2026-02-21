#pragma once

#include <stdint.h>
#include "WString.h"
#include "base.h"
#include "spi.h"

#define FPGA_MAX_PACKET_LENGTH 32
#define FPGA_PACKET_LENGTH 8 // *xxxxSS#
#define FPGA_IDX_ROUTING   0    // reserve 200 slots
#define FPGA_IDX_CH_VOL    200  // here we need 40 values here

class Fpga : public X32Base
{
    using enum MP_ID;

    public:

        SPI* spi;

        Fpga(X32BaseParameter* basepar);

        uint8_t GetInputIndex(uint8_t group, uint8_t channel);
        uint8_t GetOutputIndex(uint8_t group, uint8_t channel);
        void GetInputGroupAndChannelByIndex(uint8_t sourceIndex, uint8_t* group, uint8_t* channel);
        void GetOutputGroupAndChannelByIndex(uint8_t outputIndex, uint8_t* group, uint8_t* channel);
        void RoutingGetSourceGroupAndChannelByDspChannel(uint8_t dspChannel, uint8_t* group, uint8_t* channel);

        String GetInputName(uint8_t group, uint8_t channel);
        String GetInputNameByIndex(uint8_t sourceIndex);
        String GetOutputName(uint8_t group, uint8_t p_chan);
        String GetOutputNameByIndex(uint8_t index);

        void SendRoutingToFpga(int channel);
};