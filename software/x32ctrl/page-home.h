#pragma once
#include "page.h"
using namespace std;

class PageHome : public Page
{
    using enum MP_ID;

    private:

        lv_obj_t* vumeters[MAX_DISPLAY_ENCODER] =
        {
            objects.home_vumeter_1,
            objects.home_vumeter_2,
            objects.home_vumeter_3,
            objects.home_vumeter_4,
            objects.home_vumeter_5,
            objects.home_vumeter_6
        };

        uint lastImageOffset[MAX_DISPLAY_ENCODER] = {0, 0, 0, 0, 0, 0};


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

        void OnUpdateMeters() override
        {
            for (uint i = 0; i < MAX_DISPLAY_ENCODER; i++)
            {
                float dbValue = helper->sample2Dbfs(mixer->dsp->rChannel[i].meterDecay);
                uint imageOffset = helper->rescale(dbValue, -100.0f, 10.0f, 0.0f, 31.0f);
                uint newImageOffset = imageOffset * -lv_obj_get_width(vumeters[i]);
             
                // only set new offset if it has changed
                if (newImageOffset != lastImageOffset[i])
                {
                    lv_image_set_offset_x(vumeters[i], newImageOffset);
                    lastImageOffset[i] = newImageOffset;
                }
            }
        }
};