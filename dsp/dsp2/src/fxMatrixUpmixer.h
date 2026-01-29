#ifndef __FXMATRIXUPMIXER_H_
#define __FXMATRIXUPMIXER_H_

#include "fxBase.h"

#if FX_USE_MATRIXUPMIXER == 1

#define FX_MATRIXUPMIXER_DELAYBACK_MS	17	// 17ms delay to back-surround
#define FX_MATRIXUPMIXER_DELAY_MS_MAX	25
#define FX_MATRIXUPMIXER_BUFFER_SIZE 	((SAMPLERATE_MAX * FX_MATRIXUPMIXER_DELAY_MS_MAX) / 1000)

class fxMatrixUpmixer : public fx {
    public:
        fxMatrixUpmixer(int fxSlot, int channelMode);
        ~fxMatrixUpmixer();
        void rxData(float data[], int len);
        void process(float* __restrict bufIn[], float* __restrict bufOut[]);
    private:
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

#endif

#endif /* FXMATRIXUPMIXER_H_ */
