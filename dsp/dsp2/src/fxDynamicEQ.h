#ifndef __FXDYNAMICEQ_H_
#define __FXDYNAMICEQ_H_

#include "fxBase.h"

#define FX_DYNAMICEQ_BANDS	1	// 2 or 3 could be fine if we have enough program- and heap-memory available

typedef struct {
	// configuration-variables
	int type;				// Supported types: LowShelf / Peak / HighShelf
	int typeCtrl;			// LowPass / Bandpass / HighPass
    float frequency;		// center-frequency
    float maxDynamicGain;	// the maximum gain the DEQ can reach
    float Q;				// quality
    float threshold;		// threshold in dB
    float ratio;			// e.g. 4.0 für 4:1
    float attack;			// coefficient (0.0 to 1.0)
    float release;			// coefficient (0.0 to 1.0)

    // background-variables
    float envelope;					// current Envelope-Level
    float smoothingCoeff;			// alpha-coefficient for 1st-order low-pass
    float smoothedTargetGain;		// new target-gain filtered by 1st-order-low-pass
    float biquadCoeffsCtrl[5];		// coefficients for control-signal
    float biquadStatesCtrl[2][2];	// states for control-signal
} sDynamicEQ;

class fxDynamicEQ : public fx {
    public:
        fxDynamicEQ(int fxSlot, int channelMode);
        ~fxDynamicEQ();
        void setParameters(int band, int type, float frequency, float maxDynamicGain, float Q, float threshold, float ratio, float attack, float release);
        void rxData(float data[], int len);
        void process(float* __restrict bufIn[], float* __restrict bufOut[]);
    private:
        sDynamicEQ _deq[FX_DYNAMICEQ_BANDS];
        float _biquadStates[2][2 * FX_DYNAMICEQ_BANDS];	// coefficients for left/right-signal
};

#endif /* FXDYNAMICEQ_H_ */
