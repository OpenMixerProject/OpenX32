/*
  Control software for Behringer X32 using OpenX32
  https://github.com/xn--nding-jua/OpenX32
  v0.0.3, 02.08.2025

  This software uses LVGL and the LVGL Linux Port to display a nice user-interface
  To compile the software just call "make" and a 32-bit ARM-binary should be created
  in the build/bin-folder

  Parts of this software with kind permission of Music Tribe. Thank you!
*/

#include "x32ctrl.h"

// global variables
int8_t x32model = -1;

/*
// called every 50ms
void timer50msCallback(int signum) {
}
*/

void timer10msCallback(int sig, siginfo_t *si, void *uc) {
  // call EEZ-GUI tick
  ui_tick();

  // read data from surface and calls surfaceCallback()
  surfaceProcessUartData(uartRx(&fdSurface, &uartBufferSurface[0], sizeof(uartBufferSurface)));

  // read data from ADDA-Boards (8ch input/output)
  addaProcessUartData(uartRx(&fdAdda, &uartBufferAdda[0], sizeof(uartBufferAdda)));
}

void surfaceDemo(void) {
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

    // preamp, dynamics, meterL, meterR, meterSolo
    setMeterLedMain(0b00000111, 0b11100001, 0x0FFFFF, 0x0FFFFF, 0x000FFF);

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
      //printf("Fader   : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%04X = %f\n", boardId, class, index, value, pct);
	  lv_label_set_text_fmt(objects.debugtext, "Fader   : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%04X = %f\n", boardId, class, index, value, (double)pct);
  }else if (class == 'b') {
      //printf("Button  : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
	  lv_label_set_text_fmt(objects.debugtext, "Button  : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);

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

      // inform LVGL about this new button-press
      guiNewButtonPress(((uint16_t)boardId << 8) + (uint16_t)(value & 0x7F), (value >> 7));
  }else if (class == 'e') {
      //printf("Encoder : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
	  lv_label_set_text_fmt(objects.debugtext, "Encoder : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
  }
}


void addaCallback(char *msg) {
    if ((strlen(msg) == 4) && (msg[2] == 'Y')) {
        // we received acknowledge-message like *1Y# -> ignore it
    }else{
        // we received other messages -> print them

        // check for *i8CHIN#
        if ((strlen(msg) >= 8) && (msg[3] == 'C') && (msg[4] == 'H')) {
            uint8_t boardId = (uint8_t)msg[1]-0x30;
            uint8_t chanCount = (uint8_t)msg[2]-0x30;

            if ((msg[5] == 'I') && (msg[6] == 'N')) {
                printf("Board %d is input-card with %d channels\n", boardId, chanCount);
            }else if ((msg[5] == 'O') && (msg[6] == 'U')) {
                printf("Board %d is output-card with %d channels\n", boardId, chanCount);
            }else{
                printf("ADDA Message: %s\n", msg);
			}
        }

        lv_label_set_text_fmt(objects.debugtext, "ADDA Message: %s\n", msg);
    }
}

// the main-function - of course
int main() {
    srand(time(NULL));
    printf("OpenX32 UserInterface\n");
    printf("v0.0.3, 02.08.2025\n");
    printf("https://github.com/xn--nding-jua/OpenX32\n");

    printf("Reading config...");
    char model[12];
    char serial[12];
    char date[16];
    readConfig("/etc/x32.conf", "MDL=", model, 12);
    readConfig("/etc/x32.conf", "SN=", serial, 12);
    readConfig("/etc/x32.conf", "DATE=", date, 16);
    if (strcmp(model, "X32Core") == 0) {
        x32model = 4;
    }else if (strcmp(model, "X32Rack") == 0) {
        x32model = 3;
    }else if (strcmp(model, "X32Producer") == 0) {
        x32model = 2;
    }else if (strcmp(model, "X32Compact") == 0) {
        x32model = 1;
    }else if (strcmp(model, "X32") == 0) {
        x32model = 0;
    }else{
        x32model = -1;
    }
    printf(" Detected model: %s with Serial %s built on %s\n", model, serial, date);

    printf("Connecting to UART1...\n");
    uartOpen("/dev/ttymxc1", 115200, &fdSurface);

    printf("Connecting to UART2...\n");
    uartOpen("/dev/ttymxc2", 38400, &fdAdda);

    printf("Initializing X32 Surface...\n");
    //surfaceReset(); // resets all microcontrollers on the board
    //surfaceInit(); // initialize whole surface with default values
    surfaceDemo(); // sets demo-values for faders, leds and lcds

    printf("Initializing Analog-Boards...\n");
    addaInit(48000);
    addaSetMute(false);
    addaSetGain(1, 0, 15.5, true); // boardId, Channel, Gain in dB, +48V
    addaSetGain(3, 1, 15.5, true); // boardId, Channel, Gain in dB, +48V
    addaSetGain(0, 2, 15.5, true); // boardId, Channel, Gain in dB, +48V
    addaSetGain(2, 3, 15.5, true); // boardId, Channel, Gain in dB, +48V

/*
    printf("Start Timer...\n");
    initTimer(); // only necessary if LVGL is not used

    printf("Wait for incoming data on /dev/ttymxc1...\n");
    printf("Press Ctrl+C to terminate program.\n");
    while (1) {
	  surfaceProcessUartData(uartRx(&fdSurface, &uartBufferSurface, sizeof(uartBufferSurface)));

	  addaProcessUartData(uartRx(&fdAdda, &uartBufferAdda, sizeof(uartBufferAdda)));

      // sleep for 1ms to lower CPU-load
      usleep(1000);
    }
*/

    printf("Initializing GUI...\n");
    guiInit(); // initializes LVGL, FBDEV and starts endless loop

    return 0;
}
