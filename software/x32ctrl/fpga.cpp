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

	spi = new SPI(basepar);

	if (!state->bodyless) {
    	if (!spi->UploadBitstreamFpgaLattice()) {
	        spi->UploadBitstreamFpgaXilinx();
	    }
	    spi->OpenConnectionFpga();
	}

	// configData -> at the moment all bits are unused
	configData = 0b00000000;
	SendConfig();

	AES50Counter = 0;
}

void Fpga::Init() {
	const uint speed = 115200;
	String serial;
	
	if (state->bodyless) {
		serial = "/tmp/ttyLocalFpga";
	}
	else if (state->raspi)
	{
		//serial = "/dev/ttymxc3";
		return;
	}
	else
	{
		serial = "/dev/ttymxc3";
	}

	helper->DEBUG_FPGA(DEBUGLEVEL_NORMAL, "opening %s with %d baud", serial.c_str(), speed);
	uart->Open(serial.c_str(), speed, true);
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

	return "";
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

	return "";
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

		helper->DEBUG_FPGA(DEBUGLEVEL_NORMAL, "SendRoutingToFpga() %s -> %s",
			config->GetParameter(ROUTING_FPGA)->GetFormatedValue(channel).c_str(),
			GetOutputNameByIndex(channel+1).c_str()
		);

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

bool Fpga::GetConfigBit(uint8_t bitNumber) {
	return (configData & (1 << bitNumber)) != 0;
}

void Fpga::SetConfigBit(uint8_t bitNumber, bool value) {
	if (value) {
		configData |= (1 << bitNumber);
	} else {
		configData &= ~(1 << bitNumber);
	}

	// send data to FPGA
	if (state->bodyless || state->raspi) {
		return;
	}

	SendConfig();
}

void Fpga::SendConfig(void) {
	uint8_t txData[2];
	uint8_t rxData[2];
	txData[0] = configData;
	txData[1] = 255; // address
	spi->SendFpgaData(&txData[0], &rxData[0], 2);

	if ((rxData[0] != txData[0]) || (rxData[1] != txData[1]))
	{
		// FPGA is sending the same data back to the i.MX25 at the moment so check the received values against the sent values
		helper->Error("FPGA-ConfigBits: Received values (0x%02x 0x%02x) does not match the sent values (0x%02x 0x%02x)\n", rxData[0], rxData[1], txData[0], txData[1]);
	}
}

void Fpga::AES50Receive(void) {
	bool messageHandled = false;
	uint readBytes = uart->Rx(&fpgaRxBufferUart[0], sizeof(fpgaRxBufferUart));
	char tmpName[16];

	// check the checksum of the read message
	if ((readBytes > 2) && (readBytes >= (fpgaRxBufferUart[1] * 4)) && (AES50CalcChecksum(&fpgaRxBufferUart[0], false))) {
		// checksum is valid -> process message

		switch (fpgaRxBufferUart[0]) {
			case 0x01:
				// headamp-configuration-message

				break;
			case 0x05:
				// device-type and properties-message
				break;
			case 0x10:
				// device- and channelname-message

				// take the name of the connected device and display it in X32CTRL
				memcpy(&tmpName[0], &fpgaRxBufferUart[8], 16);
				lv_label_set_text_fmt(objects.debugtext_x32ctrl, "AES50 Connected to: %s", String(tmpName).c_str());

				break;
			default:
				// unknown message
				break;
		}
	}else{
		// checksum failed
	}

	// send data right back to the AES50-device
	//AES50Send(&fpgaRxBufferUart[0], readBytes);
}

// this function is called every 100ms
void Fpga::AES50Tick(Config* config) {
	AES50Counter++;
	if (AES50Counter == 10) {
		// send device-properties every 2 seconds
		AES50SendDeviceTypeAndProperty();
	}else if (AES50Counter >= 20) {
		// send headamp-controls every 2 seconds
		AES50Counter = 0;

		AES50SendHeadampMessage(config);
	}
}

void Fpga::AES50SendDeviceTypeAndProperty() {
	fpgaTxBufferUart[0] = 0x05;
	fpgaTxBufferUart[1] = (16 / 4); // message-length in 32-bit words
	fpgaTxBufferUart[2] = 0; // checksum (will be set in AES50Send-Function)
	fpgaTxBufferUart[3] = 0; // checksum (will be set in AES50Send-Function)

	// AES50-DeviceChars for the last four connected devices
	// (AES50 seems to support up to 6, but only the last 4 are transmitted)
	fpgaTxBufferUart[4] = 0; // fourth device in AES50-chain
	fpgaTxBufferUart[5] = 0; // third device in AES50-chain
	fpgaTxBufferUart[6] = 0; // second device in AES50-chain: S16
	fpgaTxBufferUart[7] = 'C'; // first device in AES50-chain: X32

	// data[8 .. 13] <- '0' if no headamp-control, '2' if headamp-
	// control is present
	fpgaTxBufferUart[8] = '2'; // AES50 Ch 1-8 have headamp-controls
	fpgaTxBufferUart[9] = '2'; // AES50 Ch 9-16 have headamp-controls
	fpgaTxBufferUart[10] = '0'; // AES50 Ch 17-24 have no headamp-control in current device
	fpgaTxBufferUart[11] = '0'; // AES50 Ch 25-32 have no headamp-control in current device
	fpgaTxBufferUart[12] = '0'; // AES50 Ch 33-40 have no headamp-control in current device
	fpgaTxBufferUart[13] = '0'; // AES50 Ch 41-48 have no headamp-control in current device

	// byte-stuffing to have full divider by 4
	fpgaTxBufferUart[14] = 0x00;
	fpgaTxBufferUart[15] = 0x00;

	// calculate and add checksum to begin of message
	AES50CalcChecksum(&fpgaTxBufferUart[0], true);

	// send data over UART to AES50 IP-core
	AES50Send(&fpgaTxBufferUart[0], 16);
}

void Fpga::AES50SendNames() {
	fpgaTxBufferUart[0] = 0x10;
	fpgaTxBufferUart[1] = (408 / 4); // message-length in 32-bit words
	fpgaTxBufferUart[2] = 0; // checksum (will be set in AES50Send-Function)
	fpgaTxBufferUart[3] = 0; // checksum (will be set in AES50Send-Function)

	// AES50-DeviceChars for the last four connected devices
	// (AES50 seems to support up to 6, but only the last 4 are transmitted)
	fpgaTxBufferUart[4] = 0;
	fpgaTxBufferUart[5] = 0;
	fpgaTxBufferUart[6] = 0; // S16
	fpgaTxBufferUart[7] = 'C'; // X32

	// the next bytes are zero-terminated ASCII-strings.
	// First clear the data-array with zeros
	for (int i = 8; i < 408; i++) {
		fpgaTxBufferUart[i] = 0x00;
	}

	// insert zero-terminated device-string of current AES50-device
	sprintf((char*)&fpgaTxBufferUart[8], "X32");
	fpgaTxBufferUart[24] = 0x02; // add a 0x02 as "START OF TEXT"

	// from data[25] 48x 7-char ASCII-Name for each channel is
	// sent followed by a 0x02
	for (int i = 0; i < 48; i++) {
		sprintf((char*)&fpgaTxBufferUart[25 + (i * 8)], "IN % u", i + 1);
		fpgaTxBufferUart[25 + (i * 8) + 7] = 0x02;
	}
	fpgaTxBufferUart[407] = 0x0A; // add a linefeed at the end

	// calculate and add checksum to begin of message
	AES50CalcChecksum(&fpgaTxBufferUart[0], true);

	// send data over UART to AES50 IP-core
	AES50Send(&fpgaTxBufferUart[0], 408);
}

void Fpga::AES50SendHeadampMessage(Config* config) {
	fpgaTxBufferUart[0] = 0x01;
	fpgaTxBufferUart[1] = (60 / 4); // message-length in 32-bit words
	fpgaTxBufferUart[2] = 0; // checksum (will be set in AES50Send-Function)
	fpgaTxBufferUart[3] = 0; // checksum (will be set in AES50Send-Function)

	// AES50-DeviceChars for the last four connected devices
	// (AES50 seems to support up to 6, but only the last 4 are transmitted)
	fpgaTxBufferUart[4] = 0;
	fpgaTxBufferUart[5] = 0;
	fpgaTxBufferUart[6] = 0; // S16
	fpgaTxBufferUart[7] = 'C'; // X32

	// now insert the headamp-gains
	// the specific value depends on the connected AES50 device as some
	// devices have more gain-options than others
	//
	// An S16 has settings between -2.0dB and 45.5dB resulting in 47.5dB range
	// with 2.5dB steps -> 47.5 / 2.5 = 19. So we have settings between
	// 0 = -2.0dB and 19 = 45.5dB
	/*
	// TODO: get current routing for each of the 48 AES50A-input-channels and read gain-level
	for (int i = 0; i < 48; i++) {
		fpgaTxBufferUart[8 + i] = (uint8_t) roundf(gainMap(headAmpGain[i], -2.0f, 45.5f, 0, 19));
		if (headAmpPhantom[i]) {
			fpgaTxBufferUart[8 + i] |= 0x80;
		}
	}
	*/
	// for now, set the gains to minimum value
	for (int i = 0; i < 48; i++) {
		fpgaTxBufferUart[8 + i] = 0;
	}

	// a single 32-bit word with zeros for finalizing the message
	fpgaTxBufferUart[56] = 0;
	fpgaTxBufferUart[57] = 0;
	fpgaTxBufferUart[58] = 0;
	fpgaTxBufferUart[59] = 0;

	// calculate and add checksum to begin of message
	AES50CalcChecksum(&fpgaTxBufferUart[0], true);

	// send data over UART to AES50 IP-core
	AES50Send(&fpgaTxBufferUart[0], 60);
}

void Fpga::AES50Send(char* data, uint len) {
	MessageBase* message = new MessageBase();

	message->AddDataArray(data, len);

	if (helper->DEBUG_FPGA(DEBUGLEVEL_TRACE)) {
		printf("DEBUG_FPGA: Transmit: ");
    	for (int i = 0; i < message->current_length; i++){
        	printf("%c", message->buffer[i]);
    	}
    	printf("\n");
	}

	uart->Tx(message);

	delete(message);
}

bool Fpga::AES50CalcChecksum(char* buf, bool insertChecksum) {;
	uint8_t messageID = buf[0];
	uint8_t len = buf[1];
	uint8_t deviceChar = buf[7];

	// initializing
	uint32_t ids4 = (uint32_t)deviceChar << 24;
	uint16_t cks = (uint16_t)(ids4 >> 16) ^ (uint16_t)(ids4 & 0xFFFF);

	uint32_t* dataWords = (uint32_t*)&buf[8];

	if (len > 2) {
		for (int i = 0; i <= (len - 3); i++) {
			uint32_t currentWord = dataWords[i];
			cks -= (uint16_t)(currentWord >> 16) ^ (uint16_t)(currentWord & 0xFFFF);
		}
	}
	uint16_t finalCks = cks ^ 0xFFFF;

	if (insertChecksum) {
		// insert calculated checksum into message
		buf[2] = (uint8_t)(finalCks & 0xFF);
		buf[3] = (uint8_t)(finalCks >> 8);

		return true;
	}else{
		// just compare calculated checksum against message
		return ((buf[2] == (uint8_t)(finalCks & 0xFF)) && (buf[3] == (uint8_t)(finalCks >> 8)));
	}
}

int Fpga::wrapRingBufferIndex(int index, int bufLen) {
	if (index < 0) {
		return (index + bufLen);
	}else if (index >= bufLen) {
		return (index - bufLen);
	}else{
		return index;
	}
}