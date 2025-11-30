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

#include "fpga.h"

Fpga::Fpga(X32BaseParameter* basepar): X32Base(basepar) {
	uart = new Uart(basepar);
}

void Fpga::RoutingInit(void) {
	const char serial[] = "/dev/ttymxc3";
	const uint32_t speed = 115200;
	DEBUG_MESSAGE(DEBUG_FPGA, "opening %s with %d baud", serial, speed)
	uart->Open(serial, speed, true);

	// reset routing-configuration and dsp-configuration
	RoutingDefaultConfig();
}

// set a default configuration for the audio-routing-matrix
void Fpga::RoutingDefaultConfig(void) {
	DEBUG_MESSAGE(DEBUG_FPGA, "RoutingDefaultConfig()")

	// DSP-inputs 1-32 <- XLR-inputs 1-32
	for (uint8_t ch = 1; ch <= 32; ch++) {
		RoutingSetOutputSource('d', ch, RoutingGetSourceIndex('x', ch));
	}
	// DSP-inputs 33-40 <- AUX-inputs 1-8
	for (uint8_t ch = 1; ch <= 8; ch++) {
		RoutingSetOutputSource('d', ch + 32, RoutingGetSourceIndex('a', ch));
	}

	// ---------------------------------------------

	// XLR-outputs 1-16 <- DSP-outputs 1-16
	for (uint8_t ch = 1; ch <= 16; ch++) {
		RoutingSetOutputSource('x', ch, RoutingGetSourceIndex('d', ch));
	}
	// P16-output 1-16 <- DSP-outputs 17-32
	for (uint8_t ch = 1; ch <= 16; ch++) {
		RoutingSetOutputSource('p', ch, RoutingGetSourceIndex('d', ch + 16));
	}
	// AUX-outputs 1-6 + ControlRoom L/R <- DSP-outputs 33-40
	for (uint8_t ch = 1; ch <= 8 ; ch++) {
		RoutingSetOutputSource('a', ch, RoutingGetSourceIndex('d', ch + 32));
	}

	// ---------------------------------------------

	// Card-outputs 1-32 <- XLR-inputs 1-32
	for (uint8_t ch = 1; ch <= 32; ch++) {
		RoutingSetOutputSource('c', ch, RoutingGetSourceIndex('x', ch));
	}

	// transmit routing-configuration to FPGA
	RoutingSendConfigToFpga();
}

// set the outputs of the audio-routing-matrix to desired input-sources
void Fpga::RoutingSetOutputSource(uint8_t group, uint8_t channel, uint8_t inputsource) {
	// input-sources:
	// 0 = OFF
	// 1-32 = XLR-inputs
	// 33-64 = Card-inputs
	// 65-72 = AUX-inputs
	// 73-112 = DSP-outputs
	// 113-160 = AES50A-inputs
	// 161-208 = AES50B-inputs

	// output-taps
	// 1-16 = XLR-outputs
	// 17-32 = UltraNet/P16-outputs
	// 33-64 = Card-outputs
	// 65-72 = AUX-outputs
	// 73-112 = DSP-inputs
	// 113-160 = AES50A-outputs
	// 161-208 = AES50B-outputs

	if (channel == 0){
		return;
	}

	switch (group) {
		case 'x': // XLR-Outputs 1-16
			fpgaRouting.xlr[channel - 1] = inputsource;
			break;
		case 'p': // P16-Outputs 1-16
			fpgaRouting.p16[channel - 1] = inputsource;
			break;
		case 'c': // Card-Outputs 1-32
			fpgaRouting.card[channel - 1] = inputsource;
			break;
		case 'a': // Aux-Outputs 1-8
			fpgaRouting.aux[channel - 1] = inputsource;
			break;
		case 'd': // DSP-Inputs 1-40
			fpgaRouting.dsp[channel - 1] = inputsource;
			break;
		case 'A': // AES50A-Outputs
			fpgaRouting.aes50a[channel - 1] = inputsource;
			break;
		case 'B': // AES50B-Outputs
			fpgaRouting.aes50b[channel - 1] = inputsource;
			break;
	}
}

void Fpga::RoutingSetOutputSourceByIndex(uint8_t outputIndex, uint8_t inputsource) {
	uint8_t group = 0;
	uint8_t channel = 0;
	RoutingGetOutputGroupAndChannelByIndex(outputIndex, &group, &channel); // outputIndex =  1..112, channel = 0..x
	RoutingSetOutputSource(group, channel, inputsource); // inputSource = 0..112
}

uint8_t Fpga::RoutingGetOutputSource(uint8_t group, uint8_t channel) {
	// input-sources:
	// 1-32 = XLR-inputs
	// 33-64 = Card-inputs
	// 65-72 = AUX-inputs
	// 73-112 = DSP-outputs
	// 113-160 = AES50A-inputs
	// 161-208 = AES50B-inputs

	// output-taps
	// 1-16 = XLR-outputs
	// 17-32 = UltraNet/P16-outputs
	// 33-64 = Card-outputs
	// 65-72 = AUX-outputs
	// 73-112 = DSP-inputs
	// 113-160 = AES50A-outputs
	// 161-208 = AES50B-outputs

	if ((group == 0) && (channel == 0)) {
		return 0;
	}

	switch (group) {
		case 'x': // XLR-Outputs 1-16
			return fpgaRouting.xlr[channel-1];
			break;
		case 'p': // P16-Outputs 1-16
			return fpgaRouting.p16[channel-1];
			break;
		case 'c': // Card-Outputs 1-32
			return fpgaRouting.card[channel-1];
			break;
		case 'a': // Aux-Outputs 1-8
			return fpgaRouting.aux[channel-1];
			break;
		case 'd': // DSP-Inputs 1-40
			return fpgaRouting.dsp[channel-1];
			break;
		case 'A': // AES50A-Outputs
			return fpgaRouting.aes50a[channel-1];
			break;
		case 'B': // AES50B-Outputs
			return fpgaRouting.aes50b[channel-1];
			break;
	}

	return 0;
}

uint8_t Fpga::RoutingGetOutputSourceByIndex(uint8_t outputIndex) {
	uint8_t group = 0;
	uint8_t channel = 0;
	RoutingGetOutputGroupAndChannelByIndex(outputIndex, &group, &channel); // index = 1..112
	return RoutingGetOutputSource(group, channel);
}

// get the absolute input-source (global channel-number)
uint8_t Fpga::RoutingGetSourceIndex(uint8_t group, uint8_t channel) {
	// input-sources:
	// 1-32 = XLR-inputs
	// 33-64 = Card-inputs
	// 65-72 = AUX-inputs
	// 73-112 = DSP-outputs
	// 113-160 = AES50A-inputs
	// 161-208 = AES50B-inputs

	if (channel == 0){
		return 0;
	}

	switch (group) {
		case 'x': // XLR-Inputs 1-32
			return channel;
			break;
		case 'c': // Card-Inputs 1-32
			return channel + 32;
			break;
		case 'a': // Aux-Inputs 1-8
			return channel + 64;
			break;
		case 'd': // DSP-Outputs 1-40
			return channel + 72;
			break;
		case 'A': // AES50A-Inputs 1-48
			return channel + 112;
			break;
		case 'B': // AES50B-Inputs 1-48
			return channel + 160;
			break;
	}

	return 0;
}

uint8_t Fpga::RoutingGetOutputIndex(uint8_t group, uint8_t channel) {
	// output-taps
	// 1-16 = XLR-outputs
	// 17-32 = UltraNet/P16-outputs
	// 33-64 = Card-outputs
	// 65-72 = AUX-outputs
	// 73-112 = DSP-inputs
	// 113-160 = AES50A-outputs
	// 161-208 = AES50B-outputs

	if (channel == 0){
		return 0;
	}

	switch (group) {
		case 'x': // XLR-Inputs 1-16
			return channel;
			break;
		case 'p': // UltraNet/P16-outputs 1-16
			return channel + 16;
			break;
		case 'c': // Card-Inputs 1-32
			return channel + 32;
			break;
		case 'a': // Aux-Inputs 1-8
			return channel + 64;
			break;
		case 'd': // DSP-Outputs 1-40
			return channel + 72;
			break;
		case 'A': // AES50A-Inputs 1-48
			return channel + 112;
			break;
		case 'B': // AES50B-Inputs 1-48
			return channel + 160;
			break;
	}

	return 0;
}

void Fpga::RoutingGetSourceGroupAndChannelByIndex(uint8_t sourceIndex, uint8_t* group, uint8_t* channel) {
	// input-sources:
	// 1-32 = XLR-inputs
	// 33-64 = Card-inputs
	// 65-72 = AUX-inputs
	// 73-112 = DSP-outputs
	// 113-160 = AES50A-inputs
	// 161-208 = AES50B-inputs

	if (sourceIndex == 0) {
		// OFF
		*group = 0;
		*channel = 0;
	}else if ((sourceIndex >= 1) && ( sourceIndex <= 32)) {
		*group = 'x';
		*channel = sourceIndex;
	}else if ((sourceIndex >= 33) && ( sourceIndex <= 64)) {
		*group = 'c';
		*channel = sourceIndex - 32;
	}else if ((sourceIndex >= 65) && ( sourceIndex <= 72)) {
		*group = 'a';
		*channel = sourceIndex - 64;
	}else if ((sourceIndex >= 73) && ( sourceIndex <= 112)) {
		*group = 'd';
		*channel = sourceIndex - 72;
	}else if ((sourceIndex >= 113) && ( sourceIndex <= 160)) {
		*group = 'A';
		*channel = sourceIndex - 112;
	}else if ((sourceIndex >= 161) && ( sourceIndex <= 208)) {
		*group = 'B';
		*channel = sourceIndex - 160;
	}
}

void Fpga::RoutingGetOutputGroupAndChannelByIndex(uint8_t outputIndex, uint8_t* group, uint8_t* channel) {
	// output-taps
	// 1-16 = XLR-outputs
	// 17-32 = UltraNet/P16-outputs
	// 33-64 = Card-outputs
	// 65-72 = AUX-outputs
	// 73-112 = DSP-inputs
	// 113-160 = AES50A-outputs
	// 161-208 = AES50B-outputs

	if (outputIndex == 0) {
		*group = 0;
		*channel = 0;
		return;
	}

	if ((outputIndex >= 1) && ( outputIndex <= 16)) {
		*group = 'x';
		*channel = outputIndex;
	}else if ((outputIndex >= 17) && ( outputIndex <= 32)) {
		*group = 'p';
		*channel = outputIndex - 16;
	}else if ((outputIndex >= 33) && ( outputIndex <= 64)) {
		*group = 'c';
		*channel = outputIndex - 32;
	}else if ((outputIndex >= 65) && ( outputIndex <= 72)) {
		*group = 'a';
		*channel = outputIndex - 64;
	}else if ((outputIndex >= 73) && ( outputIndex <= 112)) {
		*group = 'd';
		*channel = outputIndex - 72;
	}else if ((outputIndex >= 113) && ( outputIndex <= 160)) {
		*group = 'A';
		*channel = outputIndex - 112;
	}else if ((outputIndex >= 161) && ( outputIndex <= 208)) {
		*group = 'B';
		*channel = outputIndex - 160;
	}
}

// void Fpga::fpgaRoutingGetSourceGroupAndChannelByDspChannel(uint8_t dspChannel, uint8_t* group, uint8_t* channel) {
//     // first lets find the connected source to this dspChannel
//     uint8_t sourceIndex = fpgaRoutingGetOutputSource('d', dspChannel);
//     fpgaRoutingGetSourceGroupAndChannelByIndex(sourceIndex, group, channel);
// }

// get the input-source name
void Fpga::RoutingGetSourceName(char* p_nameBuffer, uint8_t group, uint8_t channel) {
	// input-sources:
	//       0 = OFF
	//    1-32 = XLR-inputs
	//   33-64 = Card-inputs
	//   65-72 = AUX-inputs
	//  73-112 = DSP-outputs
	// 113-160 = AES50A-inputs
	// 161-208 = AES50B-inputs

	if (channel == 0){
		sprintf(p_nameBuffer, "Off");
		return;
	}

	switch (group) {
		case 'x': // XLR-Inputs 1-32
			sprintf(p_nameBuffer, "XLR In %d", channel);
			break;
		case 'c': // Card-Inputs 1-32
			sprintf(p_nameBuffer, "Card Out %d", channel);
			break;
		case 'a': // Aux-Inputs 1-8
			sprintf(p_nameBuffer, "AUX In %d", channel);
			break;
		case 'd': // DSP-Outputs 1-40
			sprintf(p_nameBuffer, "DSP Out %d", channel);
			break;
		case 'A': // AES50A-Inputs 1-48
			sprintf(p_nameBuffer, "AESA In %d", channel);
			break;
		case 'B': // AES50B-Inputs 1-48
			sprintf(p_nameBuffer, "AESB In %d", channel);
			break;
	}
}

void Fpga::RoutingGetSourceNameByIndex(char* p_nameBuffer, uint8_t sourceIndex) {
	uint8_t group = 0;
	uint8_t channel = 0;
	RoutingGetSourceGroupAndChannelByIndex(sourceIndex, &group, &channel); // index = 0 .. 112
	RoutingGetSourceName(p_nameBuffer, group, channel);
}

// get the output destination name
void Fpga::RoutingGetOutputName(char* p_nameBuffer, uint8_t group, uint8_t channel) {
	// output-taps
	// 1-16 = XLR-outputs
	// 17-32 = UltraNet/P16-outputs
	// 33-64 = Card-outputs
	// 65-72 = AUX-outputs
	// 73-112 = DSP-inputs
	// 113-160 = AES50A-outputs
	// 161-208 = AES50B-outputs

	if (channel == 0) {
		sprintf(p_nameBuffer, "Off");
		return;
	}

	switch(group){
		case 'x':
			sprintf(p_nameBuffer, "XLR Out %d", channel);
			break;
		case 'p':
			sprintf(p_nameBuffer, "P16-%d", channel);
			break;
		case 'c':
			sprintf(p_nameBuffer, "Card In %d", channel);
			break;
		case 'a':
			sprintf(p_nameBuffer, "AUX Out %d", channel);
			break;
		case 'd':
			sprintf(p_nameBuffer, "DSP In %d", channel);
			break;
		case 'A':
			sprintf(p_nameBuffer, "AESA Out %d", channel);
			break;
		case 'B':
			sprintf(p_nameBuffer, "AESB Out %d", channel);
			break;
	}
}

void Fpga::RoutingGetOutputNameByIndex(char* p_nameBuffer, uint8_t index) {
	uint8_t group = 0;
	uint8_t channel = 0;
	RoutingGetOutputGroupAndChannelByIndex(index, &group, &channel); // index = 1..112
	RoutingGetOutputName(p_nameBuffer, group, channel);
}

// helper-function to send the audio-routing to the fpga
void Fpga::RoutingSendConfigToFpga(void) {
	data_64b routingData;

	// copy routing-struct into array
	uint8_t buf[sizeof(fpgaRouting)];
	memcpy(&buf[0], &fpgaRouting, sizeof(fpgaRouting));

	// now copy this array into chunks of 64-bit-data and transmit it to FPGA
	uint8_t chunkCount = (NUM_INPUT_CHANNEL/8);
	for (uint8_t i = 0; i < chunkCount; i++) {
		// copy 8 bytes from routing-data
		memcpy(&routingData.u8[0], &buf[i * 8], 8);

		DEBUG_MESSAGE(DEBUG_FPGA, "send routing data as chunk %d of %d", i, chunkCount-1)
		// now send data to FPGA
		uart->TxToFPGA(FPGA_IDX_ROUTING + (i * 8), &routingData);
	}
}

void Fpga::ProcessUartData() {
	uint8_t currentByte;

	int bytesToProcess = uart->Rx(&fpgaBufferUart[0], sizeof(fpgaBufferUart));

	if (bytesToProcess <= 0) {
		return;
	}

	for (int i = 0; i < bytesToProcess; i++) {
		currentByte = (uint8_t)fpgaBufferUart[i];
	
		// add received byte to buffer
		if (fpgaPacketBufLen < FPGA_MAX_PACKET_LENGTH) {
			fpgaPacketBuffer[fpgaPacketBufLen++] = currentByte;
		} else {
			// buffer full -> remove oldest byte
			memmove(fpgaPacketBuffer, fpgaPacketBuffer + 1, FPGA_MAX_PACKET_LENGTH - 1);
			fpgaPacketBuffer[FPGA_MAX_PACKET_LENGTH - 1] = currentByte;
		}

		int packetBegin = -1;
		int packetEnd = -1;
		int receivedPacketLength = 0; // length of detected packet

		// check if we received enought data. We expect *xxxxss#
		if (fpgaPacketBufLen >= FPGA_PACKET_LENGTH) {
			// check if received character is end of message ('#')
			if (fpgaPacketBuffer[fpgaPacketBufLen - 1] == '#') {
				// we received possible end of a message
				packetEnd = fpgaPacketBufLen - 1;

				// now check begin of message ('*')
				if (fpgaPacketBuffer[fpgaPacketBufLen - FPGA_PACKET_LENGTH] == '*') {
					packetBegin = fpgaPacketBufLen - FPGA_PACKET_LENGTH;

					receivedPacketLength = packetEnd - packetBegin + 1;

					// now calc sum over payload and check against transmitted sum
					uint16_t sumRemote = ((uint16_t)fpgaPacketBuffer[packetEnd - 2] << 8) + (uint16_t)fpgaPacketBuffer[packetEnd - 1];
					uint16_t sumLocal = 0;
					for (uint8_t j = 0; j < (receivedPacketLength - 4); j++) {
						sumLocal += fpgaPacketBuffer[packetBegin + 1 + j];
					}

					if (sumLocal == sumRemote) {

						// empfangene Bytes als String-Wert ausgeben
						DEBUG_MESSAGE(DEBUG_FPGA, "Received: %s", &fpgaPacketBuffer[packetBegin + 1]);
						
						
						// TODO: Implement FPGA EventBuffer

						// we received a valid packet. Offer the received data to fpgaCallback
						//uint8_t payloadLen = receivedPacketLength - 4;
						
						//callbackFpga(&fpgaPacketBuffer[packetBegin + 1], payloadLen);

						// shift remaining bytes by processed amount of data
						memmove(fpgaPacketBuffer, fpgaPacketBuffer + receivedPacketLength, fpgaPacketBufLen - receivedPacketLength);
						fpgaPacketBufLen -= receivedPacketLength;
					}
				}
			}
		}
  	}
}
