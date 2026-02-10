#pragma once
#include "page.h"
using namespace std;

class PageSetup: public Page {
    public:
        PageSetup(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            nextPage = X32_PAGE::SETUP_CARD;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 0;
            led = X32_BTN_SETUP;
        }

        void OnInit() override {
            BindEncoder(DISPLAY_ENCODER_5, false, MP_ID::LED_BRIGHTNESS);
            BindEncoder(DISPLAY_ENCODER_6, false, MP_ID::LCD_CONTRAST);
        }
};