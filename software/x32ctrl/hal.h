#ifndef HAL_H_
#define HAL_H_

#include "x32ctrl.h"

void halSyncVChannelConfigFromMixer(void);
void halSetGain(uint8_t group, uint8_t channel, float gain);
void halSetPhantomPower(uint8_t group, uint8_t channel, bool active);
void halSetVolume(uint8_t channel, float volume);
void halSetBalance(uint8_t channel, uint8_t balance);

#endif
