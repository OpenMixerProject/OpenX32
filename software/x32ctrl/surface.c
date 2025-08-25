#include "surface.h"

char uartBufferSurface[256]; // buffer for UART-readings
uint8_t receivedBoardId = 0;
sButtonInfo x32_btn_def[MAX_BUTTONS];
sEncoderInfo x32_enc_def[MAX_ENCODERS];

// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// position = 0x0000 ... 0x0FFF
void setFader(uint8_t boardId, uint8_t index, uint16_t position) {
  // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
  // 0x46, i, position.w * (1..8(9))

  messageBuilderInit(&message);

  messageBuilderAddRawByte(&message, 0xFE); // startbyte
  messageBuilderAddDataByte(&message, 0x80 + boardId); // start message for specific boardId
  messageBuilderAddDataByte(&message, 'F'); // class: F = Fader
  messageBuilderAddDataByte(&message, index); // index
  messageBuilderAddDataByte(&message, (position & 0xFF)); // LSB
  messageBuilderAddDataByte(&message, (char)((position & 0x0F00) >> 8)); // MSB
  messageBuilderAddRawByte(&message, 0xFE); // endbyte

  uartTx(&fdSurface, &message, true);
}

// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// ledId = individual index of led
// state = 0 / 1
void setLed(uint8_t boardId, uint8_t ledId, uint8_t state) {
  // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
  // 0x4C, 0x80, leds.b[]

  messageBuilderInit(&message);

  messageBuilderAddRawByte(&message, 0xFE); // startbyte
  messageBuilderAddDataByte(&message, 0x80 + boardId); // start message for specific boardId
  messageBuilderAddDataByte(&message, 'L'); // class: L = LED
  messageBuilderAddDataByte(&message, 0x80); // index - fixed at 0x80 for LEDs
  if (state > 0) {
    messageBuilderAddDataByte(&message, ledId + 0x80); // turn LED on
  }else{
    messageBuilderAddDataByte(&message, ledId); // turn LED off
  }
  messageBuilderAddRawByte(&message, 0xFE); // endbyte

  uartTx(&fdSurface, &message, true);
}

// ledNr = Nr of the LED from constants.h (contains BoardId and LED-Nr)
// state = 0 / 1
void setLedByNr(uint16_t ledNr, uint8_t state) {
  // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
  // 0x4C, 0x80, leds.b[]

  uint8_t boardId = (uint8_t)((ledNr & 0xFF00) >> 8);
  uint8_t ledId = (uint8_t)(ledNr & 0x7F);

  //x32debug("LedNr: 0x%04X -> BoardId: 0x%02X, LED: 0x%02X\n", ledNr, boardId, ledId);

  messageBuilderInit(&message);

  messageBuilderAddRawByte(&message, 0xFE); // startbyte
  messageBuilderAddDataByte(&message, 0x80 + boardId); // start message for specific boardId
  messageBuilderAddDataByte(&message, 'L'); // class: L = LED
  messageBuilderAddDataByte(&message, 0x80); // index - fixed at 0x80 for LEDs
  if (state > 0) {
    messageBuilderAddDataByte(&message, ledId + 0x80); // turn LED on
  }else{
    messageBuilderAddDataByte(&message, ledId); // turn LED off
  }
  messageBuilderAddRawByte(&message, 0xFE); // endbyte

  uartTx(&fdSurface, &message, true);
}

// ledNr = LED from X32_BTN enum
// state = 0 / 1
void setLedByEnum(X32_BTN led, uint8_t state) {
    setLedByNr(enum2button(led), state);
}


// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
void setMeterLed(uint8_t boardId, uint8_t index, uint8_t leds) {
  // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
  // 0x4C, index, leds.b[]

  messageBuilderInit(&message);

  messageBuilderAddRawByte(&message, 0xFE); // startbyte
  messageBuilderAddDataByte(&message, 0x80 + boardId); // start message for specific boardId
  messageBuilderAddDataByte(&message, 'M'); // class: M = Meter
  messageBuilderAddDataByte(&message, index); // index
  messageBuilderAddDataByte(&message, leds);
  messageBuilderAddRawByte(&message, 0xFE); // endbyte

  uartTx(&fdSurface, &message, true);
}

// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
// leds = 32-bit bitwise (bit 0=-57dB ... 22=-2, 23=-1, 24=Clip)
void setMeterLedMain(uint8_t preamp, uint8_t dynamics, uint32_t meterL, uint32_t meterR, uint32_t meterSolo) {
  // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
  // 0x4C, index, leds.b[]

  messageBuilderInit(&message);

  messageBuilderAddRawByte(&message, 0xFE); // startbyte
  messageBuilderAddDataByte(&message, 0x80 + 1); // start message for specific boardId
  messageBuilderAddDataByte(&message, 'M'); // class: M = Meter
  messageBuilderAddDataByte(&message, 0); // index
  messageBuilderAddDataByte(&message, preamp);
  messageBuilderAddDataByte(&message, dynamics);
  messageBuilderAddDataByte(&message, (uint8_t)meterL);
  messageBuilderAddDataByte(&message, (uint8_t)(meterL>>8));
  messageBuilderAddDataByte(&message, (uint8_t)(meterL>>16));
  messageBuilderAddDataByte(&message, 0x00);
  messageBuilderAddDataByte(&message, (uint8_t)meterR);
  messageBuilderAddDataByte(&message, (uint8_t)(meterR>>8));
  messageBuilderAddDataByte(&message, (uint8_t)(meterR>>16));
  messageBuilderAddDataByte(&message, 0x00);
  messageBuilderAddDataByte(&message, (uint8_t)meterSolo);
  messageBuilderAddDataByte(&message, (uint8_t)(meterSolo>>8));
  messageBuilderAddDataByte(&message, (uint8_t)(meterSolo>>16));
  messageBuilderAddDataByte(&message, 0x00);
  messageBuilderAddRawByte(&message, 0xFE); // endbyte

  uartTx(&fdSurface, &message, true);
}

// boardId = 0, 1, 4, 5, 8
// index
// ledMode = 0=increment, 1=absolute position, 2=balance l/r, 3=width/spread
// ledPct = percentage 0...100
// backlight = enable or disable
void setEncoderRing(uint8_t boardId, uint8_t index, uint8_t ledMode, uint8_t ledPct, bool backlight) {
  // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
  // 0x52, index, leds.w[]

  messageBuilderInit(&message);

  messageBuilderAddRawByte(&message, 0xFE); // startbyte
  messageBuilderAddDataByte(&message, 0x80 + boardId); // start message for specific boardId
  messageBuilderAddDataByte(&message, 'R'); // class: R = Ring
  messageBuilderAddDataByte(&message, index); // index

  uint16_t leds = 0;
  switch (ledMode) {
	case 0: // standard increment-method
		leds = calcEncoderRingLedIncrement(ledPct);
		break;
	case 1: // absolute position
		leds = calcEncoderRingLedPosition(ledPct);
		break;
	case 2: // balance left/right
		leds = calcEncoderRingLedBalance(ledPct);
		break;
	case 3: // spread/width
		leds = calcEncoderRingLedWidth(ledPct);
		break;
  }
  messageBuilderAddDataByte(&message, leds & 0xFF);
  if (backlight) {
    messageBuilderAddDataByte(&message, ((leds & 0x7F00) >> 8) + 0x80); // turn backlight on
  }else{
    messageBuilderAddDataByte(&message, ((leds & 0x7F00) >> 8)); // turn backlight off
  }
  messageBuilderAddRawByte(&message, 0xFE); // endbyte

  uartTx(&fdSurface, &message, true);
}

// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// brightness = 0 ... 255
void setBrightness(uint8_t boardId, uint8_t brightness) {
  messageBuilderInit(&message);

  messageBuilderAddRawByte(&message, 0xFE); // startbyte
  messageBuilderAddDataByte(&message, 0x80 + boardId); // start message for specific boardId
  messageBuilderAddDataByte(&message, 'C'); // class: C = Controlmessage
  messageBuilderAddDataByte(&message, 'B'); // index
  messageBuilderAddDataByte(&message, brightness);
  messageBuilderAddRawByte(&message, 0xFE); // endbyte

  uartTx(&fdSurface, &message, true);
}

// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// brightness = 0 ... 255
void setContrast(uint8_t boardId, uint8_t contrast) {
  messageBuilderInit(&message);

  messageBuilderAddRawByte(&message, 0xFE); // startbyte
  messageBuilderAddDataByte(&message, 0x80 + boardId); // start message for specific boardId
  messageBuilderAddDataByte(&message, 'C'); // class: C = Controlmessage
  messageBuilderAddDataByte(&message, 'C'); // index
  messageBuilderAddDataByte(&message, contrast & 0x3F);
  messageBuilderAddRawByte(&message, 0xFE); // endbyte

  uartTx(&fdSurface, &message, true);
}

// boardId = 0, 4, 5, 8
// index = 0 ... 8
// color = 0=BLACK, 1=RED, 2=GREEN, 3=YELLOW, 4=BLUE, 5=PINK, 6=CYAN, 7=WHITE
// icon = TODO
// sizeA/B = 0x00 (small) or 0x20 (large)
// xA/B = horizontal position in pixel
// yA/B = vertical position in pixel
// strA/B = String of Text to be displayed
void setLcd(uint8_t boardId, uint8_t index, uint8_t color, uint8_t xicon, uint8_t yicon, uint8_t icon, uint8_t sizeA, uint8_t xA, uint8_t yA, const char* strA, uint8_t sizeB, uint8_t xB, uint8_t yB, const char* strB) {
  // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
  // 0x44, i, color.b, script[]

  messageBuilderInit(&message);

  messageBuilderAddRawByte(&message, 0xFE); // startbyte
  messageBuilderAddDataByte(&message, 0x80 + boardId); // start message for specific boardId
  messageBuilderAddDataByte(&message, 'D'); // class: D = Display
  messageBuilderAddDataByte(&message, index); // index
  messageBuilderAddDataByte(&message, color & 0x07); // use only 3 bits (bit 0=R, 1=G, 2=B)

  // begin of script
  // transmit icon
  messageBuilderAddDataByte(&message, icon);
  messageBuilderAddDataByte(&message, xicon);
  messageBuilderAddDataByte(&message, yicon);

  // transmit first text
  messageBuilderAddDataByte(&message, sizeA + strlen(strA)); // size + textLength
  messageBuilderAddDataByte(&message, xA);
  messageBuilderAddDataByte(&message, yA);
  messageBuilderAddString(&message, strA); // this is ASCII, so we can omit byte-stuffing

  messageBuilderAddDataByte(&message, sizeB + strlen(strB)); // size + textLength
  messageBuilderAddDataByte(&message, xB);
  messageBuilderAddDataByte(&message, yB);
  messageBuilderAddString(&message, strB); // this is ASCII, so we can omit byte-stuffing
  // end of script

  messageBuilderAddRawByte(&message, 0xFE); // endbyte

  uartTx(&fdSurface, &message, true);
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t calcEncoderRingLedIncrement(uint8_t pct) {
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
uint16_t calcEncoderRingLedPosition(uint8_t pct) {
    uint8_t led_index = (uint8_t)(((float)pct / 100.0f) * 12.0f + 0.5f); // +0.5f für Rundung

    if (led_index > 12) {
        led_index = 12;
    }

    return (1U << led_index);
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t calcEncoderRingLedBalance(uint8_t pct) {
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
uint16_t calcEncoderRingLedWidth(uint8_t pct) {
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

void surfaceInit(void) {
    // set brightness and contrast
    setBrightness(0, 255); // brightness of LEDs
    setBrightness(1, 255);
    setBrightness(4, 255);
    setBrightness(5, 255);
    setBrightness(8, 255);
    setContrast(0, 0x24); // contrast of LCDs
    setContrast(4, 0x24);
    setContrast(5, 0x24);
    setContrast(8, 0x24);
}

void surfaceReset(void) {
    int fd = open("/sys/class/leds/reset_surface/brightness", O_WRONLY);
    write(fd, "1", 1);
    usleep(100 * 1000);
    write(fd, "0", 1);
    close(fd);
    usleep(2000 * 1000);
}

void surfaceProcessUartData(int bytesToProcess) {
    uint8_t currentByte = 0;
    uint8_t receivedClass = 0;
    uint8_t receivedIndex = 0;
    uint16_t receivedValue = 0;
    //uint8_t receivedChecksum = 0;

    int surfacePacketBufLen = 0;
    char surfacePacketBuffer[SURFACE_MAX_PACKET_LENGTH];

    if (bytesToProcess <= 0) {
        return;
	}

    for (int i = 0; i < bytesToProcess; i++) {
        currentByte = (uint8_t)uartBufferSurface[i];
        //printf("%02X ", currentByte); // empfangene Bytes als HEX-Wert ausgeben

        // add received byte to buffer
        if (surfacePacketBufLen < SURFACE_MAX_PACKET_LENGTH) {
            surfacePacketBuffer[surfacePacketBufLen++] = currentByte;
        } else {
            // buffer full -> remove oldest byte
            memmove(surfacePacketBuffer, surfacePacketBuffer + 1, SURFACE_MAX_PACKET_LENGTH - 1);
            surfacePacketBuffer[SURFACE_MAX_PACKET_LENGTH - 1] = currentByte;
        }

        // we are expecting two types of messages: long (standard) and short (repeating message)
        // Short:                          Index + Value_LSB + 0xFE + CHECKSUM = 5 Bytes
        // Short:                          Index + Value_LSB + Value_MSB + 0xFE + CHECKSUM = 6 Bytes
        // Long:  0xFE + BoardID + Class + Index + Value_LSB             + 0xFE + CHECKSUM = 7 Bytes
        // Long:  0xFE + BoardID + Class + Index + Value_LSB + Value_MSB + 0xFE + CHECKSUM = 8 Bytes
        int receivedPacketLength = 0; // length of detected packet

        // check if we received enought data to process at least the shortest message
        if (surfacePacketBufLen >= 4) {
            // check for expected end-sequence (0xFE CHECKSUM)
            uint8_t offset = 0;
            if ((surfacePacketBuffer[surfacePacketBufLen-2] != 0xFE) && (surfacePacketBuffer[surfacePacketBufLen-1] == 0xFE)) {
                // this packet has no checksum!?
                offset = 1;
            }

            if ((surfacePacketBuffer[surfacePacketBufLen - 2] == 0xFE) || (surfacePacketBuffer[surfacePacketBufLen - 1] == 0xFE)) {
                // check for long 16-bit-packet (8 bytes)
                // 0xFE + BoardID + Class + Index + Value_LSB + Value_MSB + 0xFE + CHECKSUM = 8 Bytes
                if (surfacePacketBufLen >= (8 - offset) && surfacePacketBuffer[surfacePacketBufLen - 8 + offset] == 0xFE) {
                    receivedBoardId = surfacePacketBuffer[surfacePacketBufLen - 7 + offset] & 0x7F;
                    receivedClass = surfacePacketBuffer[surfacePacketBufLen - 6 + offset];
                    receivedIndex = surfacePacketBuffer[surfacePacketBufLen - 5 + offset];
                    receivedValue = ((uint16_t)surfacePacketBuffer[surfacePacketBufLen - 3 + offset] << 8) | (uint16_t)surfacePacketBuffer[surfacePacketBufLen - 4 + offset];
                    //receivedChecksum = surfacePacketBuffer[surfacePacketBufLen - 1];
                    receivedPacketLength = 8 - offset;
                }
                // check for long 8-bit-packet (7 bytes)
                // 0xFE + BoardID + Class + Index + Value_LSB             + 0xFE + CHECKSUM = 7 Bytes
                else if ((surfacePacketBufLen >= (7 - offset)) && (surfacePacketBuffer[surfacePacketBufLen - 7 + offset] == 0xFE)) {
                    receivedBoardId = surfacePacketBuffer[surfacePacketBufLen - 6 + offset] & 0x7F;
                    receivedClass = surfacePacketBuffer[surfacePacketBufLen - 5 + offset];
                    receivedIndex = surfacePacketBuffer[surfacePacketBufLen - 4 + offset];
                    receivedValue = surfacePacketBuffer[surfacePacketBufLen - 3 + offset];
                    //receivedChecksum = surfacePacketBuffer[surfacePacketBufLen - 1];
                    receivedPacketLength = 7 - offset;
                }
                // check for short 16-bit-packet (6 bytes)
                // Class + Index + Value_LSB + Value_MSB + 0xFE + CHECKSUM = 8 Bytes
                else if (surfacePacketBufLen >= (6 - offset)) {
                    // 0xFE and BoardID is missing here
                    receivedClass = surfacePacketBuffer[surfacePacketBufLen - 6 + offset];
                    receivedIndex = surfacePacketBuffer[surfacePacketBufLen - 5 + offset];
                    receivedValue = ((uint16_t)surfacePacketBuffer[surfacePacketBufLen - 3 + offset] << 8) | (uint16_t)surfacePacketBuffer[surfacePacketBufLen - 4 + offset];
                    //receivedChecksum = surfacePacketBuffer[surfacePacketBufLen - 1];
                    receivedPacketLength = 6 - offset;
                }
                // check for short 8-bit-packet (5 bytes)
                // ID Value_MSB Value_LSB 0xFE CHECKSUM
                else if (surfacePacketBufLen >= (5 - offset)) {
                    // 0xFE and BoardID is missing here
                    receivedClass = surfacePacketBuffer[surfacePacketBufLen - 5 + offset];
                    receivedIndex = surfacePacketBuffer[surfacePacketBufLen - 4 + offset];
                    receivedValue = surfacePacketBuffer[surfacePacketBufLen - 3 + offset];
                    //receivedChecksum = surfacePacketBuffer[surfacePacketBufLen - 1];
                    receivedPacketLength = 5 - offset;
                }

/*
                // now recalculate the checksum and check against received one
                char buf[receivedPacketLength];
                for (uint8_t i=0; i<(receivedPacketLength-1); i++) {
                    buf[i] = surfacePacketBuffer[surfacePacketBufLen - receivedPacketLength - i];
                }
                calculateChecksum(buf, sizeof(buf));
                if ((receivedPacketLength > 0) && (receivedChecksum == buf[sizeof(buf)-1])) {
                    surfaceCallback(receivedBoardId, receivedClass, receivedIndex, receivedValue);
                }
*/
                if (receivedPacketLength > 0) {
                    surfaceCallback(receivedBoardId, receivedClass, receivedIndex, receivedValue);

                    // shift remaining bytes byte by processed amount of data
                    memmove(surfacePacketBuffer, surfacePacketBuffer + receivedPacketLength, surfacePacketBufLen - receivedPacketLength);
                    surfacePacketBufLen -= receivedPacketLength;
                }
                /*
                if (surfacePacketBufLen > 0) {
                  printf("Warnung: verbleibende Bytes im Puffer: %d\n", surfacePacketBufLen);
                }
                */
            }
        }
    }
}


int buttonDefinitionIndex = 0;
int encoderDefinitionIndex = 0;

void addButtonDefinition(X32_BTN p_button, uint16_t p_buttonNr) {
    if (buttonDefinitionIndex >= MAX_BUTTONS)
    {
        //TODO: Error Message
        x32log("ERROR: MAX_BUTTONS");
        return;
    }
    x32_btn_def[buttonDefinitionIndex].button = p_button;
    x32_btn_def[buttonDefinitionIndex].buttonNr = p_buttonNr;
    buttonDefinitionIndex++;
    x32debug("added button definition: Button %d -> ButtonNr %d\n", p_button, p_buttonNr);
}

void addEncoderDefinition(X32_ENC p_encoder, uint16_t p_encoderNr) {
    if (encoderDefinitionIndex >= MAX_ENCODERS)
    {
        //TODO: Error Message
        x32log("ERROR: MAX_ENCODERS");
        return;
    }
    x32_enc_def[encoderDefinitionIndex].encoder = p_encoder;
    x32_enc_def[encoderDefinitionIndex].encoderNr = p_encoderNr;
    encoderDefinitionIndex++;
    x32debug("added button definition: Button %d -> ButtonNr %d\n", p_encoder, p_encoderNr);
}

void initDefinitions() {
    switch(mixer.model) {
        case X32_MODEL_FULL:   
            addButtonDefinition( X32_BTN_TALK_A, 0x012E);
            addButtonDefinition( X32_BTN_TALK_B, 0x012F);
            addButtonDefinition( X32_BTN_VIEW_TALK, 0x0130);
            addButtonDefinition( X32_BTN_MONITOR_DIM, 0x012C);
            addButtonDefinition( X32_BTN_VIEW_MONITOR, 0x012D);

            addButtonDefinition( X32_BTN_VIEW_USB, 0x0131);

            addButtonDefinition( X32_BTN_PHANTOM_48V, 0x0101);
            addButtonDefinition( X32_BTN_PHASE_INVERT, 0x0102);
            addButtonDefinition( X32_BTN_LOW_CUT, 0x0103);
            addButtonDefinition( X32_BTN_VIEW_CONFIG, 0x0100);

            addButtonDefinition( X32_BTN_GATE, 0x0106);
            addButtonDefinition( X32_BTN_VIEW_GATE, 0x0104);

            addButtonDefinition( X32_BTN_COMPRESSOR, 0x0107);
            addButtonDefinition( X32_BTN_VIEW_COMPRESSOR, 0x0105);

            addButtonDefinition( X32_BTN_EQ, 0x0109);
            addButtonDefinition( X32_BTN_EQ_MODE, 0x0108);
            addButtonDefinition( X32_BTN_EQ_HIGH, 0x010B);
            addButtonDefinition( X32_BTN_EQ_HIGH_MID, 0x010C);
            addButtonDefinition( X32_BTN_EQ_LOW_MID, 0x010D);
            addButtonDefinition( X32_BTN_EQ_LOW, 0x010E);
            addButtonDefinition( X32_BTN_VIEW_EQ, 0x010A);

            addButtonDefinition( X32_BTN_VIEW_MIX_BUS_SENDS, 0x010F);
            addButtonDefinition( X32_BTN_MONO_BUS, 0x0115);
            addButtonDefinition( X32_BTN_MAIN_LR_BUS, 0x0116);
            addButtonDefinition( X32_BTN_VIEW_BUS_MIXES, 0x0114);
            addButtonDefinition( X32_BTN_BUS_SEND_1_4, 0x0110);
            addButtonDefinition( X32_BTN_BUS_SEND_5_8, 0x0111);
            addButtonDefinition( X32_BTN_BUS_SEND_9_12, 0x0112);
            addButtonDefinition( X32_BTN_BUS_SEND_13_16, 0x0113);

            addButtonDefinition( X32_BTN_ENCODER1, 0x0117);
            addButtonDefinition( X32_BTN_ENCODER2, 0x0118);
            addButtonDefinition( X32_BTN_ENCODER3, 0x0119);
            addButtonDefinition( X32_BTN_ENCODER4, 0x011A);
            addButtonDefinition( X32_BTN_ENCODER5, 0x011B);
            addButtonDefinition( X32_BTN_ENCODER6, 0x011C);

            addButtonDefinition( X32_BTN_HOME, 0x0122);
            addButtonDefinition( X32_BTN_METERS, 0x0123);
            addButtonDefinition( X32_BTN_ROUTING, 0x0124);
            addButtonDefinition( X32_BTN_SETUP, 0x0125);
            addButtonDefinition( X32_BTN_LIBRARY, 0x0126);
            addButtonDefinition( X32_BTN_EFFECTS, 0x0127);
            addButtonDefinition( X32_BTN_MUTE_GRP, 0x0128);
            addButtonDefinition( X32_BTN_UTILITY, 0x0129);

            addButtonDefinition( X32_BTN_UP, 0x011D);
            addButtonDefinition( X32_BTN_DOWN, 0x0120);
            addButtonDefinition( X32_BTN_LEFT, 0x011E);
            addButtonDefinition( X32_BTN_RIGHT, 0x011F);

            // Board, L);

            addButtonDefinition( X32_BTN_DAW_REMOTE, 0x400);
            addButtonDefinition( X32_BTN_CH_1_16, 0x401);
            addButtonDefinition( X32_BTN_CH_17_32, 0x402);
            addButtonDefinition( X32_BTN_AUX_IN_EFFECTS, 0x403);
            addButtonDefinition( X32_BTN_BUS_MASTER, 0x404);

            addButtonDefinition( X32_BTN_BOARD_L_CH_1_SELECT, 0x420);
            addButtonDefinition( X32_BTN_BOARD_L_CH_2_SELECT, 0x421);
            addButtonDefinition( X32_BTN_BOARD_L_CH_3_SELECT, 0x422);
            addButtonDefinition( X32_BTN_BOARD_L_CH_4_SELECT, 0x423);
            addButtonDefinition( X32_BTN_BOARD_L_CH_5_SELECT, 0x424);
            addButtonDefinition( X32_BTN_BOARD_L_CH_6_SELECT, 0x425);
            addButtonDefinition( X32_BTN_BOARD_L_CH_7_SELECT, 0x426);
            addButtonDefinition( X32_BTN_BOARD_L_CH_8_SELECT, 0x427);

            addButtonDefinition( X32_BTN_BOARD_L_CH_1_SOLO, 0x430);
            addButtonDefinition( X32_BTN_BOARD_L_CH_2_SOLO, 0x431);
            addButtonDefinition( X32_BTN_BOARD_L_CH_3_SOLO, 0x432);
            addButtonDefinition( X32_BTN_BOARD_L_CH_4_SOLO, 0x433);
            addButtonDefinition( X32_BTN_BOARD_L_CH_5_SOLO, 0x434);
            addButtonDefinition( X32_BTN_BOARD_L_CH_6_SOLO, 0x435);
            addButtonDefinition( X32_BTN_BOARD_L_CH_7_SOLO, 0x436);
            addButtonDefinition( X32_BTN_BOARD_L_CH_8_SOLO, 0x437);

            addButtonDefinition( X32_BTN_BOARD_L_CH_1_MUTE, 0x440);
            addButtonDefinition( X32_BTN_BOARD_L_CH_2_MUTE, 0x441);
            addButtonDefinition( X32_BTN_BOARD_L_CH_3_MUTE, 0x442);
            addButtonDefinition( X32_BTN_BOARD_L_CH_4_MUTE, 0x443);
            addButtonDefinition( X32_BTN_BOARD_L_CH_5_MUTE, 0x444);
            addButtonDefinition( X32_BTN_BOARD_L_CH_6_MUTE, 0x445);
            addButtonDefinition( X32_BTN_BOARD_L_CH_7_MUTE, 0x446);
            addButtonDefinition( X32_BTN_BOARD_L_CH_8_MUTE, 0x447);

            // Board, M);

            addButtonDefinition( X32_BTN_BOARD_M_CH_1_SELECT, 0x520);
            addButtonDefinition( X32_BTN_BOARD_M_CH_2_SELECT, 0x521);
            addButtonDefinition( X32_BTN_BOARD_M_CH_3_SELECT, 0x522);
            addButtonDefinition( X32_BTN_BOARD_M_CH_4_SELECT, 0x523);
            addButtonDefinition( X32_BTN_BOARD_M_CH_5_SELECT, 0x524);
            addButtonDefinition( X32_BTN_BOARD_M_CH_6_SELECT, 0x525);
            addButtonDefinition( X32_BTN_BOARD_M_CH_7_SELECT, 0x526);
            addButtonDefinition( X32_BTN_BOARD_M_CH_8_SELECT, 0x527);

            addButtonDefinition( X32_BTN_BOARD_M_CH_1_SOLO, 0x530);
            addButtonDefinition( X32_BTN_BOARD_M_CH_2_SOLO, 0x531);
            addButtonDefinition( X32_BTN_BOARD_M_CH_3_SOLO, 0x532);
            addButtonDefinition( X32_BTN_BOARD_M_CH_4_SOLO, 0x533);
            addButtonDefinition( X32_BTN_BOARD_M_CH_5_SOLO, 0x534);
            addButtonDefinition( X32_BTN_BOARD_M_CH_6_SOLO, 0x535);
            addButtonDefinition( X32_BTN_BOARD_M_CH_7_SOLO, 0x536);
            addButtonDefinition( X32_BTN_BOARD_M_CH_8_SOLO, 0x537);

            addButtonDefinition( X32_BTN_BOARD_M_CH_1_MUTE, 0x540);
            addButtonDefinition( X32_BTN_BOARD_M_CH_2_MUTE, 0x541);
            addButtonDefinition( X32_BTN_BOARD_M_CH_3_MUTE, 0x542);
            addButtonDefinition( X32_BTN_BOARD_M_CH_4_MUTE, 0x543);
            addButtonDefinition( X32_BTN_BOARD_M_CH_5_MUTE, 0x544);
            addButtonDefinition( X32_BTN_BOARD_M_CH_6_MUTE, 0x545);
            addButtonDefinition( X32_BTN_BOARD_M_CH_7_MUTE, 0x546);
            addButtonDefinition( X32_BTN_BOARD_M_CH_8_MUTE, 0x547);

            // Board, R);

            addButtonDefinition( X32_BTN_SEND_ON_FADERS, 0x800);
            addButtonDefinition( X32_BTN_GROUP_DCA_1_8, 0x801);
            addButtonDefinition( X32_BTN_BUS_1_8, 0x802);
            addButtonDefinition( X32_BTN_BUS_9_16, 0x803);
            addButtonDefinition( X32_BTN_MATRIX_MAIN_C, 0x804);
            addButtonDefinition( X32_BTN_CLEAR_SOLO, 0x806);

            addButtonDefinition( X32_BTN_SCENES_UNDO, 0x03);
            addButtonDefinition( X32_BTN_SCENES_GO, 0x04);
            addButtonDefinition( X32_BTN_SCENES_PREV, 0x01);
            addButtonDefinition( X32_BTN_SCENES_NEXT, 0x02);
            addButtonDefinition( X32_BTN_VIEW_SCENES, 0x00);

            addButtonDefinition( X32_BTN_ASSIGN_5, 0x06);
            addButtonDefinition( X32_BTN_ASSIGN_6, 0x07);
            addButtonDefinition( X32_BTN_ASSIGN_7, 0x08);
            addButtonDefinition( X32_BTN_ASSIGN_8, 0x09);
            addButtonDefinition( X32_BTN_ASSIGN_9, 0x0A);
            addButtonDefinition( X32_BTN_ASSIGN_10, 0x0B);
            addButtonDefinition( X32_BTN_ASSIGN_11, 0x0C);
            addButtonDefinition( X32_BTN_ASSIGN_12, 0x0D);
            addButtonDefinition( X32_BTN_ASSIGN_A, 0x0E);
            addButtonDefinition( X32_BTN_ASSIGN_B, 0x0F);
            addButtonDefinition( X32_BTN_ASSIGN_C, 0x10);
            addButtonDefinition( X32_BTN_VIEW_ASSIGN, 0x05);

            addButtonDefinition( X32_BTN_MUTE_GROUP_1, 0x11);
            addButtonDefinition( X32_BTN_MUTE_GROUP_2, 0x12);
            addButtonDefinition( X32_BTN_MUTE_GROUP_3, 0x13);
            addButtonDefinition( X32_BTN_MUTE_GROUP_4, 0x14);
            addButtonDefinition( X32_BTN_MUTE_GROUP_5, 0x15);
            addButtonDefinition( X32_BTN_MUTE_GROUP_6, 0x16);

            addButtonDefinition( X32_BTN_BOARD_R_CH_1_SELECT, 0x820);
            addButtonDefinition( X32_BTN_BOARD_R_CH_2_SELECT, 0x821);
            addButtonDefinition( X32_BTN_BOARD_R_CH_3_SELECT, 0x822);
            addButtonDefinition( X32_BTN_BOARD_R_CH_4_SELECT, 0x823);
            addButtonDefinition( X32_BTN_BOARD_R_CH_5_SELECT, 0x824);
            addButtonDefinition( X32_BTN_BOARD_R_CH_6_SELECT, 0x825);
            addButtonDefinition( X32_BTN_BOARD_R_CH_7_SELECT, 0x826);
            addButtonDefinition( X32_BTN_BOARD_R_CH_8_SELECT, 0x827);
            addButtonDefinition( X32_BTN_MAIN_SELECT, 0x828);

            addButtonDefinition( X32_BTN_BOARD_R_CH_1_SOLO, 0x830);
            addButtonDefinition( X32_BTN_BOARD_R_CH_2_SOLO, 0x831);
            addButtonDefinition( X32_BTN_BOARD_R_CH_3_SOLO, 0x832);
            addButtonDefinition( X32_BTN_BOARD_R_CH_4_SOLO, 0x833);
            addButtonDefinition( X32_BTN_BOARD_R_CH_5_SOLO, 0x834);
            addButtonDefinition( X32_BTN_BOARD_R_CH_6_SOLO, 0x835);
            addButtonDefinition( X32_BTN_BOARD_R_CH_7_SOLO, 0x836);
            addButtonDefinition( X32_BTN_BOARD_R_CH_8_SOLO, 0x837);
            addButtonDefinition( X32_BTN_MAIN_SOLO, 0x838);

            addButtonDefinition( X32_BTN_BOARD_R_CH_1_MUTE, 0x840);
            addButtonDefinition( X32_BTN_BOARD_R_CH_2_MUTE, 0x841);
            addButtonDefinition( X32_BTN_BOARD_R_CH_3_MUTE, 0x842);
            addButtonDefinition( X32_BTN_BOARD_R_CH_4_MUTE, 0x843);
            addButtonDefinition( X32_BTN_BOARD_R_CH_5_MUTE, 0x844);
            addButtonDefinition( X32_BTN_BOARD_R_CH_6_MUTE, 0x845);
            addButtonDefinition( X32_BTN_BOARD_R_CH_7_MUTE, 0x846);
            addButtonDefinition( X32_BTN_BOARD_R_CH_8_MUTE, 0x847);
            addButtonDefinition( X32_BTN_MAIN_MUTE, 0x848);

            // LED, only);

            // TODO: CHECK LEDs

            addButtonDefinition( X32_LED_USB_ACCESS, 0x0126);

            addButtonDefinition( X32_LED_BACKLIGHT_CONFIG_GAIN, 0x012A);
            addButtonDefinition( X32_LED_BACKLIGHT_CONFIG_FREQ, 0x012B );
            addButtonDefinition( X32_LED_BACKLIGHT_GATE_THRESHOLD, 0x012C );
            addButtonDefinition( X32_LED_BACKLIGHT_DYNAMICS_THRESHOLD, 0x012D );
            addButtonDefinition( X32_LED_BACKLIGHT_EQ_Q, 0x012E );
            addButtonDefinition( X32_LED_BACKLIGHT_EQ_FREQ, 0x012F );
            addButtonDefinition( X32_LED_BACKLIGHT_EQ_GAIN, 0x0130 );
            addButtonDefinition( X32_LED_BACKLIGHT_BUS_MIXES_LEVEL, 0x0131 );
            addButtonDefinition( X32_LED_BACKLIGHT_BUS_MIXES_PAN, 0x0132 );

            // TODO: Check and add Encoders

            // addEncoderDefinition(X32_ENC_ENCODER1, ??? );
            // addEncoderDefinition(X32_ENC_ENCODER1, ??? );
            // addEncoderDefinition(X32_ENC_ENCODER1, ??? );
            // addEncoderDefinition(X32_ENC_ENCODER1, ??? );


            break;

        case X32_MODEL_COMPACT:

            addButtonDefinition(X32_BTN_TALK_A,         0x0100);
            addButtonDefinition(X32_BTN_TALK_B,         0x0101);
            addButtonDefinition(X32_BTN_MONITOR_DIM,    0x0102);
            addButtonDefinition(X32_BTN_VIEW_MONITOR,   0x0103);
            
            addButtonDefinition(X32_BTN_VIEW_USB,      0x0104);

            addButtonDefinition(X32_BTN_PHANTOM_48V, 0x0105);
            addButtonDefinition(X32_BTN_PHASE_INVERT, 0x0106);
            addButtonDefinition(X32_BTN_LOW_CUT, 0x0107);
            addButtonDefinition(X32_BTN_VIEW_CONFIG, 0x0108);

            addButtonDefinition(X32_BTN_GATE, 0x0109);
            addButtonDefinition(X32_BTN_VIEW_GATE, 0x010A);

            addButtonDefinition(X32_BTN_COMPRESSOR, 0x010B);
            addButtonDefinition(X32_BTN_VIEW_COMPRESSOR, 0x010C);

            addButtonDefinition(X32_BTN_EQ, 0x010D);
            addButtonDefinition(X32_BTN_EQ_MODE, 0x010E);
            addButtonDefinition(X32_BTN_EQ_HIGH, 0x010F);
            addButtonDefinition(X32_BTN_EQ_HIGH_MID, 0x0110);
            addButtonDefinition(X32_BTN_EQ_LOW_MID, 0x0111);
            addButtonDefinition(X32_BTN_EQ_LOW, 0x0112);
            addButtonDefinition(X32_BTN_VIEW_EQ, 0x0113);

            addButtonDefinition(X32_BTN_VIEW_MIX_BUS_SENDS, 0x0114);
            addButtonDefinition(X32_BTN_MONO_BUS, 0x0115);
            addButtonDefinition(X32_BTN_MAIN_LR_BUS, 0x0116);
            addButtonDefinition(X32_BTN_VIEW_BUS_MIXES, 0x0117);

            addButtonDefinition(X32_BTN_ENCODER1, 0x0118);
            addButtonDefinition(X32_BTN_ENCODER2, 0x0119);
            addButtonDefinition(X32_BTN_ENCODER3, 0x011A);
            addButtonDefinition(X32_BTN_ENCODER4, 0x011B);
            addButtonDefinition(X32_BTN_ENCODER5, 0x011C);
            addButtonDefinition(X32_BTN_ENCODER6, 0x011D);

            addButtonDefinition(X32_BTN_HOME,           0x011E);
            addButtonDefinition(X32_BTN_METERS,         0x011F);
            addButtonDefinition(X32_BTN_ROUTING,        0x0120);
            addButtonDefinition(X32_BTN_SETUP,          0x0121);
            addButtonDefinition(X32_BTN_LIBRARY,        0x0122);
            addButtonDefinition(X32_BTN_EFFECTS,        0x0123);
            addButtonDefinition(X32_BTN_MUTE_GRP,       0x0124);
            addButtonDefinition(X32_BTN_UTILITY,        0x0125);

            addButtonDefinition(X32_BTN_UP,             0x0126);
            addButtonDefinition(X32_BTN_DOWN,           0x0127);
            addButtonDefinition(X32_BTN_LEFT,           0x0128);
            addButtonDefinition(X32_BTN_RIGHT,          0x0129);
        
            // Board L

            addButtonDefinition(X32_BTN_CH_1_8, 0x400);
            addButtonDefinition(X32_BTN_CH_9_16, 0x401);
            addButtonDefinition(X32_BTN_CH_17_24, 0x402);
            addButtonDefinition(X32_BTN_CH_25_32, 0x403);
            addButtonDefinition(X32_BTN_AUX_IN_1_6_USB_REC, 0x404);
            addButtonDefinition(X32_BTN_EFFECTS_RETURNS, 0x405);
            addButtonDefinition(X32_BTN_BUS_1_8_MASTER, 0x406);
            addButtonDefinition(X32_BTN_BUS_9_16_MASTER, 0x407);

            addButtonDefinition(X32_BTN_BOARD_L_CH_1_SELECT, 0x420);
            addButtonDefinition(X32_BTN_BOARD_L_CH_2_SELECT, 0x421);
            addButtonDefinition(X32_BTN_BOARD_L_CH_3_SELECT, 0x422);
            addButtonDefinition(X32_BTN_BOARD_L_CH_4_SELECT, 0x423);
            addButtonDefinition(X32_BTN_BOARD_L_CH_5_SELECT, 0x424);
            addButtonDefinition(X32_BTN_BOARD_L_CH_6_SELECT, 0x425);
            addButtonDefinition(X32_BTN_BOARD_L_CH_7_SELECT, 0x426);
            addButtonDefinition(X32_BTN_BOARD_L_CH_8_SELECT, 0x427);

            addButtonDefinition(X32_BTN_BOARD_L_CH_1_SOLO, 0x430);
            addButtonDefinition(X32_BTN_BOARD_L_CH_2_SOLO, 0x431);
            addButtonDefinition(X32_BTN_BOARD_L_CH_3_SOLO, 0x432);
            addButtonDefinition(X32_BTN_BOARD_L_CH_4_SOLO, 0x433);
            addButtonDefinition(X32_BTN_BOARD_L_CH_5_SOLO, 0x434);
            addButtonDefinition(X32_BTN_BOARD_L_CH_6_SOLO, 0x435);
            addButtonDefinition(X32_BTN_BOARD_L_CH_7_SOLO, 0x436);
            addButtonDefinition(X32_BTN_BOARD_L_CH_8_SOLO, 0x437);

            addButtonDefinition(X32_BTN_BOARD_L_CH_1_MUTE, 0x440);
            addButtonDefinition(X32_BTN_BOARD_L_CH_2_MUTE, 0x441);
            addButtonDefinition(X32_BTN_BOARD_L_CH_3_MUTE, 0x442);
            addButtonDefinition(X32_BTN_BOARD_L_CH_4_MUTE, 0x443);
            addButtonDefinition(X32_BTN_BOARD_L_CH_5_MUTE, 0x444);
            addButtonDefinition(X32_BTN_BOARD_L_CH_6_MUTE, 0x445);
            addButtonDefinition(X32_BTN_BOARD_L_CH_7_MUTE, 0x446);
            addButtonDefinition(X32_BTN_BOARD_L_CH_8_MUTE, 0x447);

            // Board R);

            addButtonDefinition(X32_BTN_DAW_REMOTE, 0x800);
            addButtonDefinition(X32_BTN_SEND_ON_FADERS, 0x801);
            addButtonDefinition(X32_BTN_GROUP_DCA_1_8, 0x802);
            addButtonDefinition(X32_BTN_BUS_1_8, 0x803);
            addButtonDefinition(X32_BTN_BUS_9_16, 0x804);
            addButtonDefinition(X32_BTN_MATRIX_MAIN_C, 0x805);
            addButtonDefinition(X32_BTN_CLEAR_SOLO, 0x806);

            addButtonDefinition(X32_BTN_SCENES_UNDO, 0x807);
            addButtonDefinition(X32_BTN_SCENES_GO, 0x808);
            addButtonDefinition(X32_BTN_SCENES_PREV, 0x809);
            addButtonDefinition(X32_BTN_SCENES_NEXT, 0x80A);
            addButtonDefinition(X32_BTN_VIEW_SCENES, 0x80B);

            addButtonDefinition(X32_BTN_ASSIGN_1, 0x80C);
            addButtonDefinition(X32_BTN_ASSIGN_2, 0x80D);
            addButtonDefinition(X32_BTN_ASSIGN_3, 0x80E);
            addButtonDefinition(X32_BTN_ASSIGN_4, 0x80F);
            addButtonDefinition(X32_BTN_ASSIGN_5, 0x810);
            addButtonDefinition(X32_BTN_ASSIGN_6, 0x811);
            addButtonDefinition(X32_BTN_ASSIGN_7, 0x812);
            addButtonDefinition(X32_BTN_ASSIGN_8, 0x813);
            addButtonDefinition(X32_BTN_VIEW_ASSIGN, 0x814);

            addButtonDefinition(X32_BTN_MUTE_GROUP_1, 0x815);
            addButtonDefinition(X32_BTN_MUTE_GROUP_2, 0x816);
            addButtonDefinition(X32_BTN_MUTE_GROUP_3, 0x817);
            addButtonDefinition(X32_BTN_MUTE_GROUP_4, 0x818);
            addButtonDefinition(X32_BTN_MUTE_GROUP_5, 0x819);
            addButtonDefinition(X32_BTN_MUTE_GROUP_6, 0x81A);

            addButtonDefinition(X32_BTN_BOARD_R_CH_1_SELECT, 0x820);
            addButtonDefinition(X32_BTN_BOARD_R_CH_2_SELECT, 0x821);
            addButtonDefinition(X32_BTN_BOARD_R_CH_3_SELECT, 0x822);
            addButtonDefinition(X32_BTN_BOARD_R_CH_4_SELECT, 0x823);
            addButtonDefinition(X32_BTN_BOARD_R_CH_5_SELECT, 0x824);
            addButtonDefinition(X32_BTN_BOARD_R_CH_6_SELECT, 0x825);
            addButtonDefinition(X32_BTN_BOARD_R_CH_7_SELECT, 0x826);
            addButtonDefinition(X32_BTN_BOARD_R_CH_8_SELECT, 0x827);
            addButtonDefinition(X32_BTN_MAIN_SELECT, 0x828);

            addButtonDefinition(X32_BTN_BOARD_R_CH_1_SOLO, 0x830);
            addButtonDefinition(X32_BTN_BOARD_R_CH_2_SOLO, 0x831);
            addButtonDefinition(X32_BTN_BOARD_R_CH_3_SOLO, 0x832);
            addButtonDefinition(X32_BTN_BOARD_R_CH_4_SOLO, 0x833);
            addButtonDefinition(X32_BTN_BOARD_R_CH_5_SOLO, 0x834);
            addButtonDefinition(X32_BTN_BOARD_R_CH_6_SOLO, 0x835);
            addButtonDefinition(X32_BTN_BOARD_R_CH_7_SOLO, 0x836);
            addButtonDefinition(X32_BTN_BOARD_R_CH_8_SOLO, 0x837);
            addButtonDefinition(X32_BTN_MAIN_SOLO, 0x838);

            addButtonDefinition(X32_BTN_BOARD_R_CH_1_MUTE, 0x840);
            addButtonDefinition(X32_BTN_BOARD_R_CH_2_MUTE, 0x841);
            addButtonDefinition(X32_BTN_BOARD_R_CH_3_MUTE, 0x842);
            addButtonDefinition(X32_BTN_BOARD_R_CH_4_MUTE, 0x843);
            addButtonDefinition(X32_BTN_BOARD_R_CH_5_MUTE, 0x844);
            addButtonDefinition(X32_BTN_BOARD_R_CH_6_MUTE, 0x845);
            addButtonDefinition(X32_BTN_BOARD_R_CH_7_MUTE, 0x846);
            addButtonDefinition(X32_BTN_BOARD_R_CH_8_MUTE, 0x847);
            addButtonDefinition(X32_BTN_MAIN_MUTE, 0x848);

            // LED only

            addButtonDefinition(X32_LED_USB_ACCESS, 0x0126);

            addButtonDefinition(X32_LED_BACKLIGHT_CONFIG_GAIN, 0x012A);
            addButtonDefinition(X32_LED_BACKLIGHT_CONFIG_FREQ, 0x012B);
            addButtonDefinition(X32_LED_BACKLIGHT_GATE_THRESHOLD, 0x012C);
            addButtonDefinition(X32_LED_BACKLIGHT_DYNAMICS_THRESHOLD, 0x012D);
            addButtonDefinition(X32_LED_BACKLIGHT_EQ_Q, 0x012E);
            addButtonDefinition(X32_LED_BACKLIGHT_EQ_FREQ, 0x012F);
            addButtonDefinition(X32_LED_BACKLIGHT_EQ_GAIN, 0x0130);
            addButtonDefinition(X32_LED_BACKLIGHT_BUS_MIXES_LEVEL, 0x0131);
            addButtonDefinition(X32_LED_BACKLIGHT_BUS_MIXES_PAN, 0x0132);

            // Encoder

            addEncoderDefinition(X32_ENC_GAIN, 0x0100);
            addEncoderDefinition(X32_ENC_LOWCUT, 0x0101);
            addEncoderDefinition(X32_ENC_GATE, 0x0102);
            addEncoderDefinition(X32_ENC_DYNAMICS, 0x0103);

            addEncoderDefinition(X32_ENC_EQ_Q, 0x0104);
            addEncoderDefinition(X32_ENC_EQ_FREQ, 0x0105);
            addEncoderDefinition(X32_ENC_EQ_GAIN, 0x0106);

            addEncoderDefinition(X32_ENC_MAIN, 0x0107);
            addEncoderDefinition(X32_ENC_PAN, 0x0108);

            addEncoderDefinition(X32_ENC_ENCODER1, 0x0109);
            addEncoderDefinition(X32_ENC_ENCODER2, 0x010A);
            addEncoderDefinition(X32_ENC_ENCODER3, 0x010B);
            addEncoderDefinition(X32_ENC_ENCODER4, 0x010C);
            addEncoderDefinition(X32_ENC_ENCODER5, 0x010D);
            addEncoderDefinition(X32_ENC_ENCODER6, 0x010E);

            break;

        case X32_MODEL_CORE:

            addButtonDefinition(X32_BTN_SCENE_SETUP,    0x0000);
            addButtonDefinition(X32_BTN_TALK_A,         0x0001);
            addButtonDefinition(X32_BTN_ASSIGN_3,       0x0002);
            addButtonDefinition(X32_BTN_ASSIGN_4,       0x0003);
            addButtonDefinition(X32_BTN_ASSIGN_5,       0x0004);
            addButtonDefinition(X32_BTN_ASSIGN_6,       0x0005);
            addButtonDefinition(X32_BTN_ENCODER1,       0x0006);
            //addButtonDefinition(X32_BTN_ENCODER1,       0x0006);

            // LED only
            addButtonDefinition(X32_LED_SCENE_SETUP_RED,0x0006);
            addButtonDefinition(X32_LED_IN,             0x0007);
            addButtonDefinition(X32_LED_AUX,            0x0008);
            addButtonDefinition(X32_LED_BUS,            0x0009);
            addButtonDefinition(X32_LED_MTX,            0x000A);
            addButtonDefinition(X32_LED_DCA,            0x000B);
            addButtonDefinition(X32_LED_AESA_GREEN,     0x000C);
            addButtonDefinition(X32_LED_AESA_RED,       0x000D);
            addButtonDefinition(X32_LED_AESB_GREEN,     0x000E);
            addButtonDefinition(X32_LED_AESB_RED,       0x000F);

        default:
            break;
    }
}

uint16_t enum2button(X32_BTN button) {
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

X32_BTN button2enum(uint16_t buttonNr) {
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

X32_ENC encoder2enum(uint16_t encoderNr) {
    x32debug("DEBUG: encoder2enum: EncoderNr %d -> ", encoderNr);
    for(int i = 0; i < buttonDefinitionIndex; i++) {
        if (x32_enc_def[i].encoderNr == encoderNr) {
            x32debug("gefunden: Encoder %d\n", x32_enc_def[i].encoder);
            return x32_enc_def[i].encoder;
        }
    }
    x32debug(" NICHT gefunden!\n");
    return X32_ENC_NONE;
}
