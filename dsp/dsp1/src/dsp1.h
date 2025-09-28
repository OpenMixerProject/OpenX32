/*****************************************************************************
 * dsp1.h
 *****************************************************************************/

#ifndef __DSP1_H__
#define __DSP1_H__

#define DSP_VERSION				0.02

#define SDRAM_START  			0x00200000	// start address of SDRAM
#define SDRAM_SIZE	 			0x00400000	// size of SDRAM in 32-bit words (16 MiB)

#define MAX_CHAN				40
#define MAX_CHAN_FULLFEATURED	32 + 8	// depending on the overall load not all channels can be full-featured (gate + dynamics)
#define MAX_CHAN_EQS			4
#define MAX_MIXBUS				16
#define MAX_MATRIX				6
#define MAX_MAIN				3
#define MAX_MONITOR				3
#define MAX_DSP2				24

#define CHANNELS_PER_TDM		8
#define TDM_INPUTS				((MAX_CHAN / CHANNELS_PER_TDM) + 3*0) // 3 channels from DSP2 DEBUG: REMOVE DSP2 for now
#define SAMPLES_IN_BUFFER		16
#define BUFFER_COUNT			2	// single-, double-, triple- or multi-buffering (e.g. for delay or other things)
#define BUFFER_SIZE				SAMPLES_IN_BUFFER * CHANNELS_PER_TDM
#define M_PI					3.1415926535897932384626433832795f
#define SRUDEBUG  					// Check SRU Routings for errors. Can be removed on final design
#define PCI						(1 << 19)	//0x00080000
#define OFFSET_MASK				0x7FFFF
#define SPI_MAX_RX_PAYLOAD_SIZE	30  // 27 int-values + * + # + parameter
#define SPI_RX_BUFFER_SIZE		(SPI_MAX_RX_PAYLOAD_SIZE * 3)  // store up to 3 payload-sets
#define SPI_TX_BUFFER_SIZE		200 // transmit up to 200 values
#define SPI_DMA_BUFFER_SIZE		1

#define DSP_BUF_IDX_OFF			0	// no audio
#define DSP_BUF_IDX_DSPCHANNEL	1	// DSP-Channel 1-32
#define DSP_BUF_IDX_AUX			33	// Aux-Channel 1-8
#define DSP_BUF_IDX_MIXBUS		41	// Mixbus 1-16
#define DSP_BUF_IDX_MATRIX		57	// Matrix 1-6
#define DSP_BUF_IDX_MAINLEFT	63	// main left
#define DSP_BUF_IDX_MAINRIGHT	64	// main right
#define DSP_BUF_IDX_MAINSUB		65	// main sub
#define DSP_BUF_IDX_MONLEFT		66	// Monitor Left
#define DSP_BUF_IDX_MONRIGHT	67	// Monitor Right
#define DSP_BUF_IDX_TALKBACK	68	// Talkback
#define DSP_BUF_IDX_DSP2_FX		69  // FXDSP2 FX-Channel 1-16
#define DSP_BUF_IDX_DSP2_AUX	85	// FXDSP2 AUX-Channel 1-8

#define DO_CYCLE_COUNTS				// enable cycle counter

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
	float pm peqCoeffsSet[5 * MAX_CHAN_EQS]; // store in program memory
	float pm peqCoeffs[5 * MAX_CHAN_EQS]; // store in program memory
	float dm peqStates[2 * MAX_CHAN_EQS]; // store in data memory
	sCompressor compressor;

	bool solo;
} sDspChannel;

struct {
	float samplerate;

	float lowcutCoeffSet[MAX_CHAN];
	float lowcutCoeff[MAX_CHAN];
	float lowcutStatesInput[MAX_CHAN];
	float lowcutStatesOutput[MAX_CHAN];
	//float highcutCoeff[MAX_CHAN];
	//float highcutStates[MAX_CHAN];

	float gateGainSet[MAX_CHAN];
	float gateGain[MAX_CHAN];
	float gateCoeff[MAX_CHAN];

	float compressorGainSet[MAX_CHAN];
	float compressorGain[MAX_CHAN];
	float compressorCoeff[MAX_CHAN];
	float compressorMakeup[MAX_CHAN];

	float channelVolume[MAX_CHAN]; // in p.u.
	float channelSendMainLeftVolume[MAX_CHAN]; // in p.u.
	float channelSendMainRightVolume[MAX_CHAN]; // in p.u.
	float channelSendMainSubVolume[MAX_CHAN]; // in p.u.
/*
	float channelSendMixbusVolume[MAX_CHAN][MAX_MIXBUS]; // in p.u.
	int channelSendMixbusTapPoint[MAX_CHAN][MAX_MIXBUS];

	float mixbusVolume[MAX_MIXBUS];
	float mixbusSendMainLeftVolume[MAX_MIXBUS];
	float mixbusSendMainRightVolume[MAX_MIXBUS];
	float mixbusSendMainSubVolume[MAX_MIXBUS];
	float mixbusSendMatrixVolume[MAX_MIXBUS][6];
	int mixbusSendMatrixTapPoint[MAX_MIXBUS][6];
	bool mixbusSolo[MAX_MIXBUS];

	float matrixVolume[6];
	bool matrixSolo[6];
*/

	float mainLeftVolume;
	float mainRightVolume;
	float mainSubVolume;
	float mainSendMatrixVolume[6];
	int mainSendMatrixTapPoint[6];
	bool mainLrSolo;
	bool mainSubSolo;

	int inputRouting[MAX_CHAN + MAX_DSP2];
	int inputTapPoint[MAX_CHAN + MAX_DSP2];
	int outputRouting[MAX_CHAN + MAX_DSP2];
	int outputTapPoint[MAX_CHAN + MAX_DSP2];
	sDspChannel dspChannel[MAX_CHAN + MAX_DSP2];
	//sMixbusChannel mixbusChannel[16];
	//sMatrixChannel matrixChannel[6];

	int monitorChannelTapPoint;
	int monitorMixbusTapPoint;
	int monitorMatrixTapPoint;
	int monitorMainTapPoint;
	int monitorTapPoint;
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
void openx32Command(unsigned short classId, unsigned short channel, unsigned short index, unsigned short valueCount, void* values);

#endif /* __DSP1_H__ */
