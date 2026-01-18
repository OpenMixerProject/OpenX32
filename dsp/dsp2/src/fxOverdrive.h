#ifndef __FXOVERDRIVE_H_
#define __FXOVERDRIVE_H_

#include "dsp2.h"

#if FX_USE_OVERDRIVE == 1

struct {
	float hpfInputCoef;
	float lpfInputCoef;
	float lpfOutputCoef;

	float hpfInputStateIn;
	float hpfInputStateOut;
	float lpfInputState;
	float lpfOutputState;

	float preGain;
	float Q;
	float clipConst;
} overdrive;

// function prototypes
void fxOverdriveInit(void);
void fxOverdriveSetFilters(float hpfInputFreq, float lpfInputFreq, float lpfOutputFreq);
void fxOverdriveSetGain(float preGain, float Q);
void fxOverdriveProcess(float* bufIn, float* bufOut);

#endif

#endif /* FXOVERDRIVE_H_ */
