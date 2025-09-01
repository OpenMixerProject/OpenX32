/*
  Control software for Behringer X32 using OpenX32
  https://github.com/xn--nding-jua/OpenX32
  v0.0.2, 22.07.2025

  This is a very basic version of the x32ctrl-software for some basic tests.
  To compile the software call ./compile.sh in the folder and a 32-bit ARM
  binary should be created in the build-folder.

  Parts of this software with kind permission of Music Tribe. Thank you!
*/

#include "x32cli.h"

void timer10msCallback(int signum) {
  // nothing to do for now
}

void surfaceInit() {
	// set brightness and contrast
    printf("  Setting brightness and contrast for all displays...\n");
    setBrightness(0, 255); // brightness of LEDs
    setBrightness(1, 255);
    setBrightness(4, 255);
    setBrightness(5, 255);
    setBrightness(8, 255);
    setContrast(0, 0x24); // contrast of LCDs
    setContrast(4, 0x24);
    setContrast(5, 0x24);
    setContrast(8, 0x24);

    printf("  Setting Button-LEDs for boards 0 and 1...\n");
    setLed(0, 0x00, 1); // boardId 0 = user-section
    setLed(1, 0x01, 1); // boardId 1 = EQ-section and monitor
    setLed(1, 0x22, 1); // boardId 0 = EQ-section and monitor

    printf("  Setting Button-LEDs for boards 4, 5 and 8...\n");
    for (uint8_t i=0; i<=8; i++) {
      // select-LEDs
      setLed(4, 0x20+i, 1); // boardId 4 = faderL
      setLed(5, 0x20+i, 1); // boardId 5 = faderM
      setLed(8, 0x20+i, 1); // boardId 8 = faderR

      // cue leds
      setLed(4, 0x30+i, 1); // boardId 4 = faderL
      setLed(5, 0x30+i, 1); // boardId 5 = faderM
      setLed(8, 0x30+i, 1); // boardId 8 = faderR

      // mute leds
      setLed(4, 0x40+i, 1); // boardId 4 = faderL
      setLed(5, 0x40+i, 1); // boardId 5 = faderM
      setLed(8, 0x40+i, 1); // boardId 8 = faderR

      // random leds
//      setLed(4, 0x00+i, 1); // boardId 4 = faderL
//      setLed(5, 0x00+i, 1); // boardId 5 = faderM
//      setLed(8, 0x00+i, 1); // boardId 8 = faderR
    }

    // fader ranges from 0...8
    printf("  Setting Faders for boards 4, 5 and 8...\n");
    for (uint8_t i=0; i<=8; i++) {
      setFader(4, i, 0x000 + 0xAA*i);
      setFader(5, i, 0x555 + 0xAA*i);
      setFader(8, i, 0xAAA + 0xAA*i);
    }

    // meterLED ranges from 1...9
    printf("  Setting VU-Meters for boards 1, 4, 5 and 8...\n");
    setMeterLed(1, 0, 0b00000011);
    for (uint8_t i=0; i<=7; i++) {
      setMeterLed(4, i, 0b00000011);
      setMeterLed(5, i, 0b00001111);
      setMeterLed(8, i, 0b01111111);
    }

    // set some encoders with different options
    for (uint8_t i=0; i<=100; i++) {
      setEncoderRing(0, 0, 0, i, true); // boardId, index, ledMode, ledPct, backlight
      setEncoderRing(0, 1, 1, i, true); // boardId, index, ledMode, ledPct, backlight
      setEncoderRing(0, 2, 2, i, true); // boardId, index, ledMode, ledPct, backlight
      setEncoderRing(0, 3, 3, i, true); // boardId, index, ledMode, ledPct, backlight
      usleep(5000);
    }

    // set display
    printf("  Setting Displays for board 0...\n");
    for (uint8_t i=0; i<4; i++) {
      setLcd(0, i, 7, 70, 8, 0xE2, 0x20, 0, 0, "Board 0", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
    }

    printf("  Setting Displays for boards 4, 5 and 8...\n");
    for (uint8_t i=0; i<=8; i++) {
      setLcd(4, i, 1, 70, 8, 0xE2, 0x20, 0, 0, "Board 1", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
      setLcd(5, i, 2, 70, 8, 0xE2, 0x20, 0, 0, "Board 2", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
      setLcd(8, i, 4, 70, 8, 0xE2, 0x20, 0, 0, "Board 3", 0x00, 0, 48, "OpenX32"); // setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
    }
}

// function will be called by uart-receive-function
void surfaceCallback(uint8_t boardId, uint8_t class, uint8_t index, uint16_t value) {
  if (class == 'f') {
      float pct = value / 40.95; // convert to percent
      printf("Fader   : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%04X = %f\n", boardId, class, index, value, pct);
  }else if (class == 'b') {
      printf("Button  : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);

      // register buttons
      switch (boardId) {
          case 0: // monitor-section
            break;
          case 1: // eq-section and screen-area
            switch(index) {
              case 0: // left
                break;
              case 1: // right
                break;
              case 2: // up
                break;
              case 3: // down
                break;
            }
            break;
          case 4: // faderBoard left
            break;
          case 5: // faderBoard middle
            break;
          case 8: // faderBoard right
            break;
      }
  }else if (class == 'e') {
      printf("Encoder : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
  }
}

// the main-function - of course
int main() {
    srand(time(NULL));
    printf("OpenX32 Surface Demo\n");
    printf("v0.0.1, 16.07.2025\n");
    printf("https://github.com/xn--nding-jua/OpenX32\n");

    printf("Connecting to UART1...\n");
    uartOpen();

    printf("Initializing X32 Surface...\n");
    surfaceInit();

    printf("Start Timer...\n");
    initTimer();

    printf("Wait for incoming data on /dev/ttymxc1...\n");
    printf("Press Ctrl+C to terminate program.\n");
    while (1) {
      uartRead();

      // sleep for 1ms to lower CPU-load
      usleep(1000);
    }

    return 0;
}
