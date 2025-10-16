#pragma once

#include <stdint.h>
#include <list>
#include <math.h>

#include "x32base.h"
#include "uart.h"
#include "constants.h"
#include "x32ctrl_types.h"
#include "lcd.h"
#include "surface-event.h"
#include "surface-message.h"
#include "helper.h"
#include "vchannel.h"

using namespace std;

#define DEBUG 0

class Surface : public X32Base
{
    private:
        Uart uart;
       
        int buttonDefinitionIndex;
        sButtonInfo x32_btn_def[MAX_BUTTONS];
        int encoderDefinitionIndex;
        sEncoderInfo x32_enc_def[MAX_ENCODERS];

        int surfacePacketCurrentIndex = 0;
        int surfacePacketCurrent = 0;
        char surfacePacketBuffer[SURFACE_MAX_PACKET_LENGTH][6];
        char surfaceBufferUart[256]; // buffer for UART-readings
        uint8_t receivedBoardId = 0; // BoardID from last received surface event, needed for short messages!

        list<SurfaceEvent*> eventBuffer;

        uint8_t int2segment(int8_t p_value);

        uint16_t CalcEncoderRingLedIncrement(uint8_t pct);
        uint16_t CalcEncoderRingLedDecrement(uint8_t pct);
        uint16_t CalcEncoderRingLedPosition(uint8_t pct);
        uint16_t CalcEncoderRingLedBalance(uint8_t pct);
        uint16_t CalcEncoderRingLedWidth(uint8_t pct);

        void InitDefinitions(void);        
        void AddButtonDefinition(X32_BTN p_button, uint16_t p_buttonNr);
        void AddEncoderDefinition(X32_ENC p_encoder, uint16_t p_encoderNr); 

    public:
        Surface(Config* config, State* state);

        void Init(void);
        void Reset(void);

        void SetBrightness(uint8_t boardId, uint8_t brightness);
        void SetContrast(uint8_t boardId, uint8_t contrast);
        void SetFader(uint8_t boardId, uint8_t index, uint16_t position);
        void SetX32RackDisplayRaw(uint8_t p_value2, uint8_t p_value1);
        void SetX32RackDisplay(uint8_t p_value);
        void SetLed(uint8_t boardId, uint8_t ledId, bool state);
        void SetLedByNr(uint16_t ledNr, bool state);
        void SetLedByEnum(X32_BTN led, bool state);
        void SetMeterLed(uint8_t boardId, uint8_t index, uint8_t leds);
        void SetMeterLedMain(uint8_t preamp, uint8_t dynamics, uint32_t meterL, uint32_t meterR, uint32_t meterSolo);
        void SetEncoderRing(uint8_t boardId, uint8_t index, uint8_t ledMode, uint8_t ledPct, bool backlight);
        void SetLcd(
            uint8_t boardId, uint8_t index, uint8_t color,
            uint8_t xicon, uint8_t yicon, uint8_t icon, 
            uint8_t sizeA, uint8_t xA, uint8_t yA, const char* strA,
            uint8_t sizeB, uint8_t xB, uint8_t yB, const char* strB
        );
        void SetLcdX(LcdData* p_data, uint8_t p_textCount);
        void SetLcdFromVChannel(uint8_t p_boardId, uint8_t p_Index, VChannel* p_chan);

        void ProcessUartData(void);
        bool HasNextEvent(void);
        SurfaceEvent* GetNextEvent(void);

        uint16_t Enum2Button(X32_BTN button);
        X32_BTN Button2Enum(uint16_t buttonNr);
        uint16_t Enum2Encoder(X32_ENC encoder);
        X32_ENC Encoder2Enum(uint16_t encoderNr);
};