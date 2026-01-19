#ifndef __FXTRANSIENTSHAPER_H_
#define __FXTRANSIENTSHAPER_H_

#include "fxBase.h"

#define FX_TRANSIENTSHAPER_DELAY_MS_MAX		50
#define FX_TRANSIENTSHAPER_BUFFER_SIZE 		((SAMPLERATE_MAX * FX_TRANSIENTSHAPER_DELAY_MS_MAX) / 1000)

class fxTransientshaper : public fx {
    public:
        fxTransientshaper(int fxSlot, int channelMode);
        ~fxTransientshaper();
        void fxTransientshaperSetParameters(float kFast, float kSlow, float attack, float sustain, float delayMs);
        void rxData(float data[], int len);
        void process(float* bufIn[], float* bufOut[]);
    private:
        float* _delayLine;

    	float _attack;
    	float _sustain;

    	float _kFast;
    	float _kSlow;

    	int _delayLineHead;
    	int _delayLineTailOffset;
    	float _envelopeFast;
    	float _envelopeSlow;
};

#endif  /* FXTRANSIENTSHAPER_H_ */
