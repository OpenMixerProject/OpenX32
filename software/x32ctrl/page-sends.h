#pragma once

#include "page.h"
using namespace std;

class PageSends : public Page
{
    using enum MP_ID;

    private:

        

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
            Mixerparameter* mp = config->GetParameter(DISPLAY_ENCODER_1_ENCODER);
            mp->SetName(String(LV_SYMBOL_REFRESH));
            mp->DefMinMaxStandard_Uint(0,7,0);
            mp->DefUOM(MP_UOM::NONE);

            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_1, MixerparameterAction::CHANGE, DISPLAY_ENCODER_1_ENCODER);
        }

        void OnChange(bool force_update) override
        {   
            if (config->HasParameterChanged(DISPLAY_ENCODER_1_ENCODER) || force_update)
            {
                uint bankingSends = config->GetUint(DISPLAY_ENCODER_1_ENCODER);
                
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_2, MixerparameterAction::CHANGE_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND01 + bankingSends * 2));
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_3, MixerparameterAction::CHANGE_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND01_TAPPOINT + bankingSends * 2));
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_4, MixerparameterAction::CHANGE_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND02 + bankingSends * 2));
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_5, MixerparameterAction::CHANGE_SELECTED_CHANNEL, (MP_ID)((uint)CHANNEL_BUS_SEND02_TAPPOINT + bankingSends * 2));
            }            
        }
};