#ifndef __FXREVERB_H_
#define __FXREVERB_H_

#include "fxBase.h"

#define FX_REVERB_INT_CHAN			8	// must be a power of 2, so 2, 4, 8, .... Code has optimized function for 8 parallel channels at the moment
#define FX_REVERB_DIFFUSION_STEPS	2	// 1,2,3,4,...
#define FX_REVERB_AVERAGE_OUTPUT	1	// 0=take fxOut as left/right directly, 1=mixdown multichannel-fxOut to stereo
#define FX_REVERN_ALTERNATIVE_RND	0	// 0=use internal rand() function, 1=use bitshifting chaos (alternative sounds more metallic... not a good choice :-) )
#define FX_REVERN_ALTERNATIVE_POW	0	// 0=use internal powf() function, 1=use approximation (approximation seems to be slower compared to internal powf())
#define FX_REVERB_DELAY_MS_MAX		110	// we need 4608 bytes per millisecond. 400ms -> 1843200 bytes
#define FX_REVERB_BUFFER_SIZE 		((SAMPLERATE_MAX * FX_REVERB_DELAY_MS_MAX) / 1000) // FX_REVERB_BUFFER_SIZE has to be power-of-2, to use optimized pointer-calculation

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

class fxReverb : public fx {
    public:
        fxReverb(int fxSlot, int channelMode);
        ~fxReverb();
        void fxReverbSetParameters(float roomSizeMs, float rt60, float feedbackLowPassFreq, float dry, float wet); // Size in ms, Reverberation Time to -60dB, Frequency for feedback-loop, dry, wet
        void rxData(float data[], int len);
        void process(float* bufIn[], float* bufOut[]);
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
