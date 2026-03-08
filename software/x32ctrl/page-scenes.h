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

        void OnInit() override
        {
            BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_6, PAGE_CUSTOM_ENCODER);

            OnChange(true);
        }

        void OnChange(bool force_update) override
        {
            if(selected_scene != selected_scene_before || force_update)
            {
                custom_encoder[DISPLAY_ENCODER_1].label = String("Load ") + selected_scene;
                custom_encoder[DISPLAY_ENCODER_6].label = String("Save ") + selected_scene;

                SyncEncoderWidgets(false);

                selected_scene_before = selected_scene;
            }
        }

        bool OnDisplayButton(X32_BTN button, bool pressed) override
        {
            bool handled = false;

        	if (pressed)
            {
                handled = true;

				switch (button)
                {
                    case X32_BTN_ENCODER1:
                        mixer->LoadConfig(selected_scene);
                        break;
                    case X32_BTN_ENCODER6:
                        mixer->SaveConfig(selected_scene);
                        break;
                    default:
                        handled = false;
                        // dummy
				}
			}

            return handled;
        }

        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override
        {
            bool handled = false;

            switch (encoder)
            {
				case X32_ENC_ENCODER1:
                    selected_scene += amount;
                    handled = true;
                    OnChange(false);
                    break;
                default:
                        handled = false;
                        // dummy
            }

            return handled;
        }

    private:

};