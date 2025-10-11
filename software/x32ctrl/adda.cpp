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
#include "uart.h"
#include "mixer.h"

char addaBufferUart[256]; // buffer for UART-readings
int addaPacketBufLen = 0;
char addaPacketBuffer[ADDA_MAX_PACKET_LENGTH];
uint16_t addaWaitForMessageCounter = 0;

void addaInit(uint32_t samplerate) {
  // send identification-commands to all possible boards (not sure if this is correct for smaller X32)
  addaSendReceive("*0I#", 2000); // ADDA-Board
  addaSendReceive("*1I#", 2000); // ADDA-Board
  addaSendReceive("*2I#", 2000); // ADDA-Board
  addaSendReceive("*3I#", 2000); // ADDA-Board
  addaSendReceive("*4I#", 2000); // ADDA-Board
  addaSendReceive("*5I#", 2000);  // ADDA-Board
  addaSendReceive("*8I#", 2000); // Expansion-Card
  addaSendReceive("*8R#", 2000); // Expansion-Card

  // -1=unknown, 0=X32, 1=X32Compact, 2=X32Producer, 3=X32Rack, 4=X32Core
  // set gains to -2dB and turnoff phantom-power
  addaSendReceive("*1G00000#*1G00000#*1G10000#*1G10000#*1G20000#*1G20000#*1G30000#*1G30000#*1G40000#*1G40000#*1G50000#*1G50000#*1G60000#*1G60000#*1G70000#*1G70000#", 2000);
  addaSendReceive("*3G00000#*3G00000#*3G10000#*3G10000#*3G20000#*3G20000#*3G30000#*3G30000#*3G40000#*3G40000#*3G50000#*3G50000#*3G60000#*3G60000#*3G70000#*3G70000#", 2000);
  addaSendReceive("*0G00000#*0G00000#*0G10000#*0G10000#*0G20000#*0G20000#*0G30000#*0G30000#*0G40000#*0G40000#*0G50000#*0G50000#*0G60000#*0G60000#*0G70000#*0G70000#", 2000);
  addaSendReceive("*2G00000#*2G00000#*2G10000#*2G10000#*2G20000#*2G20000#*2G30000#*2G30000#*2G40000#*2G40000#*2G50000#*2G50000#*2G60000#*2G60000#*2G70000#*2G70000#", 2000);

  // initialize the 8ch-input-boards
  addaSendReceive("*0R:W018B#", 2000);
  addaSendReceive("*1R:W018B#", 2000);
  addaSendReceive("*2R:W018B#", 2000);
  addaSendReceive("*3R:W018B#", 2000);

  // initialize the 8ch-output-boards
  addaSendReceive("*4R:W0281C0:W0800:W0280#", 2000);
  addaSendReceive("*5R:W0281C0:W0800:W0280#", 2000);

  if (samplerate == 44100) {
    // set all boards to 48kHz
    addaSendReceive("*8C40U#", 2000);
    addaSendReceive("*8R8#*8C40U#", 2000);
    addaSendReceive("*8C40U#", 2000);
  } else if (samplerate == 48000) {
    // set all boards to 48kHz
    addaSendReceive("*8C80U#", 2000);
    addaSendReceive("*8R8#*8C80U#", 2000);
    addaSendReceive("*8C80U#", 2000);
  } else if (samplerate == 96000) {
    // currently unknown, but the X32 is able to handle 96kHz for sure
  } else {
    // unsupported sample-rate
  }

  // and send some more commands (no information about the usage up to now)
  addaSendReceive("*9R0#*9G0#", 2000);
  addaSendReceive("*9AF#", 2000);
  addaSendReceive("*9N0#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9N0#", 2000);
  usleep(20000);
}

void addaSetSamplerate(uint32_t samplerate) {
  if (samplerate == 44100) {
    addaSendReceive("*8R#*8C40U#", 2000);
  } else if (samplerate == 48000) {
    addaSendReceive("*8R#*8C80U#", 2000);
  } else if (samplerate == 96000) {
    // currently unknown, but the X32 is able to handle 96kHz for sure
  } else {
    // unsupported sample-rate
  }

  // send initialization-commands to all boards
  addaSendReceive("*9R0#", 2000);
  addaSendReceive("*9AF#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9AN#", 2000);
  addaSendReceive("*9N0#", 2000);
}

void addaSetGain(uint8_t boardId, uint8_t channel, float gain, bool phantomPower) {
  x32debug("addaSetGain(%d, %d, %f, %d)\n", boardId, channel, gain, phantomPower);

  messageBuilderInit(&message);

  messageBuilderAddRawByte(&message, '*');
  messageBuilderAddRawByte(&message, boardId + 0x30); // convert to ASCII-number
  messageBuilderAddRawByte(&message, 'G');
  messageBuilderAddRawByte(&message, (channel-1) + 0x30); // convert to ASCII-number

  // convert gain-value between -2dB and +45.5dB to three-digit value
  char buf[4];
  uint8_t gainHardware = (uint8_t)((gain + 2.0f)/2.5f); // gain is between -2dB and +45.5dB in 2.5dB steps
  sprintf(buf, "%03d", gainHardware);
  //valueToBinaryString(gain, &buf);
  messageBuilderAddString(&message, &buf[0]);

  if (phantomPower) {
    messageBuilderAddRawByte(&message, '1');
  }else{
    messageBuilderAddRawByte(&message, '0');
  }
  messageBuilderAddRawByte(&message, '#');

  uartTx(&fdAdda, &message, false);
}

String addaSendReceive(char* cmd, uint16_t timeout) {
  x32debug("addaSendReceive(%s)\n", cmd);
  messageBuilderInit(&message);
  messageBuilderAddString(&message, cmd);

  uartTx(&fdAdda, &message, false);

  // TODO: sometime no data is received on all platforms. Check the root of this problem and reactivate this function
  return "";
/*
  // check if we have to wait for the answer (Workaround: the rack seems to have different behaviour here)
  if (timeout > 0) {
    addaWaitForMessageCounter = timeout;
    while (addaWaitForMessageCounter > 0) {
      x32debug("addaWaitForMessageCounter: %d\n", addaWaitForMessageCounter);
      uint16_t readBytes = uartRx(&fdAdda, &addaBufferUart[0], sizeof(addaBufferUart));
      if (readBytes > 0) {
        addaWaitForMessageCounter = 0;
        return addaProcessUartData(readBytes, true);
      }
      addaWaitForMessageCounter--;
      usleep(1000); // wait 1ms
    }
  }else{
    return "";
  }
*/
};

String addaProcessUartData(int bytesToProcess, bool directRead) {
  uint8_t currentByte;
  String answer;

  if (bytesToProcess <= 0) {
    return "";
  }

  x32debug("addaProcessUartData()\n");

  for (int i = 0; i < bytesToProcess; i++) {
    currentByte = (uint8_t)addaBufferUart[i];
    x32debug("%02X ", currentByte); // empfangene Bytes als HEX-Wert ausgeben

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

          // call the callback to do something with the received message
          callbackAdda(payload);
		  answer = payload;
          free(payload);

          // shift remaining bytes by processed amount of data
          memmove(addaPacketBuffer, addaPacketBuffer + receivedPacketLength, addaPacketBufLen - receivedPacketLength);
          addaPacketBufLen -= receivedPacketLength;
        }
      }
    }
  }
  x32debug("\n");

  if (directRead) {
    return answer;
  }else{
    return "";
  }
}

void addaSetMute(bool muted) {
    int fd = open("/sys/class/leds/audio_mute/brightness", O_WRONLY);

    if (muted) {
        write(fd, "1", 1);
    } else {
        write(fd, "0", 1);
    }
    close(fd);
}

int8_t addaGetBoardId(uint8_t channel) {
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
