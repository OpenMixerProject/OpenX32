#ifndef RTA_H_
#define RTA_H_

#include "dsp2.h"

// configure the FFT
#define RTA_FFT_SIZE		512
#define RTA_FFT_SIZE_HALF	256
#define RTA_DISPLAY_BANDS	64

#define RTA_FFT_FFT_MODE	1 // 0 = rfft(), 1 = rfftN

#if RTA_FFT_FFT_MODE == 0
	extern float rta_rspectrum[RTA_FFT_SIZE_HALF + 1];
#elif RTA_FFT_FFT_MODE == 1
	extern float pm rta_display[64];
#endif

// function prototypes
void rtaInit(void);
void rtaProcess(float* __restrict inBuf);

#endif /* RTA_H_ */
