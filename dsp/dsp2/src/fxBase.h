#ifndef __FXBASE_H_
#define __FXBASE_H_

#include "dsp2.h"

class fx {
	public:
		fx(); // constructor
		fx(int fxSlot, int channelMode); // constructor
		void setSampleRate(float sampleRate);
		virtual void process(float* bufIn[], float* bufOut[]) = 0;
		virtual void rxData(float data[], int len) = 0;
		virtual ~fx(void); // destructor
	protected:
		int _fxSlot;
		bool _channelMode;
		int _memoryAddress;
		float _sampleRate;
		void clearMemory();
};

#endif /* FXBASE_H_ */
