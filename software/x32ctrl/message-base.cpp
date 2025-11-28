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

#include "message-base.h"

MessageBase::MessageBase(){
    current_length = 0;
}

int MessageBase::AddRawByte(char byte){
    if (current_length >= MAX_MESSAGE_SIZE) {
        fprintf(stderr, "Error: Message buffer overflow when adding byte 0x%02X!\n", byte);
        return -1;
    }
    buffer[current_length++] = byte;
    return 0;
}

int MessageBase::AddDataByte(char byte){
    // check if we have space left in message-buffer (max. 64 bytes for payload)
    if (current_length >= MAX_MESSAGE_SIZE) {
        fprintf(stderr, "Error: Message buffer overflow before adding data byte 0x%02X!\n", byte);
        return -1;
    }

    buffer[current_length++] = byte;

    // if added byte is 0xFE -> add 0xFF (byte-stuffing)
    // check if we have space left for byte-stuffing
    if (byte == 0xFE) {
        if (current_length >= MAX_MESSAGE_SIZE) {
            fprintf(stderr, "Error: Message buffer overflow during stuffing for 0xFE!\n");
            return -1;
        }
        buffer[current_length++] = 0xFF;
    }
    return 0;
}

int MessageBase::AddString(const char* str){
    // check if we have space left in message-buffer (max. 64 bytes for payload)
    if (current_length >= MAX_MESSAGE_SIZE) {
        fprintf(stderr, "Error: Message buffer overflow before adding data!\n");
        return -1;
    }

    // as we do not apply byte-stuffing for ASCII-characters, we can copy the data directly to the buffer
    memcpy(&buffer[current_length], str, strlen(str)); // dst, src, size
    current_length += strlen(str);
    return 0;
}

int MessageBase::AddDataArray(const char* data, uint8_t len){
    // check if we have space left in message-buffer (max. 64 bytes for payload)
    if (current_length >= MAX_MESSAGE_SIZE) {
        fprintf(stderr, "Error: Message buffer overflow before adding data!\n");
        return -1;
    }

    // now step through the data, add it to the message and check if we have to do byte-stuffing
    for (uint8_t i=0; i<len; i++) {
        buffer[current_length++] = data[i];

        // if added byte is 0xFE -> add 0xFF (byte-stuffing)
        // check if we have space left for byte-stuffing
        if (data[i] == 0xFE) {
            if (current_length >= MAX_MESSAGE_SIZE) {
                fprintf(stderr, "Error: Message buffer overflow during stuffing for 0xFE!\n");
                return -1;
            }
            buffer[current_length++] = 0xFF;
        }
    }
    return 0;
}