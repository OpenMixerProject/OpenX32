#ifndef __FXOVERDRIVE_H_
#define __FXOVERDRIVE_H_

#include "fxBase.h"

#define FX_OVERDRIVE_COEFF_LENGTH 69

class fxOverdrive : public fx {
    public:
        fxOverdrive(int fxSlot, float* inBuf[], float* outBuf[], int channelMode);
        ~fxOverdrive();
        void setParameters(float preGain, float Q, float hpfInputFreq, float lpfInputFreq, float lpfOutputFreq);
        void rxData(float data[], int len);
        fxType getType() { return FX_OVERDRIVE; };
        void process();
    private:
        float* __restrict _bufIn[2];
        float* __restrict _bufOut[2];

    	float _hpfInputCoef;
    	float _lpfInputCoef;
    	float _lpfOutputCoef;

    	float _hpfInputStateIn[2];
    	float _hpfInputStateOut[2];
    	float _lpfInputState[2];
    	float _lpfOutputState[2];

    	float _preGain;
    	float _Q;
    	float _clipConst;
};

#endif /* FXOVERDRIVE_H_ */
