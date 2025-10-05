#pragma once

#include "x32ctrl.h"

#define FPGA_MAX_PACKET_LENGTH 32
#define FPGA_PACKET_LENGTH 8 // *xxxxSS#
#define FPGA_IDX_ROUTING   0    // reserve 200 slots
#define FPGA_IDX_CH_VOL    200  // here we need 40 values here

extern char fpgaBufferUart[256];

void fpgaRoutingInit(void);
void fpgaRoutingDefaultConfig(void);

void fpgaRoutingSetOutputSource(uint8_t group, uint8_t channel, uint8_t inputsource);
void fpgaRoutingSetOutputSourceByIndex(uint8_t outputIndex, uint8_t inputsource);
uint8_t fpgaRoutingGetOutputSource(uint8_t group, uint8_t channel);
uint8_t fpgaRoutingGetOutputSourceByIndex(uint8_t outputIndex);

uint8_t fpgaRoutingGetSourceIndex(uint8_t group, uint8_t channel);
uint8_t fpgaRoutingGetOutputIndex(uint8_t group, uint8_t channel);
void fpgaRoutingGetSourceGroupAndChannelByIndex(uint8_t sourceIndex, uint8_t* group, uint8_t* channel);
void fpgaRoutingGetOutputGroupAndChannelByIndex(uint8_t outputIndex, uint8_t* group, uint8_t* channel);
void fpgaRoutingGetSourceGroupAndChannelByDspChannel(uint8_t dspChannel, uint8_t* group, uint8_t* channel);

void fpgaRoutingGetSourceName(char* p_nameBuffer, uint8_t group, uint8_t channel);
void fpgaRoutingGetSourceNameByIndex(char* p_nameBuffer, uint8_t sourceIndex);
void fpgaRoutingGetOutputName(char* p_nameBuffer, uint8_t group, VChannel p_chan);
void fpgaRoutingGetOutputNameByIndex(char* p_nameBuffer, uint8_t index);

void fpgaRoutingSendConfigToFpga(void);
void fpgaProcessUartData(int bytesToProcess);


