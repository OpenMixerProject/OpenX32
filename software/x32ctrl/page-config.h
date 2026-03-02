#pragma once

#include "page.h"
using namespace std;

class PageConfig : public Page {
    public:
        PageConfig(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::HOME;
            nextPage = X32_PAGE::GATE;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 1;
            led = X32_BTN_VIEW_CONFIG;
            noLedOnRack = true;
        }

        void OnInit() override { 
            UpdateEncoderBinding(config->GetUint(MP_ID::SELECTED_CHANNEL));
        }

        void UpdateEncoderBinding(uint targetindex)
        {
            using enum MP_ID;

            BindEncoder(DISPLAY_ENCODER_1, ROUTING_DSP_INPUT, CHANNEL_PHASE_INVERT, targetindex);
            BindEncoder(DISPLAY_ENCODER_2, CHANNEL_GAIN, CHANNEL_PHANTOM, targetindex);
            BindEncoder(DISPLAY_ENCODER_3, CHANNEL_PANORAMA, targetindex);
            BindEncoder(DISPLAY_ENCODER_4, CHANNEL_VOLUME, CHANNEL_MUTE, targetindex);
            BindEncoder(DISPLAY_ENCODER_6, SELECTED_CHANNEL);

            SyncEncoderWidgets(true);
        }

        void OnChange(bool force_update) override {

            using enum MP_ID;

            uint8_t chanIndex = config->GetUint(MP_ID::SELECTED_CHANNEL);

            if (config->HasParameterChanged(SELECTED_CHANNEL))
            {
                force_update = true;
                UpdateEncoderBinding(chanIndex);
            }

            // TODO implement with better string handling -> (*** stack smashing detected ***: terminated)
            // char dspSourceName[5] = "";
            // mixer->dsp->GetSourceName(&dspSourceName[0], chanIndex, mixer->fpga->fpgaRouting.dsp[mixer->dsp->Channel[chanIndex].inputSource - 1]);
            // lv_label_set_text_fmt(objects.current_channel_source, "%02d: %s", (chanIndex + 1), dspSourceName);

            if (config->HasParameterChanged(CHANNEL_GAIN, chanIndex) || force_update)
            {
                lv_label_set_text(objects.current_channel_gain, config->GetParameter(CHANNEL_GAIN)->GetLabelAndValue(chanIndex).c_str());
                lv_image_set_rotation(objects.config_gain_knob, helper->rescale(config->GetParameter(CHANNEL_GAIN)->GetFloat(chanIndex), -12.0f, 45.5f, 0.0f, 2900.0f));
            }

            if (config->HasParameterChanged(CHANNEL_PHANTOM, chanIndex) || force_update)
            {
                lv_label_set_text(objects.current_channel_phantom, config->GetParameter(CHANNEL_PHANTOM)->GetLabelAndValue(chanIndex).c_str());
            }

            if (config->HasParameterChanged(CHANNEL_PHASE_INVERT, chanIndex) || force_update)
            {
                lv_label_set_text(objects.current_channel_invert, config->GetParameter(CHANNEL_PHASE_INVERT)->GetLabelAndValue(chanIndex).c_str());
            }

            if (config->HasParameterChanged(CHANNEL_PANORAMA, chanIndex) || force_update)
            {
                lv_label_set_text(objects.current_channel_pan_bal, config->GetParameter(CHANNEL_PANORAMA)->GetLabelAndValue(chanIndex).c_str());
                lv_image_set_rotation(objects.config_pan_knob, helper->rescale(config->GetParameter(CHANNEL_PANORAMA)->GetFloat(chanIndex), -100.0f, 100.0f, 0.0f, 2900.0f));
            }

            if (config->HasParameterChanged(CHANNEL_VOLUME, chanIndex) || force_update)
            {
                lv_label_set_text(objects.current_channel_volume, config->GetParameter(CHANNEL_VOLUME)->GetLabelAndValue(chanIndex).c_str());
                lv_image_set_rotation(objects.config_volume_knob, helper->rescale(config->GetParameter(CHANNEL_VOLUME)->GetFloat(chanIndex), -100.0f, 10.0f, 0.0f, 2900.0f));

                uint volumeTmp = helper->rescale(config->GetParameter(CHANNEL_VOLUME)->GetFloat(chanIndex), -100.0f, 10.0f, 0.0f, 7.0f);
                switch (volumeTmp) {
                    case 0:
                        lv_img_set_src(objects.config_volume_knob2, &img_knob0);
                        break;
                    case 1:
                        lv_img_set_src(objects.config_volume_knob2, &img_knob1);
                        break;
                    case 2:
                        lv_img_set_src(objects.config_volume_knob2, &img_knob2);
                        break;
                    case 3:
                        lv_img_set_src(objects.config_volume_knob2, &img_knob3);
                        break;
                    case 4:
                        lv_img_set_src(objects.config_volume_knob2, &img_knob4);
                        break;
                    case 5:
                        lv_img_set_src(objects.config_volume_knob2, &img_knob5);
                        break;
                    case 6:
                        lv_img_set_src(objects.config_volume_knob2, &img_knob6);
                        break;
                    case 7:
                        lv_img_set_src(objects.config_volume_knob2, &img_knob7);
                        break;
                }
            }

            if (config->HasParameterChanged(CHANNEL_MUTE, chanIndex) || force_update)
            {
                lv_label_set_text(objects.current_channel_mute, config->GetParameter(CHANNEL_MUTE)->GetLabelAndValue(chanIndex).c_str());
            }

            //char outputDestinationName[10] = "";
            //routingGetOutputName(&outputDestinationName[0], mixerGetSelectedChannel());
            //lv_label_set_text_fmt(objects.current_channel_destination, outputDestinationName);
        }
};