/*
  Host-side replacements for the parts of DSP1 that only exist on the SHARC
  (dsp1.c, spi.c and the CCES runtime library). They let audio.c, comm.c and
  fx.c run unchanged on a PC for the unit tests in this folder.
*/

#include "dsp1.h"
#include "spi.h"

volatile uint32_t cyclemap[CYCLEMAP_SIZE];
float spiCommData[SPI_DMA_COMMDATA_SIZE];

void spiDmaBegin(unsigned int* buffer, int len, bool receive) {
	(void)buffer;
	(void)len;
	(void)receive;
}

float* biquad_trans(float* io, const float* coeffs, float* state, int samples, int sections) {
	for (int i_sec = 0; i_sec < sections; i_sec++) {
		const float* c;
		int stride;
		if ((i_sec == sections - 1) && ((sections % 2) != 0)) {
			// odd last section is stored without interleaving
			c = &coeffs[i_sec * 5];
			stride = 1;
		} else {
			c = &coeffs[((i_sec / 2) * 2) * 5 + (i_sec % 2)];
			stride = 2;
		}
		float a0 = c[0 * stride];
		float a1 = c[1 * stride];
		float a2 = c[2 * stride];
		float nb1 = c[3 * stride]; // -b1
		float nb2 = c[4 * stride]; // -b2
		float* s1 = &state[2 * i_sec + 0];
		float* s2 = &state[2 * i_sec + 1];

		for (int s = 0; s < samples; s++) {
			float x = io[s];
			float y = a0 * x + *s1;
			*s1 = a1 * x + nb1 * y + *s2;
			*s2 = a2 * x + nb2 * y;
			io[s] = y;
		}
	}
	return io;
}
