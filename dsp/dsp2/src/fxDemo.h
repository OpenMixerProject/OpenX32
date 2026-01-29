#ifndef __FXDEMO_H_
#define __FXDEMO_H_

#include "fxBase.h"

class fxDemo : public fx {
    public:
        fxDemo(int fxSlot, int channelMode);
        ~fxDemo();
        void setParameters(float delayMs);
        void rxData(float data[], int len);
        void process(float* __restrict bufIn[], float* __restrict bufOut[]);
    private:
        int _delayLineLengthMaxMs;
        int _delayLineBufferSize;
        float* _delayLineL;
        float* _delayLineR;
    	int _delayLineHead; // this is valid for all delay-lines of same length
    	int _delayLineTailOffset; // if you want different delays for left and right, use two tail-offsets
};

#endif /* FXDEMO_H_ */
