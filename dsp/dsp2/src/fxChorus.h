#ifndef __FXCHORUS_H_
#define __FXCHORUS_H_

#include "dsp2.h"

#if FX_USE_CHORUS == 1

#define FX_CHORUS_DELAY_MS_MAX	50
#define FX_CHORUS_BUFFER_SIZE 	((48000 * FX_CHORUS_DELAY_MS_MAX) / 1000)

struct {
	float depthA;
	float depthB;
	int delayLineBaseLengthA;
	int delayLineBaseLengthB;

	float mix;

	int delayLineLengthA;
	int delayLineLengthB;
	int delayLineHeadA;
	int delayLineHeadB;

	float lfoPhaseA;
	float phaseIncA;
	float lfoPhaseB;
	float phaseIncB;
} chorus;

// function prototypes
void fxChorusInit(void);
void fxChorusSetParameters(float depthA, float depthB, float delayA, float delayB, float freqA, float freqB, float phaseA, float phaseB, float mix);
void fxChorusProcess(float* bufIn[2], float* bufOut[2]);

#endif

#endif /* FXCHORUS_H_ */
