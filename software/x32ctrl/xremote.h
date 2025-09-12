#ifndef XREMOTE_H_
#define XREMOTE_H_

#include "x32ctrl.h"

// function prototypes
int8_t xremoteInit();
void xremoteUpdateAll();
void xremoteUdpHandleCommunication();
void xremoteAnswerInfo();
void xremoteAnswerXInfo();
void xremoteAnswerStatus();
void xremoteSetFader(uint8_t ch, float value_pu);
void xremoteSetPan(uint8_t ch, float value_pu);
void xremoteSetMainFader(float value_pu);
void xremoteSetMainPan(float value_pu);
void xremoteSetName(uint8_t ch, String name);
void xremoteSetColor(uint8_t ch, int32_t color);
void xremoteSetSource(uint8_t ch, int32_t source);
void xremoteSetIcon(uint8_t ch, int32_t icon);
void xremoteSetCard(uint8_t card);
void xremoteSetMute(uint8_t ch, uint8_t muted);
void xremoteSetSolo(uint8_t ch, uint8_t solo);
void xremoteUpdateMeter();
void xremoteSendUdpPacket(char* buffer, uint16_t size);
void xremoteSendBasicMessage(char* cmd, char type, char format, char* value);
uint16_t xremotesprint(char* bd, uint16_t index, char format, const char* bs);
uint16_t xremotefprint(char* bd, uint16_t index, char* text, char format, char* bs);

#endif