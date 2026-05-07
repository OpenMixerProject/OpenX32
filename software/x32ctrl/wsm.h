#pragma once

// standard includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <math.h>
#include <linux/input.h>
#include <vector>

// includes for UDP-communication
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

// includes for reading IP-Address
#include <sys/stat.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "base.h"

class WSM : public X32Base {
    private:

        struct sockaddr_in ServerAddr;
        vector<String> clients;
        uint requestWaitCounter = 0;

    public:

        int UdpHandle;

        WSM(X32BaseParameter* basepar);
        int Init();
        void AddClient(String ip);

        void RequestDataFromClients();

        void SendPush();
        void SendCommand(String command, String ip);
};