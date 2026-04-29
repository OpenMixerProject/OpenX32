#pragma once
#include "page.h"
using namespace std;

class PagePrototypeGui: public Page
{
    private:

        char* button_map[47];

    public:
        PagePrototypeGui(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::DEBUG;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 4;
            led = X32_BTN_SETUP;
        } 

        void OnInit() override
        {
            lv_buttonmatrix_set_one_checked(objects.routingmatrix, true);
        }
        
        void OnShow() override
        {
            uint map_pointer = 0;

            for (uint i=0; i < 40; i++)
            {
                // newline after 8 elements
                if (i > 0 && i % 8 == 0)
                {
                    button_map[map_pointer++] = "\n";
                }

                String text =   config->GetString(CHANNEL_NAME, i) + String("\n") +
                                config->GetParameter(ROUTING_FPGA)->GetFormatedValue(i + 72) + String("\n")  +
                                String("[") + config->GetString(CHANNEL_NAME_INTERN, i) + String("]");

                char* buffer = (char*)lv_malloc(text.length() + 1);                                
                memccpy(buffer, text.c_str(), 0, text.length() + 1);

                button_map[map_pointer++] = buffer;
            }

            // last empty string to end Buttonmatrix
            button_map[map_pointer++] = "";//(char*)lv_malloc_zeroed(1);

            lv_buttonmatrix_set_map(objects.routingmatrix, button_map);

            

            Mixerparameter* parameter = config->GetParameter(DISPLAY_ENCODER_1_ENCODER);
            parameter->SetName(LV_SYMBOL_REFRESH);
            parameter
                ->DefMinMaxStandard_Uint(0, 39, 0)
                ->DefUOM(MP_UOM::NONE)
                ->DefCycleMode(1, 1);

            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_1, MixerparameterAction::CHANGE, DISPLAY_ENCODER_1_ENCODER);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_2, MixerparameterAction::CHANGE, DISPLAY_ENCODER_1_ENCODER);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_3, MixerparameterAction::CHANGE, DISPLAY_ENCODER_1_ENCODER);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_4, MixerparameterAction::CHANGE, DISPLAY_ENCODER_1_ENCODER);
        }

        void OnChange(bool force) override
        {
            if (config->HasParameterChanged(DISPLAY_ENCODER_1_ENCODER) || force)
            {
                lv_buttonmatrix_set_button_ctrl(objects.routingmatrix, config->GetUint(DISPLAY_ENCODER_1_ENCODER), LV_BUTTONMATRIX_CTRL_CHECKED);
            }
        }
};