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
    uint8_t dspChannel;
    bool phantomPower; // TODO dont do it localy, use mixing data
    float gain;        // TODO dont do it localy, use mixing data
    bool phaseInvert;  // TODO dont do it localy, use mixing data
} s_inputSource;

typedef struct {
        // user-settings
        int type; // 0=allpass, 1=peak, 2=low-shelf, 3=high-shelf, 4=bandpass, 5=notch, 6=lowpass, 7=highpass
        float fc; // center-frequency of PEQ
        float Q; // Quality of PEQ (bandwidth)
        float gain; // gain of PEQ

        // filter-coefficients
        float a[3];
        float b[3];
} sPEQ;

typedef struct {
        // user-settings
        float fc; // cutoff-frequency for high- or lowpass
        bool isHighpass; // choose if Highpass or Lowpass

        // filter-coefficients
        float a[3];
        float b[3];
} sLR12;

typedef struct {
        // user-settings
        float fc; // cutoff-frequency for high- or lowpass
        bool isHighpass; // choose if Highpass or Lowpass

        // filter-coefficients
        float a[5];
        float b[5];
} sLR24;

typedef struct {
        // user-settings
        float threshold; // value between -80 dBfs (no gate) and 0 dBfs (full gate)
        float range; // value between 48dB (full range) and 3dB (minimum effect)
        float attackTime_ms;
        float holdTime_ms;
        float releaseTime_ms;

        // filter-data
        float value_threshold;
        float value_gainmin;
        float value_coeff_attack;
        float value_hold_ticks;
        float value_coeff_release;
} sGate;

typedef struct {
        // user-settings
        float threshold; // value between 0 dBfs (no compression) and -80 dBfs (full compression)
        float ratio; // value between 0=oo:1, 1=1:1, 2=2:1, 4=4:1, 8=8:1, 16=16:1, 32=32:1, 64=64:1
        float makeup; // value between 0dB, 6dB, 12dB, 18dB, 24dB, 30dB, 36dB, 42dB, 48dB
        float attackTime_ms;
        float holdTime_ms;
        float releaseTime_ms;

        // filter-data
        float value_threshold;
        float value_ratio;
        float value_makeup;
        float value_coeff_attack;
        float value_hold_ticks;
        float value_coeff_release;
} sCompressor;

#include "x32ctrl.h"

typedef struct {
  float lowCutFrequency;
  sGate gate;
  sPEQ peq[MAX_CHAN_EQS];
  sCompressor compressor;
  float volumeLR; // volume in dBfs
  float volumeSub; // volume in dBfs
  float balance;
  float sendMixbus[16];
  uint8_t sendMixbusTapPoint[16];
  bool muted;
  bool solo;
} sDspChannel;

typedef struct {
  sPEQ peq[MAX_CHAN_EQS];
  sCompressor compressor;
  float volumeLR;
  float volumeSub;
  float balance;
  float sendMatrix[6];
  uint8_t sendMatrixTapPoint[6];
  bool muted;
  bool solo;
} sMixbusChannel;

typedef struct {
  sPEQ peq[MAX_CHAN_EQS];
  sCompressor compressor;
  float volume;
  bool muted;
  bool solo;
} sMatrixChannel;

#pragma pack(push, 1)
// size of sRouting must be multiplier of 8 to fit into 64-bit-value
typedef struct __attribute__((packed,aligned(1))) {
  uint8_t xlr[16];
  uint8_t p16[16];
  uint8_t card[32];
  uint8_t aux[8];
  uint8_t dsp[40];
  uint8_t aes50a[48]; // not used in FPGA at the moment
  uint8_t aes50b[48]; // not used in FPGA at the moment
} sInputRouting;
#pragma pack(pop)

typedef struct {
  float gainXlr[32];
  float gainAes50a[48];
  float gainAes50b[48];

  bool phantomPowerXlr[32];
  bool phantomPowerAes50a[48];
  bool phantomPowerAes50b[48];
} sPreamps;

typedef struct {
  sInputRouting routing; // controls the 40 audio-channels into the DSP
  sPreamps preamps;

  sDspChannel dspChannel[40];
  sMixbusChannel mixbusChannel[16];
  sMatrixChannel matrixChannel[6];

  float mainLRVolume;
  float mainSubVolume;
  float mainBalance;

  float mainSendMatrix[6];
  uint8_t mainSendMatrixTapPoint[6];

  float monitorVolume;
  uint8_t monitorTapPoint;

  uint8_t outputSource[40]; // controls the 40 audio-channels out of the DSP
  uint8_t outputTapPoint[40]; // controls the tap-point (pre/post fader/eq)
  float samplerate;
} sDsp;

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
    float sends[16];

    float volumeLR; // volume in dBfs
    float volumeSub; //volume in dBfs
    float balance; // balance between -100 .. 0 .. +100

    // 0 - normal channel
    // 1 - main channel
    uint8_t vChannelType;
    s_inputSource inputSource;
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
    uint8_t activeEQ;
    uint8_t activeBusSend;
    uint8_t selectedVChannel;
    uint8_t selectedOutputChannelIndex;
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
