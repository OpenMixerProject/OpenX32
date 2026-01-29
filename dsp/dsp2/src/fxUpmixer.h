#ifndef __FXUPMIXER_H_
#define __FXUPMIXER_H_

#include "dsp2.h"

#if FX_USE_UPMIXER == 1

#define FX_UPMIX_PHASEINVERTED_BACK_LR	1	// inverts the phase of the back-left and back-right speaker-channels
#define FX_UPMIX_CONTRAST_ENHANCEMENT	2	// 0=Regular Mask (no additional load), 1=Mask with 1.5 (+13% DSP-load), 2=Mask with 2 (+5% DSP-load), 3 and or above this as a free value: the contrast between all channels is increased by using powf() for the mask-value
#define FX_UPMIX_ADD_AMBIENCE_TO_LR		1

#define FX_UPMIXER_DELAY_BACKLEFT_MS	13	// 13ms delay to back-surround left
#define FX_UPMIXER_DELAY_BACKRIGHT_MS	17	// 17ms delay to back-surround right
#define FX_UPMIXER_SAMPLING_RATE 		48000
#define FX_UPMIXER_DELAY_MS_MAX			25	// maximum 8 meters
#define FX_UPMIXER_BUFFER_SIZE 			((FX_UPMIXER_SAMPLING_RATE * FX_UPMIXER_DELAY_MS_MAX) / 1000)

// configure the FFT
#define FX_UPMIX_WINDOW_LEN				64 		// WindowLength=64 samples takes ~32% DSP-load and has good audio-quality, WindowLength=128 takes ~60% DSP-load
#define FX_UPMIX_WINDOW_LEN_HALF		32
#define FX_UPMIX_FFT_HOP_VALUE			4		// use fine N/4 to get 75% overlap (alternatively use N/2 for 50% overlap)
#define FX_UPMIX_RX_SAMPLE_COUNT		(FX_UPMIX_WINDOW_LEN / FX_UPMIX_FFT_HOP_VALUE)

// some useful defines for better readability
#define real					0
#define imag					1

// function prototypes
void fxUpmixerInit(void);
void fxUpmixerProcess(float* __restrict inBuf[2], float* __restrict outBuf[6]);

#endif

#endif /* FXUPMIXER_H_ */
