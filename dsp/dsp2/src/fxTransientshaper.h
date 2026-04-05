#ifndef __FXTRANSIENTSHAPER_H_
#define __FXTRANSIENTSHAPER_H_

#include "fxBase.h"

#define FX_TRANSIENTSHAPER_DELAY_MS_MAX		50
#define FX_TRANSIENTSHAPER_BUFFER_SIZE 		((SAMPLERATE_MAX * FX_TRANSIENTSHAPER_DELAY_MS_MAX) / 1000)

class fxTransientshaper : public fx {
    public:
        fxTransientshaper(int fxSlot, float* inBuf[], float* outBuf[], int channelMode);
        ~fxTransientshaper();
        void setParameters(float timeFast, float timeMed, float timeSlow, float attack, float sustain, float delayMs);
        void rxData(float data[], int len);
        fxType getType() { return FX_TRANSIENTSHAPER; };
        void process();
    private:
        float* __restrict _bufIn[2];
        float* __restrict _bufOut[2];

        float* _delayLine;

    	float _attack;
    	float _sustain;

    	float _kFast;
    	float _kMed;
    	float _kSlow;

    	int _delayLineHead;
    	int _delayLineTailOffset;
    	float _envelopeFast;
    	float _envelopeMed;
    	float _envelopeSlow;
};

#endif  /* FXTRANSIENTSHAPER_H_ */
