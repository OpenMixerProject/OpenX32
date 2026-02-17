#pragma once

#include "page.h"
using namespace std;

class PageSends : public Page
{
    using enum MP_ID;

    public:
        PageSends(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::EQ;
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
            BindEncoder(DISPLAY_ENCODER_1, CHANNEL_BUS_SEND01, targetindex);
            BindEncoder(DISPLAY_ENCODER_2, CHANNEL_BUS_SEND02, targetindex);
            BindEncoder(DISPLAY_ENCODER_3, CHANNEL_BUS_SEND03, targetindex);
            BindEncoder(DISPLAY_ENCODER_4, CHANNEL_BUS_SEND04, targetindex);
            BindEncoder(DISPLAY_ENCODER_6, CHANNEL_BUS_SEND05, targetindex);

            SyncEncoderWidgets(true);
        }

        void OnChange(bool force_update) override {

            uint8_t chanIndex = config->GetUint(SELECTED_CHANNEL);

            if (config->HasParametersChanged({SELECTED_CHANNEL, BANKING_BUS_SENDS}))
            {
                UpdateEncoderBinding(chanIndex);
            }
        }
};