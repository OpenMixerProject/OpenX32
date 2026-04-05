#ifndef __FXMATRIXUPMIXER_H_
#define __FXMATRIXUPMIXER_H_

#include "fxBase.h"

#define FX_MATRIXUPMIXER_DELAYBACK_MS	17	// 17ms delay to back-surround
#define FX_MATRIXUPMIXER_DELAY_MS_MAX	25
#define FX_MATRIXUPMIXER_BUFFER_SIZE 	((SAMPLERATE_MAX * FX_MATRIXUPMIXER_DELAY_MS_MAX) / 1000)

class fxMatrixUpmixer : public fx {
    public:
        fxMatrixUpmixer(int fxSlot, float* inBuf[], float* outBuf[], int channelMode);
        ~fxMatrixUpmixer();
        void rxData(float data[], int len);
        fxType getType() { return FX_MATRIXUPMIXER; };
        void process();
    private:
        float* __restrict _bufIn[2];
        float* __restrict _bufOut[6];

    	float _bufTemp[SAMPLES_IN_BUFFER];

    	int _delayLineLengthMaxMs;
        int _delayLineBufferSize;
        float* _delayLine;
    	int _delayLineHead;

    	float _lowPassSubState;
    	float _lowPassSubCoeff;
    	float _lowPassSurroundState;
    	float _lowPassSurroundCoeff;
};

#endif /* FXMATRIXUPMIXER_H_ */
