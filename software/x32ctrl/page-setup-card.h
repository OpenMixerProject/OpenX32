#pragma once
#include "page.h"
using namespace std;

class PageSetupCard: public Page {
    private:
        uint8_t card_channelmode;

        void Encodertext() {
            String cardmode = String("Channelmode\n") + mixer->GetCardChannelModeString(card_channelmode);
            encoderSliders[0].label = cardmode;
        }

    public:
        PageSetupCard(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::SETUP;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 1;
        }

        void OnShow() override {
            // get current setting
            card_channelmode = mixer->GetCardChannelMode();
            Encodertext();
        }

        void OnChange(bool force_update) override {

            if (state->HasChanged(X32_MIXER_CHANGED_CARD) || force_update)
            {
                lv_label_set_text_fmt(objects.setup_card_detected, "%s", mixer->GetCardModelString().c_str());
                lv_label_set_text_fmt(objects.setup_card_channelmode, "%s", mixer->GetCardChannelModeString().c_str());
                Encodertext();
            }
        }

        void OnDisplayButton(X32_BTN button, bool pressed) override {
            if (pressed){
				switch (button){
					case X32_BTN_ENCODER1:
                        // Commit setting
						mixer->SetCardChannelMode(card_channelmode);
						break;
					case X32_BTN_ENCODER2:
						break;
					case X32_BTN_ENCODER3:
						break;
					case X32_BTN_ENCODER4:
						break;
					case X32_BTN_ENCODER5:
						break;
					case X32_BTN_ENCODER6:
    					break;
					default:
						break;
				}
			}
        }

        void OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) {
            switch (encoder){
                case X32_ENC_ENCODER1:
                    {
                        // just change for Display
                        if (amount > 0) {
                            card_channelmode++;
                            if (card_channelmode > 5) {
                                card_channelmode = 0;
                            }
                        } else {
                            if (card_channelmode == 0) {
                                card_channelmode = 5;
                            } else {
                                card_channelmode--;
                            }
                        }
                        Encodertext();
                    }
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
                    break;
                default:  
                    // just here to avoid compiler warnings                  
                    break;
            }
        }
};