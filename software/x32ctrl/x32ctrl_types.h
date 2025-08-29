#ifndef X32CTRL_TYPES_H_
#define X32CTRL_TYPES_H_

// define own datatypes
typedef union {
    uint64_t u64;
    int64_t s64;
    uint32_t u32[2];
    int32_t s32[2];
    uint16_t u16[4];
    int16_t s16[4];
    uint8_t u8[8];
    int8_t s8[8];
    double d;
} data_64b;

typedef union {
    uint32_t u32;
    int32_t s32;
    uint16_t u16[2];
    int16_t s16[2];
    uint8_t u8[4];
    int8_t s8[4];
    float f;
} data_32b;

typedef union {
    uint16_t u16;
    int16_t s16;
    uint8_t u8[2];
    int8_t s8[2];
} data_16b;


typedef struct {
    char buffer[MAX_MESSAGE_SIZE];
    size_t current_length;
} messageBuilder;


typedef struct{
    X32_BTN button;
    uint16_t buttonNr;
} sButtonInfo;

typedef struct{
    X32_ENC encoder;
    uint16_t encoderNr;
} sEncoderInfo;

typedef struct{
    // 0-125
    uint8_t x;
    // 0-63
    uint8_t y;
    // icon = 0xA0 (none), 0xA1 ... 0xE9
    uint8_t icon;
} sLCDIcon;

typedef struct{
    // sizeA/B = 0x00 (small) or 0x20 (large)
    uint8_t size;
    // 0-125
    uint8_t x;
    // 0-63
    uint8_t y;
    char text[255];
} sLCDText;

typedef struct{
    uint8_t boardId;
    uint8_t index;
    // 4 bits (bit 0=R, 1=G, 2=B, 3=Inverted)
    // 
    uint8_t color;
    sLCDIcon icon;
    // text(s) to display, max 7
    sLCDText texts[7];
} sLCDData;


// TouchControl Basic Plus - one fader at a time is touchcontrolled
typedef struct{
    uint8_t value;
    X32_BOARD board;
    uint8_t faderIndex;
} s_TouchControl;


typedef struct{
    uint8_t group;
    uint8_t hardwareChannel;
    bool phantomPower; // TODO dont do it localy, use mixing data
    float gain;        // TODO dont do it localy, use mixing data
    bool phaseInvert;  // TODO dont do it localy, use mixing data
} s_inputSource;

typedef struct{
    uint8_t group;
    uint8_t hardwareChannel;
} s_outputDestination;

// virtual mixer channel
typedef struct{
    uint8_t index;
    // indicates, which data has changes and need to get synced
    uint16_t changed;
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
    X32_PAGE pageEnum;
    X32_PAGE nextPage;
    X32_PAGE prevPage;    
} s_mixerPage;

typedef struct{
    X32_MODEL model;
    X32_SURFACE_MODE activeMode;
    uint8_t activeBank_inputFader;
    uint8_t activeBank_busFader;
    uint8_t selectedVChannel;
    X32_PAGE activePage;

    // solo is (somewhere) activated
    bool solo;

    // something was changed - sync surface/gui to mixer state
    uint16_t changed;

    s_TouchControl touchcontrol;

    s_bankMode modes[3];

    // all virtual - channels / busses / matrix / etc.
    s_vChannel vChannel[MAX_VCHANNELS];
    s_mixerPage pages[MAX_PAGES];    
} s_Mixer;

#endif
