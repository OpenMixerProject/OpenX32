/*****************************************************************************
 * dsp1.h
 *****************************************************************************/

#ifndef __DSP1_H__
#define __DSP1_H__

#define SRUDEBUG  // Check SRU Routings for errors. Can be removed on final design
#define NUM_SAMPLES 1024
#define PCI 0x00080000
#define OFFSET_MASK 0x7FFFF

// general includes
#include <stdio.h>     /* Get declaration of puts and definition of NULL. */
#include <stdint.h>    /* Get definition of uint32_t. */
#include <assert.h>    /* Get the definition of support for standard C asserts. */
#include <builtins.h>  /* Get definitions of compiler built-in functions */
#include <platform_include.h>      /* System and IOP register bit and address definitions. */
#include <processor_include.h>	   /* Get definitions of the part being built*/
#include <services/int/adi_int.h>  /* Interrupt HAndler API header. */
#include "adi_initialize.h"

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
extern unsigned int tdmData0[8][NUM_SAMPLES];

#endif /* __DSP1_H__ */
