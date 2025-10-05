#pragma once

#include "external.h"
#include "x32config.h"
#include "x32ctrl_types.h"

#include "vchannel.h"

// The string class
class Mixer
{
    private:
        
        X32Config config;

        uint8_t selectedVChannel;
        uint8_t selectedOutputChannelIndex;


        // solo is (somewhere) activated
        bool solo = false;

        // something was changed - sync surface/gui to mixer state
        uint16_t changed;
        sBankMode modes[3];

        // all virtual - channels / busses / matrix / etc.
        VChannel vChannel[MAX_VCHANNELS];
        sMixerPage pages[MAX_PAGES];

    public:
        X32_PAGE activePage;
        uint8_t activeBank_inputFader;
        uint8_t activeBank_busFader;
        uint8_t activeEQ;
        uint8_t activeBusSend;

        sTouchControl touchcontrol;

        Mixer(void);
        

};