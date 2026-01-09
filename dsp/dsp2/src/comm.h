#ifndef __COMM_H__
#define __COMM_H__

#include "dsp2.h"
#include "spi.h"

// function prototypes
void commExecCommand(unsigned short classId, unsigned short channel, unsigned short index, unsigned short valueCount, void* values);
#endif
