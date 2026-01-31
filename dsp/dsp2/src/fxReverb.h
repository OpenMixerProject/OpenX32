#ifndef __FXREVERB_H_
#define __FXREVERB_H_

#include "fxBase.h"

#define FX_REVERB_INT_CHAN			8	// must be a power of 2, so 2, 4, 8, .... Code has optimized function for 8 parallel channels at the moment
#define FX_REVERB_DIFFUSION_STEPS	4	// 1,2,3,4,...
#define FX_REVERB_AVERAGE_OUTPUT	1	// 0=take fxOut as left/right directly, 1=mixdown multichannel-fxOut to stereo
#define FX_REVERB_ALTERNATIVE_RND	0	// 0=use internal rand() function, 1=use bitshifting chaos (alternative sounds more metallic... not a good choice :-) )
#define FX_REVERB_ALTERNATIVE_POW	0	// 0=use internal powf() function, 1=use approximation (approximation seems to be slower compared to internal powf())

// we have 14MB / 8 = 1.75MB RAM available and for an 8-channel Reverb we need the following amount of RAM:
// 1 Diffusion-Step:	2908 Bytes per Millisecond -> maximum size = 625ms
// 2 Diffusion-Steps:	3292 Bytes per Millisecond -> maximum size = 550ms
// 3 Diffusion-Steps:	3548 Bytes per Millisecond -> maximum size = 500ms
// 4 Diffusion-Steps:	3740 Bytes per Millisecond -> maximum size = 475ms
#if FX_REVERB_DIFFUSION_STEPS == 2
	#define FX_REVERB_DELAY_MS_MAX		550
#elif FX_REVERB_DIFFUSION_STEPS == 3
	#define FX_REVERB_DELAY_MS_MAX		500
#elif FX_REVERB_DIFFUSION_STEPS == 4
	#define FX_REVERB_DELAY_MS_MAX		200 // 475
#else
	#define FX_REVERB_DELAY_MS_MAX		DEFINE YOUR MAXIMUM DELAY HERE
#endif
#define FX_REVERB_BUFFER_SIZE 		((SAMPLERATE_MAX * FX_REVERB_DELAY_MS_MAX) / 1000)

// some hadamard-scalings for common channel-count
// FX_REVERB_HADAMARD_SCALING = sqrtf(1.0f / FX_REVERB_INT_CHAN)
#if FX_REVERB_INT_CHAN == 2
	#define FX_REVERB_HADAMARD_SCALING	0.7071067811865475244f	// 1 / sqrt(2)
#elif FX_REVERB_INT_CHAN == 4
	#define FX_REVERB_HADAMARD_SCALING	0.5f	// 1 / sqrt(4)
#elif FX_REVERB_INT_CHAN == 8
	#define FX_REVERB_HADAMARD_SCALING	0.3535533905932737f	// 1 / sqrt(8)
#elif FX_REVERB_INT_CHAN == 16
	#define FX_REVERB_HADAMARD_SCALING	0.25f	// 1 / sqrt(16)
#else
	#define FX_REVERB_HADAMARD_SCALING	(1.0f / sqrtf((float)FX_REVERB_INT_CHAN))
#endif

typedef struct {
	float* memory;
	int delayLineTailOffset; // tail will be calculated relative to head: tail = (head - tailOffset)
	bool flipPolarities;
} sDiffusor;

typedef struct {
	float* memory;
	int memoryLength;
	int head; // we are using different delay-line-lengths, so we have to use individual head-pointers here
	int tailOffset; // tail will be calculated relative to head: tail = (head - tailOffset)
	float lowPassDelayState;
} sDelay;

class fxReverb : public fx {
    public:
        fxReverb(int fxSlot, int channelMode);
        ~fxReverb();
        void setParameters(float roomSizeMs, float rt60, float feedbackLowPassFreq, float dry, float wet); // Size in ms, Reverberation Time to -60dB, Frequency for feedback-loop, dry, wet
        void rxData(float data[], int len);
        void process(float* __restrict bufIn[], float* __restrict bufOut[]);
    private:
        sDiffusor _diffusor[FX_REVERB_DIFFUSION_STEPS][FX_REVERB_INT_CHAN];
        int _diffusionDelayLineLength[FX_REVERB_DIFFUSION_STEPS];
    	int _diffusionDelayLineHead[FX_REVERB_DIFFUSION_STEPS]; // we are using different delay-line-lengths, so we have to use individual head-pointers here

    	sDelay _delay[FX_REVERB_INT_CHAN];
    	float _delayLowPassCoeff;

    	float _roomSizeMs;
    	float _feedbackDelayMs;
    	float _loopsPerRt60;
    	float _dbPerCycle;
    	float _feedbackDecayGain;

    	float _dry;
    	float _wet;

    	// temporary storages
    	float _fxBuf[FX_REVERB_INT_CHAN];
    	#if FX_REVERB_INT_CHAN != 8
    		float _fxBufInput[FX_REVERB_INT_CHAN];
    	#endif
    	float _fxBufOutput[FX_REVERB_INT_CHAN];
    	float _fxBufFeedback[FX_REVERB_INT_CHAN];
};

/*
	// this is a small program to calculate the memory-demand of this Reverb
	#include <stdio.h>
	#include <stdint.h>
	#include <math.h>

	#define SDRAM_AUDIO_SIZE_BYTE	(14 * 1024 * 1024) / 8

	#define FX_REVERB_INT_CHAN          8
	#define FX_REVERB_DIFFUSION_STEPS   4
	#define SAMPLERATE_MAX              48000

	#define FX_REVERB_DELAY_MS_MAX		475
	#define FX_REVERB_BUFFER_SIZE 		((SAMPLERATE_MAX * FX_REVERB_DELAY_MS_MAX) / 1000)

	int _diffusionDelayLineLength[FX_REVERB_DIFFUSION_STEPS];
	int _delayLineLength[FX_REVERB_INT_CHAN];
	int _memoryUsed = 0;

	int main()
	{
		for (int d = 0; d < FX_REVERB_DIFFUSION_STEPS; d++) {
			_diffusionDelayLineLength[d] = (int)ceilf(FX_REVERB_BUFFER_SIZE / (2 * (d + 1)));

			_memoryUsed += _diffusionDelayLineLength[d] * FX_REVERB_INT_CHAN * sizeof(float);
		}
		for (int c = 0; c < FX_REVERB_INT_CHAN; c++) {
			_delayLineLength[c] = (int)ceilf(FX_REVERB_BUFFER_SIZE * powf(2.0f, (float)c / (float)FX_REVERB_INT_CHAN));
			_memoryUsed += (_delayLineLength[c] * sizeof(float));
		}


		printf("Used memory       = %d bytes\n", _memoryUsed);
		printf("Available memory  = %d bytes\n", SDRAM_AUDIO_SIZE_BYTE);
		printf("Free memory left  = %d bytes\n", SDRAM_AUDIO_SIZE_BYTE - _memoryUsed);

		return 0;
	}
*/

#endif /* FXREVERB_H_ */
