/*****************************************************************************
 * dsp1.h
 *****************************************************************************/

#ifndef __DSP1_H__
#define __DSP1_H__

#include "version.h"
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

#ifdef DO_CYCLE_COUNTS
#include <cycle_count.h>
#endif

// global variables
extern int audioTx_tcb[8][BUFFER_COUNT][4];
extern int audioRx_tcb[8][BUFFER_COUNT][4];
extern volatile uint32_t cyclemap[CYCLEMAP_SIZE];

typedef struct {
	// filter-data from i.MX25
	bool value_enable;
	float value_thresholdSq16; // 16 * (threshold_linear)^2 for zero-sqrt gate compare
	float value_gainmin; // linear (10^(-range/20))
	float value_coeff_attack;
	float value_hold_ticks; // number of sample-ticks (16-sample blocks)
	float value_coeff_release;
	float value_thresholdDb; // threshold in dBFS (for expander mode)
	float value_negRangeDb; // -range in dB (for expander attenuation floor)
	float value_expanderSlope; // 0.0f = hard Gate, (ratio - 1.0f) = Expander (1.0=2:1, 2.0=3:1, 3.0=4:1)

	// online parameters
	int holdTimer;
} sGate;

typedef struct {
	// filter-data from i.MX25
	float value_thresholdDb; // in dBFS
	float value_1_minus_1_by_ratio; // precalculated (1.0f - 1.0f/ratio) sent by OMC
	float value_coeff_attack;
	float value_hold_ticks; // number of sample-ticks (16-sample blocks)
	float value_coeff_release;
	float value_kneeHalf; // 0.5f * kneeWidthDb (0.0f for hard knee)
	float value_kneeCoeff; // precalculated -(1 - 1/ratio) / (2 * kneeWidthDb), or 0.0f for hard knee

	// online parameters
	int holdTimer;
} sCompressor;

struct
{
	bool lowcutEnable[MAX_CHAN_FULLFEATURED];
	float pm lowcutCoeffs[MAX_CHAN_FULLFEATURED][5 * LOWCUT_SECTIONS]; // interleaved for biquad_trans()
	float dm lowcutStates[MAX_CHAN_FULLFEATURED][2 * LOWCUT_SECTIONS];

	#if USE_HIGHCUT == 1
	bool highcutEnable[MAX_CHAN_FULLFEATURED];
	float pm highcutCoeffs[MAX_CHAN_FULLFEATURED][5 * HIGHCUT_SECTIONS]; // non-interleaved for 1-section biquad_trans()
	float dm highcutStates[MAX_CHAN_FULLFEATURED][2 * HIGHCUT_SECTIONS];
	#endif

	float gateEnvelope[MAX_CHAN_FULLFEATURED];

	bool compressorEnable[MAX_CHAN_FULLFEATURED];
	float compressorEnvelope[MAX_CHAN_FULLFEATURED];
	float compressorMakeup[MAX_CHAN_FULLFEATURED];
	//float compressorGainSmoothed[MAX_CHAN_FULLFEATURED];

	bool eqActive[CHANNELS_WITH_4BD_EQ];

	float pm peqCoeffs_4BD_EQ[CHANNELS_WITH_4BD_EQ][5 * EQ_4BD_BANDS]; // store in program memory
	float dm peqStates_4BD_EQ[CHANNELS_WITH_4BD_EQ][2 * EQ_4BD_BANDS]; // store in data memory
	float pm peqCoeffs_6BD_EQ[CHANNELS_WITH_6BD_EQ][5 * EQ_6BD_BANDS]; // store in program memory
	float dm peqStates_6BD_EQ[CHANNELS_WITH_6BD_EQ][2 * EQ_6BD_BANDS]; // store in data memory

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
	float pm channelSendMainLeftVolume[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS]; // in p.u.
	#pragma align 8
	float pm channelSendMainRightVolume[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS]; // in p.u.
	#pragma align 8
	float pm channelSendMainSubVolume[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS]; // in p.u.


	#if DEBUG_DISABLE_MATRIX == 0
		float sendMatrixVolume[MAX_MATRIX][MAX_MIXBUS + MAX_MAIN];
		short sendMatrixTapPoint[MAX_MATRIX][MAX_MIXBUS + MAX_MAIN];

		float matrixVolume[MAX_MATRIX];
		bool matrixSolo[MAX_MATRIX];
	#endif

	float mainVolumeSet[3]; // left, right, sub
	float mainVolume[3]; // left, right, sub
	bool mainLrSolo;
	bool mainSubSolo;

	float* inputSourcePtr[MAX_CHAN_FPGA];
	float* outputSourcePtr[MAX_CHAN_FPGA + MAX_CHAN_DSP2];

	sGate dspChannelGate[MAX_CHAN_FULLFEATURED];
	sCompressor dspChannelCompressor[MAX_CHAN_FULLFEATURED];
	bool dspChannelSolo[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MIXBUS];

	short monitorChannelTapPoint;
	short monitorMatrixTapPoint;
	short monitorMainTapPoint;
	float monitorVolume;
	bool soloActive;
} dsp;

struct
{
	float samplerate;
	float pm trim_pu[MAX_CHAN_FPGA];

} dsp_settings;

enum eBufferIndex {
    TAP_INPUT,
	TAP_PRE_EQ,
	TAP_POST_EQ,
	TAP_PRE_FADER,
	TAP_POST_FADER
};


// function prototypes

void openx32Init(void);

#endif /* __DSP1_H__ */
