#ifndef MIXER_H_
#define MIXER_H_

#include "x32ctrl.h"

#define MAX_VCHANNELS 80
#define MAX_NAME_LENGTH 30 + 1 // null termination!
#define VCHANNEL_NOT_SET MAX_VCHANNELS+1

#define VCHANNEL_IDX_MAIN 32

typedef enum {
    // Standard Layout like X32 Original
    X32_SURFACE_MODE_BANKING_X32,

    // User Layout
    X32_SURFACE_MODE_BANKING_USER,
} X32_SURFACE_MODE;

#define X32_DIRTY_NONE      0b0000000000000000
#define X32_DIRTY_ALL       0b1111111111111111
#define X32_DIRTY_ROUTING   0b0000000000000001
#define X32_DIRTY_VOLUME    0b0000000000000010
#define X32_DIRTY_SELECT    0b0000000000000100
#define X32_DIRTY_SOLO      0b0000000000001000
#define X32_DIRTY_MUTE      0b0000000000010000
#define X32_DIRTY_BANKING   0b0000000000100000
#define X32_DIRTY_LCD       0b0000000001000000
#define X32_DIRTY_PHANTOM   0b0000000010000000
#define X32_DIRTY_PHASE_INVERT  0b0000000100000000

// TouchControl Basic Plus - one fader at a time is touchcontrolled
typedef struct{
    uint8_t value;
    X32_BOARD board;
    uint8_t faderIndex;
} s_TouchControl;


typedef struct{
    uint8_t group;
    uint8_t hardwareChannel;
    bool phantomPower;
    float gain;
    bool phaseInvert;
} s_inputSource;

typedef struct{
    uint8_t group;
    uint8_t hardwareChannel;
} s_outputDestination;

typedef struct{

    uint8_t index;
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
    uint8_t surfaceChannel2vChannel[35];
} s_bank;

typedef struct{
    char name[MAX_NAME_LENGTH];

    // 4 banks on X32 Full, 8 banks on X32 Compact/Producer
    s_bank inputBanks[8];

    s_bank busBanks[4];
} s_bankMode;

typedef struct{
    X32_MODEL model;
    X32_SURFACE_MODE activeMode;
    uint8_t activeBank_inputFader;
    uint8_t activeBank_busFader;
    uint8_t selectedVChannel;
    
    // solo is (somewhere) activated
    bool solo;

    // something was changed - sync surface/gui to mixer state
    uint16_t dirty;
    s_TouchControl touchcontrol;

    s_bankMode modes[3];

    // all virtual - channels / busses / matrix / etc.
    s_vChannel vChannel[MAX_VCHANNELS];    
} s_Mixer;

extern s_Mixer mixer;

void initMixer(X32_MODEL model);
bool mixerIsModelX32Full();
bool mixerIsModelX32CompacrOrProducer();

void mixerSetVChannelDefaults(s_vChannel* p_vChannel, uint8_t p_vChannelIndex, bool p_disabled);

void mixerSetAllDirty(void);
void mixerSetVolumeDirty(void);
void mixerSetSelectDirty(void);
void mixerSetSoloDirty(void);
void mixerSetMuteDirty(void);
void mixerSetLCDDirty(void);
void mixerSetBankingDirty(void);
void mixerSetRoutingDirty(void);
void mixerSetPhantomDirty(void);
void mixerSetPhaseInvertDirty(void);
void mixerSetDirtClean(void);
bool mixerIsDirty(void);
bool mixerIsVolumeDirty(void);
bool mixerIsSelectDirty(void);
bool mixerIsSoloDirty(void);
bool mixerIsMuteDirty(void);
bool mixerIsLCDDirty(void);
bool mixerIsBankingDirty(void);
bool mixerIsRoutingDirty(void);
bool mixerIsPhantomDirty(void);
bool mixerIsPhaseInvertDirty(void);

void mixerTouchControllTick(void);
bool mixerTouchcontrolCanSetFader(X32_BOARD p_board, uint8_t p_faderIndex);

uint8_t mixerSurfaceChannel2vChannel(uint8_t surfaceChannel);

void mixerSurfaceButtonPressed(X32_BOARD p_board, uint8_t p_index, uint16_t p_value);
uint8_t mixerSurfaceSelectSoloMuteButtonGetvChannel(X32_BOARD p_board, uint16_t p_buttonIndex);
void mixerSurfaceFaderMoved(X32_BOARD p_board, uint8_t p_faderIndex, uint16_t p_faderValue);

void mixerSetSelect(uint8_t vChannelIndex, bool solo);
void mixerToggleSelect(uint8_t vChannelIndex);
bool mixerIsSoloActivated(void);
void mixerClearSolo(void);

void mixerSetPhantom(uint8_t vChannelIndex, bool p_phantom);
void mixerTogglePhantom(uint8_t vChannelIndex);
void mixerSetPhaseInvert(uint8_t vChannelIndex, bool p_phaseInvert);
void mixerTogglePhaseInvert(uint8_t vChannelIndex);

s_vChannel* mixerGetSelectedvChannel(void);

void mixerSetSolo(uint8_t vChannelIndex, bool solo);
void mixerToggleSolo(uint8_t vChannelIndex);
void mixerSetMute(uint8_t vChannelIndex, bool solo);
void mixerToggleMute(uint8_t vChannelIndex);
void mixerBanking(X32_BTN p_button);

void mixerSetVolume(uint8_t p_vChannelIndex, float p_volume);

void mixerShowPage(X32_PAGE page);


bool mixerIsModelX32Full(void);
bool mixerIsModelX32CompacrOrProducer(void);
bool mixerIsModelX32Core(void);

void mixerDebugPrintBank(uint8_t bank);
void mixerDebugPrintBusBank(uint8_t bank);
void mixerDebugPrintvChannels(void);

#endif