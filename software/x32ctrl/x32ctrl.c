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

  // read data from FPGA
  fpgaProcessUartData(uartRx(&fdFpga, &uartBufferFpga[0], sizeof(uartBufferFpga)));
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
      // calculate user-readable variables
      float pct = value / 40.95; // convert to percent

      //printf("Fader   : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%04X = %f\n", boardId, class, index, value, pct);
      lv_label_set_text_fmt(objects.debugtext, "Fader   : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%04X = %f\n", boardId, class, index, value, (double)pct);
  }else if (class == 'b') {
      // calculate user-readable variables
      X32_BTN button = button2enum(((uint16_t)boardId << 8) + (uint16_t)(value & 0x7F));

      bool buttonPressed = (value >> 7) == 1;

      //printf("Button  : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
      //lv_label_set_text_fmt(objects.debugtext, "Button  : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);

      switch (button) {
          case X32_BTN_HOME:
            lv_tabview_set_active(objects.maintab, 0, LV_ANIM_OFF);
            break;
            case X32_BTN_METERS:
            lv_tabview_set_active(objects.maintab, 1, LV_ANIM_OFF);
            break;
            case X32_BTN_UTILITY:
            lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
            break;
          case X32_BTN_LEFT:
              // set routing to DSP-channels 1-8 which is just an 8-channel tone-generator at the moment
              for (uint8_t ch=1; ch<=8; ch++) {
                  mixingSetRouting('x', ch, mixingGetInputSource('d', ch)); // set xlr-output 1-8
                  mixingSetRouting('x', ch+8, mixingGetInputSource('d', ch)); // set xlr-output 9-16

                  mixingSetRouting('a', ch, mixingGetInputSource('d', ch)); // set aux-channel 1-8

                  mixingSetRouting('c', ch, mixingGetInputSource('d', ch)); // set card-output 1-8
                  mixingSetRouting('c', ch+8, mixingGetInputSource('d', ch)); // set card-output 9-16
                  mixingSetRouting('c', ch+16, mixingGetInputSource('d', ch)); // set card-output 17-24
                  mixingSetRouting('c', ch+24, mixingGetInputSource('d', ch)); // set card-output 25-32

                  mixingSetRouting('p', ch, mixingGetInputSource('d', ch)); // set P16-output 1-8
                  mixingSetRouting('p', ch+8, mixingGetInputSource('d', ch)); // set P16-output 9-16
              }
              // transmit routing-configuration to FPGA
              mixingTxRoutingConfig();

              break;
          case X32_BTN_RIGHT:
              // set routing to XLR-inputs
              for (uint8_t ch=1; ch<=8; ch++) {
                  mixingSetRouting('x', ch, mixingGetInputSource('x', ch)); // set xlr-output 1-8 to xlr-inputs 1-8
                  mixingSetRouting('x', ch+8, mixingGetInputSource('x', ch+8)); // set xlr-output 9-16 to xlr-inputs 9-16

                  mixingSetRouting('a', ch, mixingGetInputSource('x', ch)); // set aux-channel 1-8 to aux-inputs 1-8

                  mixingSetRouting('c', ch, mixingGetInputSource('x', ch)); // set card-output 1-8 to xlr-inputs 1-8
                  mixingSetRouting('c', ch+8, mixingGetInputSource('x', ch+8)); // set card-output 9-16 to xlr-inputs 9-16
                  mixingSetRouting('c', ch+16, mixingGetInputSource('x', ch+16)); // set card-output 17-24 to xlr-inputs 17-24
                  mixingSetRouting('c', ch+24, mixingGetInputSource('x', ch+24)); // set card-output 25-32 to xlr-inputs 25-32

                  mixingSetRouting('p', ch, mixingGetInputSource('x', ch)); // set P16-output 1-8 to xlr-inputs 1-8
                  mixingSetRouting('p', ch+8, mixingGetInputSource('x', ch+8)); // set P16-output 9-16 to xlr-inputs 9-16
              }
              // transmit routing-configuration to FPGA
              mixingTxRoutingConfig();

              break;
          case X32_BTN_UP:
              // set routing to AUX-inputs
              for (uint8_t ch=1; ch<=8; ch++) {
                  mixingSetRouting('x', ch, mixingGetInputSource('a', ch)); // set xlr-output 1-8 to xlr-inputs 1-8
                  mixingSetRouting('x', ch+8, mixingGetInputSource('a', ch)); // set xlr-output 9-16 to xlr-inputs 9-16

                  mixingSetRouting('a', ch, mixingGetInputSource('a', ch)); // set aux-channel 1-8 to aux-inputs 1-8

                  mixingSetRouting('c', ch, mixingGetInputSource('a', ch)); // set card-output 1-8 to xlr-inputs 1-8
                  mixingSetRouting('c', ch+8, mixingGetInputSource('a', ch)); // set card-output 9-16 to xlr-inputs 9-16
                  mixingSetRouting('c', ch+16, mixingGetInputSource('a', ch)); // set card-output 17-24 to xlr-inputs 17-24
                  mixingSetRouting('c', ch+24, mixingGetInputSource('a', ch)); // set card-output 25-32 to xlr-inputs 25-32

                  mixingSetRouting('p', ch, mixingGetInputSource('a', ch)); // set P16-output 1-8 to xlr-inputs 1-8
                  mixingSetRouting('p', ch+8, mixingGetInputSource('a', ch)); // set P16-output 9-16 to xlr-inputs 9-16
              }
              // transmit routing-configuration to FPGA
              mixingTxRoutingConfig();

              break;
          case X32_BTN_DOWN:
              // set routing to Card-inputs
              for (uint8_t ch=1; ch<=8; ch++) {
                  mixingSetRouting('x', ch, mixingGetInputSource('c', ch)); // set xlr-output 1-8 to xlr-inputs 1-8
                  mixingSetRouting('x', ch+8, mixingGetInputSource('c', ch+8)); // set xlr-output 9-16 to xlr-inputs 9-16

                  mixingSetRouting('a', ch, mixingGetInputSource('c', ch)); // set aux-channel 1-8 to aux-inputs 1-8

                  mixingSetRouting('c', ch, mixingGetInputSource('c', ch)); // set card-output 1-8 to xlr-inputs 1-8
                  mixingSetRouting('c', ch+8, mixingGetInputSource('c', ch+8)); // set card-output 9-16 to xlr-inputs 9-16
                  mixingSetRouting('c', ch+16, mixingGetInputSource('c', ch+16)); // set card-output 17-24 to xlr-inputs 17-24
                  mixingSetRouting('c', ch+24, mixingGetInputSource('c', ch+24)); // set card-output 25-32 to xlr-inputs 25-32

                  mixingSetRouting('p', ch, mixingGetInputSource('c', ch)); // set P16-output 1-8 to xlr-inputs 1-8
                  mixingSetRouting('p', ch+8, mixingGetInputSource('c', ch+8)); // set P16-output 9-16 to xlr-inputs 9-16
              }
              // transmit routing-configuration to FPGA
              mixingTxRoutingConfig();

              addaSendCmd("*8C80U#"); // switch Card to 48kHz and USB-input

              break;
          default:
              break;
      }

      // inform LVGL about this new button-press
      guiNewButtonPress(button, buttonPressed);

  }else if (class == 'e') {
      // calculate user-readable variables
      uint16_t encoderNr = ((uint16_t)boardId << 8) + (uint16_t)index;
      int16_t velocity = (int16_t)value;

      //printf("Encoder : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
      lv_label_set_text_fmt(objects.debugtext, "Encoder : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
  }
}


void addaCallback(char *msg) {
    if ((strlen(msg) == 4) && (msg[2] == 'Y')) {
        // we received acknowledge-message like *1Y# -> ignore it
    }else if ((strlen(msg) == 5) && ((msg[2] == 'B') && (msg[3] == 'E'))) {
        // we received keep-alive-message *8BE# from expansion-card -> ignore it
    }else{
        // we received other messages -> print them

        // check for *i8CHIN# or *i8CHOUT#
        if ((strlen(msg) >= 8) && ((msg[3] == 'C') && (msg[4] == 'H'))) {
            uint8_t boardId = (uint8_t)msg[1]-0x30;
            uint8_t chanCount = (uint8_t)msg[2]-0x30;

            if ((msg[5] == 'I') && (msg[6] == 'N')) {
                printf("Board %d is input-card with %d channels\n", boardId, chanCount);
            }else if ((msg[5] == 'O') && (msg[6] == 'U') && (msg[7] == 'T')) {
                printf("Board %d is output-card with %d channels\n", boardId, chanCount);
            }else{
                printf("Received message: %s\n", msg);
            }
        }

        lv_label_set_text_fmt(objects.debugtext, "Received Message: %s\n", msg);
    }
}

void fpgaCallback(char *buf, uint8_t len) {
    // do something with the received data
    // at the moment the FPGA is sending "*X32 #" 5 times per second
    // later it is planned to receive information about audio-levels here
    //printf("Received: %s\n", buf);
    //lv_label_set_text_fmt(objects.debugtext, "Fpga Message: %s\n", buf);
}

void x32log(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf(format, args);
    fflush(stdout); // immediately write to console!

    va_end(args);
}

void x32debug(const char *format, ...)
{
#if DEBUG == 1
    va_list args;
    va_start(args, format);

    printf(format, args);
    fflush(stdout); // immediately write to console!

    va_end(args);
#endif
}

// the main-function - of course
int main() {
    srand(time(NULL));
    x32log("OpenX32 UserInterface\n");
    x32log("v0.0.6, 17.08.2025\n");
    x32log("https://github.com/xn--nding-jua/OpenX32\n");

    x32log("Reading config...");
    char model[12];
    char serial[12];
    char date[16];
    readConfig("/etc/x32.conf", "MDL=", model, 12);
    readConfig("/etc/x32.conf", "SN=", serial, 12);
    readConfig("/etc/x32.conf", "DATE=", date, 16);
    if (strcmp(model, "X32Core") == 0) {
        initButtonDefinition(X32_MODEL_CORE);
    }else if (strcmp(model, "X32Rack") == 0) {
        initButtonDefinition(X32_MODEL_RACK);
    }else if (strcmp(model, "X32Producer") == 0) {
        initButtonDefinition(X32_MODEL_PRODUCER);
    }else if (strcmp(model, "X32C") == 0) {
        initButtonDefinition(X32_MODEL_COMPACT);
    }else if (strcmp(model, "X32") == 0) {
        initButtonDefinition(X32_MODEL_FULL);
    }else{
        x32log("ERROR: No model detected - assume X32 Fullsize!\n");
        initButtonDefinition(X32_MODEL_NONE);

        // (for development without inserted sd card)
        //
        //x32printf("ERROR: No model detected - assume X32 Fullsize!\n");
        //initButtonDefinition(X32_MODEL_FULL);
        //
        //x32printf("ERROR: No model detected - assume X32 Compact!\n");
        //initButtonDefinition(X32_MODEL_COMPACT);
    }
    x32log(" Detected model: %s with Serial %s built on %s\n", model, serial, date);

    //x32log("Connecting to UART1 (Debug)...");
    //uartOpen("/dev/ttymxc0", 115200, &fdDebug); // this UART is not accessible from the outside

    x32log("Connecting to UART2 (Surface)...\n");
    uartOpen("/dev/ttymxc1", 115200, &fdSurface); // this UART is connected to the surface (Fader, LEDs, LCDs, Buttons) directly

    x32log("Connecting to UART3 (ADDA-Boards)...\n");
    uartOpen("/dev/ttymxc2", 38400, &fdAdda); // this UART is connected to the FPGA and routed to the 8-channel AD/DA-boards

    x32log("Connecting to UART4 (FPGA)...\n");
    uartOpen("/dev/ttymxc3", 115200, &fdFpga); // this UART is connected to the FPGA

    //x32log("Connecting to UART5 (MIDI)...\n");
    //uartOpen("/dev/ttymxc4", 115200, &fdMidi); //

    x32log("Initializing X32 Surface...\n");
    //surfaceReset(); // resets all microcontrollers on the board (not necessary)
    //surfaceInit(); // initialize whole surface with default values
    surfaceDemo(); // sets demo-values for faders, leds and lcds

    x32log("Initializing X32 Audio...\n");
    addaInit(48000);
    //mixingDefaultRoutingConfig(); // set default routing configuration

    // **************** TESTING ****************
    // set gain for all local xlr-input-channels to +15.5dB for testing with microphone
    for (uint8_t ch=1; ch<=32; ch++) {
        mixingSetGain('x', ch, 15.5);
    }

    // set routing to DSP-channels 1-8 which is just an 8-channel tone-generator at the moment
    for (uint8_t ch=1; ch<=8; ch++) {
        mixingSetRouting('x', ch, mixingGetInputSource('d', ch)); // set xlr-output 1-8
        mixingSetRouting('x', ch+8, mixingGetInputSource('d', ch)); // set xlr-output 9-16

        mixingSetRouting('a', ch, mixingGetInputSource('d', ch)); // set aux-channel 1-8

        mixingSetRouting('c', ch, mixingGetInputSource('d', ch)); // set card-output 1-8
        mixingSetRouting('c', ch+8, mixingGetInputSource('d', ch)); // set card-output 9-16
        mixingSetRouting('c', ch+16, mixingGetInputSource('d', ch)); // set card-output 17-24
        mixingSetRouting('c', ch+24, mixingGetInputSource('d', ch)); // set card-output 25-32

        mixingSetRouting('p', ch, mixingGetInputSource('d', ch)); // set P16-output 1-8
        mixingSetRouting('p', ch+8, mixingGetInputSource('d', ch)); // set P16-output 9-16
    }
    // transmit routing-configuration to FPGA
    mixingTxRoutingConfig();

    // turn on Phantom-Power on channels 1, 10, 19 and 27 for testing (LED identifies board)
    mixingSetPhantomPower('x', 1+0, true); // first channel on board 1 (1-8)
    mixingSetPhantomPower('x', 9+1, true); // second channel on board 3 (9-16)
    mixingSetPhantomPower('x', 17+2, true); // third channel on board 0 (17-24)
    mixingSetPhantomPower('x', 25+3, true); // fourth channel on board 2 (25-32)

    // set X-LIVE-Card to USB-Routing
/*
    addaSendCmd("*8I#"); // identify card
    usleep(10000);
    addaSendCmd("*8R#"); // identify card
    usleep(10000);
*/
    // **************** END OF TESTING ****************

    // unmute the local audio-boards
    addaSetMute(false);

/*
    // only necessary if LVGL is not used
    x32printf("Starting Timer...\n");
    initTimer();

    x32printf("Wait for incoming data on /dev/ttymxc1...\n");
    x32printf("Press Ctrl+C to terminate program.\n");
    while (1) {
      // read data from surface and calls surfaceCallback()
      surfaceProcessUartData(uartRx(&fdSurface, &uartBufferSurface[0], sizeof(uartBufferSurface)));

      // read data from ADDA-Boards (8ch input/output)
      addaProcessUartData(uartRx(&fdAdda, &uartBufferAdda[0], sizeof(uartBufferAdda)));

      // read data from FPGA
      fpgaProcessUartData(uartRx(&fdFpga, &uartBufferFpga[0], sizeof(uartBufferFpga)));

      // sleep for 1ms to lower CPU-load
      usleep(1000);
    }
*/

    x32log("Initializing GUI...\n");
    guiInit(); // initializes LVGL, FBDEV and starts endless loop

    return 0;
}
