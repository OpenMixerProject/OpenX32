#ifndef HELPERFCN_H_
#define HELPERFCN_H_

#include "defines.h"
#include "math.h"

float helperFcn_db2lin(float db);
float helperFcn_lin2db(float lin);
void helperFcn_calcBiquadCoeffs(int type, float frequency, float Q, float gain, float peqCoeffs[], float samplerate);
void helperFcn_calcBandpassCoeff(float frequency, float Q, float peqCoeffs[], float samplerate);
void helperFcn_calcPeqCoeff(float frequency, float Q, float gain, float peqCoeffs[], float samplerate);
void helperFcn_lowPassFilter(float input, float* output, float coeff);
void helperFcn_ringBufferWrite(float value, float* buffer, int bufferSize, int* head);
float helperFcn_ringBufferRead(float* buffer, int bufferSize, int tailOffset, int head);
float helperFcn_ringBufferReadMs(float* buffer, int bufferSize, int delayMs, int head);

#endif /* HELPERFCN_H_ */
