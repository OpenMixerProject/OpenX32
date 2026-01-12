#pragma once

#include "base.h"
#include "types.h"
#include "WString.h"
#include "uart.h"
#include "adda-message.h"

#define ADDA_MAX_PACKET_LENGTH 32
#define ADDA_MAX_BOARDS 8

#define ADDA_BOARD_XLR_IN_0 0
#define ADDA_BOARD_XLR_IN_1 1
#define ADDA_BOARD_XLR_IN_2 2
#define ADDA_BOARD_XLR_IN_3 3
#define ADDA_BOARD_XLR_OUT_0 4
#define ADDA_BOARD_XLR_OUT_1 5
#define ADDA_BOARD_EXPANSION 8
#define ADDA_BOARD_EXPANSION_RETURN 9


class Adda : public X32Base {
    private:
        Uart* uart;

        char addaBufferUart[256]; // buffer for UART-readings
        int addaPacketBufLen;
        char addaPacketBuffer[ADDA_MAX_PACKET_LENGTH];

        String addaBoards[10];

        void Send(String cmd);

    public:
        Adda(X32BaseParameter* basepar);
        void Init();

        String SendReceive(String cmd);

        bool HasXlrIn0();
        bool HasXlrIn1();
        bool HasXlrIn2();
        bool HasXlrIn3();
        bool HasXlrOut0();
        bool HasXlrOut1();
        bool HasExpansion();

        String GetXlrIn0();
        String GetXlrIn1();
        String GetXlrIn2();
        String GetXlrIn3();
        String GetXlrOut0();
        String GetXlrOut1();
        String GetExpansion();

        void SetSamplerate(uint32_t samplerate);
        String SetGain(uint8_t boardId, uint8_t channel, float gain, bool phantomPower);

        void SetMuteAll(bool muted);
        int8_t GetXlrInBoardId(uint8_t channel);


};


