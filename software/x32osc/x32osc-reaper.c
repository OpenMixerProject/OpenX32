/*
  Control software for Behringer X32 using OpenX32
  https://github.com/xn--nding-jua/OpenX32
  
  x32osc-reaper: This software connects the X32 to Reaper via OSC.

  Parts of this software with kind permission of Music Tribe. Thank you!
*/

#include "x32osc-reaper.h"


// the main-function - of course
int main(int argc, char *argv[]) {
    srand(time(NULL));
    printf("OpenX32 OSC Client for Reaper\n");
    printf("v0.0.2, 27.07.2025\n");
    printf("https://github.com/xn--nding-jua/OpenX32\n");

    if (argc != 2) {
        printf("Error: Wrong parameter!\n");
        printf("Usage: %s <IP-Address of Reaper>\n", argv[0]);
    }
    else
    {
      printf("Connecting to UART1...");
      fflush(stdout);
      uartOpen();
      printf(" done\n");
      fflush(stdout);

      printf("Initializing OSC...");
      fflush(stdout);
      oscInit(argv[1]);
      printf(" done\n");
      fflush(stdout);

      printf("Initializing X32 Surface...");
      fflush(stdout);
      surfaceReset(); // resets all microcontrollers on the board
      surfaceInit(); // sets default values for faders, leds and lcds
      printf(" done\n");
      fflush(stdout);

      // DAW Remote Button on
      setLedByNr(X32_BTN_DAW_REMOTE, 1);

      printf("Press Ctrl+C to terminate program.\n");
      fflush(stdout);

      // Trigger Reaper to send all current values
      oscSend("/action/41743", "f", 1);

      while (1) {
        uartRead();

        // OSC
        oscRun();

        // sleep for 1ms to lower CPU-load
        usleep(1000);
      }
    }

    return 0;
}


void setBrightnessAll(int value) {
    setBrightness(X32_BOARD_MAIN, value);
    setBrightness(X32_BOARD_L, value);
#ifdef X32_MODELL_FULL
    setBrightness(X32_BOARD_M, value);
#endif
    setBrightness(X32_BOARD_R, value);
}

void surfaceInit() {
	  
    setBrightness(0, X32_BRIGHTNESS_4); // brightness of LEDs
    setBrightness(1, X32_BRIGHTNESS_4);
    setBrightness(X32_BOARD_L, X32_BRIGHTNESS_4);
#ifdef X32_MODELL_FULL
    setBrightness(X32_BOARD_MAIN, X32_BRIGHTNESS_4);
#endif
    setBrightness(X32_BOARD_R, X32_BRIGHTNESS_4);

    setContrast(0, 0x24); // contrast of LCDs
    setContrast(X32_BOARD_L, 0x24);
#ifdef X32_MODELL_FULL
    setContrast(X32_BOARD_MAIN, 0x24);
#endif
    setContrast(X32_BOARD_R, 0x24);

    // move all faders to 0
    for (uint8_t i=0; i<=7; i++) {
      setFader(X32_BOARD_L, i, 0x000);
#ifdef X32_MODEL_FULL
      setFader(X32_BOARD_MAIN, i, 0x555 + 0xAA*i);
#endif
      setFader(X32_BOARD_R, i, 0x000);
    }
    setFader(X32_BOARD_R, 8, 0x000); // Master
}

void surfaceLedReset()
{
    // Buttons und LEDs Main ausschalten
    for (int i=0; i<=50; i++) {
      setLed(X32_BOARD_MAIN, i, 0); 
    }

    // Meters
    for (uint8_t i=0; i<=7; i++) {
        setMeterLed(X32_BOARD_L, i, 0b00000000);
        setMeterLed(X32_BOARD_R, i, 0b00000000);
    }
    setMeterLed(X32_BOARD_R, 8, 0b00000000);

    for (uint8_t i=0; i<=7; i++) {
      // select-LEDs
      setLed(X32_BOARD_L, 0x20+i, 0); // boardId 4 = faderL
      setLed(X32_BOARD_R, 0x20+i, 0); // boardId 8 = faderR

      // solo leds
      setLed(X32_BOARD_L, 0x30+i, 0); // boardId 4 = faderL
      setLed(X32_BOARD_R, 0x30+i, 0); // boardId 8 = faderR

      // mute leds
      setLed(X32_BOARD_L, 0x40+i, 0); // boardId 4 = faderL
      setLed(X32_BOARD_R, 0x40+i, 0); // boardId 8 = faderR
    }

    // Faders
    for (uint8_t i=0; i<7; i++) {
      setFader(X32_BOARD_L, i, 0);
      setFader(X32_BOARD_R, i, 0);
    }

    // MAIN Fader
    setFader(X32_BOARD_R, 8, 0);
    setLedByNr(X32_BTN_MAIN_SELECT, 0);
    setLedByNr(X32_BTN_MAIN_SOLO, 0); 
    setLedByNr(X32_BTN_MAIN_MUTE, 0);



    
}

// function will be called by uart-receive-function
void surfaceCallback(uint8_t boardId, uint8_t class, uint8_t index, uint16_t value) {
if (class == 'f') {
      float pct = value / 40.95; // convert to percent
      float reaperFaderPos = (pct * 1)/100;

      uint8_t trackIndex = index + 1;

#ifdef X32_MODEL_FULL
      if (boardId == X32_BOARD_M)
      {
        trackIndex += 8;
      } 
      else if (boardId == X32_BOARD_L)
      {
        trackIndex += 16;
      }
#endif
#ifdef X32_MODEL_COMPACT
      if (boardId == X32_BOARD_R)
      {
        trackIndex += 8;
      }
#endif


      printf("Fader   : boardId = 0x%02X | idx = %d | data = 0x%04X = %f, ReaperFaderPos = %f\n", boardId, trackIndex, value, pct, reaperFaderPos);
  
      char* string;
      asprintf(&string, "/track/%d/volume", trackIndex);

      oscSend(string, "f", reaperFaderPos);

  }else if (class == 'b') {
      	    
      bool pressed = (value >> 7);
      int buttonIndex = (uint16_t)(value & 0x7F);
      int buttonNr = ((uint16_t)boardId << 8) + buttonIndex;
      

      // register buttons
      if (pressed){

        printf("Button boardId=0x%02X class=0x%02X index=0x%02X data=0x%02X pressed=%d index=0x%02X number=0x%02X\n", boardId, class, index, value, pressed, buttonIndex, buttonNr);

        switch (buttonNr) {
          case X32_BTN_UP:
            for (uint32_t ii = 0; ii<= 0xFFFFFF; ii=(ii<<1) + 1 )
            {
              setMeterLedMain(0b00000011, 0b00001111, ii, ii, ii);
              usleep(20000);
            }
            break;
          case X32_BTN_DOWN:
            surfaceReset();
            surfaceInit();
            break;
          case X32_BTN_CLEAR_SOLO:
            oscSend("/anysolo", "f", 1);
            break;
          default:
              if (boardId == X32_BOARD_L || boardId == X32_BOARD_R ){
                char* action;
                int trackIndex;

                if (buttonIndex >= 0x20 && buttonIndex <= 0x27)
                {
                  action  = "select";
                  trackIndex = buttonIndex - 0x20;
                } 
                else if (buttonIndex >= 0x30 && buttonIndex <= 0x37)
                {
                  action  = "solo";
                  trackIndex = buttonIndex - 0x30;
                }
                else if (buttonIndex >= 0x40 && buttonIndex <= 0x47)
                {
                  action  = "mute";
                  trackIndex = buttonIndex - 0x40;
                }
                else 
                {
                  break;
                }

#ifdef X32_MODEL_FULL
                if (boardId == X32_BOARD_M)
                {
                  trackIndex += 8;
                } 
                else if (boardId == X32_BOARD_L)
                {
                  trackIndex += 16;
                }
#endif
#ifdef X32_MODEL_COMPACT
                if (boardId == X32_BOARD_R)
                {
                  trackIndex += 8;
                }
#endif


                char* string;
                asprintf(&string, "/track/%d/%s/toggle", trackIndex + 1, action);

                oscSend(string, "f", 1);
              }
            break;
        }

        
        

      }
  }else if (class == 'e') {
    printf("Encoder : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
  } else 
  {
    printf("Unbekannt : boardId = 0x%02X | class = 0x%02X | index = 0x%02X | data = 0x%02X\n", boardId, class, index, value);
  }
}

uint32_t masterL  = 0;
uint32_t masterR  = 0;
uint32_t masterSolo  = 0;

void processMessage(tosc_message *osc) {
    char* oscString = tosc_getAddress(osc);

    char* marker = osc->marker;
    

    fflush(stdout);

    // "/track/1/solo"
    //  0123456789012

    // "/track/12/solo"
    //  01234567890123

    if (StartsWith(oscString, "/track/")) {
      
      char numberstr[3] = "";
      for (int i=0; i<2; i++)
      {
        if (oscString[7 + i] == '/')
        {
          break;
        }

        numberstr[i] = oscString[7+i];
      }
      int tracknumber = atoi(numberstr);
      int trackIndex = tracknumber - 1;
      int oscStringNumberOffset = 8 + strlen(numberstr);

      uint8_t board = X32_BOARD_L;
      if (tracknumber >= 9 && tracknumber <=16)
      {
        trackIndex = tracknumber - 8 -1;

#ifdef X32_MODEL_FULL
        board = X32_BOARD_M;
#endif
#ifdef X32_MODEL_COMPACT
        board = X32_BOARD_R;
#endif
      } else if (tracknumber >= 17 && tracknumber <=24)
     {
        trackIndex = tracknumber - 16;

#ifdef X32_MODEL_FULL
        board = X32_BOARD_L;
#endif
#ifdef X32_MODEL_COMPACT
        board = 0xFF; // TODO: implement nothing
#endif 
     }

      if (strcmp(oscString + oscStringNumberOffset, "mute") == 0) 
      {
        setLedByNr((uint16_t)(board << 8) + 0x40 + trackIndex, tosc_getNextFloat(osc));
      } 
      else if (strcmp(oscString + oscStringNumberOffset, "solo") == 0) 
      {
        setLedByNr(((uint16_t)board << 8) + 0x30 + trackIndex, tosc_getNextFloat(osc));
      }
      else if (strcmp(oscString + oscStringNumberOffset, "select") == 0) 
      {
        setLedByNr((uint16_t)(board << 8) + 0x20 + trackIndex, tosc_getNextFloat(osc));
      } 
      else if (strcmp(oscString + oscStringNumberOffset, "name") == 0) 
      {
        setLcd(board, trackIndex, 7, 70, 8, 0xE2, 0x20, 0, 0, tosc_getNextString(osc), 0x00, 0, 48, "Track");
      } 
      else if (strcmp(oscString + oscStringNumberOffset, "volume") == 0) 
      {
        setFader(board, trackIndex, (uint16_t)(tosc_getNextFloat(osc) * 0xFFF));
      }
      else if (strcmp(oscString + oscStringNumberOffset, "vu") == 0) 
      {
        float vu = tosc_getNextFloat(osc);
        int8_t bits = 0;

        // leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
        // 0b10000000 //comp
        // 0b01000000 //gate/pre
        // 0b00100000 //clip == 0db
        // 0b00010000 //-6db
        // 0b00001000 //-12
        // 0b00000100 //-18
        // 0b00000010 //-30
        // 0b00000001 //-60db

        if (vu >= 0.90)
        {
            bits = 0b00111111;
        } 
        else if (vu >= 0.82)
        {
            bits = 0b00011111; // -6db
        }
        else if (vu >= 0.7)
        {
            bits = 0b00001111; // -12db
        }
        else if (vu >= 0.64)
        {
            bits = 0b00000111; // -18db
        }
        else if (vu >= 0.47)
        {
            bits = 0b00000011; // -30db
        }
        else if (vu >= 0.3)
        {
            bits = 0b00000001; // -60db
        }
        else 
        {
            bits = 0b00000000;
        }

        setMeterLed(board, trackIndex,  bits);
      }
    }
    else if (strcmp(oscString, "/master/vu/L") == 0) 
    {
        float vu = tosc_getNextFloat(osc);
        
        // clip
        if (vu == 1.0)
        {
          masterL = 0xFFFFFF;
        }
        else
        {
          masterL = 0;
          for (int i = 0; i < (uint32_t)(24*vu); i++)
          {
            masterL = (masterL << 1) + 1;
          }
        }
    } 
    else if (strcmp(oscString, "/master/vu/R") == 0) 
    {
        float vu = tosc_getNextFloat(osc);

        // clip
        if (vu == 1.0)
        {
          masterR = 0xFFFFFF;
        }
        else
        {
          masterR = 0;
          for (int i = 0; i < (uint32_t)(24*vu); i++)
          {
            masterR = (masterR << 1) + 1;
          }
        }
    }
    else if (strcmp(oscString, "/anysolo") == 0) 
    {
        setLedByNr(X32_BTN_CLEAR_SOLO, tosc_getNextFloat(osc));
    }
    else if (strcmp(oscString, "/play") == 0) 
    {
        setLedByNr(X32_BTN_SCENES_GO, tosc_getNextFloat(osc));
    }
    else if (strcmp(oscString, "/stop") == 0) 
    {
        setLedByNr(X32_BTN_SCENES_UNDO, tosc_getNextFloat(osc));
    }
    else if (strcmp(oscString, "/pause") == 0) 
    {
        setLedByNr(X32_BTN_SCENES_PREV, tosc_getNextFloat(osc));
    }
    else
    {
      osc->marker = marker;
      tosc_printMessage(osc);
    
      //printf("board: 0x%02X, tracknumber: %d, trackindex: %d, numberstr: %s\n",board, tracknumber, trackIndex, numberstr);
    }

    if (StartsWith(oscString, "/master/vu"))
    {
      setMeterLedMain(0, 0, masterL, masterR, masterSolo);
    }

    fflush(stdout);
} 


