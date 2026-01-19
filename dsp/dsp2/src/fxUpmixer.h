#ifndef __FXUPMIXER_H_
#define __FXUPMIXER_H_

#include "fxBase.h"

#define FX_UPMIX_PHASEINVERTED_BACK_LR	1	// inverts the phase of the back-left and back-right speaker-channels
#define FX_UPMIX_CONTRAST_ENHANCEMENT	2	// 0=Regular Mask (no additional load), 1=Mask with 1.5 (+13% DSP-load), 2=Mask with 2 (+5% DSP-load), 3 and or above this as a free value: the contrast between all channels is increased by using powf() for the mask-value

#define FX_UPMIXER_DELAY_MS_MAX			25	// maximum 8 meters
#define FX_UPMIXER_BUFFER_SIZE 			((SAMPLERATE_MAX * FX_UPMIXER_DELAY_MS_MAX) / 1000)

#define UPMIX_WINDOW_LEN				64 	// WindowLength=64 samples takes ~32% DSP-load and has good audio-quality, WindowLength=128 takes ~60% DSP-load
#define UPMIX_WINDOW_LEN_HALF			32
#define UPMIX_FFT_HOP_VALUE				4	// use fine N/4 to get 75% overlap (alternatively use N/2 for 50% overlap)
#define UPMIX_RX_SAMPLE_COUNT			(UPMIX_WINDOW_LEN / UPMIX_FFT_HOP_VALUE)

// some useful defines for better readability
#define real					0
#define imag					1

class fxUpmixer : public fx {
    public:
        fxUpmixer(int fxSlot, int channelMode);
        ~fxUpmixer();
        void fxUpmixerSetParameters(float delayMsBackLeft, float delayMsBackRight, float ambientToLR);
        void fxUpmixerSetFilters(float lpfFreqLfe, float lpfFreqSurround);
        void rxData(float data[], int len);
        void process(float* bufIn[], float* bufOut[]);
    private:
        float _delayMsBackLeft;
        float _delayMsBackRight;
        float _ambientToLR;

        float hannWindow[UPMIX_WINDOW_LEN];
        float lowPassSubState;
        float lowPassSubCoeff;
        //float lowPassSurroundState[2];
        //float lowPassSurroundCoeff;
        float surroundCutoff;
        float bin_to_hz;

        // variables for the FFT and calculation
        float upmixInputBuffer[2][UPMIX_WINDOW_LEN]; // we need ringbuffer for the full window-size
        #pragma align (2 * 2 * UPMIX_WINDOW_LEN)
        float upmixFftInputBufferLR[2][2][UPMIX_WINDOW_LEN]; // imag/real for two channels
        #pragma align (2 * 2 * UPMIX_WINDOW_LEN)
        float upmixFftOutputBufferLR[2][2][UPMIX_WINDOW_LEN]; // imag/real
        #pragma align (2 * UPMIX_WINDOW_LEN)
        float upmixFftOutputBufferC[2][UPMIX_WINDOW_LEN]; // imag/real
        #pragma align (2 * 2 * UPMIX_WINDOW_LEN)
        float upmixFftOutputBufferBackLR[2][2][UPMIX_WINDOW_LEN]; // imag/real
        #pragma align (2 * UPMIX_WINDOW_LEN_HALF)
        float twidtab[2][UPMIX_WINDOW_LEN_HALF];
        #pragma align (2 * UPMIX_WINDOW_LEN)
        float upmixFftTempBuffer[2][UPMIX_WINDOW_LEN]; // imag/real
        float upmixOutputBuffer[5][UPMIX_WINDOW_LEN]; // left, right, center, back-left, back-right

        float maskCenter[UPMIX_WINDOW_LEN_HALF];
        float maskLeft[UPMIX_WINDOW_LEN_HALF];
        float maskRight[UPMIX_WINDOW_LEN_HALF];
        float maskAmbient[UPMIX_WINDOW_LEN_HALF];
        float maskCenter_z[UPMIX_WINDOW_LEN_HALF];
        float maskLeft_z[UPMIX_WINDOW_LEN_HALF];
        float maskRight_z[UPMIX_WINDOW_LEN_HALF];
        float maskAmbient_z[UPMIX_WINDOW_LEN_HALF];

        /*
        	// accessing external RAM
        	typedef float fft_block_t[2][UPMIX_WINDOW_LEN];
        	fft_block_t* upmixFftBuffer = (fft_block_t*)EXTERNAL_RAM_START;
        */

        int upmixInputBufferHead;
        int upmixInputBufferTail;
        int upmixInputSampleCounter;
        int upmixOutputSampleCounter;

        int delayLineHead;
        float delayLineBackLeft[FX_UPMIXER_BUFFER_SIZE];
        float delayLineBackRight[FX_UPMIXER_BUFFER_SIZE];
};

#endif /* FXUPMIXER_H_ */
