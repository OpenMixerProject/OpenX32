#ifndef SURFACE_H_
#define SURFACE_H_

#include "x32ctrl.h"

extern char surfaceBufferUart[256];

void surfaceSync(void);
void surfaceSyncBoardMain();
void surfaceSyncBoard(X32_BOARD board);
void surfaceSyncBankIndicator(void);

void setFader(uint8_t boardId, uint8_t index, uint16_t position);
void setDisplay(uint8_t p_value);
void setDisplayRaw(uint8_t p_value2, uint8_t p_value1);
uint8_t int2segment(int8_t p_value);
void setLed(uint8_t boardId, uint8_t ledId, uint8_t state);
void setLedByNr(uint16_t ledNr, uint8_t state);
void setLedByEnum(X32_BTN led, uint8_t state);
void setLedChannelIndicator();
void setMeterLed(uint8_t boardId, uint8_t index, uint8_t leds);
void setMeterLedMain(uint8_t preamp, uint8_t dynamics, uint32_t meterL, uint32_t meterR, uint32_t meterSolo);
void setEncoderRing(uint8_t boardId, uint8_t index, uint8_t ledMode, uint8_t pct, bool backlight);
void setBrightness(uint8_t boardId, uint8_t brightness);
void setContrast(uint8_t boardId, uint8_t contrast);
void setLcd(uint8_t boardId, uint8_t index, uint8_t color, uint8_t xicon, uint8_t yicon, uint8_t icon, uint8_t sizeA, uint8_t xA, uint8_t yA, const char* strA, uint8_t sizeB, uint8_t xB, uint8_t yB, const char* strB);
void setLcdX(sLCDData* p_data, uint8_t p_textCount);
void setLcdFromChannel(uint8_t p_boardId, uint8_t p_Index, sChannel* p_chan);
#if DEBUG
void setLcdX2(uint8_t plen, uint8_t textlen);
#endif

uint16_t calcEncoderRingLedIncrement(uint8_t pct);
uint16_t calcEncoderRingLedPosition(uint8_t pct);
uint16_t calcEncoderRingLedBalance(uint8_t pct);
uint16_t calcEncoderRingLedWidth(uint8_t pct);
uint16_t calcEncoderRingLedDecrement(uint8_t pct);

void surfaceKeepalive(void);

void surfaceInit(void);
void surfaceReset(void);
void surfaceProcessUartData(int bytesToProcess);

void initDefinitions(void);
void addButtonDefinition(X32_BTN p_button, uint16_t p_buttonNr);
void addEncoderDefinition(X32_ENC p_encoder, uint16_t p_encoderNr);

uint16_t enum2button(X32_BTN button);
X32_BTN button2enum(uint16_t button);
uint16_t enum2encoder(X32_ENC encoder);
X32_ENC encoder2enum(uint16_t encoderNr);

#endif
