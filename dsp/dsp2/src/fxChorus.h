#ifndef __FXCHORUS_H_
#define __FXCHORUS_H_

#include "fxBase.h"

class fxChorus : public fx {
    public:
        fxChorus(int fxSlot, float* inBuf[], float* outBuf[], int channelMode);
        ~fxChorus();
        void setParameters(float depthA, float depthB, float delayA, float delayB, float freqA, float freqB, float phaseA, float phaseB, float mix);
        void rxData(float data[], int len);
        fxType getType() { return FX_CHORUS; };
        void process();
    private:
        float* __restrict _bufIn[2];
        float* __restrict _bufOut[2];

        int _delayLineLengthMaxMs;
        int _delayLineBufferSize;
        float* _delayLineA;
        float* _delayLineB;

        float _depthA;
    	float _depthB;
    	int _delayLineBaseLengthA;
    	int _delayLineBaseLengthB;

    	float _mix;

    	int _delayLineHeadA;
    	int _delayLineHeadB;

    	float _lfoPhaseA;
    	float _lfoPhaseB;
    	float _phaseIncA;
    	float _phaseIncB;
};

#endif /* FXCHORUS_H_ */
