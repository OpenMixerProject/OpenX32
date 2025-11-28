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
#include "mixer.h"

class XRemote : public X32Base {
    private:
        struct sockaddr_in ServerAddr;
        char TxMessage[450]; // the largest binary blob will take up to 20+(70+1))*4 bytes = 408 bytes
        int counter = 0;
    public:
        int UdpHandle;
        struct sockaddr_in ClientAddr;

        XRemote(X32BaseParameter* basepar);
        int8_t Init();
        void AnswerInfo();
        void AnswerXInfo();
        void AnswerStatus();
        void SetFader(String type, uint8_t ch, float value_pu);
        void SetPan(uint8_t ch, float value_pu);
        void SetMainFader(float value_pu);
        void SetMainPan(float value_pu);
        void SetName(uint8_t ch, String name);
        void SetColor(uint8_t ch, int32_t color);
        void SetSource(uint8_t ch, int32_t source);
        void SetIcon(uint8_t ch, int32_t icon);
        void SetCard(uint8_t card);
        void SetMute(uint8_t ch, uint8_t muted);
        void SetSolo(uint8_t ch, uint8_t solo);
        void UpdateMeter(Mixer* mixer);
        void SendUdpPacket(char* buffer, uint16_t size);
        void SendBasicMessage(const char* cmd, char type, char format, char* value);
        uint16_t sprint(char* bd, uint16_t index, char format, const char* bs);
        uint16_t fprint(char* bd, uint16_t index, char* text, char format, char* bs);
};