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

pm sSpiRxRingBuffer spiRxRingBuffer;

#pragma section("seg_pmda")
pm float spiCommData[SPI_DMA_COMMDATA_SIZE];
// setup DMA-chaining
#pragma section("seg_pmda")
pm int spiTx_tcb[4];

volatile bool spiNewRxDataReady = false;
bool spiDmaMode = false;
volatile bool spiDesiredMode = 0;

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
	//*pSPICTL = ISSEN | MSBF | WL32 | OPD; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit | OpenDrainMode
	*pSPICTL = ISSEN | MSBF | WL32; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit
	*pSPICTL |= CPHASE | CLKPL; // set SPI_MODE_3
	*pSPICTL |= SPIEN; // enable SPI-interface after one clock-cycle


	spiTx_tcb[0] = 0; // CPSPI chain-pointer <- DEBUG: jump to main-bus-audio-data directly without mixbusses and gains
	spiTx_tcb[1] = SPI_DMA_COMMDATA_SIZE - 5; // ICSPI internal count
	spiTx_tcb[2] = 1; // IMSPI internal modifier (offset for next index)
	spiTx_tcb[3] = (int)&spiCommData[5]; // IISPI internal index

	// configure the spiCommData-struct (header and tail of the protocol)
	unsigned int parameter;
	unsigned int _classId;
	unsigned int _channel;
	unsigned int _index;
	unsigned int _valueCount;

	parameter = 0x0000002A; // *
	memcpy(&spiCommData[0], &parameter, sizeof(uint32_t));

	_classId = 's';
	_channel = 'u';
	_index = 0;
	_valueCount = SPI_DMA_COMMDATA_SIZE - 3;
	parameter = (_valueCount << 24) + (_index << 16) + (_channel << 8) + _classId;
	memcpy(&spiCommData[1], &parameter, sizeof(uint32_t));

	parameter = 0x00000023; // #
	memcpy(&spiCommData[SPI_DMA_COMMDATA_SIZE - 1], &parameter, sizeof(uint32_t));

	spiRxRingBuffer.head = 0;
	spiRxRingBuffer.tail = 0;
}

void spiCallback(void) {
	if ((spiDesiredMode == 0) && (spiDmaMode)) {
		// try to switch to SpiCoreMode

		// check if chain-loading still in progress
		if (!(SPICHS & *pSPIDMAC)) {
			spiDmaEnd(); // reconfigure to Core-Mode to receive new commands
		}
	}else if ((spiDesiredMode == 1) && (!spiDmaMode)) {
		// try to switch to SpiDmaMode
	}
}

void spiCoreRxBegin() {
    // disable SPI-port
    *pSPICTL &= ~SPIEN;

	// disable DMA-chain
	*pIISPI = 0;
	*pCPSPI = 0;

	// reset all registers
	*pSPIFLG = DS0EN; // Enable SRU2 output for SPI device-select-0
	*pSPIDMAC = 0; // spi-dma-register

	spiRxRingBuffer.head = 0;
	spiRxRingBuffer.tail = 0;

	spiDmaMode = false; // switch internal processing to SPI-core-mode

	// reconfigure for CoreWrite-TransferMode (Init Transfer by read of receive-buffer, ISR when buffer is full)
	*pSPICTL = ISSEN | MSBF | WL32; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit
	*pSPICTL |= CPHASE | CLKPL; // set SPI_MODE_3
	*pSPICTL |= SPIEN; // enable SPI-interface after one clock-cycle
}

void spiDmaBegin(unsigned int* buffer, int len, bool receive) {
	// more information in SHARC Processor Hardware Reference 12-36

	spiDesiredMode = 1;

	// Step 1: disable SPI-port directly as we only received data previously
	*pSPICTL &= ~SPIEN;
	spiDmaMode = true; // switch our processing to DMA-mode

	// Step 2: Write to SPICTL-register to setup DMA-transfer. Important: Enable SPI before enabling DMA
	*pSPICTL = ISSEN | MSBF | WL32 | TIMOD2; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit | DMA-TransferMode
	*pSPICTL |= CPHASE | CLKPL; // set SPI_MODE_3
	*pSPICTL |= SPIEN; // enable SPI-interface after one clock-cycle


	// Step 3: define DMA receive/transmit parameters
	// reconfigure SPI for DMA-TransferMode
	*pIISPI = (unsigned int)buffer; // pointer to (first) buffer
	*pCSPI = len; // number of 32-bit words to transmit from (first) buffer
	*pIMSPI = 1; // Internal memory DMA access modifier
	//*pSPIBAUD = 0; // only for SPI-Master: SPICLK baud rate = PCLK /(4 x BAUDR) = 264MHz / (4 x 5MHz) = 13.2 = 13


	// Step 4: write to SPIDMAC-register to enable SPI DMA engine
	if (receive) {
		*pSPIDMAC = SPIRCV | INTEN | SPIDEN | SPICHEN; // Receiver enabled | Interrupts enabled | DMA enabled
	}else{
		*pSPIDMAC = INTEN | SPIDEN | SPICHEN; // Interrupts enabled | DMA enabled
	}

	// begin the DMA-chain by writing the chain-pointer for the second DMA
	*pCPSPI = ((unsigned int)&spiTx_tcb[0] + 3) & OFFSET_MASK;
}

void spiDmaEnd(void) {
	// stop SPI and flush remaining data
	// wait for complete DMA-transfer
	if ((SPIS0 | SPIS1) & *pSPIDMAC) {
       return;
    }
	// wait for TX-buffer to be emptied into shift-register
	if ((TXS & *pSPISTAT)) {
		return;
    }
    // wait for the SPI-shift-register to finish shifting out
	if (!(SPIF & *pSPISTAT)) {
		return;
    }

    spiCoreRxBegin();
}

void spiISR(int sig) {
	// this interrupt is called either when the DMA transfer to SPI Master is completed
	// or when data is available via Core-Mode (SPIRX is full). In this case this interrupt is active 1 PCLK after RXS is set

	// check if our system is in DMA-processing-mode
	if (spiDmaMode) {
		// interrupt because last DMA-Transmission has completed
		spiDesiredMode = 0;
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

		// we are using SPI-DMA-Mode to transmit data
		// in this mode we are using spiTxRingBuffer[] as DmaBuffer always starting at index 0
		// set 0x00 as dummy output
		*pTXSPI = 0x00000000;
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
					commExecCommand(classId, channel, index, valueCount, &payload[1]); // classId, channel, index, valuecount, values[]

					state = LOOKING_FOR_START_MARKER; // reset state
				}else{
					// error: end-marker not found
					state = LOOKING_FOR_START_MARKER; // reset state anyway
				}
				break;
		}
	}
}
