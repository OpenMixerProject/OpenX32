#ifndef ADDA_H_
#define ADDA_H_

#include "x32ctrl.h"

#define ADDA_MAX_PACKET_LENGTH 32

extern char uartBufferAdda[256];

void addaInit(uint32_t samplerate);
void addaSetSamplerate(uint32_t samplerate);
void addaSetGain(uint8_t boardId, uint8_t channel, float gain, bool phantomPower);
void addaSendCmd(char *cmd);
void addaProcessUartData(int bytesToProcess);
void addaSetMute(bool muted);
int8_t addaGetBoardId(uint8_t channel);

#endif
