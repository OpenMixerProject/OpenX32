#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "dsp2.h"

#ifdef __cplusplus
extern "C" {
#endif

// function prototypes
void systemPllInit(void);
void systemExternalMemoryInit(void);

void systemPcgInit(void);
void systemSruInit(void);
void systemSportInit(void);
void systemSpdifTxInit(void);

void systemCrash(void);

#ifdef __cplusplus
}
#endif

#endif
