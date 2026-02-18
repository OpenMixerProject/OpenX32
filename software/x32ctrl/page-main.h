#pragma once

#include "page.h"
using namespace std;

class PageMain : public Page
{
    using enum MP_ID;

    private:

        uint bankingSends = 0;
        uint bankingSendsBefore = 0;

    public:
        PageMain(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::SENDS;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 6;
            led = X32_BTN_VIEW_MAIN;
            noLedOnRack = true;
        }

        void OnChange(bool forceUpdate) override
        {
            if (config->HasParameterChanged(SELECTED_CHANNEL) || forceUpdate)
            {
                UpdateEncoderBinding(config->GetUint(SELECTED_CHANNEL));
            }
        }

        void UpdateEncoderBinding(uint targetindex)
        {
            BindEncoder(DISPLAY_ENCODER_1, CHANNEL_VOLUME, CHANNEL_SEND_LR, config->GetUint(SELECTED_CHANNEL));
            BindEncoder(DISPLAY_ENCODER_2, CHANNEL_PANORAMA, config->GetUint(SELECTED_CHANNEL));

            BindEncoder(DISPLAY_ENCODER_4, CHANNEL_VOLUME_SUB, CHANNEL_SEND_SUB, config->GetUint(SELECTED_CHANNEL));
            
            SyncEncoderWidgets(true);
        }
};