#pragma once
#include "page.h"
using namespace std;

class PageSetup: public Page
{
    using enum MP_ID;

    public:
        PageSetup(PageBaseParameter* pagebasepar) : Page(pagebasepar)
        {
            nextPage = X32_PAGE::SETUP_CARD;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 0;
            led = X32_BTN_SETUP;
        }

        void OnInit() override
        {
            BindEncoder(DISPLAY_ENCODER_4, LED_BRIGHTNESS);
            BindEncoder(DISPLAY_ENCODER_5, CHANNEL_LCD_MODE);
            BindEncoder(DISPLAY_ENCODER_6, LCD_CONTRAST);
        }

        void OnChange(bool forceUpdate)
        {
            if (config->HasParameterChanged(SELECTED_CHANNEL) || forceUpdate)
            {
                BindEncoder(DISPLAY_ENCODER_2, CHANNEL_COLOR, CHANNEL_COLOR_INVERTED, config->GetUint(SELECTED_CHANNEL));
            }
        }
};