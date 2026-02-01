#ifndef __FXBASE_H_
#define __FXBASE_H_

#include "dsp2.h"

class fx {
	public:
		fx(int fxSlot, int channelMode); // constructor
		void setSampleRate(float sampleRate);
		virtual void process(float* __restrict bufIn[], float* __restrict bufOut[]) = 0;
		virtual void rxData(float data[], int len) = 0;
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
