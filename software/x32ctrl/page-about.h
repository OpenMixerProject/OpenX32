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
/*
            BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_2, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_3, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_4, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_5, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_6, PAGE_CUSTOM_ENCODER);
*/
            EncoderText();

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
/*
                    case X32_BTN_ENCODER1:
                        mixer->fpga->SetConfigBit(0, !mixer->fpga->GetConfigBit(0));
                        break;
                    case X32_BTN_ENCODER2:
                        mixer->fpga->SetConfigBit(2, !mixer->fpga->GetConfigBit(2));
                        break;
                    case X32_BTN_ENCODER3:
                        mixer->fpga->SetConfigBit(1, !mixer->fpga->GetConfigBit(1));
                        break;
                    case X32_BTN_ENCODER4:
                        mixer->fpga->SetConfigBit(3, !mixer->fpga->GetConfigBit(3));
                        break;
                    case X32_BTN_ENCODER5:
                        mixer->fpga->SetConfigBit(4, !mixer->fpga->GetConfigBit(4));
                        break;
                    case X32_BTN_ENCODER6:
                        mixer->fpga->SetConfigBit(5, !mixer->fpga->GetConfigBit(5));
                        break;
*/
                    default:
                        handled = false;
                        // dummy
                }

                if (handled)
                {
                    EncoderText();

                    // update encoders as values have changed
                    SyncEncoderWidgets(true);
                }
            }

            return handled;
        }

        void EncoderText()
        {
/*
            custom_encoder[DISPLAY_ENCODER_1].label = String("AES50 Online\n") + String(mixer->fpga->GetConfigBit(0) ? "1" : "0");  // AES online/offline
            custom_encoder[DISPLAY_ENCODER_2].label = String("SysMode Bit 1\n") + String(mixer->fpga->GetConfigBit(2) ? "1" : "0"); // SysMode Bit 1
            custom_encoder[DISPLAY_ENCODER_3].label = String("SysMode Bit 0\n") + String(mixer->fpga->GetConfigBit(1) ? "1" : "0"); // SysMode Bit 0
            custom_encoder[DISPLAY_ENCODER_4].label = String("Bit 3\n") + String(mixer->fpga->GetConfigBit(3) ? "1" : "0");
            custom_encoder[DISPLAY_ENCODER_5].label = String("Bit 4\n") + String(mixer->fpga->GetConfigBit(4) ? "1" : "0");
            custom_encoder[DISPLAY_ENCODER_6].label = String("FPGA-Debugmode\n") + String(mixer->fpga->GetConfigBit(5) ? "1" : "0");
*/
        }
};