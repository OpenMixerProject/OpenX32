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
	spi = new SPI(basepar);

	if (!state->bodyless) {
    	if (!spi->UploadBitstreamFpgaLattice()) {
	        spi->UploadBitstreamFpgaXilinx();
	    }
	    spi->OpenConnectionFpga();
	}
}

// get the absolute input-source (global channel-number)
uint8_t Fpga::GetInputIndex(uint8_t group, uint8_t channel) {
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

uint8_t Fpga::GetOutputIndex(uint8_t group, uint8_t channel) {
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

void Fpga::GetInputGroupAndChannelByIndex(uint8_t sourceIndex, uint8_t* group, uint8_t* channel) {
	// Inputs
	//
	// 1-32 = XLR-inputs
	// 33-64 = Card-inputs
	// 65-72 = AUX-inputs
	// 73-112 = DSP-return
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

void Fpga::GetOutputGroupAndChannelByIndex(uint8_t outputIndex, uint8_t* group, uint8_t* channel) {
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

String Fpga::GetInputName(uint8_t group, uint8_t channel) {
	// Inputs
	//       0 = OFF
	//    1-32 = XLR-inputs
	//   33-64 = Card-inputs
	//   65-72 = AUX-inputs
	//  73-112 = DSP1 Return
	// 113-160 = AES50A-inputs
	// 161-208 = AES50B-inputs

	if (channel == 0){
		return "Off";
	}

	switch (group) {
		case 'x': // XLR-Inputs 1-32
			return String("XLR In ") + String(channel);
		case 'c': // Card-Inputs 1-32
			return String("Card In ") + String(channel);
		case 'a': // Aux-Inputs 1-8
			return String("AUX In ") + String(channel);
		case 'd': // DSP-Outputs 1-40
			return String("DSP1 Return ") + String(channel);
		case 'A': // AES50A-Inputs 1-48
			return String("AES50A In ") + String(channel);
		case 'B': // AES50B-Inputs 1-48
			return String("AES50B In ") + String(channel);
	}
}

String Fpga::GetInputNameByIndex(uint8_t sourceIndex) {
	uint8_t group = 0;
	uint8_t channel = 0;
	GetInputGroupAndChannelByIndex(sourceIndex, &group, &channel); // index = 0 .. 208
	return GetInputName(group, channel);
}

// get the output destination name
String Fpga::GetOutputName(uint8_t group, uint8_t channel) {
	// output-taps
	// 1-16 = XLR-outputs
	// 17-32 = UltraNet/P16-outputs
	// 33-64 = Card-outputs
	// 65-72 = AUX-outputs
	// 73-112 = DSP-inputs
	// 113-160 = AES50A-outputs
	// 161-208 = AES50B-outputs

	if (channel == 0) {
		return "Off";
	}

	switch(group){
		case 'x':
			return "XLR Out " + String(channel);
		case 'p':
			return "P16-" + String(channel);
		case 'c':
			return "Card Out " + String(channel);
		case 'a':
			switch (channel)
			{
				case 1 ... 6:
					return "AUX Out " + String(channel);
				case 7:
					return String("Monitor L");
				case 8:
					return String("Monitor R");
			}
			break;
		case 'd':
			return "DSP In " + String(channel);
		case 'A':
			return "AES50A Out " + String(channel);
		case 'B':
			return "AES50B Out " + String(channel);
	}
}

String Fpga::GetOutputNameByIndex(uint8_t index) {
	uint8_t group = 0;
	uint8_t channel = 0;
	GetOutputGroupAndChannelByIndex(index, &group, &channel); // index = 1..208
	return GetOutputName(group, channel);
}

// helper-function to send the audio-routing to the fpga
void Fpga::SendRoutingToFpga(int channel) {
	if (state->bodyless) {
		return;
	}

	uint8_t txData[2];
	uint8_t rxData[2];

	// FPGA expects data in this format:
	// 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
	// |------- DATA ------||---- ADDR ----|

	if ((channel >= 0) && (channel < 208))
	{
		// send only one routing-data to FPGA

		txData[0] = (uint8_t)config->GetUint(ROUTING_FPGA, channel);
		txData[1] = channel;

		helper->DEBUG_FPGA(DEBUGLEVEL_NORMAL, "SendRoutingToFpga() channelindex %d: %d", channel, txData[0]);

		spi->SendFpgaData(&txData[0], &rxData[0], 2);

		if ((rxData[0] != txData[0]) || (rxData[1] != txData[1]))
		{
			// FPGA is sending the same data back to the i.MX25 at the moment so check the received values against the sent values
			helper->Error("FPGA-Routing: Received values (0x%02x 0x%02x) does not match the sent values (0x%02x 0x%02x)\n", rxData[0], rxData[1], txData[0], txData[1]);
		}
	}
	else
	{
		// send all routing-data to FPGA

		for (uint8_t i = 0; i < 208; i++)
		{
			helper->DEBUG_FPGA(DEBUGLEVEL_TRACE, "send routing data %d", channel+1);
			txData[0] = (uint8_t)config->GetUint(ROUTING_FPGA, i);
			txData[1] = i; 
			spi->SendFpgaData(&txData[0], &rxData[0], 2);

			if ((rxData[0] != txData[0]) || (rxData[1] != txData[1]))
			{
				// FPGA is sending the same data back to the i.MX25 at the moment so check the received values against the sent values
				helper->Error("FPGA-Routing: Received values (0x%02x 0x%02x) does not match the sent values (0x%02x 0x%02x)\n", rxData[0], rxData[1], txData[0], txData[1]);
			}
		}
	}
}

bool Fpga::GetDebugBit(uint8_t bitNumber) {
	return (debugByte & (1 << bitNumber)) != 0;
}

void Fpga::SetDebugBit(uint8_t bitNumber, bool value) {
	if (value) {
		debugByte |= (1 << bitNumber);
	} else {
		debugByte &= ~(1 << bitNumber);
	}

	// send data to FPGA
	if (state->bodyless) {
		return;
	}

	uint8_t txData[2];
	uint8_t rxData[2];
	txData[0] = debugByte;
	txData[1] = 255; // address
	spi->SendFpgaData(&txData[0], &rxData[0], 2);

	if ((rxData[0] != txData[0]) || (rxData[1] != txData[1]))
	{
		// FPGA is sending the same data back to the i.MX25 at the moment so check the received values against the sent values
		helper->Error("FPGA-DebugBits: Received values (0x%02x 0x%02x) does not match the sent values (0x%02x 0x%02x)\n", rxData[0], rxData[1], txData[0], txData[1]);
	}
}