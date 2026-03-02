#pragma once
#include "page.h"
using namespace std;

class PageHome : public Page
{
    using enum MP_ID;

    public:

        PageHome(PageBaseParameter* pagebasepar) : Page(pagebasepar)
        {
            nextPage = X32_PAGE::CONFIG;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 0;
            led = X32_BTN_HOME;
        }

        void OnInit() override 
        {
            BindEncoder(DISPLAY_ENCODER_1, CHANNEL_VOLUME, CHANNEL_MUTE, 0);
            BindEncoder(DISPLAY_ENCODER_2, CHANNEL_VOLUME, CHANNEL_MUTE, 1);
            BindEncoder(DISPLAY_ENCODER_3, CHANNEL_VOLUME, CHANNEL_MUTE, 2);
            BindEncoder(DISPLAY_ENCODER_4, CHANNEL_VOLUME, CHANNEL_MUTE, 3);
            BindEncoder(DISPLAY_ENCODER_5, CHANNEL_VOLUME, CHANNEL_MUTE, 4);
            BindEncoder(DISPLAY_ENCODER_6, CHANNEL_VOLUME, CHANNEL_MUTE, 5);
        }

        void OnChange(bool force_update) override
        {
            if (config->HasParameterChanged(CHANNEL_NAME) || force_update)
            {
                lv_label_set_text(objects.home_ch_1, config->GetString(CHANNEL_NAME, 0).c_str());
                lv_label_set_text(objects.home_ch_2, config->GetString(CHANNEL_NAME, 1).c_str());
                lv_label_set_text(objects.home_ch_3, config->GetString(CHANNEL_NAME, 2).c_str());
                lv_label_set_text(objects.home_ch_4, config->GetString(CHANNEL_NAME, 3).c_str());
                lv_label_set_text(objects.home_ch_5, config->GetString(CHANNEL_NAME, 4).c_str());
                lv_label_set_text(objects.home_ch_6, config->GetString(CHANNEL_NAME, 5).c_str());
            }
        }
};