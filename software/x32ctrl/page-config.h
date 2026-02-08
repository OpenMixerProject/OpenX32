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

			BindEncoder(DISPLAY_ENCODER_1, CHANNEL_SOURCE, CHANNEL_PHASE_INVERT);
			BindEncoder(DISPLAY_ENCODER_2, CHANNEL_GAIN, CHANNEL_PHANTOM);
			BindEncoder(DISPLAY_ENCODER_3, CHANNEL_PANORAMA);
			BindEncoder(DISPLAY_ENCODER_4, CHANNEL_VOLUME, CHANNEL_MUTE);

			// old
            SetEncoder(DISPLAY_ENCODER_5, "DEBUG: 1-16", "Phanton + 47dB");
            SetEncoder(DISPLAY_ENCODER_6, "DEBUG: 1-32", "Phanton + 47dB");
        }

        void OnChange(bool force_update) override {

            uint8_t chanIndex = config->selectedVChannel;

            // TODO implement with better string handling -> (*** stack smashing detected ***: terminated)
            // char dspSourceName[5] = "";
            // mixer->dsp->GetSourceName(&dspSourceName[0], chanIndex, mixer->fpga->fpgaRouting.dsp[mixer->dsp->Channel[chanIndex].inputSource - 1]);
            // lv_label_set_text_fmt(objects.current_channel_source, "%02d: %s", (chanIndex + 1), dspSourceName);

            {
                using enum MP_ID;

                if (mixer->HasParameterChanged(CHANNEL_GAIN, chanIndex) || force_update)
                {
                    lv_label_set_text(objects.current_channel_gain, mixer->GetParameter(CHANNEL_GAIN)->GetLabelAndValue(chanIndex).c_str());
                }

                if (mixer->HasParameterChanged(CHANNEL_PHANTOM, chanIndex) || force_update)
                {
                    lv_label_set_text(objects.current_channel_phantom, mixer->GetParameter(CHANNEL_PHANTOM)->GetLabelAndValue(chanIndex).c_str());
                }

                if (mixer->HasParameterChanged(CHANNEL_PHASE_INVERT, chanIndex) || force_update)
                {
                    lv_label_set_text(objects.current_channel_invert, mixer->GetParameter(CHANNEL_PHASE_INVERT)->GetLabelAndValue(chanIndex).c_str());
                }

                if (mixer->HasParameterChanged(CHANNEL_PANORAMA, chanIndex) || force_update)
                {
                    lv_label_set_text(objects.current_channel_pan_bal, mixer->GetParameter(CHANNEL_PANORAMA)->GetLabelAndValue(chanIndex).c_str());
                }

                if (mixer->HasParameterChanged(CHANNEL_VOLUME, chanIndex) || force_update)
                {
                    lv_label_set_text_fmt(objects.current_channel_volume, mixer->GetParameter(CHANNEL_VOLUME)->GetLabelAndValue(chanIndex).c_str());
                }

                if (mixer->HasParameterChanged(CHANNEL_MUTE, chanIndex) || force_update)
                {
                    lv_label_set_text_fmt(objects.current_channel_mute, mixer->GetParameter(CHANNEL_MUTE)->GetLabelAndValue(chanIndex).c_str());
                }
            }

            //char outputDestinationName[10] = "";
            //routingGetOutputName(&outputDestinationName[0], mixerGetSelectedChannel());
            //lv_label_set_text_fmt(objects.current_channel_destination, outputDestinationName);
        }


        bool OnDisplayButton(X32_BTN button, bool pressed) override {
			bool message_handled = false;

            if (pressed){
				switch (button){
					case X32_BTN_ENCODER5:
						{
							for (int i =0; i < 32; i++) {
								mixer->SetPhantom(i, true);
								mixer->SetGain(i, 47);
							}
						}
						message_handled = true;
						break;
					case X32_BTN_ENCODER6:
						{
							for (int i =0; i < 16; i++) {
								mixer->SetPhantom(i, true);
								mixer->SetGain(i, 47);
							}
						}
						message_handled = true;
						break;
					default:
                        // just here to avoid compiler warnings
						break;
				}
            }

			return message_handled;
        }
};