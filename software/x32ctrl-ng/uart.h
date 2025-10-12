#pragma once

#include <cstdint>
#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h> // for FIONREAD
#include <unistd.h>

#include "message.h"

class Uart{
    
    int fd;
    uint8_t calculateChecksum(const char* data, uint16_t len);

    public:
        int Open(char* ttydev, uint32_t baudrate, bool raw);
        int Tx(Message* message, bool addChecksum);
        int Rx(char* buf, uint16_t bufLen);
};