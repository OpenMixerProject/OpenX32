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

char fpgaBufferUart[256]; // buffer for UART-readings
int fpgaPacketBufLen = 0;
char fpgaPacketBuffer[FPGA_MAX_PACKET_LENGTH];

void fpgaProcessUartData(int bytesToProcess) {
  uint8_t currentByte;

  if (bytesToProcess <= 0) {
    return;
  }

  for (int i = 0; i < bytesToProcess; i++) {
    currentByte = (uint8_t)fpgaBufferUart[i];
    //printf("%02X ", currentByte); // empfangene Bytes als HEX-Wert ausgeben

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
            // we received a valid packet. Offer the received data to fpgaCallback
            uint8_t payloadLen = receivedPacketLength - 4;
            fpgaCallback(&fpgaPacketBuffer[packetBegin + 1], payloadLen);

            // shift remaining bytes by processed amount of data
            memmove(fpgaPacketBuffer, fpgaPacketBuffer + receivedPacketLength, fpgaPacketBufLen - receivedPacketLength);
            fpgaPacketBufLen -= receivedPacketLength;
          }
        }
      }
    }
  }
}
