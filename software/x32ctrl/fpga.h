#pragma once

#include <stdint.h>
#include "WString.h"
#include "base.h"
#include "uart.h"
#include "message-base.h"
#include "spi.h"

// includes for UDP-communication
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#define FPGA_MAX_PACKET_LENGTH 32
#define FPGA_PACKET_LENGTH 8 // *xxxxSS#
#define FPGA_IDX_ROUTING   0    // reserve 200 slots
#define FPGA_IDX_CH_VOL    200  // here we need 40 values here
#define FPGA_UART_BUFFER_SIZE  450 // slightly more than the longest message (408 bytes)

class Fpga : public X32Base
{
    using enum MP_ID;

    public:

        SPI* spi;

        Fpga(X32BaseParameter* basepar);
        void Init();

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

        bool GetConfigBit(uint8_t bitNumber);
        void SetConfigBit(uint8_t bitNumber, bool value);
        void SendConfig(void);
    
        void AES50Receive(void);
        void AES50Send(char* data, uint len);

        void AES50Tick(X32Config* config);
        void AES50SendDeviceTypeAndProperty(void);
        void AES50SendNames(void);
        void AES50SendHeadampMessage(X32Config* config);

    private:
        Uart* uart;
        char fpgaRxBufferUart[FPGA_UART_BUFFER_SIZE];
        char fpgaTxBufferUart[FPGA_UART_BUFFER_SIZE];
        uint8_t configData;
        uint8_t AES50Counter;
        
        bool AES50CalcChecksum(char* buf, bool insertChecksum);
        int wrapRingBufferIndex(int index, int bufLen);
};