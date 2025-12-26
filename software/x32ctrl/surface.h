#pragma once

#include <stdint.h>
#include <list>
#include <math.h>

#include "base.h"
#include "uart.h"
#include "defines.h"
#include "types.h"
#include "lcd.h"
#include "surface-event.h"
#include "surface-message.h"
#include "surface-fader.h"
#include "surface-button.h"
#include "helper.h"
#include "vchannel.h"


using namespace std;

class Surface : public X32Base
{
    private:
        SurfaceFader faders[MAX_FADERS];

        bool blinkstate = false;
        set<uint16_t> blinklist;

        uint8_t int2segment(int8_t p_value);

        uint16_t CalcEncoderRingLedIncrement(uint8_t pct);
        uint16_t CalcEncoderRingLedDecrement(uint8_t pct);
        uint16_t CalcEncoderRingLedPosition(uint8_t pct);
        uint16_t CalcEncoderRingLedDbfs(float dbfs, bool onlyPosition);
        uint16_t CalcEncoderRingLedBalance(uint8_t pct);
        uint16_t CalcEncoderRingLedWidth(uint8_t pct);

        void InitDefinitions(void);        
        void AddLedDefinition(X32_BTN led, uint16_t ledNr);
        void AddButtonDefinition(X32_BTN p_button, uint16_t p_buttonNr, bool noLed);
        void AddEncoderDefinition(X32_ENC p_encoder, uint16_t p_encoderNr); 

        void SetFaderRaw(uint8_t boardId, uint8_t index, uint16_t position);
        uint8_t GetFaderIndex(uint8_t boardId, uint8_t index);
        uint8_t GetBoardId(uint8_t faderindex);
        uint8_t GetFaderId(uint8_t faderindex);

        void Blink();

    public:
        Surface(X32BaseParameter* basepar);
        Uart* uart;

        void Init();
        void Reset();
        void Tick10ms();
        void Tick100ms();

        void SetBrightness(uint8_t boardId, uint8_t brightness);
        void SetContrast(uint8_t boardId, uint8_t contrast);
        void SetFader(uint8_t boardId, uint8_t index, uint16_t position);
        void SetX32RackDisplayRaw(uint8_t p_value2, uint8_t p_value1);
        void SetX32RackDisplay(uint8_t p_value);
        void SetLed(uint8_t boardId, uint8_t ledId, bool state);
        void SetLedByNr(uint16_t ledNr, bool state, bool blink=false);
        void SetLedByEnum(X32_BTN led, bool state, bool blink=false);
        void SetMeterLed(uint8_t boardId, uint8_t index, uint8_t leds);
        void SetMeterLedMain_Rack(uint8_t preamp, uint32_t meterL, uint32_t meterR, uint32_t meterSolo);
        void SetMeterLedMain_FullCompactProducer(uint8_t preamp, uint8_t dynamics, uint32_t meterL, uint32_t meterR, uint32_t meterSolo);
        void SetEncoderRing(uint8_t boardId, uint8_t index, uint8_t ledMode, uint8_t ledPct, bool backlight);
        void SetEncoderRingDbfs(uint8_t boardId, uint8_t index, float dbfs, bool muted, bool backlight);
        void SetLcd(
            uint8_t boardId, uint8_t index, uint8_t color,
            uint8_t xicon, uint8_t yicon, uint8_t icon, 
            uint8_t sizeA, uint8_t xA, uint8_t yA, const char* strA,
            uint8_t sizeB, uint8_t xB, uint8_t yB, const char* strB
        );
        void SetLcdX(LcdData* p_data, uint8_t p_textCount);

        map<uint16_t, X32_BTN> Button2Enum;
        map<X32_BTN, uint16_t> Enum2Led;
        map<X32_ENC, uint16_t> Enum2Encoder;
        map<uint16_t, X32_ENC> Encoder2Enum;

        void FaderMoved(SurfaceEvent* event);
        
        void BlockFader(uint8_t boardId, uint8_t faderIndex);
        bool IsFaderBlocked(uint8_t boardId, uint8_t faderIndex);
        void Touchcontrol();
};