#ifndef __FXREVERB_H_
#define __FXREVERB_H_

#include "fxBase.h"

#define FX_REVERB_INT_CHAN			8	// must be a power of 2, so 2, 4, 8, .... Code has optimized function for 8 parallel channels at the moment
#define FX_REVERB_DIFFUSION_STEPS	3	// 1,2,3,4,...
#define FX_REVERB_AVERAGE_OUTPUT	1	// 0=take fxOut as left/right directly, 1=mixdown multichannel-fxOut to stereo
#define FX_REVERB_ALTERNATIVE_RND	0	// 0=use internal rand() function, 1=use bitshifting chaos (alternative sounds more metallic... not a good choice :-) )
#define FX_REVERB_ALTERNATIVE_POW	0	// 0=use internal powf() function, 1=use approximation (approximation seems to be slower compared to internal powf())

// we need (FX_REVERB_DIFFUSION_STEPS + 1) * FX_REVERB_INT_CHAN * 48 = (3+1) * 8 * 48 = 1536 Words = 6144 bytes per millisecond
// we have 14MB / 8 = 1.75MB RAM available: 290ms * 6144 Bytes = 1.7 MB
// automatic calculation of maximum DELAY:
#define FX_REVERB_DELAY_MS_MAX		(((SDRAM_AUDIO_SIZE_BYTE / 8) / ((FX_REVERB_DIFFUSION_STEPS + 1) * FX_REVERB_INT_CHAN * 48 * 4)) - 10) // = ((1.835.008 / ((3 + 1) * 8 * 48 * 4)) - 10) = 288ms
//#define FX_REVERB_DELAY_MS_MAX		290
#define FX_REVERB_BUFFER_SIZE 		((SAMPLERATE_MAX * FX_REVERB_DELAY_MS_MAX) / 1000) // FX_REVERB_BUFFER_SIZE has to be power-of-2, to use optimized pointer-calculation

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
	int delayLineTailOffset[FX_REVERB_INT_CHAN]; // tail will be calculated relative to head: tail = (head - tailOffset)
	bool flipPolarities[FX_REVERB_INT_CHAN];
} sDiffusor;

typedef struct {
	int head;
	int tailOffset[FX_REVERB_INT_CHAN]; // tail will be calculated relative to head: tail = (head - tailOffset)
	float lowPassDelayCoeff;
	float lowPassDelayState[FX_REVERB_INT_CHAN];
} sDelay;

class fxReverb : public fx {
    public:
        fxReverb(int fxSlot, int channelMode);
        ~fxReverb();
        void setParameters(float roomSizeMs, float rt60, float feedbackLowPassFreq, float dry, float wet); // Size in ms, Reverberation Time to -60dB, Frequency for feedback-loop, dry, wet
        void rxData(float data[], int len);
        void process(float* __restrict bufIn[], float* __restrict bufOut[]);
    private:
        sDiffusor _diffusor[FX_REVERB_DIFFUSION_STEPS];
    	sDelay _delay;

    	float* _diffusionDelayLine[FX_REVERB_DIFFUSION_STEPS][FX_REVERB_INT_CHAN];
    	float* _delayLine[FX_REVERB_INT_CHAN];
    	int _diffusionDelayLineHead;

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

#endif /* FXREVERB_H_ */
