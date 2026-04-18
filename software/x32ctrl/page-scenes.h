#pragma once
#include "page.h"

using namespace std;
using enum MP_ID;

class PageScenes: public Page
{
    private:
        uint selected_scene = 0;
        uint selected_scene_before = 0;

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
            OnChange(true);
        }

        void OnChange(bool force_update) override
        {
            if (config->HasParameterChanged(DISPLAY_ENCODER_6_ENCODER))
            {
                int amount = config->GetInt(DISPLAY_ENCODER_6_ENCODER);
                selected_scene += amount;
            }

            if (config->HasParameterChanged(DISPLAY_ENCODER_1_BUTTON))
            {
                mixer->LoadConfig(selected_scene);
            }

            if (config->HasParameterChanged(DISPLAY_ENCODER_6_BUTTON))
            {
                mixer->SaveConfig(selected_scene);
            }

            if(selected_scene != selected_scene_before || force_update)
            {
                config->GetParameter(DISPLAY_ENCODER_1_BUTTON)->SetName(String("Load ") + selected_scene);
                config->GetParameter(DISPLAY_ENCODER_6_BUTTON)->SetName(String("Save ") + selected_scene);

                SyncEncoderWidgets(false);

                selected_scene_before = selected_scene;
            }
        }
};