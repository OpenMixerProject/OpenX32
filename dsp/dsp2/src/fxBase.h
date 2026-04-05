#ifndef __FXBASE_H_
#define __FXBASE_H_

#include "dsp2.h"

enum fxType {
	FX_REVERB,
	FX_CHORUS,
	FX_TRANSIENTSHAPER,
	FX_OVERDRIVE,
	FX_DELAY,
	FX_MULTIBANDCOMPRESSOR,
	FX_DEQ,
	FX_DEFEEDBACK,
	FX_MATRIXUPMIXER,

	FX_DEMO,
    FX_NONE
};

class fx {
	public:
		fx(int fxSlot, float* bufIn[], float* bufOut[], int channelMode); // constructor
		void setSampleRate(float sampleRate);
		virtual void process() = 0;
		virtual void rxData(float data[], int len) = 0;
		virtual fxType getType() = 0;
		virtual ~fx(); // destructor
	protected:
		bool _startup;
		int _fxSlot;
		bool _channelMode;
		int _memoryAddress;
		float _sampleRate;
		//void clearMemory();
};

#endif /* FXBASE_H_ */
