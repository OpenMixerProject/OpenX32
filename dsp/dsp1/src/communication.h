#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include "dsp1.h"

void spiInit(void);
void spiStop(void);
unsigned int spiRxTx(unsigned int data);

#endif
