#ifndef SURFACE_H_
#define SURFACE_H_

#include "x32ctrl.h"

#define SURFACE_MAX_PACKET_LENGTH 32

#define SURFACE_COLOR_BLACK 0
#define SURFACE_COLOR_RED 1
#define SURFACE_COLOR_GREEN 2
#define SURFACE_COLOR_YELLOW 3
#define SURFACE_COLOR_BLUE 4
#define SURFACE_COLOR_PINK 5
#define SURFACE_COLOR_CYAN 6
#define SURFACE_COLOR_WHITE 7
#define SURFACE_COLOR_INVERTED 8

typedef struct{
    X32_BTN button;
    uint16_t buttonNr;
} sButtonInfo;

typedef struct{
    X32_ENC encoder;
    uint16_t encoderNr;
} sEncoderInfo;

extern char uartBufferSurface[256];

void setFader(uint8_t boardId, uint8_t index, uint16_t position);
void setLed(uint8_t boardId, uint8_t ledId, uint8_t state);
void setLedByNr(uint16_t ledNr, uint8_t state);
void setLedByEnum(X32_BTN led, uint8_t state);
void setMeterLed(uint8_t boardId, uint8_t index, uint8_t leds);
void setMeterLedMain(uint8_t preamp, uint8_t dynamics, uint32_t meterL, uint32_t meterR, uint32_t meterSolo);
void setEncoderRing(uint8_t boardId, uint8_t index, uint8_t ledMode, uint8_t pct, bool backlight);
void setBrightness(uint8_t boardId, uint8_t brightness);
void setContrast(uint8_t boardId, uint8_t contrast);
void setLcd(uint8_t boardId, uint8_t index, uint8_t color, uint8_t xicon, uint8_t yicon, uint8_t icon, uint8_t sizeA, uint8_t xA, uint8_t yA, const char* strA, uint8_t sizeB, uint8_t xB, uint8_t yB, const char* strB);

uint16_t calcEncoderRingLedIncrement(uint8_t pct);
uint16_t calcEncoderRingLedPosition(uint8_t pct);
uint16_t calcEncoderRingLedBalance(uint8_t pct);
uint16_t calcEncoderRingLedWidth(uint8_t pct);

void surfaceInit(void);
void surfaceReset(void);
void surfaceProcessUartData(int bytesToProcess);

void initDefinitions();
void addButtonDefinition(X32_BTN p_button, uint16_t p_buttonNr);
void addEncoderDefinition(X32_ENC p_encoder, uint16_t p_encoderNr);

uint16_t enum2button(X32_BTN button);
X32_BTN button2enum(uint16_t button);
X32_ENC encoder2enum(uint16_t encoderNr);

#endif
