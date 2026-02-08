#pragma once
#include "page.h"
using namespace std;

class PageSetup: public Page {
    public:
        PageSetup(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            nextPage = X32_PAGE::SETUP_CARD;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 0;
            led = X32_BTN_SETUP;
        }

        void OnInit() override {
            BindEncoder(DISPLAY_ENCODER_5, MP_ID::LED_BRIGHTNESS);
            BindEncoder(DISPLAY_ENCODER_6, MP_ID::LCD_CONTRAST);
        }

        void OnChange(bool force_update) override {
            if (state->HasChanged(X32_MIXER_CHANGED_LED_BRIGHTNESS) || force_update) {
                SetEncoderValue(DISPLAY_ENCODER_5, state->ledbrightness);
            }
            if (state->HasChanged(X32_MIXER_CHANGED_LCD_CONTRAST) || force_update) {
                SetEncoderValue(DISPLAY_ENCODER_6, mixer->GetParameter(MP_ID::LCD_CONTRAST)->GetUint());
            }
        }

        void OnDisplayEncoderTurned(X32_ENC encoder, int amount) override {
            switch (encoder){
				case X32_ENC_ENCODER1:
					break;
				case X32_ENC_ENCODER2:
					break;
				case X32_ENC_ENCODER3:
					break;
				case X32_ENC_ENCODER4:
					break;
				case X32_ENC_ENCODER5:
                    mixer->Change(MP_ID::LED_BRIGHTNESS, amount);

                    // TODO: move to new eventsystem
                    surface->SetBrightnessAllBoards(mixer->Get(MP_ID::LED_BRIGHTNESS));
					break;
				case X32_ENC_ENCODER6:
                    mixer->Change(MP_ID::LCD_CONTRAST, amount);

                    // TODO: move to new eventsystem
                    surface->SetContrastAllBoards(mixer->Get(MP_ID::LCD_CONTRAST));
					break;
				default:
					break;
			}
        }

        void OnDisplayButton(X32_BTN button, bool pressed) override {
            if (pressed){
				switch (button){
					case X32_BTN_ENCODER1:
						break;
					case X32_BTN_ENCODER2:
						break;
					case X32_BTN_ENCODER3:
						break;
					case X32_BTN_ENCODER4:
						break;
					case X32_BTN_ENCODER5:
                        mixer->Reset(MP_ID::LED_BRIGHTNESS);

                        // TODO: move to new eventsystem
                        surface->SetBrightnessAllBoards(mixer->Get(MP_ID::LED_BRIGHTNESS));
						break;
					case X32_BTN_ENCODER6:
                        mixer->Reset(MP_ID::LCD_CONTRAST);
                        
                        // TODO: move to new eventsystem
                        surface->SetContrastAllBoards(mixer->Get(MP_ID::LCD_CONTRAST));
						break;
					default:
                        // just here to avoid compiler warnings
						break;
				}
            }
        }
};