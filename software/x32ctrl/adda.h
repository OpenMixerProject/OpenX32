#pragma once

#include "base.h"
#include "ctrl_types.h"
#include "WString.h"
#include "uart.h"
#include "adda-message.h"

#define ADDA_MAX_PACKET_LENGTH 32


class Adda : public X32Base {
    private:
        Uart* uart;

        char addaBufferUart[256]; // buffer for UART-readings
        int addaPacketBufLen;
        char addaPacketBuffer[ADDA_MAX_PACKET_LENGTH];
        uint16_t addaWaitForMessageCounter;

    public:
        Adda(X32BaseParameter* basepar);
        void Init();
        void SetSamplerate(uint32_t samplerate);
        void SetGain(uint8_t boardId, uint8_t channel, float gain, bool phantomPower);
        String SendReceive(const char* cmd, uint16_t timeout);
        String ProcessUartData(bool directRead);
        String ProcessUartData(int bytesToProcess, bool directRead);
        void SetMute(bool muted);
        int8_t GetBoardId(uint8_t channel);
        //void callbackAdda(char* msg);
};


