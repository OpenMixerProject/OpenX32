#ifndef __FXMULTIBANDCOMPRESSOR_H_
#define __FXMULTIBANDCOMPRESSOR_H_

#include "fxBase.h"

typedef struct {
	// filter-data from i.MX25
	float value_threshold;
	float value_ratio;
	float value_coeff_attack;
	float value_hold_ticks;
	float value_coeff_release;
	float value_makeup;

	// online parameters
	int holdTimer;
	float envelope;
	float coeff;

	float coeffs[5 * 4];
	float states[2 * 4];
} sCompressor;

class fxMultibandCompressor : public fx {
    public:
        fxMultibandCompressor(int fxSlot, float* inBuf[], float* outBuf[], int channelMode);
        ~fxMultibandCompressor();
        void setFrequencies(int channel, float f0, float f1, float f2, float f3);
        void setParameters(int channel, int band, float threshold, float ratio, float attack, float hold, float release, float makeup);
        void rxData(float data[], int len);
        fxType getType() { return FX_MULTIBANDCOMPRESSOR; };
        void process();
    private:
        float* __restrict _bufIn[2];
        float* __restrict _bufOut[2];

        bool _dualMono;

    	float _buffer[2][SAMPLES_IN_BUFFER]; // buffer for audio-sample left/right
    	sCompressor _compressor[2][5]; // we are implementing a dual-mono-multiband-compressor here

    	void processLogic(sCompressor* compressor, float sample);
};

#endif /* FXMULTIBANDCOMPRESSOR */
