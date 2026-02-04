#pragma once
#include "page.h"
using namespace std;

class PageSetup: public Page {
    public:
        PageSetup(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            nextPage = X32_PAGE_SETUP_CARD;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 0;
            led = X32_BTN_SETUP;
        }

        void OnInit() override {
            SetEncoder(DISPLAY_ENCODER_5, MIXERPARAMETER_LED_BRIGHTNESS);
            SetEncoder(DISPLAY_ENCODER_6, MIXERPARAMETER_LCD_CONTRAST);
        }

        void OnChange(bool force_update) override {
            if (state->HasChanged(X32_MIXER_CHANGED_LED_BRIGHTNESS) || force_update) {
                SetEncoderValue(DISPLAY_ENCODER_5, state->ledbrightness);
            }
            if (state->HasChanged(X32_MIXER_CHANGED_LCD_CONTRAST) || force_update) {
                SetEncoderValue(DISPLAY_ENCODER_6, state->lcdcontrast);
            }
        }

        void OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) {
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
                    {
                        if (amount > 0) {
                            state->ledbrightness += 64;
                        } else {
                            state->ledbrightness -= 64;
                        }
                        surface->SetBrightnessAllBoards(state->ledbrightness);
                        state->SetChangeFlags(X32_MIXER_CHANGED_LED_BRIGHTNESS);
                    }
					break;
				case X32_ENC_ENCODER6:
                    state->lcdcontrast += amount;
                    surface->SetContrastAllBoards(state->lcdcontrast);
                    state->SetChangeFlags(X32_MIXER_CHANGED_LCD_CONTRAST);
					break;
				default:
					break;
			}
        }

        void OnDisplayButton(X32_BTN button, bool pressed) {
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
                        state->ledbrightness = helper->GetMixerparameterDefinition(encoderSliders[DISPLAY_ENCODER_5].mp).uint8_t_value_default;
                        surface->SetBrightnessAllBoards(state->ledbrightness);
                        state->SetChangeFlags(X32_MIXER_CHANGED_LED_BRIGHTNESS);
						break;
					case X32_BTN_ENCODER6:
                        state->lcdcontrast = helper->GetMixerparameterDefinition(encoderSliders[DISPLAY_ENCODER_6].mp).uint8_t_value_default;
                        surface->SetContrastAllBoards(state->lcdcontrast);
                        state->SetChangeFlags(X32_MIXER_CHANGED_LCD_CONTRAST);
						break;
					default:
                        // just here to avoid compiler warnings
						break;
				}
            }
        }
};