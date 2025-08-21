#ifndef MIXER_H_
#define MIXER_H_

#include "x32ctrl.h"

#define MAX_BANKS 4
#define MAX_VCHANNELS 100
#define MAX_SURFACECHANNELS 35
#define MAX_NAME_LENGTH 30 + 1 // null termination!
#define VCHANNEL_NOT_SET MAX_VCHANNELS+1

#define VCHANNEL_MAIN 24

typedef struct{
    char name[MAX_NAME_LENGTH];
    uint8_t input;
    bool phantomPower;
    bool phaseInvert;
} s_inputSource;

typedef struct{
    char name[MAX_NAME_LENGTH];
    uint8_t group;
    uint8_t hardwareChannel;
} s_outputDestination;

typedef struct{
    char name[MAX_NAME_LENGTH];
    uint8_t color;
    uint8_t icon;
    bool mute;
    bool solo;
    bool selected;
    // volume in dBfs
    float volume;
    

    // 0 - normal channel
    // 1 - main channel
    uint8_t vChannelType;
    s_inputSource inputSource;
    s_outputDestination outputDestination;
} s_vChannel;

typedef struct{
    char name[MAX_NAME_LENGTH];

    // assing surfaceChannel to vChannel
    //
    // surfaceChannels:
    // 0-7   Board L
    // 8-15  Board M
    // 16-23 Board R
    // 24    Main Fader
    // 25-28 Assing 1-4 (only X32 Full)
    // 29-34 Display Encoder 1-6
    uint8_t surfaceChannel2vChannel[MAX_SURFACECHANNELS];
} s_bank;

typedef struct{
    X32_MODEL model;
    uint8_t activeBank;
    uint8_t activeMode;


    // something was changed - sync surface/gui to mixer state
    bool dirty;

    s_bank bank[4];
    s_vChannel vChannel[MAX_VCHANNELS];    

    uint8_t selectedVChannel;
} s_Mixer;

extern s_Mixer mixer;


void initMixer(X32_BOARD model);
uint8_t mixerSurfaceChannel2vChannel(uint8_t surfaceChannel);
void mixerSetSelect(uint8_t vChannelIndex, bool solo);
void mixerToggleSelect(uint8_t vChannelIndex);
uint8_t mixerGetSelectedvChannel();
void mixerSetSolo(uint8_t vChannelIndex, bool solo);
void mixerToggleSolo(uint8_t vChannelIndex);
void mixerSetMute(uint8_t vChannelIndex, bool solo);
void mixerToggleMute(uint8_t vChannelIndex);
void mixerBanking(X32_BTN p_button);

void mixerSetVolume(uint8_t p_vChannelIndex, float p_volume);

void mixerDebugPrintBank(uint8_t bank);
void mixerDebugPrintvChannels();

#endif