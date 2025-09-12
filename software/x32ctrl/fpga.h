#ifndef FPGA_H_
#define FPGA_H_

#include "x32ctrl.h"

#define FPGA_MAX_PACKET_LENGTH 32
#define FPGA_PACKET_LENGTH 8 // *xxxxSS#
#define FPGA_IDX_ROUTING   0    // reserve 200 slots
#define FPGA_IDX_CH_VOL    200  // here we need 40 values here

extern char fpgaBufferUart[256];

void fpgaProcessUartData(int bytesToProcess);

#endif
