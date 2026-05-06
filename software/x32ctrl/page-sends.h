#pragma once

#include "page.h"
using namespace std;

class PageSends : public Page
{
    using enum MP_ID;

    private:

        uint bankingSends = 0;

    public:
        PageSends(PageBaseParameter* pagebasepar) : Page(pagebasepar)
        {
            prevPage = X32_PAGE::EQ;
            nextPage = X32_PAGE::MAIN;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 5;
            led = X32_BTN_VIEW_MIX_BUS_SENDS;
            noLedOnRack = true;
        }

        void OnShow()
        {
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_1, String(LV_SYMBOL_REFRESH));
            UpdateBinding();
        }

        void OnChangeCustomEncoder(SurfaceElementId surface_element_id, int amount) override
        {   
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_1:
                
                    bankingSends = helper->CheckBoundaries(bankingSends, amount, 0, 7);

                    UpdateBinding();
                    break;
            }
        }

        void UpdateBinding()
        {
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_2, MixerparameterAction::CHANGE_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND01 + bankingSends * 2));
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_3, MixerparameterAction::CHANGE_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND01_TAPPOINT + bankingSends * 2));
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_4, MixerparameterAction::CHANGE_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND02 + bankingSends * 2));
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_5, MixerparameterAction::CHANGE_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND02_TAPPOINT + bankingSends * 2));
        }
};