#pragma once

#include "base.h"
#include "defines.h"

#include "adda.h"

#define CARD_TYPE_NONE 0
#define CARD_TYPE_XUSB 1
#define CARD_TYPE_XLIVE 2
#define CARD_TYPE_XADAT 3
#define CARD_TYPE_XMADI 4
#define CARD_TYPE_UNKNOWN 255

class Card : X32Base
{
    using enum MP_ID;

    private:
        Adda* adda;

        void XUSB_XLIVE_SetConfig(uint8_t channelparamter, uint source);

    public:
        Card(X32BaseParameter* basepar, Adda* _adda);

        uint type; // 0 = no card, 1 = X-UF/X-USB, 2 = X-LIVE, 3 = X-ADAT, 4 = X-MADI
        uint currentSongNumberChannels; // number of channels (e.g. 16 or 32)
        uint currentSongTotalSeconds;
        uint currentSongPositionSeconds;

        bool XLIVE_Playing = false;
        bool XLIVE_Recording = false;
        bool XLIVE_CardPresent[2] = {false, false};
        uint XLIVE_CardTotalSpaceMB[2] = {0, 0};
        uint XLIVE_CardRemaingSpaceMB[2] = {0, 0};
        uint XLIVE_CardUsedSpaceMB[2] = {0, 0};

        void Init();
        void Sync();
        String SendCommand(String command);
        void ProcessCommand(String command);
        void FlushRxBuffer();

        // X-LIVE specific functions
        bool XLIVE_Stop();
        bool XLIVE_PlayPause();
        bool XLIVE_Seek(uint sampleIndex);
        String XLIVE_RequestToc(uint* numberOfEntries);
        void XLIVE_ReadRemainingCardSpace(uint card);
        void XLIVE_ReadTotalCardSpaceMB(uint card);
        bool XLIVE_SelectInterface(uint option, uint interface);
        bool XLIVE_DeleteSession(String session);
        bool XLIVE_RecordNewSession();
        bool XLIVE_FormatCard();
        bool XLIVE_SelectCard(uint card);
        bool XLIVE_SelectSession(String session);
        String XLIVE_SecondsToSampleIndex(uint seconds);
        uint XLIVE_SampleIndexToSeconds(String sampleIndexHex);
        
        String XLIVE_SessionNameToString(String timecodeHex);
        String XLIVE_DateTimeToSessionName(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second);
   };