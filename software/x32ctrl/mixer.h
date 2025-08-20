#ifndef MIXER_H_
#define MIXER_H_

#include "x32ctrl.h"

typedef struct{
    char name[10];
    uint8_t input;
    bool phantomPower;
    bool phaseInvert;
} s_inputSource;

typedef struct{
    char name[10];
    uint8_t group;
    uint8_t hardwareChannel;
} s_outputDestination;

typedef struct{
    char name[30];
    bool mute;
    bool solo;
    bool selected;
    s_inputSource inputSource;
    s_outputDestination outputDestination;
} s_vChannel;

typedef struct {
    s_vChannel *vChannel;

    // 0-7   Board L
    // 8-15  Board M
    // 16-23 Board R
    // 24    Main Fader
    // 25-28 Assing 1-4 (only X32 Full)
    // 29-34 Display Encoder 1-6
    uint8_t surfaceChannel;
}s_vChannel2surfaceChannel;

typedef struct{
    s_vChannel2surfaceChannel v2sChannel[25];
} s_bank;

typedef struct{
    uint8_t activeBank;
    uint8_t activeMode;
    s_bank bank[4];
    s_vChannel vChannel[100];    
} s_Mixer;

#endif