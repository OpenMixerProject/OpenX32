#ifndef __FXOVERDRIVE_H_
#define __FXOVERDRIVE_H_

#include "fxBase.h"

#define FX_OVERDRIVE_COEFF_LENGTH 69

class fxOverdrive : public fx {
    public:
        fxOverdrive(int fxSlot, int channelMode);
        ~fxOverdrive();
        void setFilters(float hpfInputFreq, float lpfInputFreq, float lpfOutputFreq);
        void setGain(float preGain, float Q);
        void rxData(float data[], int len);
        void process(float* __restrict bufIn[], float* __restrict bufOut[]);
    private:
    	float _hpfInputCoef;
    	float _lpfInputCoef;
    	float _lpfOutputCoef;

    	float _hpfInputStateIn;
    	float _hpfInputStateOut;
    	float _lpfInputState;
    	float _lpfOutputState;

    	float _preGain;
    	float _Q;
    	float _clipConst;
};

#endif /* FXOVERDRIVE_H_ */
