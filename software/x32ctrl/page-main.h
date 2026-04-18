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

        PageMain(PageBaseParameter* pagebasepar) : Page(pagebasepar)
        {
            prevPage = X32_PAGE::SENDS;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 6;
            led = X32_BTN_VIEW_MAIN;
            noLedOnRack = true;
        }

        void OnInit() override
        {
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_1, MixerparameterAction::SET_TO_SELECTED_CHANNEL, CHANNEL_VOLUME);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_1, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_SEND_LR);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_2, MixerparameterAction::SET_TO_SELECTED_CHANNEL, CHANNEL_PANORAMA);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_4, MixerparameterAction::SET_TO_SELECTED_CHANNEL, CHANNEL_VOLUME_SUB);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_4, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_SEND_SUB);
        }
};