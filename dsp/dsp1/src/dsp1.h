/*****************************************************************************
 * dsp1.h
 *****************************************************************************/

#ifndef __DSP1_H__
#define __DSP1_H__

#define SAMPLERATE			48000
#define SAMPLES_IN_BUFFER	16
#define BUFFER_SIZE			SAMPLES_IN_BUFFER * 8
#define pi					3.1415926535897932384626433832795
#define SRUDEBUG  					// Check SRU Routings for errors. Can be removed on final design

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
extern volatile int intCounter;
extern unsigned int audioRx1a_tcb[];
extern unsigned int audioRx1b_tcb[];
extern unsigned int audioRx3a_tcb[];
extern unsigned int audioRx3b_tcb[];
extern unsigned int audioRx5a_tcb[];
extern unsigned int audioRx5b_tcb[];
extern unsigned int audioRx7a_tcb[];
extern unsigned int audioRx7b_tcb[];
extern unsigned int audioTx0a_tcb[];
extern unsigned int audioTx0b_tcb[];
extern unsigned int audioTx2a_tcb[];
extern unsigned int audioTx2b_tcb[];
extern unsigned int audioTx4a_tcb[];
extern unsigned int audioTx4b_tcb[];
extern unsigned int audioTx6a_tcb[];
extern unsigned int audioTx6b_tcb[];

#endif /* __DSP1_H__ */
