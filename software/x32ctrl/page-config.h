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

			BindEncoder(DISPLAY_ENCODER_1, CHANNEL_SOURCE, CHANNEL_INVERT);
			BindEncoder(DISPLAY_ENCODER_2, CHANNEL_GAIN);
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

            lv_label_set_text_fmt(objects.current_channel_gain, "%f", (double)mixer->GetGain(chanIndex));
            lv_label_set_text_fmt(objects.current_channel_phantom, "%d", mixer->GetPhantomPower(chanIndex));
            lv_label_set_text_fmt(objects.current_channel_invert, "%d", mixer->GetPhaseInvert(chanIndex));
            lv_label_set_text_fmt(objects.current_channel_pan_bal, "%f", (double)mixer->GetBalance(chanIndex));
            lv_label_set_text_fmt(objects.current_channel_volume, "%f", (double)mixer->GetVolumeDbfs(chanIndex));


            //char outputDestinationName[10] = "";
            //routingGetOutputName(&outputDestinationName[0], mixerGetSelectedChannel());
            //lv_label_set_text_fmt(objects.current_channel_destination, outputDestinationName);
        }

        void OnDisplayEncoderTurned(X32_ENC encoder, int amount) override {
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

        void OnDisplayButton(X32_BTN button, bool pressed) override {
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