#pragma once

//#include "x32ctrl.h"
#include "external.h"
#include "x32ctrl_types.h"
#include "WString.h"
#include "auxiliary.h"
#include "uart.h"
#include "x32config.h"

#define ADDA_MAX_PACKET_LENGTH 32

extern char addaBufferUart[256];
extern uint16_t addaWaitForMessageCounter;

void addaInit(uint32_t samplerate);
void addaSetSamplerate(uint32_t samplerate);
void addaSetGain(uint8_t boardId, uint8_t channel, float gain, bool phantomPower);
String addaSendReceive(char* cmd, uint16_t timeout);
String addaProcessUartData(int bytesToProcess, bool directRead);
void addaSetMute(bool muted);
int8_t addaGetBoardId(uint8_t channel);
void callbackAdda(char* msg);
