#ifndef __FXDELAY_H_
#define __FXDELAY_H_

#include "fxBase.h"

class fxDelay : public fx {
    public:
        fxDelay(int fxSlot, int channelMode);
        ~fxDelay();
        void setParameters(float delayMsL, float delayMsR);
        void rxData(float data[], int len);
        void process(float* __restrict bufIn[], float* __restrict bufOut[]);
    private:
        int _delayLineLengthMaxMs;
        int _delayLineBufferSize;
        float* _delayLineL;
        float* _delayLineR;
    	int _delayLineHead; // this is valid for all delay-lines of same length
    	int _delayLineTailOffsetL;
    	int _delayLineTailOffsetR;
};

#endif /* fxDelay_H_ */
