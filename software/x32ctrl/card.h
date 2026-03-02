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

        bool XLIVE_Playing = false;
        bool XLIVE_Recording = false;

        void XUSB_XLIVE_SetConfig(uint8_t channelparamter, uint source);

    public:
        Card(X32BaseParameter* basepar, Adda* _adda);

        uint type; // 0 = no card, 1 = X-UF/X-USB, 2 = X-LIVE, 3 = X-ADAT, 4 = X-MADI
        uint currentSongNumberChannels; // number of channels (e.g. 16 or 32)
        uint currentSongTotalSeconds;

        void Init();
        void Sync();
        String SendCommand(String command);

        // X-LIVE specific functions
        bool XLIVE_Stop();
        bool XLIVE_PlayPause();
        bool XLIVE_Seek(uint sampleIndex);
        String XLIVE_RequestToc(uint* numberOfEntries);
        uint XLIVE_Remaining();
        uint XLIVE_Total();
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