#ifndef __SPI_H__
#define __SPI_H__

#include "dsp1.h"
#include "comm.h"

extern float pm spiCommData[SPI_DMA_COMMDATA_SIZE];

extern volatile bool spiNewRxDataReady;

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

void spiInit(void);
void spiCallback(void);
void spiCoreRxBegin(void);
void spiDmaBegin(unsigned int* buffer, int len, bool receive);
void spiDmaEnd(void);
void spiISR(int sig);
void spiProcessRxData(void);

#endif
