/*****************************************************************************
 * dsp1.h
 *****************************************************************************/

#ifndef __DSP1_H__
#define __DSP1_H__

#define BUFFER_COUNT		2	// single-, double-, triple- or multi-buffering (e.g. for delay or other things)
#define SAMPLERATE			48000.0f
#define SAMPLES_IN_BUFFER	16
#define BUFFER_SIZE			SAMPLES_IN_BUFFER * 8
#define pi					3.1415926535897932384626433832795f
#define SRUDEBUG  					// Check SRU Routings for errors. Can be removed on final design
#define PCI					(1 << 19)	//0x00080000
#define OFFSET_MASK			0x7FFFF

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

// includes for hardware-pins
#include <SRU.h>
#include <sysreg.h>
#include <signal.h>

// own includes
#include "system.h"
#include "audio.h"
#include "communication.h"

// function prototypes
static void timerIsr(uint32_t iid, void* handlerArg);
void delay(int i);
void timerInit(void);

// global variables
extern volatile int audioProcessing;
extern volatile int audioReady;
extern volatile int audioIsrCounter;

extern int audioTx0a_tcb[BUFFER_COUNT][4];
extern int audioTx0b_tcb[BUFFER_COUNT][4];
extern int audioTx2a_tcb[BUFFER_COUNT][4];
extern int audioTx2b_tcb[BUFFER_COUNT][4];
extern int audioTx4a_tcb[BUFFER_COUNT][4];
extern int audioTx4b_tcb[BUFFER_COUNT][4];
extern int audioTx6a_tcb[BUFFER_COUNT][4];
extern int audioTx6b_tcb[BUFFER_COUNT][4];

extern int audioRx1a_tcb[BUFFER_COUNT][4];
extern int audioRx1b_tcb[BUFFER_COUNT][4];
extern int audioRx3a_tcb[BUFFER_COUNT][4];
extern int audioRx3b_tcb[BUFFER_COUNT][4];
extern int audioRx5a_tcb[BUFFER_COUNT][4];
extern int audioRx5b_tcb[BUFFER_COUNT][4];
extern int audioRx7a_tcb[BUFFER_COUNT][4];
extern int audioRx7b_tcb[BUFFER_COUNT][4];

#endif /* __DSP1_H__ */
