#pragma once
#include "page.h"
using namespace std;

class PageAbout: public Page {
    public:
        PageAbout(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::SETUP_CARD;
            nextPage = X32_PAGE::DEBUG;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 2;
            led = X32_BTN_SETUP;
        }

        void OnInit() override
        {
            BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_2, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_3, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_4, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_5, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_6, PAGE_CUSTOM_ENCODER);

            custom_encoder[DISPLAY_ENCODER_1].label = "AES50 Online";
            custom_encoder[DISPLAY_ENCODER_2].label = "SysMode Bit 1";
            custom_encoder[DISPLAY_ENCODER_3].label = "SysMode Bit 0";
            custom_encoder[DISPLAY_ENCODER_4].label = "Bit 3";
            custom_encoder[DISPLAY_ENCODER_5].label = "Bit 4";
            custom_encoder[DISPLAY_ENCODER_6].label = "FPGA-Debugmode";

            custom_encoder[DISPLAY_ENCODER_1].value = mixer->fpga->GetConfigBit(0) ? "1" : "0"; // AES online/offline
            custom_encoder[DISPLAY_ENCODER_2].value = mixer->fpga->GetConfigBit(2) ? "1" : "0"; // SysMode Bit 1
            custom_encoder[DISPLAY_ENCODER_3].value = mixer->fpga->GetConfigBit(1) ? "1" : "0"; // SysMode Bit 0

            custom_encoder[DISPLAY_ENCODER_4].value = mixer->fpga->GetConfigBit(3) ? "1" : "0";
            custom_encoder[DISPLAY_ENCODER_5].value = mixer->fpga->GetConfigBit(4) ? "1" : "0";
            custom_encoder[DISPLAY_ENCODER_6].value = mixer->fpga->GetConfigBit(5) ? "1" : "0";

            SyncEncoderWidgets(true);
        }

        bool OnDisplayButton(X32_BTN button, bool pressed) override
        {
            bool handled = false;

        	if (pressed)
            {
                handled = true;

				switch (button)
                {
                    case X32_BTN_ENCODER1:
                        mixer->fpga->SetConfigBit(0, !mixer->fpga->GetConfigBit(0));
                        custom_encoder[0].value = mixer->fpga->GetConfigBit(0) ? "1" : "0";
                        break;
                    case X32_BTN_ENCODER2:
                        mixer->fpga->SetConfigBit(2, !mixer->fpga->GetConfigBit(2));
                        custom_encoder[1].value = mixer->fpga->GetConfigBit(2) ? "1" : "0"; // this is bit 1
                        break;
                    case X32_BTN_ENCODER3:
                        mixer->fpga->SetConfigBit(1, !mixer->fpga->GetConfigBit(1));
                        custom_encoder[2].value = mixer->fpga->GetConfigBit(1) ? "1" : "0"; // this is bit 0
                        break;
                    case X32_BTN_ENCODER4:
                        mixer->fpga->SetConfigBit(3, !mixer->fpga->GetConfigBit(3));
                        custom_encoder[3].value = mixer->fpga->GetConfigBit(3) ? "1" : "0";
                        break;
                    case X32_BTN_ENCODER5:
                        mixer->fpga->SetConfigBit(4, !mixer->fpga->GetConfigBit(4));
                        custom_encoder[4].value = mixer->fpga->GetConfigBit(4) ? "1" : "0";
                        break;
                    case X32_BTN_ENCODER6:
                        mixer->fpga->SetConfigBit(5, !mixer->fpga->GetConfigBit(5));
                        custom_encoder[5].value = mixer->fpga->GetConfigBit(5) ? "1" : "0";
                        break;
                    default:
                        handled = false;
                        // dummy
                }

                // update encoders as values have changed
                SyncEncoderWidgets(true);
            }

            return handled;
        }
};