#pragma once

#include "page.h"
using namespace std;

class PageSends : public Page
{
    using enum MP_ID;

    private:

        uint bankingSends = 0;
        uint bankingSendsBefore = 0;

    public:
        PageSends(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::EQ;
            nextPage = X32_PAGE::MAIN;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 5;
            led = X32_BTN_VIEW_MIX_BUS_SENDS;
            noLedOnRack = true;
        }

        void OnInit() override { 
            UpdateEncoderBinding(config->GetUint(SELECTED_CHANNEL));
        }

        void UpdateEncoderBinding(uint targetindex)
        {
            BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_2, (MP_ID)((uint)CHANNEL_BUS_SEND01 + bankingSends * 2), targetindex);
            BindEncoder(DISPLAY_ENCODER_3, (MP_ID)((uint)CHANNEL_BUS_SEND01_TAPPOINT + bankingSends * 2), targetindex);
            BindEncoder(DISPLAY_ENCODER_4, (MP_ID)((uint)CHANNEL_BUS_SEND02 + bankingSends * 2), targetindex);
            BindEncoder(DISPLAY_ENCODER_5, (MP_ID)((uint)CHANNEL_BUS_SEND02_TAPPOINT + bankingSends * 2), targetindex);

            custom_encoder[DISPLAY_ENCODER_1].label = String("Select ") + ((bankingSends * 2) + 1) + String("/") + ((bankingSends * 2) + 2);

            SyncEncoderWidgets(true);
        }

        void OnChange(bool force_update) override {

            uint8_t chanIndex = config->GetUint(SELECTED_CHANNEL);
            bool bankinHasChanged = (bankingSends != bankingSendsBefore);

            if (
                config->HasParametersChanged({SELECTED_CHANNEL, BANKING_BUS_SENDS}) || 
                bankinHasChanged || 
                force_update
            )
            {
                UpdateEncoderBinding(chanIndex);

                bankingSendsBefore = bankingSends;
            }
        }

        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override
        {
            if (encoder == X32_ENC_ENCODER1)
            {
                if (amount > 0)
                {
                    bankingSends++;
                }
                else if (bankingSends > 0)
                {
                    bankingSends--;
                }

                if (bankingSends >= 8)
                {
                    bankingSends = 7;
                }

                OnChange(false);

                return true; // handled
            }

            return false; // unhandled
        }
};