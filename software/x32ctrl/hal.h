#ifndef HAL_H_
#define HAL_H_

#include "x32ctrl.h"

void halSyncVChannelConfigFromMixer(void);
void halSendGain(uint8_t group, uint8_t channel, float gain);
void halSendPhantomPower(uint8_t group, uint8_t channel, bool active);

#endif
