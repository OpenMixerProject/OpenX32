/*****************************************************************************
 * dsp1.h
 *****************************************************************************/

#ifndef __DSP2_H__
#define __DSP2_H__

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
extern int audioTx_tcb[4][BUFFER_COUNT][4];
extern int audioRx_tcb[4][BUFFER_COUNT][4];
extern uint32_t cyclesAudio;
extern uint32_t cyclesMain;
extern uint32_t cyclesTotal;

typedef struct {
	float pm peqCoeffs[5 * MAX_CHAN_EQS]; // store in program memory
	float dm peqStates[2 * MAX_CHAN_EQS]; // store in data memory
} sDspChannel;

struct {
	float samplerate;

	float channelFxReturnVolume[16];
	sDspChannel dspChannel[MAX_CHAN];
} dsp;


enum eBufferIndex {
    TAP_INPUT,
	TAP_POST_EQ,
	TAP_PRE_FADER,
	TAP_POST_FADER
};

void openx32Init(void);
void openx32Command(unsigned short classId, unsigned short channel, unsigned short index, unsigned short valueCount, void* values);

#endif /* __DSP2_H__ */
