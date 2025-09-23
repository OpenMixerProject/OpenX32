#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include "dsp1.h"

// variables and types for SPI-transmitter in Slave-Mode
typedef struct {
	unsigned int buffer[SPI_RX_BUFFER_SIZE];
	volatile int head; // write-pointer
	volatile int tail; // read-pointer
} sSpiRxRingBuffer;
typedef struct {
	unsigned int buffer[SPI_TX_BUFFER_SIZE];
	volatile int head; // write-pointer
	volatile int tail; // read-pointer
} sSpiTxRingBuffer;

extern volatile sSpiTxRingBuffer spiTxRingBuffer;

void spiInit(void);
void spiStop(void);
void spiISR(int sig);
void spiDmaBegin(bool receive, int len);
void spiDmaEnd(void);
void spiProcessRxData(void);
void spiPushValueToTxBuffer(unsigned int value);
void spiSendArray(unsigned short classId, unsigned short channel, unsigned short index, unsigned short valueCount, void* values);
void spiSendValue(unsigned short classId, unsigned short channel, unsigned short index, float value);
void spiSendValue_uint32(unsigned short classId, unsigned short channel, unsigned short index, unsigned int value);
//unsigned int spiMasterRxTx(unsigned int data);

#endif
