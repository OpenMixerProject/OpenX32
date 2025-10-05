#pragma once

#include "constants.h"

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
} sTouchControl;

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

        // online parameter
        float gain;
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

        // online parameter
        float gain;
} sCompressor;

typedef struct {
  uint8_t inputSource; // controls the 40 audio-channels into the DSP
  uint8_t inputTapPoint; // controls the tap-point (pre/post fader/eq)
  uint8_t outputSource; // controls the 40 audio-channels out of the DSP
  uint8_t outputTapPoint; // controls the tap-point (pre/post fader/eq)

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
  float meterPu; // meter information in p.u.
  uint8_t meterInfo; // compatible to MeterLED on surface: 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
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

typedef struct {
  uint8_t inputSource; // controls which channel should be routed to DSP2 (FX)
} sFxChannel;

typedef struct {
  uint8_t inputSource; // controls which channel should be routed to DSP2 (FX)
} sDsp2AuxChannel;

typedef struct {
  float volume;
  float balance;
  float sendMatrix[6];
  uint8_t sendMatrixTapPoint[6];
  bool muted;
  bool solo;
  float meterPu[2]; // meter information in p.u.
  uint32_t meterInfo[2];
} sMainChannel;

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
} sFpgaRouting;
#pragma pack(pop)

typedef struct {
  float gainXlr[32];
  float gainAes50a[48];
  float gainAes50b[48];

  bool phantomPowerXlr[32];
  bool phantomPowerAes50a[48];
  bool phantomPowerAes50b[48];

  bool phaseInvertXlr[32];
  bool phaseAes50a[32];
  bool phaseAes50b[32];
} sPreamps;

typedef struct {
  float samplerate;

  sDspChannel dspChannel[40];
  sMixbusChannel mixbusChannel[16];
  sMatrixChannel matrixChannel[6];
  sFxChannel fxChannel[16];
  sDsp2AuxChannel dsp2AuxChannel[8];
  sMainChannel mainChannelLR;
  sMainChannel mainChannelSub;
  float volumeFxReturn[8];
  float volumeDca[8];

  float volumeSpecial;
  float monitorVolume;
  uint8_t monitorTapPoint;

  // status messages
  float dspLoad[2];
  float dspVersion[2];
  uint32_t dataToRead[2];
} sDsp;

typedef struct{
    char name[MAX_NAME_LENGTH];

    // assing surfaceChannel to VChannel
    //
    // surfaceChannels:
    // 0-7   Board L
    // 8-15  Board M
    // 16-23 Board R
    // 24    Main Fader
    // 25-28 Assing 1-4 (only X32 Full)
    // 29-34 Display Encoder 1-6
    uint8_t surfaceChannel2VChannel[35];
} sBank;

typedef struct{
    char name[MAX_NAME_LENGTH];

    // 4 banks on X32 Full, 8 banks on X32 Compact/Producer
    sBank inputBanks[8];
    sBank busBanks[4];
} sBankMode;

typedef struct{
    X32_PAGE pageEnum;
    X32_PAGE nextPage;
    X32_PAGE prevPage;
} sMixerPage;