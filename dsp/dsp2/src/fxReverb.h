#ifndef __FXREVERB_H_
#define __FXREVERB_H_

#include "dsp2.h"

#if FX_USE_REVERB == 1

#define FX_REVERB_INT_CHAN			8	// must be a power of 2, so 2, 4, 8, .... Code has optimized function for 8 parallel channels at the moment
#define FX_REVERB_DIFFUSION_STEPS	2	// 1,2,3,4,...
#define FX_REVERB_AVERAGE_OUTPUT	1	// 0=take fxOut as left/right directly, 1=mixdown multichannel-fxOut to stereo
#define FX_REVERN_ALTERNATIVE_RND	0	// 0=use internal rand() function, 1=use bitshifting chaos (alternative sounds more metallic... not a good choice :-) )
#define FX_REVERN_ALTERNATIVE_POW	0	// 0=use internal powf() function, 1=use approximation (approximation seems to be slower compared to internal powf())
#define FX_REVERB_SAMPLING_RATE 	48000
#define FX_REVERB_DELAY_MS_MAX		800
#define FX_REVERB_BUFFER_SIZE 		((FX_REVERB_SAMPLING_RATE * FX_REVERB_DELAY_MS_MAX) / 1000) // FX_REVERB_BUFFER_SIZE has to be power-of-2, to use optimized pointer-calculation

// FX_REVERB_HADAMARD_SCALING = sqrtf(1.0f / FX_REVERB_INT_CHAN)
#if FX_REVERB_INT_CHAN == 2
	#define FX_REVERB_HADAMARD_SCALING	0.7071067811865475244f
#elif FX_REVERB_INT_CHAN == 4
	#define FX_REVERB_HADAMARD_SCALING	0.5f
#elif FX_REVERB_INT_CHAN == 8
	#define FX_REVERB_HADAMARD_SCALING	0.3535533905932737f
#elif FX_REVERB_INT_CHAN == 16
	#define FX_REVERB_HADAMARD_SCALING	0.25f
#else
	#error "FX_REVERB_HADAMARD_SCALING is not defined for this reverb-channel-configuration"
#endif

typedef struct {
	int delayLineTailOffset[FX_REVERB_INT_CHAN]; // tail will be calculated relative to head: tail = (head - tailOffset)
	bool flipPolarities[FX_REVERB_INT_CHAN];
} sDiffusor;

typedef struct {
	int head;
	int tailOffset[FX_REVERB_INT_CHAN]; // tail will be calculated relative to head: tail = (head - tailOffset)
	float lowPassDelayCoeff;
	float lowPassDelayState[FX_REVERB_INT_CHAN];
} sDelay;

struct {
	float roomSizeMs;
	float feedbackDelayMs;
	float loopsPerRt60;
	float dbPerCycle;
	float feedbackDecayGain;

	sDiffusor diffusor[FX_REVERB_DIFFUSION_STEPS];
	int diffusionDelayLineHead;
	sDelay delay;

	float dry;
	float wet;
} reverb;

// function prototypes
void fxReverbInit(void);
void fxReverbSetParameters(float roomSizeMs, float rt60, float feedbackLowPassFreq, float dry, float wet); // Size in ms, Reverberation Time to -60dB, Frequency for feedback-loop, dry, wet
void fxReverbProcess(float* bufIn[2], float* bufOut[2]);

#endif

#endif /* FXREVERB_H_ */
