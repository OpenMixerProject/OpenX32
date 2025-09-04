#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include "dsp1.h"

void spiInit(void);
void spiStop(void);
void spiTxISR(int sig);
void spiRxISR(int sig);
unsigned int spiRxTx(unsigned int data);

#endif
