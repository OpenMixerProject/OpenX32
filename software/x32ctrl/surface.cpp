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

Surface::Surface(X32BaseParameter* basepar): X32Base(basepar){
    uart = new Uart(basepar);
}

void Surface::Init(void) {
    if (state->bodyless) {

        /* 
        
        How to connect x32ctrl bodyless mode to a X32 runnig Linux:

        Developer PC
        ############

        // create two virtual serial ports and connect them together as bridge
        # socat -d -d pty,raw,link=/tmp/ttyLocal,echo=0 pty,raw,link=/tmp/ttyRemote,echo=0

        // start netcat server on port 10000
        # nc -l 10000 </tmp/ttyRemote >/tmp/ttyRemote

        X32
        ###
        
        // set serial to 115200 baud
        # stty -F /dev/ttymxc1 115200 raw -echo -echoe -echok

        // start netcat client to transmit/receive serial from/to devloper pc
        # nc <ip of Developer PC> 10000 </dev/ttymxc1 >/dev/ttymxc1

        Developer PC
        ############        
        
        // start x32ctrl with bodyless commandline parameter "-b"
        # x32ctrl -b
        
        */

        uart->Open("/tmp/ttyLocal", 115200, true);
    }
    else if (state->raspi)
    {
        uart->Open("/dev/ttyUSB0", 115200, true);
    }
    else
    {
        uart->Open("/dev/ttymxc1", 115200, true);
    }

    Reset();
}

void Surface::Reset(void) {
     helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Reset surface ...");

    if (state->bodyless) 
    {
        // TODO: integrate in Testing GUI
    }
    else
    {  
        int fd = open("/sys/class/leds/reset_surface/brightness", O_WRONLY);
        write(fd, "1", 1);
        usleep(100 * 1000);
        write(fd, "0", 1);
        close(fd);
        usleep(2000 * 1000);
    }

    //Reset twice (the first faders are not reseted if we do it only once)
    FaderReset();
    FaderReset();

    helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "... Done");
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


uint16_t Surface::CalcEncoderRingLedDirect(uint8_t num_leds_to_light)
{
    if (num_leds_to_light > 13)
    {
        num_leds_to_light = 13;
    }

    uint16_t led_mask = 0;
    if (num_leds_to_light > 0)
    {
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

uint16_t Surface::CalcEncoderRingLedDbfs(float dbfs, bool onlyPosition) {
    uint16_t led_mask = 0;

   // if (config->IsModelX32Rack()){
        // X32Rack: Channel Level, Mail LR Level

        // LEDs dBfs
        // 1 -50
        // 2 -40
        // 3 -30
        // 4 -24
        // 5 -18
        // 6 -12
        // 7 -9
        // 8 -6
        // 9 -3
        // 10 0
        // 11 3
        // 12 6
        // 13 10


        if (dbfs > -60) { 
            uint8_t led_index = 0;

            if (dbfs >= 10) led_index = 12;
            else if (dbfs >= 6) led_index = 11;
            else if (dbfs >= 3) led_index = 10;
            else if (dbfs >= 0) led_index = 9;
            else if (dbfs >= -3) led_index = 8;
            else if (dbfs >= -6) led_index = 7;
            else if (dbfs >= -9) led_index = 6;
            else if (dbfs >= -12) led_index = 5;
            else if (dbfs >= -18) led_index = 4;
            else if (dbfs >= -24) led_index = 3;
            else if (dbfs >= -30) led_index = 2;
            else if (dbfs >= -40) led_index = 1;
            else if (dbfs > -50) led_index = 0;

            if (onlyPosition){
                led_mask = (1U << led_index);
            } else {
                led_mask = (1U << (led_index + 1)) -1;
            }
        } 

   // } else {
    
        // led_index = (uint8_t)(((float)pct / 100.0f) * 12.0f + 0.5f); // +0.5f für Rundung

        // if (led_index > 12) {
        //     led_index = 12;
        // }
    //}

    return led_mask;
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

void Surface::SetBrightnessAllBoards(uint8_t brightness) {
    SetBrightness(X32_BOARD_MAIN, brightness);
    SetBrightness(X32_BOARD_L, brightness);
    SetBrightness(X32_BOARD_M, brightness);
    SetBrightness(X32_BOARD_R, brightness);
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
    SendData(&message, true);
}

void Surface::SetContrastAllBoards(uint8_t contrast) {
    SetContrast(X32_BOARD_MAIN, contrast);
    SetContrast(X32_BOARD_L, contrast);
    SetContrast(X32_BOARD_M, contrast);
    SetContrast(X32_BOARD_R, contrast);
}

// boardId = 0, 1, 4, 5, 8
// contrast = 0 ... 255
void Surface::SetContrast(uint8_t boardId, uint8_t contrast) {
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('C'); // class: C = Controlmessage
    message.AddDataByte('C'); // index
    message.AddDataByte(contrast & 0x3F);
    SendData(&message, true);
}

void Surface::SetLed(SurfaceElementId buttonOrLed, bool ledOn, bool blink)
{
    if(blink)
    {
        blinklist.insert(buttonOrLed);
    }
    else
    {
        if (!blinklist.empty())
        {
            set<SurfaceElementId>::iterator it = blinklist.find(buttonOrLed);
            if (it != blinklist.end())
            {
                blinklist.erase(it);
            }
        }
    }

    SetLed(buttonOrLed, ledOn);
}

void Surface::SetLed(SurfaceElementId buttonOrLed, bool ledOn)
{
    SurfaceElement *element = config->GetSurfaceElement(buttonOrLed);
    SetLedRaw((uint)element->GetBoard(), (uint)element->GetIndex(), ledOn);
}

void Surface::SetLedRaw(uint board, uint index, bool ledOn)
{
    SurfaceMessage message;
    message.AddDataByte(0x80 + board);
    message.AddDataByte('L');  // class: L = LED
    message.AddDataByte(0x80); // index - fixed at 0x80 for LEDs
    if (ledOn)
    {
        message.AddDataByte(index + 0x80); // turn LED on
    }
    else
    {
        message.AddDataByte(index); // turn LED off
    }
    SendData(&message, true);
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
    SendData(&message, true);
}

// set 7-Segment display on X32 Rack
// dot = 128 or 256
void Surface::SetX32RackDisplay(uint8_t vChannelIndex){
    uint8_t vChannelNumber = vChannelIndex + 1;
    if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK::NORMAL)) {
        uint8_t segment_l = vChannelNumber < 10 ? 0 : int2segment((uint8_t)(vChannelNumber/10));
        
        SetX32RackDisplayRaw(segment_l, int2segment(vChannelNumber % 10));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK::AUX)) {
        SetX32RackDisplayRaw(int2segment('A'), int2segment(vChannelNumber - (uint)X32_VCHANNEL_BLOCK::AUX));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK::FXRET)) {
        SetX32RackDisplayRaw(int2segment('F'), int2segment(vChannelNumber - (uint)X32_VCHANNEL_BLOCK::FXRET));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK::BUS)) {
        uint8_t number = vChannelNumber - (uint)X32_VCHANNEL_BLOCK::BUS;
        SetX32RackDisplayRaw(int2segment((uint8_t)(number/10)), int2segment(number % 10));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK::MATRIX)) {
        SetX32RackDisplayRaw(int2segment('M'), int2segment(vChannelNumber - (uint)X32_VCHANNEL_BLOCK::MATRIX));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK::DCA)) {
        SetX32RackDisplayRaw(int2segment('D'), int2segment(vChannelNumber - (uint)X32_VCHANNEL_BLOCK::DCA));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK::MAIN)) {
        SetX32RackDisplayRaw(int2segment('M'), int2segment(' '));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK::MAINSUB)) {
        SetX32RackDisplayRaw(int2segment('M'), int2segment(5));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK::SPECIAL)) {
        SetX32RackDisplayRaw(int2segment(5), int2segment(' '));
    }
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
        case 'f':
        case 'F':
            return 1 + 32 + 64 + 16;
        case 'm':
        case 'M':
            return 55;
        case ' ':
            return 0;
        default:
            return 0;
    }
}

// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
void Surface::SetMeterLed(uint8_t boardId, uint8_t index, uint8_t leds)
{
  // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
  // 0x4C, index, leds.b[]
  SurfaceMessage message;
  message.AddDataByte(0x80 + boardId); // start message for specific boardId
  message.AddDataByte('M'); // class: M = Meter
  message.AddDataByte(index); // index
  message.AddDataByte(leds);
  SendData(&message, true);
}

// preamp = 8-bit bitwise (bit 0=Sig, 1=-30dB ... 6=-3dB, 7=Clip)
// meter = 32-bit bitwise (bit 0=-45dB ... 15=-4, 16=-2, 19=Clip, 20+=unused)
void Surface::SetMeterLedMain_Rack(uint8_t preamp, uint32_t meterL, uint32_t meterR, uint32_t meterSolo)
{
    SurfaceMessage message;
    message.AddDataByte(0x80); // start message for specific boardId
    message.AddDataByte('M'); // class: M = Meter
    message.AddDataByte(0); // index
    message.AddDataByte(preamp);

    // Example for Big Meters on X32Rack (!different scale to X32Full/Compact)
    //message.AddDataByte(0b11110000);  // first nibble shifted 4 to left
    //message.AddDataByte(0b11111111);  // bit 4..12 shifted 4 to right
    //message.AddDataByte(0b10110111);  // last bits are crazy splitted :-/

    message.AddDataByte((uint8_t)(meterL<<4));
    message.AddDataByte((uint8_t)(meterL>>4));
    message.AddDataByte((((uint8_t)(meterL>>12))&0b10000111) | (((uint8_t)(meterL>>11))&0b00110000));
    message.AddDataByte(0);
    message.AddDataByte((uint8_t)(meterR<<4));
    message.AddDataByte((uint8_t)(meterR>>4));
    message.AddDataByte((((uint8_t)(meterR>>12))&0b10000111) | (((uint8_t)(meterR>>11))&0b00110000));
    message.AddDataByte(0x00);
    message.AddDataByte((uint8_t)(meterSolo<<4));
    message.AddDataByte((uint8_t)(meterSolo>>4));
    message.AddDataByte((((uint8_t)(meterSolo>>12))&0b10000111) | (((uint8_t)(meterSolo>>11))&0b00110000));
    message.AddDataByte(0x00);
    SendData(&message, true);
}

void Surface::SetMeterLedMain_Producer(uint8_t preamp, uint8_t dynamics, uint32_t meterL, uint32_t meterR, uint32_t meterSolo)  {
    
    // TODO
    // meter scale is from Rack
    // dynamics is like Full/COmpact
    // --> has to be tested
    
}

// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
// leds = 32-bit bitwise (bit 0=-57dB ... 22=-2, 23=-1, 24=Clip)
void Surface::SetMeterLedMain_FullOrCompact(uint8_t preamp, uint8_t dynamics, uint32_t meterL, uint32_t meterR, uint32_t meterSolo) {
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
    SendData(&message, true);
}

// boardId = 0, 1, 4, 5, 8
// index
// ledMode = 0=increment, 1=absolute position, 2=balance l/r, 3=width/spread, 4=decrement, 5=direct -> the number of leds to turn on (max. 13)
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
        case 5: // direct number of leds (max 13)
            leds = CalcEncoderRingLedDirect(ledPct);
            break;
        case 6: // direct position of led (max 13)
            leds = (1U << (ledPct-1));
            break;
    }
    message.AddDataByte(leds & 0xFF);
    if (backlight) {
        message.AddDataByte(((leds & 0x7F00) >> 8) + 0x80); // turn backlight on
    }else{
        message.AddDataByte(((leds & 0x7F00) >> 8)); // turn backlight off
    }
    SendData(&message, true);
}

void Surface::SetEncoderRingDbfs(uint8_t boardId, uint8_t index, float dbfs, bool muted, bool backlight) {
    // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
    // 0x52, index, leds.w[]
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('R'); // class: R = Ring
    message.AddDataByte(index); // index

    uint16_t leds = CalcEncoderRingLedDbfs(dbfs, muted);

    message.AddDataByte(leds & 0xFF);
    if (backlight) {
        message.AddDataByte(((leds & 0x7F00) >> 8) + 0x80); // turn backlight on
    }else{
        message.AddDataByte(((leds & 0x7F00) >> 8)); // turn backlight off
    }
    SendData(&message, true);
}

// boardId = 0, 4, 5, 8
// index = 0 ... 8
// color = 0=BLACK, 1=RED, 2=GREEN, 3=YELLOW, 4=BLUE, 5=PINK, 6=CYAN, 7=WHITE
// icon = 0xA0 (none), 0xA1 ... 0xE9
// sizeA/B = 0x00 (small) or 0x20 (large)
// xA/B = horizontal position in pixel
// yA/B = vertical position in pixel
// strA/B = String of Text to be displayed
void Surface::SetLcd(
    uint8_t boardId, uint8_t index, uint8_t color,
    uint8_t xicon, uint8_t yicon, uint8_t icon, 
    uint8_t sizeA, uint8_t xA, uint8_t yA, const char* strA,
    uint8_t sizeB, uint8_t xB, uint8_t yB, const char* strB
    ) {
    // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
    // 0x44, i, color.b, script[]

    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('D'); // class: D = Display
    message.AddDataByte(index); // index
    message.AddDataByte(color & 0x0F); // use only 4 bits (bit 0=R, 1=G, 2=B, 3=Inverted)

    // begin of script
    // transmit icon
    message.AddDataByte(icon);
    message.AddDataByte(xicon);
    message.AddDataByte(yicon);

    // transmit first text
    message.AddDataByte(sizeA + strlen(strA)); // size + textLength
    message.AddDataByte(xA);
    message.AddDataByte(yA);
    message.AddString(strA); // this is ASCII, so we can omit byte-stuffing

    message.AddDataByte(sizeB + strlen(strB)); // size + textLength
    message.AddDataByte(xB);
    message.AddDataByte(yB);
    message.AddString(strB); // this is ASCII, so we can omit byte-stuffing
    SendData(&message, true);
}

void Surface::SetLcdX(LcdData* p_data, uint8_t p_textCount) {
    SurfaceMessage message;
    message.AddDataByte(0x80 + p_data->boardId);
    message.AddDataByte('D'); // class: D = Display
    message.AddDataByte(p_data->lcdIndex); 
    message.AddDataByte((p_data->color) & 0x0F);
    message.AddDataByte(p_data->icon.icon);
    message.AddDataByte(p_data->icon.x);
    message.AddDataByte(p_data->icon.y);
    for (int i=0;i<p_textCount;i++){
        message.AddDataByte(p_data->texts[i].size + strlen(p_data->texts[i].text.c_str())); // size + textLength
        message.AddDataByte(p_data->texts[i].x);
        message.AddDataByte(p_data->texts[i].y);
        message.AddString(p_data->texts[i].text.c_str()); // this is ASCII, so we can omit byte-stuffing  
    }
    SendData(&message, true);
}

void Surface::Blink()
{
    if (blinkwait == 0)
    {
        blinkstate = !blinkstate;

        for(SurfaceElementId button : blinklist) {
            SetLed(button, blinkstate);
        }

        blinkwait = 5;
    }

    blinkwait--; 
}



// ####################################################################
// #
// #
// #        Touchcontrol
// #
// #
// ####################################################################

/// @brief Set All Faders to 0 (-Unlimited dBFS)
void Surface::FaderReset()
{
    // Reset touchcontrol wait time
    for(uint8_t faderindex=0; faderindex<MAX_FADERS; faderindex++){
        faders[faderindex].wait = 0;
    }

    // Reset position of faders
    uint8_t maxfaderindex = 0;
    if (config->IsModelX32FullOrM32()){
        maxfaderindex = MAX_FADERS;
    }
    if (config->IsModelX32CompactOrProducerOrM32R()){
        maxfaderindex = MAX_FADERS-8;
    }

    for(uint8_t faderindex=0; faderindex<maxfaderindex; faderindex++)
    {
        faders[faderindex].position_real = 0;
        SetFaderRaw(GetBoardId(faderindex), GetFaderId(faderindex), 0);
    }
}

// Want to move Fader to Position
// position = 0x0000 ... 0x0FFF
void Surface::SetFader(uint8_t boardId, uint8_t index, uint16_t position) {
    uint8_t faderindex = GetChannelstripIndex(boardId, index);

    //if (faders[faderindex].position_real != faders[faderindex].position_wanted) {
        helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "Want to move fader at index %d to %d", faderindex, position);
        faders[faderindex].position_wanted = position;
   // }
}

// Fader was physically moved (by us or by operator)
void Surface::FaderMoved(uint8_t boardId, uint8_t index, uint16_t value)
{
    uint8_t faderindex = GetChannelstripIndex(boardId, index);
    helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "Fader at index %d moved to %d", faderindex, value);
    faders[faderindex].position_wanted = value;
    faders[faderindex].position_real = value;
    faders[faderindex].wait = 10; // wait 100x 10ms
}

uint8_t Surface::GetChannelstripIndex(uint8_t boardId, uint8_t index)
{
    switch (boardId)
    {
        case X32_BOARD_L:
            return index;
        case X32_BOARD_M: // only X32 Full
            return index + 8;
        case X32_BOARD_R:
            return index + (config->IsModelX32FullOrM32() ? 16 : 8);  // 16 - X32 Full, 8 - X32 Compact/Producer
        default:
            return 0;
    }
}

uint8_t Surface::GetBoardId(uint8_t faderindex) {
    if(config->IsModelX32FullOrM32())
    {
        if (faderindex < 8){
            return X32_BOARD_L;
        }
        if (faderindex < 16){
            return X32_BOARD_M;
        }
        return X32_BOARD_R;
    }

    if(config->IsModelX32CompactOrProducerOrM32R())
    {
        if (faderindex < 8){
            return X32_BOARD_L;
        }
        return X32_BOARD_R;
    }

    return 0;
}

uint8_t Surface::GetFaderId(uint8_t faderindex) {
    if(config->IsModelX32FullOrM32())
    {
        if (faderindex < 8){
            return faderindex;
        }
        if (faderindex < 16){
            return faderindex-8;
        }
        return faderindex-16;
    }

    if(config->IsModelX32CompactOrProducerOrM32R())
    {
        if (faderindex < 8){
            return faderindex;
        }
        return faderindex-8;
    }

    return 0;
}

void Surface::Touchcontrol() {

    uint8_t maxfaderindex = 0;
    if (config->IsModelX32FullOrM32()){
        maxfaderindex = MAX_FADERS;
    }
    if (config->IsModelX32CompactOrProducerOrM32R()){
        maxfaderindex = MAX_FADERS-8;
    }

    for(uint8_t faderindex=0; faderindex<maxfaderindex; faderindex++)
    {
        if (faders[faderindex].wait > 0)
        {
            faders[faderindex].wait--;
            helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE, "Reduced wait time on fader at index %d to %d", faderindex, faders[faderindex].wait);
        }
        else if (faders[faderindex].position_real != faders[faderindex].position_wanted)
        {
            helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "Move fader at index %d from %d to %d", faderindex, faders[faderindex].position_real, faders[faderindex].position_wanted);
            faders[faderindex].position_real = faders[faderindex].position_wanted;
            SetFaderRaw(GetBoardId(faderindex), GetFaderId(faderindex), faders[faderindex].position_wanted);
        }
    }
}

// position = 0x0000 ... 0x0FFF
void Surface::SetFaderRaw(uint8_t boardId, uint8_t index, uint16_t position) {
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('F'); // class: F = Fader
    message.AddDataByte(index); // index
    message.AddDataByte((position & 0xFF)); // LSB
    message.AddDataByte((char)((position & 0x0F00) >> 8)); // MSB

    helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE, "Set fader position on board %d at index %d to %d", boardId, index, position);

    SendData(&message, true);
}

// incoming message has the form: 0xFE 0x8i Class Index Data[] 0xFE
// Checksum is calculated using the following equation:
// chksum = ( 0xFE - i - class - index - sumof(data[]) - sizeof(data[]) ) and 0x7F
uint8_t Surface::calculateChecksum(const char* data, uint16_t len) {
  // a single message can contain up to max. 64 chars
  int32_t sum = 0xFE;
  for (uint8_t i = 0; i < (len-1); i++) {
    sum -= data[i];
  }
  sum -= (len - 3); // remove 2-byte HEADER (0xFE 0x8i) and 1-byte end (0xFE)

  // write the calculated sum to the last element of the array
  return (sum & 0x7F);
}

int Surface::SendData(MessageBase* message, bool addChecksum) {
    message->AddRawByte(0xFE); // Endbyte

    if (addChecksum) {
        char checksum = 0;
        if (message->current_length >= 2) { // at least start- and end-byte
            checksum = calculateChecksum(message->buffer, message->current_length);
        }

        // add checksum to message and send data via serial-port
        message->AddRawByte(checksum);
    }

    return uart->Tx(message);
}