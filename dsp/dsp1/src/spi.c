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
#if USE_SPI_TXD_MODE == 0
	float pm spiCommData[150]; // dont use the stack-memory for this and put it into the program memory
	pm sSpiTxRingBuffer spiTxRingBuffer;
#elif USE_SPI_TXD_MODE == 1
	float pm spiCommData[65];
#elif USE_SPI_TXD_MODE == 2
	float pm spiCommData[8];
	// setup DMA-chaining
	int spiTx_tcb[5][4]; // 5 buffers for AudioSamples, CompressorGain, GateGain and additional data
#endif
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
	//*pSPICTL = ISSEN | MSBF | WL32 | OPD; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit | OpenDrainMode
	*pSPICTL = ISSEN | MSBF | WL32; // InputSlaveSelect | MostSignificantBit First | WordLength=32bit
	*pSPICTL |= CPHASE | CLKPL; // set SPI_MODE_3
	*pSPICTL |= SPIEN; // enable SPI-interface after one clock-cycle

	#if USE_SPI_TXD_MODE == 0
		spiTxRingBuffer.head = 0;
		spiTxRingBuffer.tail = 0;
	#elif USE_SPI_TXD_MODE == 2
		// setup DMA-chaining
		// caution: chain-pointer registers must point to the LAST location in the TCB, hence tcb_address + 3

		// audio-data of main-DSP-channels and FX-return
		spiTx_tcb[0][0] = ((int)&spiTx_tcb[1][0] + 3) & OFFSET_MASK; // CPSPI chain-pointer
		spiTx_tcb[0][1] = (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN); // ICSPI internal count
		spiTx_tcb[0][2] = 1; // IMSPI internal modifier
		spiTx_tcb[0][3] = (int)&audioBuffer[TAP_PRE_FADER][0][DSP_BUF_IDX_DSPCHANNEL]; // IISPI internal index

		// audio-data of 8 mixbusses
		spiTx_tcb[1][0] = ((int)&spiTx_tcb[4][0] + 3) & OFFSET_MASK; // CPSPI chain-pointer
		spiTx_tcb[1][1] = ACTIVE_MIX_BUSSES; // ICSPI internal count
		spiTx_tcb[1][2] = 1; // IMSPI internal modifier
		spiTx_tcb[1][3] = (int)&audioBuffer[TAP_INPUT][0][DSP_BUF_IDX_MIXBUS]; // IISPI internal index

/*
		// compressor gains
		spiTx_tcb[2][0] = ((int)&spiTx_tcb[3][0] + 3); // CPSPI chain-pointer
		spiTx_tcb[2][1] = MAX_CHAN_FULLFEATURED; // ICSPI internal count
		spiTx_tcb[2][2] = 1; // IMSPI internal modifier
		spiTx_tcb[2][3] = (int)&dsp.compressorGain[0]; // IISPI internal index

		// gate-gains
		spiTx_tcb[3][0] = ((int)&spiTx_tcb[4][0] + 3); // CPSPI chain-pointer
		spiTx_tcb[3][1] = MAX_CHAN_FULLFEATURED; // ICSPI internal count
		spiTx_tcb[3][2] = 1; // IMSPI internal modifier
		spiTx_tcb[3][3] = (int)&dsp.gateGain[0]; // IISPI internal index
*/
		spiTx_tcb[2][1] = 0;
		spiTx_tcb[3][1] = 0;

		// closing data (audio-data of main-busses and final "#")
		spiTx_tcb[4][0] = 0; // CPSPI chain-pointer ("0" ends DMA-chain)
		spiTx_tcb[4][1] = 4; // ICSPI internal count
		spiTx_tcb[4][2] = 1; // IMSPI internal modifier
		spiTx_tcb[4][3] = (int)&spiCommData[4]; // IISPI internal index



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
		//_valueCount = 2 + (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN) + ACTIVE_MIX_BUSSES + MAX_CHAN_FULLFEATURED + MAX_CHAN_FULLFEATURED + 3;
		//_valueCount = 2 + (MAX_CHAN_FPGA + MAX_DSP2_FXRETURN) + ACTIVE_MIX_BUSSES + 3;
		_valueCount = 2 + spiTx_tcb[0][1] + spiTx_tcb[1][1] + spiTx_tcb[2][1] + spiTx_tcb[3][1] + 3;
		parameter = (_valueCount << 24) + (_index << 16) + (_channel << 8) + _classId;
		memcpy(&spiCommData[1], &parameter, sizeof(uint32_t));

		parameter = 0x00000023; // #
		memcpy(&spiCommData[7], &parameter, sizeof(uint32_t));
	#endif
	spiRxRingBuffer.head = 0;
	spiRxRingBuffer.tail = 0;
}

void spiDmaBegin(unsigned int* buffer, int len, bool receive) {
	// more information in SHARC Processor Hardware Reference 12-36

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
	#if USE_SPI_TXD_MODE == 1
		if (receive) {
			*pSPIDMAC = SPIRCV | INTEN | SPIDEN; // Receiver enabled | Interrupts enabled | DMA enabled
		}else{
			*pSPIDMAC = INTEN | SPIDEN; // Interrupts enabled | DMA enabled
		}
	#elif USE_SPI_TXD_MODE == 2
		if (receive) {
			*pSPIDMAC = SPIRCV | INTEN | SPIDEN | SPICHEN; // Receiver enabled | Interrupts enabled | DMA enabled
		}else{
			*pSPIDMAC = INTEN | SPIDEN | SPICHEN; // Interrupts enabled | DMA enabled
		}
	#endif

	#if USE_SPI_TXD_MODE == 2
		// begin the DMA-chain by writing the chain-pointer for the second DMA
		*pCPSPI = ((unsigned int)&spiTx_tcb[0][0] + 3) & OFFSET_MASK;
	#endif
}

void spiDmaEnd(void) {
	// stop SPI and flush remaining data
	// wait for complete DMA-transfer
	while ((SPIS0 | SPIS1) & *pSPIDMAC) {
       NOP();
    }
	// wait for TX-buffer to be emptied into shift-register
    while ((TXS & *pSPISTAT)) {
       NOP();
    }
    // wait for the SPI-shift-register to finish shifting out
    while (!(SPIF & *pSPISTAT)) {
       NOP();
    }
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

void spiISR(int sig) {
	// this interrupt is called either when the DMA transfer to SPI Master is completed
	// or when data is available via Core-Mode (SPIRX is full). In this case this interrupt is active 1 PCLK after RXS is set

	// check if our system is in DMA-processing-mode
	if (spiDmaMode) {
		// interrupt because last DMA-Transmission has completed
		#if USE_SPI_TXD_MODE == 1
			spiDmaEnd(); // reconfigure to Core-Mode to receive new commands
		#elif USE_SPI_TXD_MODE == 2
			// check if chain-loading still in progress
			if (!(SPICHS & *pSPIDMAC)) {
				spiDmaEnd(); // reconfigure to Core-Mode to receive new commands
			}
		#endif
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

void spiPushValueToTxBuffer(unsigned int value) {
	#if USE_SPI_TXD_MODE == 0
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
	#endif
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
