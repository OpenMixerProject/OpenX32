/*
    ____                  __   ______ ___  
   / __ \                 \ \ / /___ \__ \ 
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
  | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |                               
         |_|                               
  
  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "surface.h"

Surface::Surface(Config* config): X32Base(config){
    int buttonDefinitionIndex = 0;
    int encoderDefinitionIndex = 0;
}

uint16_t Surface::Enum2Button(X32_BTN button) {
    //x32debug("DEBUG: enum2button: Button %d -> ", button);
    for(int i = 0; i < buttonDefinitionIndex; i++) {
        if (x32_btn_def[i].button == button) {
            //x32debug("gefunden: Button %d\n", x32_btn_def[i].buttonNr);
            return x32_btn_def[i].buttonNr;
        }
    }
    //x32debug(" NICHT gefunden!\n");
    return 0;
}

X32_BTN Surface::Button2Enum(uint16_t buttonNr) {
    //x32debug("DEBUG: button2enum: ButtonNr %d -> ", buttonNr);
    for(int i = 0; i < buttonDefinitionIndex; i++) {
        if (x32_btn_def[i].buttonNr == buttonNr) {
            //x32debug("gefunden: Button %d\n", x32_btn_def[i].button);
            return x32_btn_def[i].button;
        }
    }
    //x32debug(" NICHT gefunden!\n");
    return X32_BTN_NONE;
}

uint16_t Surface::Enum2Encoder(X32_ENC encoder) {
    //x32debug("DEBUG: enum2button: Encoder %d -> ", encoder);
    for(int i = 0; i < encoderDefinitionIndex; i++) {
        if (x32_enc_def[i].encoder == encoder) {
            //x32debug("gefunden: Encoder %d\n", x32_enc_def[i].encoderNr);
            return x32_enc_def[i].encoderNr;
        }
    }
    //x32debug(" NICHT gefunden!\n");
    return 0;
}

X32_ENC Surface::Encoder2Enum(uint16_t encoderNr) {
    //x32debug("DEBUG: encoder2enum: EncoderNr %d -> ", encoderNr);
    for(int i = 0; i < encoderDefinitionIndex; i++) {
        if (x32_enc_def[i].encoderNr == encoderNr) {
            //x32debug("gefunden: Encoder %d\n", x32_enc_def[i].encoder);
            return x32_enc_def[i].encoder;
        }
    }
    //x32debug(" NICHT gefunden!\n");
    return X32_ENC_NONE;
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t Surface::CalcEncoderRingLedIncrement(uint8_t pct) {
    uint8_t num_leds_to_light = 0;
/*
    if (pct <= 50) {
        // Scale 0-50 to 0-6 LEDs
        num_leds_to_light = (uint8_t)((float)pct / 50.0f * 6.0f);
    } else {
        // Scale 51-100 to 7-12 LEDs (6 more LEDs)
        // From 51 to 100, there are 50 steps.
        // We need to add (pct - 50) steps mapped to the remaining 6 LEDs.
        // (pct - 50) / 50.0f * 6.0f
        num_leds_to_light = 6 + (uint8_t)(((float)(pct - 50) / 50.0f) * 6.0f);

        if (num_leds_to_light > 12) {
            num_leds_to_light = 12;
        }
    }
*/
    num_leds_to_light = (uint8_t)((float)pct / 100.0f * 12.5f);
    if (num_leds_to_light > 13) {
        num_leds_to_light = 13;
    }

    uint16_t led_mask = 0;
    if (num_leds_to_light > 0) {
        led_mask = (1U << num_leds_to_light) - 1;
    }

    return led_mask;
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t Surface::CalcEncoderRingLedDecrement(uint8_t pct) {
    uint8_t num_leds_to_light = 0;
/*
    if (pct <= 50) {
        // Scale 0-50 to 0-6 LEDs
        num_leds_to_light = (uint8_t)((float)pct / 50.0f * 6.0f);
    } else {
        // Scale 51-100 to 7-12 LEDs (6 more LEDs)
        // From 51 to 100, there are 50 steps.
        // We need to add (pct - 50) steps mapped to the remaining 6 LEDs.
        // (pct - 50) / 50.0f * 6.0f
        num_leds_to_light = 6 + (uint8_t)(((float)(pct - 50) / 50.0f) * 6.0f);

        if (num_leds_to_light > 12) {
            num_leds_to_light = 12;
        }
    }
*/
    num_leds_to_light = (uint8_t)((float)pct / 100.0f * 12.5f);
    if (num_leds_to_light > 13) {
        num_leds_to_light = 13;
    }

    uint16_t led_mask = 0;
    
    // Bits von rechts nach links setzen
    for (uint8_t i = 0; i < num_leds_to_light; ++i) {
        led_mask |= (1U << (12 - i));
    }

    return led_mask;
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t Surface::CalcEncoderRingLedPosition(uint8_t pct) {
    uint8_t led_index = (uint8_t)(((float)pct / 100.0f) * 12.0f + 0.5f); // +0.5f für Rundung

    if (led_index > 12) {
        led_index = 12;
    }

    return (1U << led_index);
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t Surface::CalcEncoderRingLedBalance(uint8_t pct) {
    uint16_t led_mask = 0;

    if (pct < 50) {
        float scale = (float)pct / 50.0f; // Skaliert 0-49 auf 0-0.98
        // (scale * 6) = Anzahl der LEDs, die an sind.
        uint8_t num_on_left_side = (uint8_t)roundf(scale * 6.5f);

        // Sicherstellen, dass mindestens Bit 6 an ist, wenn pct < 50
        if (num_on_left_side < 1) num_on_left_side = 1;

        // Setze die Bits von Bit 0 bis zum berechneten Index
        for (int i = 0; i < num_on_left_side; ++i) {
            if (i <= 6) { // Sicherstellen, dass wir im Bereich Bits 0-6 bleiben
                led_mask |= (1U << i);
            }
        }

        // invert LED-mask
        led_mask ^= 0xFFFF;
        led_mask &= 0b0000000001111111;

    } else { // pct >= 50
        // Skaliere (pct - 50) von 1-50 auf die Anzahl der LEDs, die von Bit 6 nach rechts zusätzlich an sein sollen.
        float scale = (float)(pct - 50) / 50.0f; // Skaliert 51-100 auf 0.02-1
        uint8_t num_on_right_side = (uint8_t)roundf(1.0f + (scale * 6.5f)); // 1 für Bit 6, plus 6 weitere LEDs

        // Sicherstellen, dass mindestens Bit 6 an ist, wenn pct > 50
        if (num_on_right_side < 1) num_on_right_side = 1;

        // Setze die Bits von Bit 6 bis zum berechneten Index
        for (int i = 0; i < num_on_right_side; ++i) {
            if ((6 + i) <= 12) { // Sicherstellen, dass wir im Bereich Bits 6-12 bleiben
                led_mask |= (1U << (6 + i));
            }
        }
    }

    return led_mask;
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t Surface::CalcEncoderRingLedWidth(uint8_t pct) {
    if (pct == 0) {
        return (1U << 6); // Setzt nur Bit 6
    }

    float scaled_value = (float)(pct - 1) / 99.0f; // Skaliert 1-100 auf 0-1

    // Anzahl der zusätzlichen LEDs, die auf der linken Seite von Bit 6 aus eingeschaltet werden sollen.
    // Max 6 LEDs (Bits 0-5).
    uint8_t num_left_additional_leds = (uint8_t)roundf(scaled_value * 6.0f);
    // Anzahl der zusätzlichen LEDs, die auf der rechten Seite von Bit 6 aus eingeschaltet werden sollen.
    // Max 7 LEDs (Bits 7-12).
    uint8_t num_right_additional_leds = (uint8_t)roundf(scaled_value * 6.0f);

    uint16_t led_mask = (1U << 6); // Starte mit Bit 6 gesetzt

    // Schalte die zusätzlichen LEDs auf der linken Seite ein
    for (int i = 0; i < num_left_additional_leds; ++i) {
        if ((6 - (i + 1)) >= 0) { // Sicherstellen, dass der Index nicht negativ wird
            led_mask |= (1U << (6 - (i + 1)));
        }
    }

    // Schalte die zusätzlichen LEDs auf der rechten Seite ein
    for (int i = 0; i < num_right_additional_leds; ++i) {
        if ((6 + (i + 1)) <= 12) { // Sicherstellen, dass der Index nicht über 15 hinausgeht
            led_mask |= (1U << (6 + (i + 1)));
        }
    }

    // Bei 100% sollen alle Bits gesetzt sein (0x1FFF).
    // Die Berechnung oben sollte dies bereits erreichen, aber eine explizite Prüfung schadet nicht.
    if (pct == 100) {
        return 0x1FFF; // Alle 13 Bits setzen
    }

    return led_mask;
}

void Surface::AddButtonDefinition(X32_BTN p_button, uint16_t p_buttonNr) {
    if (buttonDefinitionIndex >= MAX_BUTTONS)
    {
        //TODO: Error Message
        //x32log("ERROR: MAX_BUTTONS");
        return;
    }
    x32_btn_def[buttonDefinitionIndex].button = p_button;
    x32_btn_def[buttonDefinitionIndex].buttonNr = p_buttonNr;
    buttonDefinitionIndex++;
    //x32debug("added button definition: Button %d -> ButtonNr %d\n", p_button, p_buttonNr);
}

void Surface::AddEncoderDefinition(X32_ENC p_encoder, uint16_t p_encoderNr) {
    if (encoderDefinitionIndex >= MAX_ENCODERS)
    {
        //TODO: Error Message
        //x32log("ERROR: MAX_ENCODERS");
        return;
    }
    x32_enc_def[encoderDefinitionIndex].encoder = p_encoder;
    x32_enc_def[encoderDefinitionIndex].encoderNr = p_encoderNr;
    encoderDefinitionIndex++;
    //x32debug("added encoder definition: Encoder %d -> EncoderNr %d\n", p_encoder, p_encoderNr);
}

void Surface::Init(void) {
    uart.Open("/dev/ttymxc1", 115200, true);

    Reset();

    // set brightness and contrast
    SetBrightness(0, 255); // brightness of LEDs
    SetBrightness(1, 255);
    SetBrightness(4, 255);
    SetBrightness(5, 255);
    SetBrightness(8, 255);
    SetContrast(0, 0x24); // contrast of LCDs
    SetContrast(4, 0x24);
    SetContrast(5, 0x24);
    SetContrast(8, 0x24);
}

void Surface::Reset(void) {
    int fd = open("/sys/class/leds/reset_surface/brightness", O_WRONLY);
    write(fd, "1", 1);
    usleep(100 * 1000);
    write(fd, "0", 1);
    close(fd);
    usleep(2000 * 1000);
}



// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// brightness = 0 ... 255
void Surface::SetBrightness(uint8_t boardId, uint8_t brightness) {
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('C'); // class: C = Controlmessage
    message.AddDataByte('B'); // index
    message.AddDataByte(brightness);
    uart.Tx(&message, true);
}

// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// brightness = 0 ... 255
void Surface::SetContrast(uint8_t boardId, uint8_t contrast) {
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('C'); // class: C = Controlmessage
    message.AddDataByte('C'); // index
    message.AddDataByte(contrast & 0x3F);
    uart.Tx(&message, true);
}

void Surface::SetLed(uint8_t boardId, uint8_t ledId, bool state){
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId);
    message.AddDataByte('L'); // class: L = LED
    message.AddDataByte(0x80); // index - fixed at 0x80 for LEDs
    if (state > 0) {
        message.AddDataByte(ledId + 0x80); // turn LED on
    }else{
        message.AddDataByte(ledId); // turn LED off
    }
    uart.Tx(&message, true);
}

// position = 0x0000 ... 0x0FFF
void Surface::SetFader(uint8_t boardId, uint8_t index, uint16_t position) {
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('F'); // class: F = Fader
    message.AddDataByte(index); // index
    message.AddDataByte((position & 0xFF)); // LSB
    message.AddDataByte((char)((position & 0x0F00) >> 8)); // MSB
    uart.Tx(&message, true);
}

// set 7-Segment display on X32 Rack
// dot = 128
void Surface::SetX32RackDisplayRaw(uint8_t p_value2, uint8_t p_value1){
    SurfaceMessage message;
    message.AddDataByte(0x80);
    message.AddDataByte('D'); // Display
    message.AddDataByte(0x80);
    message.AddDataByte(p_value2); 
    message.AddDataByte(p_value1);
    uart.Tx(&message, true);
}

// set 7-Segment display on X32 Rack
// dot = 128 or 256
void Surface::SetX32RackDisplay(uint8_t p_value){
    SetX32RackDisplayRaw(int2segment((uint8_t)(p_value/10)), int2segment(p_value % 10));
}

uint8_t Surface::int2segment(int8_t p_value){
    switch (p_value){
        case 0:
            return 63;
        case 1:
            return 6;
        case 2:
            return 91;
        case 3:
            return 79;
        case 4:
            return 102;
        case 5:
            return 109;
        case 6:
            return 125;
        case 7:
            return 7;
        case 8:
            return 127;
        case 9:
            return 111;
        case 'a':
        case 'A':
            return 119;
        case 'b':
        case 'B':
            return 124;
        case 'd':
        case 'D':
            return 94;
        case 'm':
        case 'M':
            return 55;
        default:
            return 0;
    }
}

// ledNr = Nr of the LED from constants.h (contains BoardId and LED-Nr)
// state = 0 / 1
void Surface::SetLedByNr(uint16_t ledNr, bool state) {
  uint8_t boardId = (uint8_t)((ledNr & 0xFF00) >> 8);
  uint8_t ledId = (uint8_t)(ledNr & 0x7F);

  //x32debug("LedNr: 0x%04X -> BoardId: 0x%02X, LED: 0x%02X\n", ledNr, boardId, ledId);

  SetLed(boardId, ledId, state);
}

// ledNr = LED from X32_BTN enum
// state = 0 / 1
void Surface::SetLedByEnum(X32_BTN led, bool state) {
    SetLedByNr(Enum2Button(led), state);
}

// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
void Surface::SetMeterLed(uint8_t boardId, uint8_t index, uint8_t leds) {
  // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
  // 0x4C, index, leds.b[]
  SurfaceMessage message;
  message.AddDataByte(0x80 + boardId); // start message for specific boardId
  message.AddDataByte('M'); // class: M = Meter
  message.AddDataByte(index); // index
  message.AddDataByte(leds);
  uart.Tx(&message, true);
}

// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
// leds = 32-bit bitwise (bit 0=-57dB ... 22=-2, 23=-1, 24=Clip)
void Surface::SetMeterLedMain(uint8_t preamp, uint8_t dynamics, uint32_t meterL, uint32_t meterR, uint32_t meterSolo) {
    // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
    // 0x4C, index, leds.b[]
    SurfaceMessage message;
    message.AddDataByte(0x80 + 1); // start message for specific boardId
    message.AddDataByte('M'); // class: M = Meter
    message.AddDataByte(0); // index
    message.AddDataByte(preamp);
    message.AddDataByte(dynamics);
    message.AddDataByte((uint8_t)meterL);
    message.AddDataByte((uint8_t)(meterL>>8));
    message.AddDataByte((uint8_t)(meterL>>16));
    message.AddDataByte(0x00);
    message.AddDataByte((uint8_t)meterR);
    message.AddDataByte((uint8_t)(meterR>>8));
    message.AddDataByte((uint8_t)(meterR>>16));
    message.AddDataByte(0x00);
    message.AddDataByte((uint8_t)meterSolo);
    message.AddDataByte((uint8_t)(meterSolo>>8));
    message.AddDataByte((uint8_t)(meterSolo>>16));
    message.AddDataByte(0x00);
    uart.Tx(&message, true);
}

// boardId = 0, 1, 4, 5, 8
// index
// ledMode = 0=increment, 1=absolute position, 2=balance l/r, 3=width/spread, 4=decrement
// ledPct = percentage 0...100
// backlight = enable or disable
void Surface::SetEncoderRing(uint8_t boardId, uint8_t index, uint8_t ledMode, uint8_t ledPct, bool backlight) {
    // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
    // 0x52, index, leds.w[]
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('R'); // class: R = Ring
    message.AddDataByte(index); // index

    uint16_t leds = 0;
    switch (ledMode) {
        case 0: // standard increment-method
            leds = CalcEncoderRingLedIncrement(ledPct);
            break;
        case 1: // absolute position
            leds = CalcEncoderRingLedPosition(ledPct);
            break;
        case 2: // balance left/right
            leds = CalcEncoderRingLedBalance(ledPct);
            break;
        case 3: // spread/width
            leds = CalcEncoderRingLedWidth(ledPct);
            break;
        case 4: // decrement-method
            leds = CalcEncoderRingLedDecrement(ledPct);
            break;
    }
    message.AddDataByte(leds & 0xFF);
    if (backlight) {
        message.AddDataByte(((leds & 0x7F00) >> 8) + 0x80); // turn backlight on
    }else{
        message.AddDataByte(((leds & 0x7F00) >> 8)); // turn backlight off
    }
    uart.Tx(&message, true);
}

void Surface::ProcessUartData() {
    uint8_t currentByte = 0;
    uint8_t receivedClass = 0;
    uint8_t receivedIndex = 0;
    uint16_t receivedValue = 0;
    uint8_t receivedChecksum = 0;
    bool lastPackageIncomplete = false;

    int bytesToProcess = uart.Rx(&surfaceBufferUart[0], sizeof(surfaceBufferUart));

    if (bytesToProcess <= 0) {
        return;
	}

    // first init package buffer with 0x00s
    // start at surfacePacketCurrentIndex to not overwrite saved data from last incomplete package
    for (uint8_t package=0; package<SURFACE_MAX_PACKET_LENGTH;package++){
        for (int i = surfacePacketCurrentIndex; i < 6; i++) {
            surfacePacketBuffer[package][i]=0x00;
        }
        surfacePacketCurrentIndex=0;
    }



#if DEBUG
    // print received values on one row
    helper->Debug("surfaceProcessUartData(): ");
    bool divide_after_next_dbg = false;
    for (int i = 0; i < bytesToProcess; i++) {
        if (divide_after_next_dbg && ((uint8_t)surfaceBufferUart[i] == 0xFE)) {
            helper->Debug("| ");
            divide_after_next_dbg = false;
        }
        helper->Debug("%02X ", (uint8_t)surfaceBufferUart[i]); // empfangene Bytes als HEX-Wert ausgeben
        if (divide_after_next_dbg){
            helper->Debug("| ");
            divide_after_next_dbg = false;
        } 
        if ((uint8_t)surfaceBufferUart[i] == 0xFE) {
            divide_after_next_dbg=true;
        }
    }
    helper->Debug("\n");
#endif

    // break up received data into packages
    bool divide_after_next = false;
    for (int i = 0; i < bytesToProcess; i++) {

        if (divide_after_next && ((uint8_t)surfaceBufferUart[i] == 0xFE)) {
            // previous package had no checksum
            surfacePacketCurrent++;
            surfacePacketCurrentIndex=0;
            divide_after_next = false;
        }

        surfacePacketBuffer[surfacePacketCurrent][surfacePacketCurrentIndex++] = (uint8_t)surfaceBufferUart[i];

        if (divide_after_next) {
            surfacePacketCurrent++;
            surfacePacketCurrentIndex=0;
            divide_after_next = false;
        }

        // use 0xFE as package divider
        if (((uint8_t)surfaceBufferUart[i] == 0xFE))
        {
            divide_after_next = true;
        }
    }

    if (divide_after_next){
        // divide_after_next got no usage, because the uartBuffer was emptied out -> reason: no checksum was send
        // clean up this situation
        surfacePacketCurrent++;
        while (surfacePacketCurrentIndex < 6){  
            // fill with zero - maybe not needed
            surfacePacketBuffer[surfacePacketCurrent][surfacePacketCurrentIndex++]=0x00;
        }
        surfacePacketCurrentIndex=0;
    }

    if (
        (surfacePacketCurrentIndex!=0) &&
        !((surfacePacketBuffer[surfacePacketCurrent][3]==0xFE) | (surfacePacketBuffer[surfacePacketCurrent][4]==0xFE))
    ){
        // last package was incomplete, save it for next run
        /*
            Example1:                                  _____ incomplete, has no 0xFE (and is too short)
                                                      /  
            this run         66 01 FB 00 FE 12 | 66 02

            next run         46 02 FE 44 | 66 03 D6 02 FE 33 | 66 04 73 02 FE 15 | 66 05 4E 03 FE 38 | 66 06 21 02 FE 65 |
                             \
                              \____ take the bytes from the last incomplete package and glue it together


            Example2:                                        _____ incomplete, has no 0xFE
                                                            / 
            this run         66 05 EF 0E FE 0C | 66 06 52 0D

            next run         FE 29 | 66 07 C2 0C FE 39
                             \
                              \____ take the bytes from the last incomplete package and glue it together
            
        */

        helper->Debug("surfacePacketCurrent=%d seems incomplete? surfacePacketCurrentIndex=%d\n", surfacePacketCurrent, surfacePacketCurrentIndex);
        lastPackageIncomplete = true;
    }


#if DEBUG
    // print packages, one in a row    
    uint8_t packagesToPrint = surfacePacketCurrent;
    if (lastPackageIncomplete){
        packagesToPrint++;
    }
    //x32debug("surfacePacketCurrent=%d\n", surfacePacketCurrent);

    for (int package=0; package < packagesToPrint; package++) {
        //x32debug("surfaceProcessUartData(): Package %d: ", package);
        for (uint8_t i = 0; i<6; i++){
            //x32debug("%02X ", surfacePacketBuffer[package][i]);
        }
        if (surfacePacketBuffer[package][0] == 0xFE){
            //x32debug("  <--- Board %d", surfacePacketBuffer[package][1] & 0x7F);
        } else if (lastPackageIncomplete){
            //x32debug("  <--- incomplete, saved for next run");
        }
        //x32debug("\n");
    }    
#endif

    for (int8_t package=0; package < surfacePacketCurrent;package++){

        if (surfacePacketBuffer[package][0] == 0xFE){
            // received BoardId
            uint8_t receivedBoardIdtemp = surfacePacketBuffer[package][1] & 0x7F;
            switch(receivedBoardIdtemp){
                case 0:
                case 1:
                case 4:
                case 5:
                case 8:
                    receivedBoardId = receivedBoardIdtemp;
                    break;
            }
        } else {
            receivedClass = surfacePacketBuffer[package][0];
            receivedIndex = surfacePacketBuffer[package][1];
            if (surfacePacketBuffer[package][3] == 0xFE){
                // short package
                receivedValue = surfacePacketBuffer[package][2];
                receivedChecksum = surfacePacketBuffer[package][4];
            } else if (surfacePacketBuffer[package][4] == 0xFE){
                // long package
                receivedValue = ((uint16_t)surfacePacketBuffer[package][3] << 8) | (uint16_t)surfacePacketBuffer[package][2];
                receivedChecksum = surfacePacketBuffer[package][5];
            }
        

            // only process valid packages
            bool valid = true;

            switch (receivedClass){
                case 'f':
                case 'b':
                case 'e':
                    break;
                default:
                    valid = false;
                    break;
            }       

            if (valid){
                helper->Debug("surfaceCallback(%d, %02X, %02X, %04X)\n", receivedBoardId, receivedClass, receivedIndex, receivedValue);
                eventBuffer.push_back(new SurfaceEvent((X32_BOARD)receivedBoardId, receivedClass, receivedIndex, receivedValue));
            } 
        }
    }

    // all packages are processed
    // now clean up for next run

    if (lastPackageIncomplete){
        // copy last incomplete package to package0 for next run
        for (uint8_t i=0; i < surfacePacketCurrentIndex; i++){
            surfacePacketBuffer[0][i] = surfacePacketBuffer[surfacePacketCurrent][i];
        }

        // reset index for next run
        lastPackageIncomplete=false;
        surfacePacketCurrent=0;
        // do NOT touch surfacePacketCurrentIndex!
    }else {
        // reset index for next run
        surfacePacketCurrent=0;
        surfacePacketCurrentIndex=0;
    }
}

bool Surface::HasNextEvent(void){
    return eventBuffer.size() > 0;
}

SurfaceEvent* Surface::GetNextEvent(void){
    SurfaceEvent* event = eventBuffer.back();
    eventBuffer.pop_back();
    return event;
}