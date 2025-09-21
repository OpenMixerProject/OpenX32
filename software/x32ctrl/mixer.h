#ifndef MIXER_H_
#define MIXER_H_

#include "x32ctrl.h"

extern sMixer mixer;

void mixerInit(char model[]);
bool mixerIsModelX32Full(void);
bool mixerIsModelX32FullOrCompacrOrProducer(void);
bool mixerIsModelX32FullOrCompacrOrProducerOrRack(void);
bool mixerIsModelX32CompacrOrProducer(void);
bool mixerIsModelX32Core(void);
bool mixerIsModelX32Rack(void);

void mixerSetChannelDefaults(sChannel* pChannel, uint8_t pChannelIndex, bool p_disabled);

void mixerSetChangeFlags(uint16_t p_flag);
bool mixerHasChanged(uint16_t p_flag);
bool mixerHasAnyChanged(void);
void mixerResetChangeFlags(void);
void mixerSetChannelChangeFlagsFromIndex(uint8_t p_chanIndex, uint16_t p_flag);
void mixerSetChannelChangeFlags(sChannel* p_chan, uint16_t p_flag);
bool mixerHasChannelChanged(sChannel* p_chan, uint16_t p_flag);
bool mixerHasChannelAnyChanged(sChannel* p_chan);
void mixerResetChannelChangeFlags(sChannel* p_chan);

uint8_t mixerSurfaceChannel2Channel(uint8_t surfaceChannel);

void mixerSurfaceButtonPressed(X32_BOARD p_board, uint8_t p_index, uint16_t p_value);
uint8_t mixerGetChannelIndexFromButtonOrFaderIndex(X32_BOARD p_board, uint16_t p_buttonIndex);
void mixerSurfaceFaderMoved(X32_BOARD p_board, uint8_t p_faderIndex, uint16_t p_faderValue);
void mixerSurfaceEncoderTurned(X32_BOARD p_board, uint8_t p_index, uint16_t p_value);

void mixerChangeSelect(uint8_t direction);
void mixerSetSelect(uint8_t channelIndex, bool solo);
void mixerToggleSelect(uint8_t channelIndex);
bool mixerIsSoloActivated(void);
void mixerClearSolo(void);

void mixerSetPhantom(uint8_t channelIndex, bool p_phantom);
void mixerTogglePhantom(uint8_t channelIndex);
void mixerSetPhaseInvert(uint8_t channelIndex, bool p_phaseInvert);
void mixerTogglePhaseInvert(uint8_t channelIndex);

uint8_t mixerGetSelectedChannelIndex(void);
sChannel* mixerGetSelectedChannel(void);

void mixerSetSolo(uint8_t channelIndex, bool solo);
void mixerToggleSolo(uint8_t channelIndex);
void mixerSetMute(uint8_t channelIndex, bool solo);
void mixerToggleMute(uint8_t channelIndex);
void mixerBankingSends(X32_BTN p_button);
void mixerBankingEQ(X32_BTN p_button);
void mixerBanking(X32_BTN p_button);

void mixerChangeGain(uint8_t pChannelIndex, int8_t p_amount);
void mixerChangeVolume(uint8_t pChannelIndex, int8_t p_amount);
void mixerSetVolume(uint8_t pChannelIndex, float p_volume);
void mixerChangePan(uint8_t pChannelIndex, int8_t p_amount);
void mixerChangeBusSend(uint8_t pChannelIndex, uint8_t encoderIndex, int8_t p_amount);
void mixerChangeGate(uint8_t pChannelIndex, int8_t p_amount);
void mixerChangeLowcut(uint8_t pChannelIndex, int8_t p_amount);
void mixerChangeDynamics(uint8_t pChannelIndex, int8_t p_amount);
void mixerChangePeq(uint8_t pChannelIndex, char option, int8_t p_amount);

void mixerInitPages();
void mixerShowPage(X32_PAGE page);
void mixerShowPrevPage(void);
void mixerShowNextPage(void);

void mixerDebugPrintBank(uint8_t bank);
void mixerDebugPrintBusBank(uint8_t bank);
void mixerDebugPrintChannels(void);

#endif
