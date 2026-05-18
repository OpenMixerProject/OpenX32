#pragma once

#include "page.h"
using namespace std;

class PageConfig : public Page
{
    private:
        
        uint lastImageOffset;

        lv_obj_t* config_mute_group[MUTE_GROUPS];
        lv_obj_t* config_dca_group[8];

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
            config_mute_group[0] = objects.config_mute_group_1;
            config_mute_group[1] = objects.config_mute_group_2;
            config_mute_group[2] = objects.config_mute_group_3;
            config_mute_group[3] = objects.config_mute_group_4;
            config_mute_group[4] = objects.config_mute_group_5;
            config_mute_group[5] = objects.config_mute_group_6;

            config_dca_group[0] = objects.config_dca_group_1;
            config_dca_group[1] = objects.config_dca_group_2;
            config_dca_group[2] = objects.config_dca_group_3;
            config_dca_group[3] = objects.config_dca_group_4;
            config_dca_group[4] = objects.config_dca_group_5;
            config_dca_group[5] = objects.config_dca_group_6;
            config_dca_group[6] = objects.config_dca_group_7;
            config_dca_group[7] = objects.config_dca_group_8;
        }

        void OnShow() override 
        {
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_1, MixerparameterAction::CHANGE, SELECTED_CHANNEL);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_2, MixerparameterAction::CHANGE_SELECTED_CHANNEL, CHANNEL_GAIN);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_2, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_PHANTOM);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_3, MixerparameterAction::CHANGE_SELECTED_CHANNEL, ROUTING_DSP_INPUT);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_3, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_PHASE_INVERT);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_4, MixerparameterAction::CHANGE_SELECTED_CHANNEL, DELAY_DSP_INPUT);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_4, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_SOLO);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_5, MixerparameterAction::CHANGE_SELECTED_CHANNEL, CHANNEL_VOLUME);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_5, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_MUTE);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_6, MixerparameterAction::CHANGE_SELECTED_CHANNEL, CHANNEL_PANORAMA);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_6, MixerparameterAction::RESET_SELECTED_CHANNEL, CHANNEL_PANORAMA);
        }

        void OnChange(bool force_update) override
        {

            using enum MP_ID;

            uint8_t chanIndex = config->GetUint(MP_ID::SELECTED_CHANNEL);

            if (config->HasParameterChanged(SELECTED_CHANNEL))
            {
                force_update = true;
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

            for (uint i = 0; i < MUTE_GROUPS; i++)
            {
                if (config->HasParameterChanged(config->MpCalcId(MUTE_GROUP_1, i), chanIndex) || force_update)
                {
                    lv_obj_set_state(config_mute_group[i], LV_STATE_CHECKED, config->GetBool(config->MpCalcId(MUTE_GROUP_1, i), chanIndex));
                }
            }

            for (uint i = 0; i < 8; i++)
            {
                if (config->HasParameterChanged(config->MpCalcId(DCA_GROUP_1, i), chanIndex) || force_update)
                {
                    lv_obj_set_state(config_dca_group[i], LV_STATE_CHECKED, config->GetBool(config->MpCalcId(DCA_GROUP_1, i), chanIndex));
                }
            }

        }

        

        void OnUpdateMeters() override
        {
            float dbValue = helper->sample2Dbfs(mixer->dsp->rChannel[config->GetUint(MP_ID::SELECTED_CHANNEL)].meterDecay);
            uint imageOffset = helper->rescale(dbValue, -100.0f, 10.0f, 0.0f, 31.0f);
            uint newImageOffset = imageOffset * -lv_obj_get_width(objects.config_vumeter);

            // only set new offset if it has changed
            if (newImageOffset != lastImageOffset)
            {
                lv_image_set_offset_x(objects.config_vumeter, newImageOffset);
                lastImageOffset = newImageOffset;
            }
        }
};