/*****************************************************************************
 * dsp1.h
 *****************************************************************************/

#ifndef __DSP1_H__
#define __DSP1_H__

#define BUFFER_COUNT			2	// single-, double-, triple- or multi-buffering (e.g. for delay or other things)
#define SAMPLERATE				48000.0f
#define SAMPLES_IN_BUFFER		16
#define BUFFER_SIZE				SAMPLES_IN_BUFFER * 8
#define PI						3.1415926535897932384626433832795f
#define SRUDEBUG  					// Check SRU Routings for errors. Can be removed on final design
#define PCI						(1 << 19)	//0x00080000
#define OFFSET_MASK				0x7FFFF
#define MAX_CHAN				40
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
void timerInit(void);
void openx32Init(void);
void openx32Command(unsigned int parameter, unsigned int value);

// global variables
extern volatile int audioProcessing;
extern volatile int audioReady;
extern volatile int audioIsrCounter;
extern volatile bool spiNewRxData;
extern int audioTx_tcb[8][BUFFER_COUNT][4];
extern int audioRx_tcb[8][BUFFER_COUNT][4];

typedef struct {
  // user-settings
  int type; // 0=allpass, 1=peak, 2=low-shelf, 3=high-shelf, 4=bandpass, 5=notch, 6=lowpass, 7=highpass
  float fc; // center-frequency of PEQ
  float Q; // Quality of PEQ (bandwidth)
  float gain; // gain of PEQ

  double a[3];
  double b[3];
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

  // filter-coefficients
  double a[5];
  double b[5];
} sLR24;

typedef struct {
  // user-settings
  float threshold; // value between -80 dBfs (no gate) and 0 dBfs (full gate) -> 2^23..2^13.33
  float range; // value between 48dB (full range) and 3dB (minimum effect)
  float attackTime_ms;
  float holdTime_ms;
  float releaseTime_ms;

  // filter-data
  float value_threshold;
  float value_gainmin;
  float value_coeff_attack;
  float value_hold_ticks;
  float value_coeff_release;
} sNoisegate;

typedef struct {
  // user-settings
  float threshold; // value between 0 dBfs (no compression) and -80 dBfs (full compression) -> 2^23..2^13.33
  float ratio; // value between 0=oo:1, 1=1:1, 2=2:1, 4=4:1, 8=8:1, 16=16:1, 32=32:1, 64=64:1
  float makeup; // value between 0dB, 6dB, 12dB, 18dB, 24dB, 30dB, 36dB, 42dB, 48dB
  float attackTime_ms;
  float holdTime_ms;
  float releaseTime_ms;

  // filter-data
  float value_threshold;
  float value_ratio;
  float value_makeup;
  float value_coeff_attack;
  float value_hold_ticks;
  float value_coeff_release;
} sCompressor;

typedef struct {
	float volume;
	float balance;
	sPEQ peq;
} sChannel;

struct {
	float mainVolume;
	float mainBalance;
	float mainVolumeSub;

	sChannel channel[MAX_CHAN];
}openx32;

#endif /* __DSP1_H__ */
