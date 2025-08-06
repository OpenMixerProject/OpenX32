#ifndef FPGA_H_
#define FPGA_H_

#include "x32ctrl.h"

#define FPGA_MAX_PACKET_LENGTH 32
#define FPGA_PACKET_LENGTH 8

extern char uartBufferFpga[256];

void fpgaProcessUartData(int bytesToProcess);

#endif
