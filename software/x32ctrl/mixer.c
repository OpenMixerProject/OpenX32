#include "mixer.h"


s_Mixer mixer;

void initMixer(X32_BOARD p_model){

    x32debug("############# InitMixer() #############\n");

    // disable all vChannels
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        mixerSetVChannelDefaults(&mixer.vChannel[i], i, true);
    }

    // empty all banks
    for (uint8_t b = 0; b < MAX_BANKS; b++){
        for (uint8_t sCh = 0; sCh < MAX_SURFACECHANNELS; sCh++){
            mixer.bank[b].surfaceChannel2vChannel[sCh] = VCHANNEL_NOT_SET;
        }
    }

    mixer.model = p_model;

    //##################################################################################
    //#
    //#   create default vChannels (what the user is refering to as "mixer channel")
    //#
    //#   0-31     traditional channels, xlr input 1-32 connected as source, xlr output 1-8 as destination on first 8 vChannels
    //#   32       main bus - not really implemented right now
    //#
    //##################################################################################
    for (int i=0; i<=31; i++)
    {
        s_vChannel *vChannel = &mixer.vChannel[i];
        mixerSetVChannelDefaults(vChannel, i+1, false);

        s_inputSource *input = &vChannel->inputSource;
        sprintf(input->name, "x%d", i);
        input->phantomPower = false;
        input->phaseInvert = false;
        input->input = mixingGetInputSource('x', i);

        if ((i>0) && (i<=7)) {
            s_outputDestination *output = &vChannel->outputDestination;
            sprintf(output->name, "xlr out %d", i);
            output->group = 'x';
            output->hardwareChannel = i;       
        }
    }

    s_vChannel *mainChannel = &mixer.vChannel[VCHANNEL_IDX_MAIN];
    mixerSetVChannelDefaults(mainChannel, VCHANNEL_IDX_MAIN + 1, false);
    sprintf(mainChannel->name, "Main");
    mainChannel->vChannelType = 1; // main channel

    // vChannel0 selected by default
    mixerSetSelect(0, 1);

    //##################################################################################
    //#
    //#   assign vChannels to bank0, 1, (2, 3) - (easy mode - 1:1)
    //#
    //##################################################################################

    if (mixer.model == X32_MODEL_FULL){
        // bank is 16 channels wide
        for (int i = 0; i <=15; i++) {
            // bank0 - vChannel 0-15
            mixer.bank[0].surfaceChannel2vChannel[i] = i;
            // bank1 - vChannel 16-31
            mixer.bank[1].surfaceChannel2vChannel[i] = i+16;
        }
    } 

    if ((mixer.model == X32_MODEL_COMPACT) || (mixer.model == X32_MODEL_COMPACT)){
        // bank is 8 channels wide
        for (int i = 0; i <=7; i++) {
            // bank0 - vChannel 0-7
            mixer.bank[0].surfaceChannel2vChannel[i] = i;
            // bank1 - vChannel 8-15
            mixer.bank[1].surfaceChannel2vChannel[i] = i+8;
            // bank2 - vChannel 16-23
            mixer.bank[2].surfaceChannel2vChannel[i] = i+16;
            // bank3 - vChannel 24-31
            mixer.bank[3].surfaceChannel2vChannel[i] = i+24;
        }
    }

    // Main Fader
    //mixer.bank[0].surfaceChannel2vChannel[24] = 24;
    
    mixer.activeBank = 0;
    mixer.activeMode = 0;

    x32debug("END ############# InitMixer() ############# END \n\n");
}

void mixerSetDirty(X32_DIRTY p_dirtyState){
    mixer.dirty = p_dirtyState;
}

void mixerSetVChannelDefaults(s_vChannel* p_vChannel, uint8_t p_number, bool p_disabled){
    p_vChannel->number = p_number;
    sprintf(p_vChannel->name, "Kanal %d", p_number); // german label :-)
    if (p_disabled){
        p_vChannel->color = 0;
    } else {
        p_vChannel->color = 7;
    }
    p_vChannel->icon = 0;
    p_vChannel->selected = false;
    p_vChannel->solo = false;
    p_vChannel->mute = false;
    p_vChannel->volume = VOLUME_MIN;
    p_vChannel->vChannelType = 0; // normal channel
}

uint8_t mixerSurfaceChannel2vChannel(uint8_t surfaceChannel)
{
    // if (mixer.model == X32_MODEL_COMPACT | mixer.model == X32_MODEL_PRODUCER){
    //     if (surfaceChannel == 17)
    //     {
    //         surfaceChannel= 24;
    //     }
    // }
    return mixer.bank[mixer.activeBank].surfaceChannel2vChannel[surfaceChannel];
}


void mixerSetSelect(uint8_t vChannelIndex, bool select){
    for(uint8_t i=0; i<MAX_VCHANNELS;i++){
        mixer.vChannel[i].selected = false;
    }
    mixer.vChannel[vChannelIndex].selected = select;
    mixer.selectedVChannel=vChannelIndex;
    mixerSetDirty(X32_DIRTY_SELECT);
}

void mixerToggleSelect(uint8_t vChannelIndex){
    mixerSetSelect(vChannelIndex, !mixer.vChannel[vChannelIndex].selected);
}

uint8_t mixerGetSelectedvChannel(){
    return mixer.selectedVChannel;
}

void mixerSetSolo(uint8_t vChannelIndex, bool solo){
    mixer.vChannel[vChannelIndex].solo = solo;
    mixerSetDirty(X32_DIRTY_SOLO);

    //TODOs
    // - activate oder clear ClearSolo
    // - switch monitor source
}

void mixerToggleSolo(uint8_t vChannelIndex){
    mixerSetSolo(vChannelIndex, !mixer.vChannel[vChannelIndex].solo);
}

void mixerSetMute(uint8_t vChannelIndex, bool mute){
    mixer.vChannel[vChannelIndex].mute = mute;
    mixerSetDirty(X32_DIRTY_MUTE);
}

void mixerToggleMute(uint8_t vChannelIndex){
    mixerSetMute(vChannelIndex, !mixer.vChannel[vChannelIndex].mute);
}

void mixerBanking(X32_BTN p_button){
    if (mixer.model == X32_MODEL_FULL){
        switch (p_button){
            case X32_BTN_CH_9_16:
                mixer.activeBank = 0;
                mixer.dirty = true;
                break;
            case X32_BTN_CH_17_32:
                mixer.activeBank = 1;
                mixer.dirty = true;
                break;    
        }
    }
    if (mixer.model == X32_MODEL_COMPACT | mixer.model == X32_MODEL_PRODUCER){
        switch (p_button){
            case X32_BTN_CH_1_8:
                mixer.activeBank = 0;
                mixer.dirty = true;
                break;
            case X32_BTN_CH_9_16:
                mixer.activeBank = 1;
                mixer.dirty = true;
                break;
            case X32_BTN_CH_17_24:
                mixer.activeBank = 2;
                mixer.dirty = true;
                break;
            case X32_BTN_CH_25_32:
                mixer.activeBank = 3;
                mixer.dirty = true;
                break;
        }
    }
}

// volume in dBfs
void mixerSetVolume(uint8_t p_vChannelIndex, float p_volume){
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    mixer.vChannel[p_vChannelIndex].volume = p_volume;
    mixerSetDirty(X32_DIRTY_VOLUME);
}

void mixerDebugPrintBank(uint8_t bank)
{
    x32debug("################# BANK%d ###################\n", bank);
    for (uint8_t i=0;i<MAX_SURFACECHANNELS;i++){
        x32debug("surfaceChannel%d -> vChannel%d\n", i, mixer.bank[bank].surfaceChannel2vChannel[i]);
    }
    x32debug("END ############# BANK ################ END\n");
}

void mixerDebugPrintvChannels(){
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        s_vChannel* vChannel = &mixer.vChannel[i];
        x32debug("vChannel%d: name=%s color=%d icon=%d selected=%d solo=%d mute=%d volume=%2.1F input=%s output=%c%d\n",
                 i, vChannel->name, vChannel->color, vChannel->icon, vChannel->selected, vChannel->solo, vChannel->mute, vChannel->volume,
                 vChannel->inputSource.name, vChannel->outputDestination.group, vChannel->outputDestination.hardwareChannel);
    }
}