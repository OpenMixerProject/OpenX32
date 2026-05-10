#pragma once
#include "page.h"
using namespace std;

class PagePrototypeGui: public Page
{
    private:

        char* button_map[47];
        uint selectionindex = 0;

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
            
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_1, LV_SYMBOL_REFRESH);
        }


        void OnChangeCustomEncoder(SurfaceElementId surface_element_id, int amount) override
        {
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_1:
                    selectionindex += amount;
                    lv_buttonmatrix_set_button_ctrl(objects.routingmatrix, selectionindex, LV_BUTTONMATRIX_CTRL_CHECKED);
                    break;
                default:
                    break;
            }
        }
};