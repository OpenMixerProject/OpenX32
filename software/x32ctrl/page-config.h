#pragma once

#include "page.h"

using namespace std;

class PageConfig : public Page {
    public:
        PageConfig(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE_HOME;
            nextPage = X32_PAGE_GATE;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 1;
            led = X32_BTN_VIEW_CONFIG;
            noLedOnRack = true;
        }

        void OnInit() override { 
			SetEncoder(DISPLAY_ENCODER_1, "Source", "Invert");
            SetEncoder(DISPLAY_ENCODER_2, MIXERPARAMETER_CHANNEL_GAIN, "Phantom");
            SetEncoder(DISPLAY_ENCODER_3, MIXERPARAMETER_CHANNEL_PANORAMA);
            SetEncoder(DISPLAY_ENCODER_4, MIXERPARAMETER_CHANNEL_VOLUME, "Mute");
            SetEncoder(DISPLAY_ENCODER_5, "DEBUG: 1-16", "Phanton + 47dB");
            SetEncoder(DISPLAY_ENCODER_6, "DEBUG: 1-32", "Phanton + 47dB");
        }

        void OnChange(bool force_update) override {
            uint8_t chanIndex = config->selectedVChannel;

            // TODO implement with better string handling -> (*** stack smashing detected ***: terminated)
            // char dspSourceName[5] = "";
            // mixer->dsp->GetSourceName(&dspSourceName[0], chanIndex, mixer->fpga->fpgaRouting.dsp[mixer->dsp->Channel[chanIndex].inputSource - 1]);
            // lv_label_set_text_fmt(objects.current_channel_source, "%02d: %s", (chanIndex + 1), dspSourceName);

            lv_label_set_text_fmt(objects.current_channel_gain, "%f", (double)mixer->GetGain(chanIndex));
            lv_label_set_text_fmt(objects.current_channel_phantom, "%d", mixer->GetPhantomPower(chanIndex));
            lv_label_set_text_fmt(objects.current_channel_invert, "%d", mixer->GetPhaseInvert(chanIndex));
            lv_label_set_text_fmt(objects.current_channel_pan_bal, "%f", (double)mixer->GetBalance(chanIndex));
            lv_label_set_text_fmt(objects.current_channel_volume, "%f", (double)mixer->GetVolumeDbfs(chanIndex));


            //char outputDestinationName[10] = "";
            //routingGetOutputName(&outputDestinationName[0], mixerGetSelectedChannel());
            //lv_label_set_text_fmt(objects.current_channel_destination, outputDestinationName);

			SetEncoderHighlight(DISPLAY_ENCODER_1, mixer->GetPhaseInvert(chanIndex));

			SetEncoderValue(DISPLAY_ENCODER_2, mixer->GetGain(chanIndex));
			SetEncoderHighlight(DISPLAY_ENCODER_2, mixer->GetPhantomPower(chanIndex));
			
			SetEncoderValue(DISPLAY_ENCODER_3, mixer->GetBalance(chanIndex));
			
			SetEncoderValue(DISPLAY_ENCODER_4, mixer->GetVolumeDbfs(chanIndex));
			SetEncoderHighlight(DISPLAY_ENCODER_4, mixer->GetMute(chanIndex));
        }

        void OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) {
            switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeDspInput(config->selectedVChannel, amount);
					break;
				case X32_ENC_ENCODER2:
					mixer->ChangeGain(config->selectedVChannel, amount);
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangeBalance(config->selectedVChannel, amount);
					break;
				case X32_ENC_ENCODER4:
					mixer->ChangeVolume(config->selectedVChannel, amount);
					break;
				case X32_ENC_ENCODER5:
					break;
				case X32_ENC_ENCODER6:
					break;
				default:  
					// just here to avoid compiler warnings                  
					break;
			}
        }

        void OnDisplayButton(X32_BTN button, bool pressed) {
            if (pressed){
				switch (button){
					case X32_BTN_ENCODER1:
						mixer->TogglePhaseInvert(config->selectedVChannel);
						break;
					case X32_BTN_ENCODER2:
						mixer->TogglePhantom(config->selectedVChannel);
						break;
					case X32_BTN_ENCODER3:
						mixer->SetBalance(config->selectedVChannel, 0.0);
						break;
					case X32_BTN_ENCODER4:
						mixer->ToggleMute(config->selectedVChannel);
						break;
					case X32_BTN_ENCODER5:
						{
							for (int i =0; i < 32; i++) {
								mixer->SetPhantom(i, true);
								mixer->SetGain(i, 47);
							}
						}
						break;
					case X32_BTN_ENCODER6:
						{
							for (int i =0; i < 16; i++) {
								mixer->SetPhantom(i, true);
								mixer->SetGain(i, 47);
							}
						}
						break;
					default:
                        // just here to avoid compiler warnings
						break;
				}
            }
        }
};