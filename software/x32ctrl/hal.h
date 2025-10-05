#pragma once

#include "x32ctrl.h"
#include "uart.h"
#include "adda.h"
#include "mixer.h"
#include "dsp.h"

void halSyncChannelConfigFromMixer(void);
void halSetVolume(uint8_t dspChannel, float volume);
void halSetMute(uint8_t dspChannel, bool mute);
void halSetSolo(uint8_t dspChannel, bool solo);
void halSetBalance(uint8_t dspChannel, float balance);
void halSetGain(uint8_t dspChannel, float gain);
void halSetPhantomPower(uint8_t dspChannel, bool phantomPower);
void halSetPhaseInversion(uint8_t dspChannel, bool phaseInverted);
void halSetBusSend(uint8_t dspChannel, uint8_t index, float value);

void halSendGain(uint8_t dspChannel);
void halSendPhantomPower(uint8_t dspChannel);

uint8_t halGetDspInputSource(uint8_t dspChannel);
float halGetVolume(uint8_t dspChannel);
bool halGetMute(uint8_t dspChannel);
bool halGetSolo(uint8_t dspChannel);
float halGetBalance(uint8_t dspChannel);
float halGetGain(uint8_t dspChannel);
bool halGetPhantomPower(uint8_t dspChannel);
bool halGetPhaseInvert(uint8_t dspChannel);
float halGetBusSend(uint8_t dspChannel, uint8_t index);
