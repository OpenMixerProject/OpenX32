/*****************************************************************************
 * dsp1.h
 *****************************************************************************/

#ifndef __DSP1_H__
#define __DSP1_H__

#define MAX_CHAN				40
#define CHANNELS_PER_TDM		8
#define TDM_INPUTS				(MAX_CHAN / CHANNELS_PER_TDM)
#define SAMPLES_IN_BUFFER		16
#define BUFFER_COUNT			2	// single-, double-, triple- or multi-buffering (e.g. for delay or other things)
#define BUFFER_SIZE				SAMPLES_IN_BUFFER * CHANNELS_PER_TDM
#define PI						3.1415926535897932384626433832795f
#define SRUDEBUG  					// Check SRU Routings for errors. Can be removed on final design
#define PCI						(1 << 19)	//0x00080000
#define OFFSET_MASK				0x7FFFF
#define SPI_BUFFER_SIZE			20
#define SPI_DMA_BUFFER_SIZE		20
#define SPI_PAYLOAD_SIZE		2

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

// function prototypes
static void timerIsr(uint32_t iid, void* handlerArg);
void delay(int i);
void openx32Init(void);
void openx32Command(unsigned int parameter, unsigned int value);

// global variables
extern volatile int audioProcessing;
extern volatile int audioReady;
extern volatile int audioIsrCounter;
extern volatile bool spiNewRxDataReady;
extern int audioTx_tcb[8][BUFFER_COUNT][4];
extern int audioRx_tcb[8][BUFFER_COUNT][4];

typedef struct {
	// user-settings
	int type; // 0=allpass, 1=peak, 2=low-shelf, 3=high-shelf, 4=bandpass, 5=notch, 6=lowpass, 7=highpass
	float fc; // center-frequency of PEQ
	float Q; // Quality of PEQ (bandwidth)
	float gain; // gain of PEQ

	// filter-coefficients (TODO: could be calculated in i.MX25 lateron to save processing power)
	double a[3];
	double b[3];

	// online parameters
	float in[2]; // in[0] = z-1, in[1] = z-2
	float out[2]; // out[0] = z-1, out[1] = z-2
} sPEQ;

typedef struct {
	// user-settings
	float fc; // cutoff-frequency for high- or lowpass
	bool isHighpass; // choose if Highpass or Lowpass

	// filter-coefficients
	double a[3];
	double b[3];
} sLR12;

typedef struct {
	// user-settings
	float fc; // cutoff-frequency for high- or lowpass
	bool isHighpass; // choose if Highpass or Lowpass

	// filter-coefficients (TODO: could be calculated in i.MX25 lateron to save processing power)
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
	// user-settings
	float threshold; // value between -80 dBfs (no gate) and 0 dBfs (full gate)
	float range; // value between 48dB (full range) and 3dB (minimum effect)
	float attackTime_ms;
	float holdTime_ms;
	float releaseTime_ms;

	// filter-data (TODO: could be calculated in i.MX25 lateron to save processing power)
	float value_threshold;
	float value_gainmin;
	float value_coeff_attack;
	float value_hold_ticks;
	float value_coeff_release;

	// online parameters
	short int holdCounter;
	float gainSet;
	float gainCurrent;
	float coeff;
	gateState state;
	bool closed;
} sGate;

typedef enum {
	COMPRESSOR_IDLE,
	COMPRESSOR_ATTACK,
	COMPRESSOR_ACTIVE,
	COMPRESSOR_HOLD,
	COMPRESSOR_RELEASE
} compressorState;
typedef struct {
	// user-settings
	float threshold; // value between 0 dBfs (no compression) and -80 dBfs (full compression)
	float ratio; // value between 0=oo:1, 1=1:1, 2=2:1, 4=4:1, 8=8:1, 16=16:1, 32=32:1, 64=64:1
	float makeup; // value between 0dB, 6dB, 12dB, 18dB, 24dB, 30dB, 36dB, 42dB, 48dB
	float attackTime_ms;
	float holdTime_ms;
	float releaseTime_ms;

	// filter-data (TODO: could be calculated in i.MX25 lateron to save processing power)
	float value_threshold;
	float value_makeup;
	float value_coeff_attack;
	float value_hold_ticks;
	float value_coeff_release;

	// online parameters
	short int holdCounter;
	float gainSet;
	float gainCurrent;
	float coeff;
	bool active;
	compressorState state;
} sCompressor;

typedef struct {
	float volume; // in p.u.
	float balance; // -100 .. 0 .. +100
	float sends[16];
	sGate gate;
	short peqMax;
	sPEQ peq[5];
	sCompressor compressor;
} sChannel;

struct {
	float mainVolume; // in p.u.
	float mainBalance; // -100 .. 0 .. +100
	float mainVolumeSub; // in p.u.

	float samplerate;

	sChannel channel[MAX_CHAN];
} openx32;

#endif /* __DSP1_H__ */
