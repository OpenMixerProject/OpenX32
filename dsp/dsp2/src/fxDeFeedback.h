#ifndef __FXDEFEEDBACK_H_
#define __FXDEFEEDBACK_H_

#include "fxBase.h"

#include "fxDeFeedback_coeffs.h"

#define TAPS 					128      // filter-length
#define LEAKAGE 				0.99995f // allow a soft leakage into the signal
#define INTERVENTION_THRESHOLD	0.95f    // a threshold when the filter should fight against the feedback
#define MU_MAX					0.001f	 // map the AI-decision to a small learning-rate

class fxDeFeedback : public fx {
    public:
		fxDeFeedback(int fxSlot, float* inBuf[], float* outBuf[], int channelMode);
        ~fxDeFeedback();
        void setParameters(float delayMs);
        void rxData(float data[], int len);
        fxType getType() { return FX_DEFEEDBACK; };
        void process();
    private:
        float* _bufIn[2];
        float* _bufOut[2];

        int weightsRst;
        int historyRst;

        // filter-storages
        float weights[TAPS];
        float history[TAPS + SAMPLES_IN_BUFFER];

        float nn_inference_scalar(float* __restrict input);
};

#endif /* FXDEFEEDBACK_H_ */
