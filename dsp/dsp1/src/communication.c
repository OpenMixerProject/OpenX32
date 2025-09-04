#include "communication.h"

unsigned int spiSourceData[20];
unsigned int spiRxData[20];

void spiInit(void) {
	// write dummy data to register
	for(int i = 0; i < 20; i++ ) {
		spiSourceData[i] = i;
	}

	// reset all registers
	*pSPIFLG = 0; // flag-register
	*pSPICTL = 0; // spi-control-register
	*pSPIDMAC = 0; // spi-dma-register

	// configure SPI interface as Slave
	*pSPIBAUD = 0x02;
	*pIISPI = &spiSourceData[0]; // Internal memory DMA address
	*pIMSPI = 1; // Internal memory DMA access modifier
	*pCSPI = 20; // Contains number of DMA transfers remaining data

	*pSPIFLG = DS0EN; // Enable SRU2 output for SPI device-select-0
	*pSPICTL = ISSEN | MSBF | WL32 | TIMOD2 | SPIEN; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit | DMA-TransferMode | spi enabled
	*pSPIDMAC = SPIDEN | INTEN; // DMA enabled | Interrupts enabled
}

void spiStop(void) {
	*pSPICTL = (TXFLSH | RXFLSH);
}

void spiTxISR(int sig) {
	// this interrupt is called when the DMA transfer to SPI Master is completed

	// copy the received register-address and preload data for next read
	*pIISPI = &spiSourceData[0];
	*pIMSPI = 1;
	*pCSPI = 20;
}

void spiRxISR(int sig) {
}

unsigned int spiRxTx(unsigned int data) {
	*pTXSPI = data;

	// wait for SPI to finish transmission
	delay(100);
    while (!(SPIF & *pSPISTAT)) {
       NOP();
    }
    while (!(SPIFE & *pSPISTAT)) {
       NOP();
    }
    return *pRXSPI;
}
