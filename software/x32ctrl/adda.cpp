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

#include "adda.h"


Adda::Adda(X32BaseParameter* basepar): X32Base(basepar) {
	uart = new Uart(basepar);

	addaPacketBufLen = 0;
}

void Adda::Init() {
	const char serial[] = "/dev/ttymxc2";
	const uint16_t speed = 38400;
	helper->DEBUG_ADDA(DEBUGLEVEL_NORMAL, "opening %s with %d baud", serial, speed);
	uart->Open(serial, speed, true);

	// send identification-commands to all possible boards
	addaBoards[ADDA_BOARD_XLR_IN_0] = SendReceive("*0I#"); // XLR IN
	addaBoards[ADDA_BOARD_XLR_IN_1] = SendReceive("*1I#"); // XLR IN
	addaBoards[ADDA_BOARD_XLR_IN_2] = SendReceive("*2I#"); // XLR IN
	addaBoards[ADDA_BOARD_XLR_IN_3] = SendReceive("*3I#"); // XLR IN
	addaBoards[ADDA_BOARD_XLR_OUT_0] = SendReceive("*4I#"); // XLR OUT
	addaBoards[ADDA_BOARD_XLR_OUT_1] = SendReceive("*5I#"); // XLR OUT
	addaBoards[ADDA_BOARD_EXPANSION] = SendReceive("*8I#"); // Expansion-Card
	addaBoards[ADDA_BOARD_EXPANSION_RETURN] = SendReceive("*8R#"); // Check if Expansion-Card has return
	

	// -1=unknown, 0=X32, 1=X32Compact, 2=X32Producer, 3=X32Rack, 4=X32Core
	// set gains to -2dB and turnoff phantom-power
	for(uint8_t board=0;board<4;board++) {
		for(uint8_t channel=0;channel<8;channel++) {
			SendReceive((String("*") + board + "G" + channel + "0000#").c_str());
		}
	}

	// SendReceive("*1G00000#*1G00000#*1G10000#*1G10000#*1G20000#*1G20000#*1G30000#*1G30000#*1G40000#*1G40000#*1G50000#*1G50000#*1G60000#*1G60000#*1G70000#*1G70000#");
	// SendReceive("*3G00000#*3G00000#*3G10000#*3G10000#*3G20000#*3G20000#*3G30000#*3G30000#*3G40000#*3G40000#*3G50000#*3G50000#*3G60000#*3G60000#*3G70000#*3G70000#");
	// SendReceive("*0G00000#*0G00000#*0G10000#*0G10000#*0G20000#*0G20000#*0G30000#*0G30000#*0G40000#*0G40000#*0G50000#*0G50000#*0G60000#*0G60000#*0G70000#*0G70000#");
	// SendReceive("*2G00000#*2G00000#*2G10000#*2G10000#*2G20000#*2G20000#*2G30000#*2G30000#*2G40000#*2G40000#*2G50000#*2G50000#*2G60000#*2G60000#*2G70000#*2G70000#");

	// initialize the 8ch-input-boards
	SendReceive("*0R:W018B#");
	SendReceive("*1R:W018B#");
	SendReceive("*2R:W018B#");
	SendReceive("*3R:W018B#");

	// initialize the 8ch-output-boards
	SendReceive("*4R:W0281C0:W0800:W0280#");
	SendReceive("*5R:W0281C0:W0800:W0280#");


	uint32_t samplerate = config->GetSamplerate();
	if (samplerate == 44100) {
		// set all boards to 48kHz
		SendReceive("*8C40U#");
		SendReceive("*8R8#*8C40U#");
		SendReceive("*8C40U#");
	} else if (samplerate == 48000) {
		// set all boards to 48kHz
		SendReceive("*8C80U#");
		SendReceive("*8R8#*8C80U#");
		SendReceive("*8C80U#");
	} else if (samplerate == 96000) {
		// currently unknown, but the X32 is able to handle 96kHz for sure
	} else {
		// unsupported sample-rate
	}

	// and send some more commands (no information about the usage up to now)
	SendReceive("*9R0#*9G0#");
	SendReceive("*9AF#");
	SendReceive("*9N0#");
	SendReceive("*9AN#");
	SendReceive("*9AN#");
	SendReceive("*9AN#");
	SendReceive("*9AN#");
	SendReceive("*9AN#");
	SendReceive("*9AN#");
	SendReceive("*9N0#");
	usleep(20000);
}

bool Adda::HasXlrIn0(){
	return addaBoards[ADDA_BOARD_XLR_IN_0] != "";
}

bool Adda::HasXlrIn1(){
	return addaBoards[ADDA_BOARD_XLR_IN_1] != "";
}

bool Adda::HasXlrIn2(){
	return addaBoards[ADDA_BOARD_XLR_IN_2] != "";
}

bool Adda::HasXlrIn3(){
	return addaBoards[ADDA_BOARD_XLR_IN_3] != "";
}

bool Adda::HasXlrOut0(){
	return addaBoards[ADDA_BOARD_XLR_OUT_0] != "";
}

bool Adda::HasXlrOut1(){
	return addaBoards[ADDA_BOARD_XLR_OUT_1] != "";
}

bool Adda::HasExpansion(){
	return addaBoards[ADDA_BOARD_EXPANSION] != "";
}

void Adda::SetSamplerate(uint32_t samplerate) {
	helper->DEBUG_ADDA(DEBUGLEVEL_NORMAL, "Set samplerate to %d", samplerate);

	if (samplerate == 44100) {
		SendReceive("*8R#*8C40U#");
	} else if (samplerate == 48000) {
		SendReceive("*8R#*8C80U#");
	} else if (samplerate == 96000) {
		// currently unknown, but the X32 is able to handle 96kHz for sure
	} else {
		// unsupported sample-rate
	}

	helper->DEBUG_ADDA(DEBUGLEVEL_VERBOSE, "send initialization-commands to all boards");
	SendReceive("*9R0#");
	SendReceive("*9AF#");
	SendReceive("*9AN#");
	SendReceive("*9AN#");
	SendReceive("*9AN#");
	SendReceive("*9AN#");
	SendReceive("*9AN#");
	SendReceive("*9AN#");
	SendReceive("*9N0#");
}

String Adda::SetGain(uint8_t boardId, uint8_t channel, float gain, bool phantomPower) {
	helper->DEBUG_ADDA(DEBUGLEVEL_NORMAL, "SetGain: Board=%d Channel=%d Gain=%f Phantom=%d)", boardId, channel, (double)gain, phantomPower);

	String message;

	message += '*';
	message += (char)(boardId + 0x30); // convert to ASCII-number
	message += 'G';
	message += (char)((channel-1) + 0x30); // convert to ASCII-number

	// convert gain-value between -2dB and +45.5dB to three-digit value
	char buf[4];
	uint8_t gainHardware = (uint8_t)((gain + 2.0f)/2.5f); // gain is between -2dB and +45.5dB in 2.5dB steps
	sprintf(buf, "%03d", gainHardware);
	
	message += &buf[0];

	if (phantomPower) {
		message += '1';
	}else{
		message += '0';
	}
	message += '#';

	return SendReceive(message);
}

String Adda::SendReceive(String cmd) {
	AddaMessage message;
	uint8_t currentByte;
	String answer;

	message.AddString(cmd.c_str());

	if (helper->DEBUG_ADDA(DEBUGLEVEL_TRACE)) {
		printf("DEBUG_ADDA: Transmit: ");
    	for (int i =0; i < message.current_length; i++){
        	printf("%c", message.buffer[i]);
    	}
    	printf("\n");
	}

	uart->TxRaw(&message);

	uint8_t waitForMessage = 10; // wait ~10 ms
	uint16_t readBytes = 0;
	while (waitForMessage > 0) {
		helper->DEBUG_ADDA(DEBUGLEVEL_TRACE, "Waiting for Message from ADDA-Boards, Counter: %d", waitForMessage);
		readBytes += uart->Rx(&addaBufferUart[readBytes], sizeof(addaBufferUart));
		if (readBytes > 0) {

			if (helper->DEBUG_ADDA(DEBUGLEVEL_TRACE)){
				printf("Received %d Bytes\n", readBytes); 

				for (int i = 0; i < readBytes; i++) {
					printf("%c", addaBufferUart[i]);
				}
				printf("\n");
				fflush(stdout);
			}

			for (int i = 0; i < readBytes; i++) {
				currentByte = (uint8_t)addaBufferUart[i];

				// add received byte to buffer
				if (addaPacketBufLen < ADDA_MAX_PACKET_LENGTH) {
					addaPacketBuffer[addaPacketBufLen++] = currentByte;
				} else {
					// buffer full -> remove oldest byte
					memmove(addaPacketBuffer, addaPacketBuffer + 1, ADDA_MAX_PACKET_LENGTH - 1);
					addaPacketBuffer[ADDA_MAX_PACKET_LENGTH - 1] = currentByte;
				}

				int packetBegin = -1;
				int packetEnd = -1;
				int receivedPacketLength = 0; // length of detected packet

				// check if we received enought data to process at least the shortest message (e.g. *2Y#)
				if (addaPacketBufLen >= 3) {
					// check if received character is end of message ('#')
					if (addaPacketBuffer[addaPacketBufLen - 1] == '#') {
						// we received possible end of a message
						packetEnd = addaPacketBufLen - 1;

						// now search begin of message ('*')
						for (uint16_t j = 0; j < ADDA_MAX_PACKET_LENGTH; j++) {
							if (addaPacketBuffer[ADDA_MAX_PACKET_LENGTH - 1 - j] == '*') {
								// found begin of message
								packetBegin = ADDA_MAX_PACKET_LENGTH - 1 - j;
								break;
							}
						}

						receivedPacketLength = (packetEnd - packetBegin + 1);
						if ((packetBegin >= 0) && (packetEnd > 0) && (receivedPacketLength > 0)) {
							// we found a valid answer from the ADDA-boards

							// copy the message including * and # into a new buffer and 0-terminate it
							char* payload = (char*)malloc(sizeof(char) * (receivedPacketLength + 1));
							memcpy(payload, &addaPacketBuffer[packetBegin], receivedPacketLength);
							payload[receivedPacketLength] = '\0';

							answer = payload;
							free(payload);

							// shift remaining bytes by processed amount of data
							memmove(addaPacketBuffer, addaPacketBuffer + receivedPacketLength, addaPacketBufLen - receivedPacketLength);
							addaPacketBufLen -= receivedPacketLength;

							helper->DEBUG_ADDA(DEBUGLEVEL_VERBOSE, "%s --> %s", cmd.c_str(), answer.c_str());
							return answer;
						}
					}
				}
			}
		}
		waitForMessage--;
		usleep(1000); // wait 1 ms
	}

	// No Answer
	helper->DEBUG_ADDA(DEBUGLEVEL_VERBOSE, "%s --> No Answer", cmd.c_str());
	return "";
}

void Adda::SetMute(bool muted) {
		int fd = open("/sys/class/leds/audio_mute/brightness", O_WRONLY);

		if (muted) {
				write(fd, "1", 1);
		} else {
				write(fd, "0", 1);
		}
		close(fd);
}

int8_t Adda::GetBoardId(uint8_t channel) {
		// we have up to 4 boards. Now we have to find the right BoardId for the desired channel

		// TODO: check why the order is 1->3->0->2 and if it could change between X32-types and even startups
		if ((channel >= 1) && (channel <= 8)) {
				return 1;
		}else if ((channel >= 9) && (channel <= 16)) {
				return 3;
		}else if ((channel >= 17) && (channel <= 24)) {
				return 0;
		}else if ((channel >= 25) && (channel <= 32)) {
				return 2;
		}else{
				return -1;
		}
}