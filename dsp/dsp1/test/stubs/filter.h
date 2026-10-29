/* host-test stub for the CCES header <filter.h> */
#pragma once

/* Reference model of biquad_trans(): in-place cascade of transposed direct form II sections.
   Coefficient layout per section is a0 a1 a2 -b1 -b2; pairs of sections are interleaved
   (a0 a0 a1 a1 ...) and an odd last section is stored plainly, see fxSetPeqCoeffs(). */
float* biquad_trans(float* io, const float* coeffs, float* state, int samples, int sections);
