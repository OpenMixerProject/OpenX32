/*****************************************************************************
 * dsp1.h
 *****************************************************************************/

#ifndef __DSP1_H__
#define __DSP1_H__

#define SDRAM_START  0x00200000	// start address of SDRAM
#define SDRAM_SIZE	 0x00400000	// size of SDRAM in 32-bit words (16 MiB)

#define MAX_CHAN				40
#define MAX_CHAN_FULLFEATURED	32
#define MAX_CHAN_EQS			5

#define CHANNELS_PER_TDM		8
#define TDM_INPUTS				(MAX_CHAN / CHANNELS_PER_TDM)
#define SAMPLES_IN_BUFFER		16
#define BUFFER_COUNT			2	// single-, double-, triple- or multi-buffering (e.g. for delay or other things)
#define BUFFER_SIZE				SAMPLES_IN_BUFFER * CHANNELS_PER_TDM
#define PI						3.1415926535897932384626433832795f
#define SRUDEBUG  					// Check SRU Routings for errors. Can be removed on final design
#define PCI						(1 << 19)	//0x00080000
#define OFFSET_MASK				0x7FFFF
#define SPI_MAX_PAYLOAD_SIZE	30  // 27 int-values + * + # + parameter
#define SPI_BUFFER_SIZE			(SPI_MAX_PAYLOAD_SIZE * 3)  // store up to 3 payload-sets
#define SPI_DMA_BUFFER_SIZE		1

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
	short int holdCounter;
	bool active;
	compressorState state;
} sCompressor;

typedef struct {
	sGate gate;
	float pm peqCoeffs[5 * MAX_CHAN_EQS]; // store in proram memory
	float dm peqStates[2 * MAX_CHAN_EQS]; // store in data memory

	sCompressor compressor;
} sChannel;

struct {
	float samplerate;

	float gateGainSet[MAX_CHAN];
	float gateGain[MAX_CHAN];
	float gateCoeff[MAX_CHAN];

	float compressorGainSet[MAX_CHAN];
	float compressorGain[MAX_CHAN];
	float compressorCoeff[MAX_CHAN];
	float compressorMakeup[MAX_CHAN];

	float channelVolume[MAX_CHAN];
	float channelVolumeLeft[MAX_CHAN]; // in p.u.
	float channelVolumeRight[MAX_CHAN]; // in p.u.
	float channelVolumeSub[MAX_CHAN]; // in p.u.
	sChannel dspChannel[MAX_CHAN];
} dsp;

// function prototypes
/*
static void timerIsr(uint32_t iid, void* handlerArg);
void delay(int i);
*/
void openx32Init(void);
void openx32Command(unsigned short classId, unsigned short channel, unsigned short index, unsigned short valueCount, void* values);

#endif /* __DSP1_H__ */
