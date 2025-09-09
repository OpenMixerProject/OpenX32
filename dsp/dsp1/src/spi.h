#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include "dsp1.h"

void spiInit(void);
void spiStop(void);
void spiISR(int sig);
void spiDmaBegin(bool receive, int len);
void spiDmaEnd(void);
void spiProcessRxData(void);
//unsigned int spiRxTx(unsigned int data);

#endif
