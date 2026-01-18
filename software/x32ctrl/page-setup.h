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
            SetEncoder(5, MIXERPARAMETER_LCD_CONTRAST);
        }

        void OnChange(bool force_update) override {
            if (state->HasChanged(X32_MIXER_CHANGED_LCD_CONTRAST) || force_update) {
                SetEncoderValue(5, state->lcdcontrast);
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
};