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

    public:
        Uart(X32BaseParameter* basepar);
        int Open(const char* ttydev, uint32_t baudrate, bool raw);
        int Tx(MessageBase* message);
        int Rx(char* buf, uint16_t bufLen);
        //void MirrorBack();
        void FlushRxBuffer();
};