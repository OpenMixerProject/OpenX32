#pragma once

#include <cstdint>
#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h> // for FIONREAD
#include <unistd.h>

#include "base.h"
#include "message-base.h"
#include "types.h"

class Uart : public X32Base
 {
    
    private:
        int fd;
        uint8_t calculateChecksum(const char* data, uint16_t len);

    public:
        Uart(X32BaseParameter* basepar);
        int Open(const char* ttydev, uint32_t baudrate, bool raw);
        int Tx(MessageBase* message, bool addChecksum);
        int TxRaw(MessageBase* message);
        int Rx(char* buf, uint16_t bufLen);
        int TxToFPGA(uint16_t cmd, data_64b* data);
};