#ifndef MIXER_H_
#define MIXER_H_

#include "x32ctrl.h"

extern s_Mixer mixer;

void initMixer(X32_MODEL model);
bool mixerIsModelX32Full();
bool mixerIsModelX32CompacrOrProducer();

void mixerSetVChannelDefaults(s_vChannel* p_vChannel, uint8_t p_vChannelIndex, bool p_disabled);

void mixerSetChangeFlags(uint16_t p_flag);
bool mixerHasChanged(uint16_t p_flag);
bool mixerHasAnyChanged(void);
void mixerResetChangeFlags(void);
void mixerSetVChannelChangeFlagsFromIndex(uint8_t p_chanIndex, uint16_t p_flag);
void mixerSetVChannelChangeFlags(s_vChannel *p_chan, uint16_t p_flag);
bool mixerHasVChannelChanged(s_vChannel *p_chan, uint16_t p_flag);
bool mixerHasVChannelAnyChanged(s_vChannel *p_chan);
void mixerResetVChannelChangeFlags(s_vChannel *p_chan);

uint8_t mixerSurfaceChannel2vChannel(uint8_t surfaceChannel);

void mixerSurfaceButtonPressed(X32_BOARD p_board, uint8_t p_index, uint16_t p_value);
uint8_t mixerGetvChannelIndexFromButtonOrFaderIndex(X32_BOARD p_board, uint16_t p_buttonIndex);
void mixerSurfaceFaderMoved(X32_BOARD p_board, uint8_t p_faderIndex, uint16_t p_faderValue);
void mixerSurfaceEncoderTurned(X32_BOARD p_board, uint8_t p_index, uint16_t p_value);

void mixerChangeSelect(uint8_t direction);
void mixerSetSelect(uint8_t vChannelIndex, bool solo);
void mixerToggleSelect(uint8_t vChannelIndex);
bool mixerIsSoloActivated(void);
void mixerClearSolo(void);

void mixerSetPhantom(uint8_t vChannelIndex, bool p_phantom);
void mixerTogglePhantom(uint8_t vChannelIndex);
void mixerSetPhaseInvert(uint8_t vChannelIndex, bool p_phaseInvert);
void mixerTogglePhaseInvert(uint8_t vChannelIndex);

uint8_t mixerGetSelectedvChannelIndex(void);
s_vChannel* mixerGetSelectedvChannel(void);

void mixerSetSolo(uint8_t vChannelIndex, bool solo);
void mixerToggleSolo(uint8_t vChannelIndex);
void mixerSetMute(uint8_t vChannelIndex, bool solo);
void mixerToggleMute(uint8_t vChannelIndex);
void mixerBanking(X32_BTN p_button);

void mixerChangeGain(uint8_t p_vChannelIndex, int8_t p_amount);
void mixerSetVolume(uint8_t p_vChannelIndex, float p_volume);

void mixerShowPage(X32_PAGE page);


bool mixerIsModelX32Full(void);
bool mixerIsModelX32CompacrOrProducer(void);
bool mixerIsModelX32Core(void);

void mixerDebugPrintBank(uint8_t bank);
void mixerDebugPrintBusBank(uint8_t bank);
void mixerDebugPrintvChannels(void);

#endif