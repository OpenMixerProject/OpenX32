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
			using enum MP_ID;

			BindEncoder(DISPLAY_ENCODER_1, true, CHANNEL_SOURCE, CHANNEL_PHASE_INVERT);
			BindEncoder(DISPLAY_ENCODER_2, true, CHANNEL_GAIN, CHANNEL_PHANTOM);
			BindEncoder(DISPLAY_ENCODER_3, true, CHANNEL_PANORAMA);
			BindEncoder(DISPLAY_ENCODER_4, true, CHANNEL_VOLUME, CHANNEL_MUTE);
            BindEncoder(DISPLAY_ENCODER_6, false, SELECTED_CHANNEL);
        }

        void OnChange(bool force_update) override {

            using enum MP_ID;

            if (config->HasParameterChanged(SELECTED_CHANNEL))
            {
                force_update = true;
            }

            uint8_t chanIndex = config->GetUint(MP_ID::SELECTED_CHANNEL);

            // TODO implement with better string handling -> (*** stack smashing detected ***: terminated)
            // char dspSourceName[5] = "";
            // mixer->dsp->GetSourceName(&dspSourceName[0], chanIndex, mixer->fpga->fpgaRouting.dsp[mixer->dsp->Channel[chanIndex].inputSource - 1]);
            // lv_label_set_text_fmt(objects.current_channel_source, "%02d: %s", (chanIndex + 1), dspSourceName);

            if (config->HasParameterChanged(CHANNEL_GAIN, chanIndex) || force_update)
            {
                lv_label_set_text(objects.current_channel_gain, config->GetParameter(CHANNEL_GAIN)->GetLabelAndValue(chanIndex).c_str());
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
            }

            if (config->HasParameterChanged(CHANNEL_VOLUME, chanIndex) || force_update)
            {
                lv_label_set_text(objects.current_channel_volume, config->GetParameter(CHANNEL_VOLUME)->GetLabelAndValue(chanIndex).c_str());
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