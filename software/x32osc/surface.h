#ifndef SURFACE_H_
#define SURFACE_H_

#include "x32osc-reaper.h"
#include "../x32ctrl/constants.h"

void setFader(uint8_t boardId, uint8_t index, uint16_t position);
void setLed(uint8_t boardId, uint8_t ledId, uint8_t state);
void setLedByNr(uint16_t ledNr, uint8_t state);
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

void surfaceReset();

#endif
