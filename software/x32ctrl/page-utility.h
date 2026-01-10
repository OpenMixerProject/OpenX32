#pragma once
#include "page.h"
using namespace std;

class PageUtility: public Page {
    public:
        PageUtility(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 7;
            led = X32_BTN_UTILITY;
        }

        void OnChange() override {
            // TODO only update if values are changed
            SetEncoderText("Reload DSPs", String("CARD Ch\n") + String(state->cardchannels), "*8R#", "-", String("D2: ") + String(state->debugvalue2), String("D1: ") + String(state->debugvalue));
        }

        void OnDisplayButton(X32_BTN button, bool pressed) override {
        	if (pressed){
				switch (button){
                // DEBUG
				// case X32_BTN_UP:
				// 	surface->SetLed(state->debugvalue2, state->debugvalue, true);
				// 	break;
				// case X32_BTN_DOWN:
				// 	surface->SetLed(state->debugvalue2, state->debugvalue, false);
				// 	break;
                case X32_BTN_ENCODER1:
                    // Reload DSP1
                    mixer->dsp->spi->CloseConnectionDsps();
                    mixer->dsp->spi->UploadBitstreamDsps();
                    mixer->dsp->spi->OpenConnectionDsps();
                    usleep(50000); // wait 50ms
                    mixer->dsp->SendAll();
                    mixer->dsp->SendAll(); // currently we need to send the data twice. Maybe a bug in the SPI-connection or a timing-issue?
                    break;
                case X32_BTN_ENCODER2:
                    mixer->Card_SetChannels(state->cardchannels);
                    break;
                case X32_BTN_ENCODER3:
                    mixer->Card_SendCommand("*8R#");
                    break;
                case X32_BTN_ENCODER4:
                    break;
                case X32_BTN_ENCODER5:
                    state->debugvalue2 = 0;
                    state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
                    break;
                case X32_BTN_ENCODER6:
                    state->debugvalue = 0;
                    state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
                    break;
                default:
                    break;
				}
			}
        }

        void OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) override {
            if (state->activePage == X32_PAGE_UTILITY) {
                switch (encoder){
                    case X32_ENC_ENCODER1:
                        break;
                    case X32_ENC_ENCODER2:
                        state->cardchannels += amount;
                        state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
                        break;
                    case X32_ENC_ENCODER3:
                        break;
                    case X32_ENC_ENCODER4:
                        break;
                    case X32_ENC_ENCODER5:
                        state->debugvalue2 += amount;
                        state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
                        break;
                    case X32_ENC_ENCODER6:
                        state->debugvalue += amount;
                        state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
                        break;
                    default:  
                        // just here to avoid compiler warnings                  
                        break;
                }
		    }
        }
};