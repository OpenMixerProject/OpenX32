#pragma once
#include "page.h"

using namespace std;
using enum MP_ID;

class PageScenes: public Page
{
    public:
        PageScenes(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 7;
            if (config->IsModelX32FullOrCompact())
            {
                led = X32_BTN_VIEW_SCENES;
            }
            else if (config->IsModelX32ProducerOrRack())
            {
                led = X32_BTN_VIEW_SCENES;
            }            
        }

        void OnShow() override
        {
            config->GetParameter(DISPLAY_ENCODER_1_ENCODER)->SetName(String("Select Scene"));
            config->GetParameter(DISPLAY_ENCODER_1_BUTTON)->SetName(String("Load"));
            config->GetParameter(DISPLAY_ENCODER_6_BUTTON)->SetName(String("Save"));

            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_1, MixerparameterAction::REFRESH, DISPLAY_ENCODER_1_BUTTON);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_1, MixerparameterAction::CHANGE, DISPLAY_ENCODER_1_ENCODER);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_6, MixerparameterAction::REFRESH, DISPLAY_ENCODER_6_BUTTON);
        }

        void OnChange(bool force_update) override
        {
            if (config->HasParameterChanged(DISPLAY_ENCODER_1_BUTTON))
            {
                mixer->LoadConfig(config->GetInt(DISPLAY_ENCODER_1_ENCODER));
            }

            if (config->HasParameterChanged(DISPLAY_ENCODER_6_BUTTON))
            {
                mixer->SaveConfig(config->GetInt(DISPLAY_ENCODER_1_ENCODER));
            }
        }
};