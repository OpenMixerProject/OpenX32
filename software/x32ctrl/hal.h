#ifndef HAL_H_
#define HAL_H_

#include "x32ctrl.h"

void halSyncVChannelConfigFromMixer(void);
void halSendGain(uint8_t group, uint8_t channel, float gain);
void halSendPhantomPower(uint8_t group, uint8_t channel, bool active);
void halSendVolume(uint8_t dspChannel, float volumeLR, float volumeSub, float balance);
void halSendGate(uint8_t dspChannel);
void halSendEQ(uint8_t dspChannel);
void halSendCompressor(uint8_t dspChannel);

#endif
