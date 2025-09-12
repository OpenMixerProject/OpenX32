#ifndef ROUTING_H_
#define ROUTING_H_

#include "x32ctrl.h"

void routingInit(void);
void routingDefaultConfig(void);
void routingSyncConfigFromMixer(void);

void routingSetOutputSource(uint8_t group, uint8_t channel, uint8_t inputsource);
void routingSetOutputSourceByIndex(uint8_t index, uint8_t inputsource);
uint8_t routingGetOutputSource(uint8_t group, uint8_t channel);
uint8_t routingGetOutputSourceByIndex(uint8_t index);

uint8_t routingGetSourceIndex(uint8_t group, uint8_t channel);
uint8_t routingGetOutputIndex(uint8_t group, uint8_t channel);
void routingGetSourceGroupAndChannelByIndex(uint8_t index, uint8_t* group, uint8_t* channel);
void routingGetOutputGroupAndChannelByIndex(uint8_t index, uint8_t* group, uint8_t* channel);
void routingGetSourceGroupAndChannelByDspChannel(uint8_t dspChannel, uint8_t* group, uint8_t* channel);

void routingGetSourceName(char* p_nameBuffer, uint8_t group, uint8_t channel);
void routingGetSourceNameByIndex(char* p_nameBuffer, uint8_t index);
void routingGetOutputName(char* p_nameBuffer, s_vChannel* p_chan);
void routingGetOutputNameByIndex(char* p_nameBuffer, uint8_t index);
void routingGetDspSourceName(char* p_nameBuffer, uint8_t dspChannel);

void routingSendConfigToFpga(void);

#endif
