#include "spi.h"

unsigned int spiDmaBuffer[SPI_DMA_BUFFER_SIZE];
bool spiDmaMode = false;

// variables and types for SPI-transmitter in Slave-Mode
typedef struct {
	unsigned int buffer[SPI_BUFFER_SIZE];
	volatile int head; // write-pointer
	volatile int tail; // read-pointer
} sSpiRingBuffer;
volatile sSpiRingBuffer spiRingBuffer;
volatile bool spiNewRxData = false;

typedef enum {
	LOOKING_FOR_START_MARKER,
	COLLECTING_PAYLOAD,
	LOOKING_FOR_END_MARKER
} spiParserState;
const unsigned int SPI_START_MARKER = 0x0000002A;
const unsigned int SPI_END_MARKER = 0x00000023;

void spiInit(void) {
	// reset all registers
	*pSPIFLG = 0; // flag-register
	*pSPICTL = 0; // spi-control-register
	*pSPIDMAC = 0; // spi-dma-register

	// configure SPI interface as Slave
	*pSPIFLG = DS0EN; // Enable SRU2 output for SPI device-select-0

	// Start SPI in CoreWrite-TransferMode (Init Transfer by read of receive-buffer, ISR when buffer is full)
	*pSPICTL = ISSEN | MSBF | WL32 | SPIEN; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit | spi enabled
}

void spiStop(void) {
	// stop and flush SPI
	*pSPIDMAC = 0;
	*pSPICTL = (TXFLSH | RXFLSH);
    while (!(SPIF & *pSPISTAT)) {
       NOP();
    }
    while (!(SPIFE & *pSPISTAT)) {
       NOP();
    }
}

void spiDmaBegin(bool receive, int len) {
	// stop and flush SPI
	*pSPICTL &= ~SPIEN;
	*pSPICTL |= (TXFLSH | RXFLSH);
    while (!(SPIF & *pSPISTAT)) {
       NOP();
    }
    while (!(SPIFE & *pSPISTAT)) {
       NOP();
    }

	// reconfigure for DMA-TransferMode
	*pIISPI = (unsigned int)&spiDmaBuffer[0]; // Internal memory DMA address
	*pIMSPI = 1; // Internal memory DMA access modifier
	*pCSPI = len; // Contains number of DMA transfers remaining data
	*pSPICTL = ISSEN | MSBF | WL32 | TIMOD2 | SPIEN; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit | DMA-TransferMode | spi enabled
	*pSPIDMAC = SPIDEN | INTEN; // DMA enabled | Interrupts enabled
	if (receive) {
		*pSPIDMAC |= SPIRCV;
	}

	spiDmaMode = true;
}

void spiDmaEnd(void) {
	// stop and flush SPI
	*pSPICTL &= ~SPIEN;
	*pSPICTL |= (TXFLSH | RXFLSH);
    while (!(SPIF & *pSPISTAT)) {
       NOP();
    }
    while (!(SPIFE & *pSPISTAT)) {
       NOP();
    }

	// reconfigure for CoreWrite-TransferMode (Init Transfer by read of receive-buffer, ISR when buffer is full)
	*pSPIDMAC = 0;
	*pSPICTL = ISSEN | MSBF | WL32 | SPIEN; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit | spi enabled

	spiDmaMode = false;
}

void spiISR(int sig) {
	// this interrupt is called either when the DMA transfer to SPI Master is completed
	// or when data is available via CoreWrite-Mode

	if (spiDmaMode) {
		// end of last DMA-Transmission
		spiDmaEnd();
	}else{
		// valid data in RXSPI -> add to receive-buffer
		unsigned int rxData = *pRXSPI;

		// TEST: mirror the received byte back to the SPI-Master
		*pTXSPI = rxData;

		// check for buffer-overflow
		int next_head = (spiRingBuffer.head + 1) % SPI_BUFFER_SIZE;
		if (next_head != spiRingBuffer.tail) {
			// no overflow -> store data
			spiRingBuffer.buffer[spiRingBuffer.head] = rxData;
			spiRingBuffer.head = next_head;

			spiNewRxData = (rxData == 0x00000023); // check for '#'
		}else{
			// buffer-overflow -> reject new data
		}
	}
}

void spiProcessRxData(void) {
	spiNewRxData = false;

	// check for new valid data in spiRxBuffer
	// we expect a message like:
	// *PV#
	// * PARAMETER VALUE # (each with 32-bit)

	static spiParserState state = LOOKING_FOR_START_MARKER;
	static unsigned int payload[SPI_PAYLOAD_SIZE];
	static int payloadIdx = 0;

	while (spiRingBuffer.head != spiRingBuffer.tail) {
		unsigned int data = spiRingBuffer.buffer[spiRingBuffer.tail];
		spiRingBuffer.tail = (spiRingBuffer.tail + 1) % SPI_BUFFER_SIZE;

		switch (state) {
			case LOOKING_FOR_START_MARKER:
				// check for character '*'
				if (data == SPI_START_MARKER) {
					state = COLLECTING_PAYLOAD;
					payloadIdx = 0;
				}
				break;
			case COLLECTING_PAYLOAD:
				// read data
				payload[payloadIdx++] = data;
				if (payloadIdx == SPI_PAYLOAD_SIZE) {
					// payload is complete. Now check the end marker
					state = LOOKING_FOR_END_MARKER;
				}
				break;
			case LOOKING_FOR_END_MARKER:
				// check for character '#'
				if (data == SPI_END_MARKER) {
					// we received a valid payload
					openx32Command(payload[0], payload[1]); // parameter, value

					state = LOOKING_FOR_START_MARKER; // reset state
				}else{
					// error: end-marker not found
					state = LOOKING_FOR_START_MARKER; // reset state anyway
				}
				break;
		}
	}

	// read data via DMA
	// spiDmaBegin(true, 20);

	// send data via DMA
	// spiDmaBegin(false, 20);
}

/*
// SPI-Master Read/Transmit
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
*/
