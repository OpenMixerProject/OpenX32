#pragma once
#include "page.h"

using namespace std;
using enum MP_ID;

class PageScenes: public Page
{
    private:
        int configindex = 0;

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
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_1, "Select Scene");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_2, "Load");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_2);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_6, "Save");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_6);
        }

        void OnChangeCustomEncoder(SurfaceElementId surface_element_id, int amount) override
        {
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_1:
                    configindex += amount;
                    break;
            }
        }

        void OnChangeCustomButton(SurfaceElementId surface_element_id) override
        {
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_BUTTON_2:
                    mixer->LoadConfig(configindex);
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_BUTTON_6:
                    mixer->SaveConfig(configindex);
                    break;
            }
        }
};