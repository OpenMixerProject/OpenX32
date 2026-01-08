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

volatile sSpiRxRingBuffer spiRxRingBuffer;
volatile sSpiTxRingBuffer spiTxRingBuffer;
volatile bool spiNewRxDataReady = false;
bool spiDmaMode = false;

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
	// TIMOD0 (no set TIMODx) = The SPI interrupt is latched in every core clock cycle in which the RXSPI buffer has a word in it
	// TIMOD1 = The SPI interrupt is latched in every core clock cycle in which the TXSPI buffer is empty
	*pSPICTL = ISSEN | MSBF | WL32; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit
	*pSPICTL |= CPHASE | CLKPL; // set SPI_MODE_3
	*pSPICTL |= SPIEN; // enable SPI-interface after one clock-cycle

	spiTxRingBuffer.head = 0;
	spiTxRingBuffer.tail = 0;
	spiRxRingBuffer.head = 0;
	spiRxRingBuffer.tail = 0;
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
	/*
	*pSPICTL |= (TXFLSH | RXFLSH);
    while (!(SPIF & *pSPISTAT)) {
       NOP();
    }
    while (!(SPIFE & *pSPISTAT)) {
       NOP();
    }
    */

	// reconfigure SPI for DMA-TransferMode
    if (receive) {
    	*pIISPI = (unsigned int)&spiRxRingBuffer.buffer[0]; // Internal memory DMA address
    	*pCSPI = len; // words to be read via SPI and DMA to buffer
    }else{
    	*pIISPI = (unsigned int)&spiTxRingBuffer.buffer[0]; // Internal memory DMA address
    	*pCSPI = spiTxRingBuffer.head; // Contains number of 32-bit words in DMA transfers remaining data
    }
	*pIMSPI = 1; // Internal memory DMA access modifier
	//*pSPIBAUD = 0; // only for SPI-Master: SPICLK baud rate = PCLK /(4 x BAUDR) = 264MHz / (4 x 5MHz) = 13.2 = 13

	if (receive) {
		*pSPIDMAC = SPIRCV | INTEN | SPIDEN; // Receiver enabled | Interrupts enabled | DMA enabled
	}else{
		*pSPIDMAC = INTEN | SPIDEN; // Interrupts enabled | DMA enabled
	}

	*pSPICTL = ISSEN | MSBF | WL32 | OPD | TIMOD2; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit | OpenDrainOutputEnabled | DMA-TransferMode
	*pSPICTL |= SPIEN; // enable SPI-interface after one clock-cycle

	spiDmaMode = true; // switch our processing to DMA-mode
}

void spiDmaEnd(void) {
	// stop SPI and flush remaining data
	*pSPICTL &= ~SPIEN; // disable SPI
	/*
	*pSPICTL |= (TXFLSH | RXFLSH); // flush Rx and Tx
    while (!(SPIF & *pSPISTAT)) { // wait for SPI Transmit Transfer Complere
       NOP();
    }
    while (!(SPIFE & *pSPISTAT)) { // wait for SPI Transaction Complete
       NOP();
    }
	*/

	// reconfigure for CoreWrite-TransferMode (Init Transfer by read of receive-buffer, ISR when buffer is full)
	*pSPIDMAC = 0; // clear SPI DMA control register
	*pSPICTL = ISSEN | MSBF | WL32 | OPD; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit | OpenDrainOutputEnabled
	*pSPICTL |= SPIEN; // enable SPI-interface after one clock-cycle

	spiDmaMode = false; // switch internal processing to SPI-core-mode
}

void spiISR(int sig) {
	// this interrupt is called either when the DMA transfer to SPI Master is completed
	// or when data is available via Core-Mode (SPIRX is full). In this case this interrupt is active 1 PCLK after RXS is set

	if (spiDmaMode) {
		// interrupt because last DMA-Transmission has completed

		spiTxRingBuffer.head = 0; // reset tx-buffer-pointer to first element again
		spiDmaEnd();
	}else{
		// a new word has been received -> put it in the Rx Ring-Buffer

		// check state of RXS in SPI Status Register
		// RXS == 0 -> Empty
		// RXS == 1 -> Full
		if (RXS & *pSPISTAT) { // for a slave device, SPIF is set at the same time as RXS
			// valid data in RXSPI -> add to receive-buffer
			unsigned int rxData = *pRXSPI;

			// check for buffer-overflow
			int next_head = (spiRxRingBuffer.head + 1);
			if (next_head >= SPI_RX_BUFFER_SIZE) {
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

		#if USE_SPI_TXD_MODE == 0
			// use SPI-Core-Mode to transmit our TxBuffer
			// send tx-buffer
			if (spiTxRingBuffer.head != spiTxRingBuffer.tail) {
				*pTXSPI = spiTxRingBuffer.buffer[spiTxRingBuffer.tail];
				spiTxRingBuffer.tail += 1;
				if (spiTxRingBuffer.tail >= SPI_TX_BUFFER_SIZE) {
					spiTxRingBuffer.tail -= SPI_TX_BUFFER_SIZE;
				}
			}else{
				// tx-buffer is empty
				*pTXSPI = 0x00000000;
			}
		#else
			// we are using SPI-DMA-Mode to transmit data
			// in this mode we are using spiTxRingBuffer[] as DmaBuffer always starting at index 0
			// set 0x00 as dummy output
			*pTXSPI = 0x00000000;
		#endif
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
		if (spiRxRingBuffer.tail >= SPI_RX_BUFFER_SIZE) {
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
					unsigned short classId    = payload[0] & 0x000000FF;
					unsigned short channel    = ((payload[0] & 0x0000FF00) >> 8);
					unsigned short index      = ((payload[0] & 0x00FF0000) >> 16);
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
	if (next_head >= SPI_TX_BUFFER_SIZE) {
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

// following code can be used to send data in SPI-Core-Mode
void spiSendArray(unsigned short classId, unsigned short channel, unsigned short index, unsigned short valueCount, void* values) {
	spiPushValueToTxBuffer(SPI_START_MARKER); // StartMarker = '*'
	unsigned int parameter = ((unsigned int)valueCount << 24) + ((unsigned int)index << 16) + ((unsigned int)channel << 8) + (unsigned int)classId;
	spiPushValueToTxBuffer(parameter);
	for (int i = 0; i < valueCount; i++) {
		spiPushValueToTxBuffer(((unsigned int*)values)[i]);
	}
	spiPushValueToTxBuffer(SPI_END_MARKER); // EndMarker = '#'

	#if USE_SPI_TXD_MODE == 1
		// enable SPI-DMA-Mode to transmit data to i.MX253
		spiDmaBegin(false, 0);
	#endif
}

void spiSendValue(unsigned short classId, unsigned short channel, unsigned short index, float value) {
	spiSendArray(classId, channel, index, 1, &value);
}

void spiSendValue_uint32(unsigned short classId, unsigned short channel, unsigned short index, unsigned int value) {
	spiSendArray(classId, channel, index, 1, &value);
}

/*
// this code can be used to act as SPI-master
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
