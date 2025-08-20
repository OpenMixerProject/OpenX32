#include "mixer.h"


s_Mixer mixer;

void initMixer(X32_BOARD model){

    // create default vChannels (what the user is refering to as "mixer channel")
    for (int i = 0; i <=23; i++)
    {
        s_vChannel *vChannel = &mixer.vChannel[i];
        sprintf(vChannel->name, "Kanal %d", i+1);
        vChannel->selected = false;
        vChannel->solo = false;
        vChannel->mute = false;

        s_inputSource *input = &vChannel->inputSource;
        sprintf(input->name, "x%d", i);
        input->phantomPower = false;
        input->phaseInvert = false;
        input->input = mixingGetInputSource('x', i);

        s_outputDestination *output = &vChannel->outputDestination;
        sprintf(output->name, "output %d", i);
        output->group = 'x';
        output->hardwareChannel = i;
    }

    s_vChannel mainChannel = mixer.vChannel[24];

    mixer.activeBank = 0;
    mixer.activeMode = 0;


    // create bank[0] and assing vChannels (easy mode - 1:1)

    // Board L
    for (int i = 0; i <=7; i++)
    {
        mixer.bank[0].v2sChannel[i].surfaceChannel=i;
        mixer.bank[0].v2sChannel[i].vChannel = &mixer.vChannel[i];
    }
    
    // Board M
    if (model == X32_MODEL_FULL){
        for (int i = 8; i <=15; i++)
        {
            mixer.bank[0].v2sChannel[i].surfaceChannel=i;
            mixer.bank[0].v2sChannel[i].vChannel = &mixer.vChannel[i];
        }
    }

    // Board R
    for (int i = 16; i <=23; i++)
    {
        mixer.bank[0].v2sChannel[i].surfaceChannel=i;
        mixer.bank[0].v2sChannel[i].vChannel = &mixer.vChannel[i];
    }

    // Main Fader
    mixer.bank[0].v2sChannel[24].surfaceChannel=24;
    mixer.bank[0].v2sChannel[24].vChannel = &mixer.vChannel[24];
}
