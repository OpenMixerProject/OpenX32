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

        void OnChange(bool force_update) override
        {   
            if (config->HasParameterChanged(DISPLAY_ENCODER_1_ENCODER) || force_update)
            {
                int amount = config->GetInt(DISPLAY_ENCODER_1_ENCODER);
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

                config->GetParameter(DISPLAY_ENCODER_1_ENCODER)->SetName(String(LV_SYMBOL_REFRESH) + String("\nSelect ") + ((bankingSends * 2) + 1) + String("/") + ((bankingSends * 2) + 2));

                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_2, MixerparameterAction::SET_TO_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND01 + bankingSends * 2));
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_3, MixerparameterAction::SET_TO_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND01_TAPPOINT + bankingSends * 2));
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_4, MixerparameterAction::SET_TO_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND02 + bankingSends * 2));
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_5, MixerparameterAction::SET_TO_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND02_TAPPOINT + bankingSends * 2));
            }            
        }
};