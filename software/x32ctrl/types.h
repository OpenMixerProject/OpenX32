#pragma once

#include <stdint.h>

#include <map>
#include <set>

#include "defines.h"
#include "enum.h"
#include "WString.h"

#include "mixerparameter.h"

using namespace std;

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

typedef struct{
    X32_ENC encoder;
    uint16_t encoderNr;
} sEncoderInfo;


// TouchControl Basic Plus - one fader at a time is touchcontrolled
typedef struct{
    uint8_t value;
    X32_BOARD board;
    uint8_t faderIndex;
} sTouchControl;

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
	int type; // 0=allpass, 1=peak, 2=low-shelf, 3=high-shelf, 4=bandpass, 5=notch, 6=lowpass, 7=highpass
	float fc; // center-frequency of PEQ
	float Q; // Quality of PEQ (bandwidth)
	float gain; // gain of PEQ

	// filter-coefficients
	float a[3];
	float b[3];
} sPEQ;

// values stored in config
typedef struct {
	//uint8_t input; // controls the 40 audio-channels into the DSP
	//DSP_TAP inputTapPoint; // controls the tap-point (pre/post fader/eq)

	sPEQ peq[MAX_CHAN_EQS];
	float sendMixbus[16];
	uint8_t sendMixbusTapPoint[16];
} sDspChannel;

typedef struct {
	uint8_t input; // controls the 40 audio-channels out of the DSP
	DSP_TAP tapPoint; // controls the tap-point (pre/post fader/eq)
} sDspOutchannel;

typedef struct {
	uint8_t input; // controls which channel should be routed to DSP2 (FX)
	DSP_TAP tapPoint; // controls the tap-point (pre/post fader/eq)
} sFxChannel;

typedef struct {
	uint8_t outputSource; // controls which channel should be routed to DSP2 (FX)
	uint8_t outputTapPoint; // controls the tap-point (pre/post fader/eq)
} sDsp2AuxChannel;

typedef struct {
	float volumeLR;
	float volumeSub;
	float balance;
	float sendMatrix[6];
	uint8_t sendMatrixTapPoint[6];
	bool muted;
	bool solo;
} sMixbusChannel;

typedef struct {
	float volume;
	bool muted;
	bool solo;
} sMatrixChannel;

typedef struct {
	float volume;
	float balance;
	float sendMatrix[6];
	uint8_t sendMatrixTapPoint[6];
	bool muted;
	bool solo;

	float meterPu[2]; // meter information in p.u.
	uint32_t meter[2];
	uint32_t meterDecay[2]; // meter information with decay
	uint8_t meterPeakIndex[2];
	uint8_t meterPeakHoldTimer[2]; // will be updated every 100ms. On 0 the current value will be used
	uint8_t meterPeakDecayTimer[2];
	uint32_t meterInfo[2];
} sMainChannel;

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

// values only for runtime use
typedef struct {
	float meterPu; // meter information in p.u.
	uint32_t meter;

	uint32_t meterDecay; // meter information with decay

	uint8_t meterPeak6Index;
	uint8_t meterPeak6HoldTimer; // will be updated every 100ms. On 0 the current value will be used
	uint8_t meterPeak6DecayTimer;
	uint8_t meter6Info; // compatible to MeterLED on surface: 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)

	uint8_t meterPeak8Index;
	uint8_t meterPeak8HoldTimer; // will be updated every 100ms. On 0 the current value will be used
	uint8_t meterPeak8DecayTimer;
	uint8_t meter8Info; // compatible to MeterLED on surface
} srDspChannel;


typedef struct {
	String label;	
} sDisplayEncoder;

typedef map<MP_ID, set<uint>>			mixerparameter_changed_t;
//typedef map<MP_ID, Mixerparameter*>		mixerparameter_map_t;
//typedef Mixerparameter[]*		mixerparameter_map_t;