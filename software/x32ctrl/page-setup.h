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

            String versioninfo;
            if (mixer->adda->HasXlrIn0()) {
                versioninfo += "IN0: " + mixer->adda->GetXlrIn0().substring(2, mixer->adda->GetXlrIn0().length() - 1) + "\n";
            }
            if (mixer->adda->HasXlrIn1()) {
                versioninfo += "IN1: " + mixer->adda->GetXlrIn1().substring(2, mixer->adda->GetXlrIn1().length() - 1) + "\n";
            }
            if (mixer->adda->HasXlrIn2()) {
                versioninfo += "IN2: " + mixer->adda->GetXlrIn2().substring(2, mixer->adda->GetXlrIn2().length() - 1) + "\n";
            }
            if (mixer->adda->HasXlrIn3()) {
                versioninfo += "IN3: " + mixer->adda->GetXlrIn3().substring(2, mixer->adda->GetXlrIn3().length() - 1) + "\n";
            }
            if (mixer->adda->HasXlrOut0()) {
                versioninfo += "OUT0: " + mixer->adda->GetXlrOut0().substring(2, mixer->adda->GetXlrOut0().length() - 1) + "\n";
            }
            if (mixer->adda->HasXlrOut1()) {
                versioninfo += "OUT1: " + mixer->adda->GetXlrOut1().substring(2, mixer->adda->GetXlrOut1().length() - 1) + "\n";
            }
            if (mixer->adda->HasExpansion()) {
                versioninfo += "CARD: " + mixer->adda->GetExpansion().substring(2, mixer->adda->GetExpansion().length() - 1) + "\n";
            }
            lv_label_set_text_fmt(objects.setup_versioninfotext, "%s", versioninfo.c_str());
        }

        void OnChange(bool forceUpdate)
        {
            if (config->HasParameterChanged(SELECTED_CHANNEL) || forceUpdate)
            {
                BindEncoder(DISPLAY_ENCODER_2, CHANNEL_COLOR, CHANNEL_COLOR_INVERTED, config->GetUint(SELECTED_CHANNEL));
            }
        }
};