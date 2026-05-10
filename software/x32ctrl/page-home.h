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

        uint channelindex[6] = {0, 1, 2, 3, 4, 5};


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

        void OnShow() override 
        {
            EncoderBind_NormalMode();
        }

        void EncoderBind_NormalMode()
        {
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_1, MixerparameterAction::CHANGE, CHANNEL_VOLUME, channelindex[0]);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_2, MixerparameterAction::CHANGE, CHANNEL_VOLUME, channelindex[1]);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_3, MixerparameterAction::CHANGE, CHANNEL_VOLUME, channelindex[2]);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_4, MixerparameterAction::CHANGE, CHANNEL_VOLUME, channelindex[3]);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_5, MixerparameterAction::CHANGE, CHANNEL_VOLUME, channelindex[4]);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_6, MixerparameterAction::CHANGE, CHANNEL_VOLUME, channelindex[5]);

            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_1, MixerparameterAction::TOGGLE, CHANNEL_MUTE, channelindex[0]);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_2, MixerparameterAction::TOGGLE, CHANNEL_MUTE, channelindex[1]);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_3, MixerparameterAction::TOGGLE, CHANNEL_MUTE, channelindex[2]);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_4, MixerparameterAction::TOGGLE, CHANNEL_MUTE, channelindex[3]);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_5, MixerparameterAction::TOGGLE, CHANNEL_MUTE, channelindex[4]);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_6, MixerparameterAction::TOGGLE, CHANNEL_MUTE, channelindex[5]);
        }

        void EncoderBind_EditMode()
        {
            String edittext = String("\n\nSelect ") + String(LV_SYMBOL_REFRESH);

            // Show the Channelname
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_1, config->GetString(CHANNEL_NAME, channelindex[0]) + edittext);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_2, config->GetString(CHANNEL_NAME, channelindex[1]) + edittext);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_3, config->GetString(CHANNEL_NAME, channelindex[2]) + edittext);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_4, config->GetString(CHANNEL_NAME, channelindex[3]) + edittext);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_5, config->GetString(CHANNEL_NAME, channelindex[4]) + edittext);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_6, config->GetString(CHANNEL_NAME, channelindex[5]) + edittext);

            // Button has no function in this mode
            config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_1);
            config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_2);
            config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_3);
            config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_4);
            config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_5);
            config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_6);
        }

        void OnChange(bool force_update) override
        {
            if (config->HasParameterChanged(CHANNEL_NAME) || force_update)
            {
                lv_label_set_text(objects.home_ch_1, config->GetString(CHANNEL_NAME, channelindex[0]).c_str());
                lv_label_set_text(objects.home_ch_2, config->GetString(CHANNEL_NAME, channelindex[1]).c_str());
                lv_label_set_text(objects.home_ch_3, config->GetString(CHANNEL_NAME, channelindex[2]).c_str());
                lv_label_set_text(objects.home_ch_4, config->GetString(CHANNEL_NAME, channelindex[3]).c_str());
                lv_label_set_text(objects.home_ch_5, config->GetString(CHANNEL_NAME, channelindex[4]).c_str());
                lv_label_set_text(objects.home_ch_6, config->GetString(CHANNEL_NAME, channelindex[5]).c_str());
            }

            if (config->HasParameterChanged(DISPLAY_UTILITY))
            {
                if (config->GetBool(DISPLAY_UTILITY))
                {
                    EncoderBind_EditMode();
                }
                else
                {
                    EncoderBind_NormalMode();
                }
            }
        }

        void OnChangeCustomEncoder(SurfaceElementId surface_element_id, int amount)
        {
            switch(surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_1:
                case SurfaceElementId::DISPLAY_ENCODER_2:
                case SurfaceElementId::DISPLAY_ENCODER_3:
                case SurfaceElementId::DISPLAY_ENCODER_4:
                case SurfaceElementId::DISPLAY_ENCODER_5:
                case SurfaceElementId::DISPLAY_ENCODER_6:
                    {
                        uint array_index = (uint)surface_element_id - (uint)SurfaceElementId::DISPLAY_ENCODER_1;
                        channelindex[array_index] = helper->CheckBoundaries(channelindex[array_index], amount, 0, MAX_VCHANNELS-1);

                        EncoderBind_EditMode();
                        OnChange(true);
                    }
                    break;
                default:
                    break;
            }
        }

        void OnUpdateMeters() override
        {
            for (uint i = 0; i < MAX_DISPLAY_ENCODER; i++)
            {
                float dbValue = helper->sample2Dbfs(mixer->dsp->rChannel[channelindex[i]].meterDecay);
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