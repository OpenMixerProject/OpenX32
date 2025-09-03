#include "communication.h"

void spiInit(void) {
	*pSPICTL = (TXFLSH | RXFLSH);
	*pSPIFLG = 0; // clear register
	*pSPICTL = 0; // clear register
	*pSPIBAUD = 100; // set baudrate to 500kHz
	*pSPIFLG = 0xF708; // setup the SPI flag register to FLAG3 (0xF708)

	// setup SPI control register (0x5281)
	*pSPICTL = (SPIEN | SPIMS | MSBF | WL16 | TIMOD1); // SPI enable | SPI master | MostSignificantByte | Wordlength=16 | core writes instead of DMA
}

void spiStop(void) {
	*pSPICTL = (TXFLSH | RXFLSH);
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
