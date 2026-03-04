#pragma once

#include "page.h"
using namespace std;

class PageConfig : public Page
{
    public:

        PageConfig(PageBaseParameter* pagebasepar) : Page(pagebasepar)
        {
            prevPage = X32_PAGE::HOME;
            nextPage = X32_PAGE::GATE;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 1;
            led = X32_BTN_VIEW_CONFIG;
            noLedOnRack = true;
        }

        void OnInit() override
        { 
            UpdateEncoderBinding(config->GetUint(MP_ID::SELECTED_CHANNEL));
        }

        void UpdateEncoderBinding(uint targetindex)
        {
            using enum MP_ID;

            BindEncoder(DISPLAY_ENCODER_1, SELECTED_CHANNEL);
            BindEncoder(DISPLAY_ENCODER_2, CHANNEL_GAIN, CHANNEL_PHANTOM, targetindex);
            BindEncoder(DISPLAY_ENCODER_3, ROUTING_DSP_INPUT, CHANNEL_PHASE_INVERT, targetindex);
            BindEncoder(DISPLAY_ENCODER_5, CHANNEL_VOLUME, CHANNEL_MUTE, targetindex);
            BindEncoder(DISPLAY_ENCODER_6, CHANNEL_PANORAMA, targetindex);

            SyncEncoderWidgets(true);
        }

        void OnChange(bool force_update) override
        {

            using enum MP_ID;

            uint8_t chanIndex = config->GetUint(MP_ID::SELECTED_CHANNEL);

            if (config->HasParameterChanged(SELECTED_CHANNEL))
            {
                force_update = true;
                UpdateEncoderBinding(chanIndex);
            }

            if (config->HasParameterChanged(CHANNEL_GAIN, chanIndex) || force_update)
            {
                lv_label_set_text(objects.current_channel_gain, config->GetParameter(CHANNEL_GAIN)->GetFormatedValue(chanIndex).c_str());
                lv_image_set_rotation(objects.config_gain_knob, helper->rescale(config->GetParameter(CHANNEL_GAIN)->GetFloat(chanIndex), -12.0f, 45.5f, 0.0f, 2900.0f));
            }

            if (config->HasParameterChanged(CHANNEL_PHANTOM, chanIndex) || force_update)
            {
                lv_image_set_offset_x(objects.config_phantom_button, config->GetUint(CHANNEL_PHANTOM, chanIndex) * -lv_obj_get_width(objects.config_phantom_button));
                lv_image_set_offset_x(objects.config_phantom_checkbox, config->GetUint(CHANNEL_PHANTOM, chanIndex) * -lv_obj_get_width(objects.config_phantom_checkbox));
            }

            if (config->HasParameterChanged(CHANNEL_PHASE_INVERT, chanIndex) || force_update)
            {
                lv_image_set_offset_x(objects.config_phase_checkbox, config->GetUint(CHANNEL_PHASE_INVERT, chanIndex) * -lv_obj_get_width(objects.config_phase_checkbox));
            }

            if (config->HasParameterChanged(CHANNEL_PANORAMA, chanIndex) || force_update)
            {
                lv_label_set_text(objects.current_channel_pan_bal, config->GetParameter(CHANNEL_PANORAMA)->GetFormatedValue(chanIndex).c_str());
                lv_image_set_rotation(objects.config_pan_knob, helper->rescale(config->GetFloat(CHANNEL_PANORAMA, chanIndex), -100.0f, 100.0f, 0.0f, 2900.0f));
            }

            if (config->HasParameterChanged(CHANNEL_VOLUME, chanIndex) || force_update)
            {
                lv_label_set_text(objects.current_channel_volume, config->GetParameter(CHANNEL_VOLUME)->GetFormatedValue(chanIndex).c_str());
                lv_image_set_rotation(objects.config_volume_knob, helper->rescale(config->GetFloat(CHANNEL_VOLUME, chanIndex), -100.0f, 10.0f, 0.0f, 2900.0f));

                uint imageOffset = helper->rescale(config->GetFloat(CHANNEL_VOLUME, chanIndex), -100.0f, 10.0f, 0.0f, 18.0f);
                lv_image_set_offset_x(objects.openx32_demo_knob, imageOffset * -lv_obj_get_width(objects.openx32_demo_knob));
            }

            if (config->HasParameterChanged(CHANNEL_MUTE, chanIndex) || force_update)
            {
                lv_image_set_offset_x(objects.config_mute_checkbox, config->GetUint(CHANNEL_MUTE, chanIndex) * -lv_obj_get_width(objects.config_mute_checkbox));
            }
        }

        void OnUpdateMeters() override
        {
            float dbValue = helper->sample2Dbfs(mixer->dsp->rChannel[config->GetUint(MP_ID::SELECTED_CHANNEL)].meterDecay);
            uint imageOffset = helper->rescale(dbValue, -100.0f, 10.0f, 0.0f, 31.0f);
            lv_image_set_offset_x(objects.config_vumeter, imageOffset * -lv_obj_get_width(objects.config_vumeter));
        }
};