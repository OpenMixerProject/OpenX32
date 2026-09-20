#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Host implementation of Analog Devices SHARC biquad_trans().
 *
 * Implements Direct Form II Transposed cascaded biquad sections.
 * The coefficient layout matches ADI SHARC runtime conventions as
 * documented in fxSetPeqCoeffs():
 *   Pairs of sections (2k, 2k+1) are interleaved:
 *     a0[2k], a0[2k+1], a1[2k], a1[2k+1], a2[2k], a2[2k+1],
 *     -b1[2k], -b1[2k+1], -b2[2k], -b2[2k+1]
 *   Trailing single section (if n_sections is odd):
 *     a0, a1, a2, -b1, -b2
 *
 * State array stores 2 delay states per section (w1, w2):
 *   y[n]  = a0 * x[n] + w1[n-1]
 *   w1[n] = a1 * x[n] + (-b1) * y[n] + w2[n-1]
 *   w2[n] = a2 * x[n] + (-b2) * y[n]
 *
 * Note: The stored b-coefficients in peqCoeffs already have the negation applied
 * (i.e. stored as -b1, -b2). Thus we add (stored_b * y).
 */
static inline void biquad_trans(float input_output[], const float coeffs[], float state[], int n_samples, int n_sections) {
    for (int sec = 0; sec < n_sections; sec++) {
        float a0, a1, a2, neg_b1, neg_b2;
        int pair_base = ((sec / 2) * 2) * 5;
        if (((n_sections % 2) == 0) || (sec < (n_sections - 1))) {
            int odd = (sec % 2);
            a0     = coeffs[pair_base + 0 + odd];
            a1     = coeffs[pair_base + 2 + odd];
            a2     = coeffs[pair_base + 4 + odd];
            neg_b1 = coeffs[pair_base + 6 + odd];
            neg_b2 = coeffs[pair_base + 8 + odd];
        } else {
            int single_base = (n_sections - 1) * 5;
            a0     = coeffs[single_base + 0];
            a1     = coeffs[single_base + 1];
            a2     = coeffs[single_base + 2];
            neg_b1 = coeffs[single_base + 3];
            neg_b2 = coeffs[single_base + 4];
        }

        float w1 = state[sec * 2 + 0];
        float w2 = state[sec * 2 + 1];

        for (int s = 0; s < n_samples; s++) {
            float x = input_output[s];
            float y = a0 * x + w1;
            w1 = a1 * x + neg_b1 * y + w2;
            w2 = a2 * x + neg_b2 * y;
            input_output[s] = y;
        }

        state[sec * 2 + 0] = w1;
        state[sec * 2 + 1] = w2;
    }
}

#ifdef __cplusplus
}
#endif
