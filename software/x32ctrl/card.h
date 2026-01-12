#pragma once

#include "base.h"
#include "defines.h"

#include "adda.h"

class Card : X32Base {
    private:
        Adda* adda;

        void XUSB_SetNumberOfChannels(uint8_t channelparameter);
        void XLIVE_SetNumberOfChannelAndMode(uint8_t channelparamter, bool modeUSB);

    public:
        Card(X32BaseParameter* basepar, Adda* _adda);

        void Init();
        void Sync();

        String Card_GetChannelModeText(uint8_t card_channelmode);
        void Card_SendCommand(String command);
        
};