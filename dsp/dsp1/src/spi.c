/*
    ____                  __   ______ ___
   / __ \                 \ \ / /___ \__ \
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / /
  | |__| | |_) |  __/ | | |/ . \ ___) / /_
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |
         |_|

  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "spi.h"

unsigned int spiDmaBuffer[SPI_DMA_BUFFER_SIZE];
bool spiDmaMode = false;

volatile sSpiRxRingBuffer spiRxRingBuffer;
volatile sSpiTxRingBuffer spiTxRingBuffer;
volatile bool spiNewRxDataReady = false;

typedef enum {
	LOOKING_FOR_START_MARKER,
	COLLECTING_PAYLOAD,
	LOOKING_FOR_END_MARKER
} spiParserState;
const unsigned int SPI_START_MARKER = 0x0000002A; // '*'
const unsigned int SPI_END_MARKER = 0x00000023; // '#'

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

		// send tx-buffer
		if (spiRxRingBuffer.head != spiRxRingBuffer.tail) {
			*pTXSPI = spiTxRingBuffer.buffer[spiTxRingBuffer.tail];
			spiTxRingBuffer.tail += 1;
			if (spiTxRingBuffer.tail > SPI_TX_BUFFER_SIZE) {
				spiTxRingBuffer.tail -= SPI_TX_BUFFER_SIZE;
			}
		}else{
			// tx-buffer is empty
			*pTXSPI = 0x00000000;
		}

		// check for buffer-overflow
		int next_head = (spiRxRingBuffer.head + 1);
		if (next_head > SPI_RX_BUFFER_SIZE) {
			next_head -= SPI_RX_BUFFER_SIZE;
		}
		if (next_head != spiRxRingBuffer.tail) {
			// no overflow -> store data
			spiRxRingBuffer.buffer[spiRxRingBuffer.head] = rxData;
			spiRxRingBuffer.head = next_head;
			spiNewRxDataReady = (rxData == 0x00000023); // check for '#'
		}else{
			// buffer-overflow -> reject new data
		}
	}
}

void spiProcessRxData(void) {
	spiNewRxDataReady = false;

	// check for new valid data in spiRxBuffer
	// we expect a message like:
	// *LPV#
	// * LENGTH PARAMETER VALUE-ARRAY # (each with 32-bit)

	static spiParserState state = LOOKING_FOR_START_MARKER;
	static unsigned int payload[SPI_MAX_RX_PAYLOAD_SIZE];
	static int payloadIdx = 0;
	static unsigned short payloadLength;

	while (spiRxRingBuffer.head != spiRxRingBuffer.tail) {
		unsigned int data = spiRxRingBuffer.buffer[spiRxRingBuffer.tail];
		spiRxRingBuffer.tail += 1;
		if (spiRxRingBuffer.tail > SPI_RX_BUFFER_SIZE) {
			spiRxRingBuffer.tail -= SPI_RX_BUFFER_SIZE;
		}

		switch (state) {
			case LOOKING_FOR_START_MARKER:
				// check for character '*'
				if (data == SPI_START_MARKER) {
					payloadIdx = 0;
					state = COLLECTING_PAYLOAD;
				}
				break;
			case COLLECTING_PAYLOAD:
				if (payloadIdx == 0) {
					// the current data contains the expected message length
					payloadLength = ((data & 0xFF000000) >> 24) + 1; // '*'   parameter  values       '#'
				}

				// read data
				payload[payloadIdx++] = data;

				if ((payloadIdx == payloadLength) || (payloadIdx == SPI_MAX_RX_PAYLOAD_SIZE)) {
					// payload is complete. Now check the end marker
					state = LOOKING_FOR_END_MARKER;
				}
				break;
			case LOOKING_FOR_END_MARKER:
				// check for character '#'
				if (data == SPI_END_MARKER) {
					// we received a valid payload
					unsigned short classId = payload[0] & 0x000000FF;
					unsigned short channel = ((payload[0] & 0x0000FF00) >> 8);
					unsigned short index = ((payload[0] & 0x00FF0000) >> 16);
					unsigned short valueCount = ((payload[0] & 0xFF000000) >> 24);
					openx32Command(classId, channel, index, valueCount, &payload[1]); // classId, channel, index, valuecount, values[]

					state = LOOKING_FOR_START_MARKER; // reset state
				}else{
					// error: end-marker not found
					state = LOOKING_FOR_START_MARKER; // reset state anyway
				}
				break;
		}
	}
}

void spiPushValueToTxBuffer(unsigned int value) {
	// check for buffer-overflow
	int next_head = spiTxRingBuffer.head + 1;
	if (next_head > SPI_TX_BUFFER_SIZE) {
		next_head -= SPI_TX_BUFFER_SIZE;
	}
	if (next_head != spiTxRingBuffer.tail) {
		// no overlow -> store data
		spiTxRingBuffer.buffer[spiTxRingBuffer.head] = value;
		spiTxRingBuffer.head = next_head;
	}else{
		// buffer-overflow -> reject new data
	}
}

void spiSendArray(unsigned short classId, unsigned short channel, unsigned short index, unsigned short valueCount, void* values) {
	spiPushValueToTxBuffer(0x00000023); // StartMarker = '*'
	unsigned int parameter = ((unsigned int)valueCount << 24) + ((unsigned int)index << 16) + ((unsigned int)channel << 8) + (unsigned int)classId;
	spiPushValueToTxBuffer(parameter);

	for (int i = 0; i < valueCount; i++) {
		spiPushValueToTxBuffer(((unsigned int*)values)[i]);
	}
	spiPushValueToTxBuffer(0x00000023); // EndMarker = '#'
}

void spiSendValue(unsigned short classId, unsigned short channel, unsigned short index, unsigned int value) {
	spiSendArray(classId, channel, index, 1, &value);
}

/*
// SPI-Master Read/Transmit
unsigned int spiMasterRxTx(unsigned int data) {
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
