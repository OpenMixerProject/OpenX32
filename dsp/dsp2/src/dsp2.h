/*****************************************************************************
 * dsp1.h
 *****************************************************************************/

#ifndef __DSP2_H__
#define __DSP2_H__

#define DSP_VERSION				0.01

#define SDRAM_START  			0x00200000	// start address of SDRAM
#define SDRAM_SIZE	 			0x00400000	// size of SDRAM in 32-bit words (16 MiB)

#define MAX_CHAN				32 // we are using only 24 channels, but SPORT seems to request correct pointers for both Channels
#define CHANNELS_PER_TDM		8
#define TDM_INPUTS				4  // we are using only 3 TDM-inputs, but SPORT seems to request correct pointers for both Channels
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

// SPDIF configuration
#define CLKA_DIVIDER			8	// provides SCLK serial clock to S/PDIF TX and SPORT0
#define FSA_DIVIDER				512	// provides Frame Sync to S/PDIF TX and SPORT0
#define CLKB_DIVIDER			2 	// provides HFCLK to S/PDIF TX

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
extern int audioTx_tcb[TDM_INPUTS][BUFFER_COUNT][4];
extern int audioRx_tcb[TDM_INPUTS][BUFFER_COUNT][4];

struct {
	float samplerate;
} dsp;


void openx32Init(void);
void openx32Command(unsigned short classId, unsigned short channel, unsigned short index, unsigned short valueCount, void* values);

#endif /* __DSP2_H__ */
