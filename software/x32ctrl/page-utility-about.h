#pragma once
#include "page.h"
using namespace std;

class PageUtilityAbout: public Page {
    public:
        PageUtilityAbout(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::UTILITY;
            tabLayer0 = objects.maintab;
            tabIndex0 = 7;
            tabLayer1 = objects.utilitytab;
            tabIndex1 = 1;
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
            custom_encoder[DISPLAY_ENCODER_2].label = "PHY CLK 0";
            custom_encoder[DISPLAY_ENCODER_3].label = "PHY CLK 1";
            custom_encoder[DISPLAY_ENCODER_4].label = "TDM CLK Invert";
            custom_encoder[DISPLAY_ENCODER_5].label = "SysMode Bit 1";
            custom_encoder[DISPLAY_ENCODER_6].label = "SysMode Bit 0";

            custom_encoder[DISPLAY_ENCODER_1].value = mixer->fpga->GetDebugBit(0) ? "1" : "0";
            custom_encoder[DISPLAY_ENCODER_2].value = mixer->fpga->GetDebugBit(1) ? "1" : "0";
            custom_encoder[DISPLAY_ENCODER_3].value = mixer->fpga->GetDebugBit(2) ? "1" : "0";
            custom_encoder[DISPLAY_ENCODER_4].value = mixer->fpga->GetDebugBit(3) ? "1" : "0";

            custom_encoder[DISPLAY_ENCODER_5].value = mixer->fpga->GetDebugBit(5) ? "1" : "0"; // this is bit 1
            custom_encoder[DISPLAY_ENCODER_6].value = mixer->fpga->GetDebugBit(4) ? "1" : "0"; // this is bit 0

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
                        mixer->fpga->SetDebugBit(0, !mixer->fpga->GetDebugBit(0));
                        custom_encoder[0].value = mixer->fpga->GetDebugBit(0) ? "1" : "0";
                        break;
                    case X32_BTN_ENCODER2:
                        mixer->fpga->SetDebugBit(1, !mixer->fpga->GetDebugBit(1));
                        custom_encoder[1].value = mixer->fpga->GetDebugBit(1) ? "1" : "0";
                        break;
                    case X32_BTN_ENCODER3:
                        mixer->fpga->SetDebugBit(2, !mixer->fpga->GetDebugBit(2));
                        custom_encoder[2].value = mixer->fpga->GetDebugBit(2) ? "1" : "0";
                        break;
                    case X32_BTN_ENCODER4:
                        mixer->fpga->SetDebugBit(3, !mixer->fpga->GetDebugBit(3));
                        custom_encoder[3].value = mixer->fpga->GetDebugBit(3) ? "1" : "0";
                        break;
                    case X32_BTN_ENCODER5:
                        mixer->fpga->SetDebugBit(5, !mixer->fpga->GetDebugBit(5));
                        custom_encoder[4].value = mixer->fpga->GetDebugBit(5) ? "1" : "0"; // this is bit 1
                        break;
                    case X32_BTN_ENCODER6:
                        mixer->fpga->SetDebugBit(4, !mixer->fpga->GetDebugBit(4));
                        custom_encoder[5].value = mixer->fpga->GetDebugBit(4) ? "1" : "0"; // this is bit 0
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