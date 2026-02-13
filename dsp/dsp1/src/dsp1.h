/*****************************************************************************
 * dsp1.h
 *****************************************************************************/

#ifndef __DSP1_H__
#define __DSP1_H__

#include "defines.h"

// general includes
#include <stdio.h>     				// Get declaration of puts and definition of NULL
#include <stdint.h>    				// Get definition of uint32_t
#include <assert.h>    				// Get the definition of support for standard C asserts
#include <builtins.h>  				// Get definitions of compiler built-in functions
#include <platform_include.h>      	// System and IOP register bit and address definitions
#include <processor_include.h>	   	// Get definitions of the part being built
#include <services/int/adi_int.h>  	// Interrupt HAndler API header
#include "adi_initialize.h"
#include <math.h>
#include <string.h>
#include <matrix.h>
#include <vector.h>
#include <stats.h>

// include for fir, iir, biquad, fft, etc.
#include <filter.h>                 // vectorized version
//#include <filters.h>              // scalar version for fir, iir, biquad
//#include <trans.h>                // scalar version for fft
#include <window.h>

// includes for hardware-pins
#include <sru.h>
#include <sysreg.h>
#include <signal.h>

// global variables
extern volatile int audioProcessing;
extern volatile int audioReady;
extern volatile bool spiNewRxDataReady;
extern int audioTx_tcb[8][BUFFER_COUNT][4];
extern int audioRx_tcb[8][BUFFER_COUNT][4];
extern uint32_t cyclesAudio;
extern uint32_t cyclesMain;
extern uint32_t cyclesTotal;

typedef struct {
	// filter-coefficients
	double a[3];
	double b[3];
} sLR12;

typedef struct {
	// filter-coefficients
	double a[5];
	double b[5];
} sLR24;

typedef enum {
	GATE_CLOSED,
	GATE_ATTACK,
	GATE_OPEN,
	GATE_HOLD,
	GATE_CLOSING
} gateState;
typedef struct {
	// filter-data from i.MX25
	float value_threshold;
	float value_gainmin;
	float value_coeff_attack;
	float value_hold_ticks;
	float value_coeff_release;

	// online parameters
	short int holdCounter;
	bool closed;
	gateState state;
} sGate;

typedef enum {
	COMPRESSOR_IDLE,
	COMPRESSOR_ATTACK,
	COMPRESSOR_ACTIVE,
	COMPRESSOR_HOLD,
	COMPRESSOR_RELEASE
} compressorState;
typedef struct {
	// filter-data from i.MX25
	float value_threshold;
	float value_ratio;
	float value_coeff_attack;
	float value_hold_ticks;
	float value_coeff_release;

	// online parameters
	int holdCounter;
	bool triggered;
	compressorState state;
} sCompressor;

typedef struct {
	sGate gate;
	sCompressor compressor;
	bool solo;
} sDspChannel;

typedef struct {
	//sCompressor compressor;
	bool solo;
} sMixbusChannel;

struct {
	float samplerate;

	float lowcutCoeff[MAX_CHAN_FULLFEATURED];
	float lowcutStatesInput[MAX_CHAN_FULLFEATURED];
	float lowcutStatesOutput[MAX_CHAN_FULLFEATURED];
	//float highcutCoeff[MAX_CHAN_FULLFEATURED];
	//float highcutStates[MAX_CHAN_FULLFEATURED];

	float gateGainSet[MAX_CHAN_FULLFEATURED];
	float gateGain[MAX_CHAN_FULLFEATURED];
	float gateCoeff[MAX_CHAN_FULLFEATURED];

	float compressorGainSet[MAX_CHAN_FULLFEATURED];
	float compressorGain[MAX_CHAN_FULLFEATURED];
	float compressorCoeff[MAX_CHAN_FULLFEATURED];
	float compressorMakeup[MAX_CHAN_FULLFEATURED];

	float pm peqCoeffs[CHANNELS_WITH_4BD_EQ][5 * MAX_CHAN_EQS]; // store in program memory
	float dm peqStates[CHANNELS_WITH_4BD_EQ][2 * MAX_CHAN_EQS]; // store in data memory

	// volume-settings
	float channelVolume[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS]; // in p.u.
	float channelVolumeSet[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS]; // in p.u.
	#if TEST_MATRIXMULTIPLICATION_MIXBUS == 1
		float channelSendMixbusVolume[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN][MAX_MIXBUS]; // in p.u.
	#else
		float channelSendMixbusVolume[MAX_MIXBUS][MAX_CHAN_FPGA + MAX_DSP2_FXRETURN]; // in p.u.
	#endif
	short channelSendMixbusTapPoint[MAX_MIXBUS][MAX_CHAN_FPGA + MAX_DSP2_FXRETURN];

	#pragma align 8 // align for 2 floats
	float pm channelSendMainLeftVolume[MAX_CHAN_FPGA + MAX_CHAN_DSP2 + MAX_MIXBUS]; // in p.u.
	#pragma align 8
	float pm channelSendMainRightVolume[MAX_CHAN_FPGA + MAX_CHAN_DSP2 + MAX_MIXBUS]; // in p.u.
	#pragma align 8
	float pm channelSendMainSubVolume[MAX_CHAN_FPGA + MAX_CHAN_DSP2 + MAX_MIXBUS]; // in p.u.


/*
	float sendMatrixVolume[MAX_MATRIX][MAX_MIXBUS + 3]; // Mixbus and MainLRS can be sent to matrix
	int sendMatrixTapPoint[MAX_MATRIX][MAX_MIXBUS + 3]; // Mixbus and MainLRS can be sent to matrix
	bool mixbusSolo[MAX_MIXBUS];

	float matrixVolume[MAX_MATRIX];
	bool matrixSolo[MAX_MATRIX];
*/

	float mainVolumeSet[3]; // left, right, sub
	float mainVolume[3]; // left, right, sub
	float mainSendMatrixVolume[MAX_MATRIX];
	short mainSendMatrixTapPoint[MAX_MATRIX];
	bool mainLrSolo;
	bool mainSubSolo;

	short inputRouting[MAX_CHAN_FPGA + MAX_CHAN_DSP2];
	short inputTapPoint[MAX_CHAN_FPGA + MAX_CHAN_DSP2];
	short outputRouting[MAX_CHAN_FPGA + MAX_CHAN_DSP2];
	short outputTapPoint[MAX_CHAN_FPGA + MAX_CHAN_DSP2];
	sDspChannel dspChannel[MAX_CHAN_FPGA + MAX_CHAN_DSP2];
	sMixbusChannel mixbusChannel[MAX_MIXBUS];
	//sMatrixChannel matrixChannel[MAX_MATRIX];

	short monitorChannelTapPoint;
	short monitorMixbusTapPoint;
	short monitorMatrixTapPoint;
	short monitorMainTapPoint;
	short monitorTapPoint;
	float monitorVolume;
	bool soloActive;
} dsp;

enum eBufferIndex {
    TAP_INPUT,
	TAP_PRE_EQ,
	TAP_POST_EQ,
	TAP_PRE_FADER,
	TAP_POST_FADER
};


// function prototypes
/*
static void timerIsr(uint32_t iid, void* handlerArg);
void delay(int i);
*/
void openx32Init(void);

#endif /* __DSP1_H__ */
