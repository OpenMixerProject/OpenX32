#pragma once

#include "external.h"
#include "config.h"
#include "x32ctrl_types.h"

#include "helper.h"

#include "vchannel.h"
#include "surface.h"

using namespace std;

#define DEBUG 0

// The string class
class Mixer
{
    private:
        
        Config* config;
        Helper* helper;
        Surface* surface;
        
        uint8_t selectedVChannel;
        uint8_t selectedOutputChannelIndex;


        // solo is (somewhere) activated
        bool solo = false;

        // something was changed - sync surface/gui to mixer state
        uint16_t changed;
        sBankMode modes[3];

        // all virtual - channels / busses / matrix / etc.
        VChannel vchannel[MAX_VCHANNELS];
        sMixerPage pages[MAX_PAGES];

        

    public:
        X32_PAGE activePage;
        uint8_t activeBank_inputFader;
        uint8_t activeBank_busFader;
        uint8_t activeEQ;
        uint8_t activeBusSend;

        sTouchControl touchcontrol;

        Mixer(Config* config);
        void Tick10ms(void);
        void Tick100ms(void);
        void ProcessEvents(void);

};