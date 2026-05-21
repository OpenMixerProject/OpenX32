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
            tabIndex1 = 5;
            led = X32_BTN_SETUP;
        } 

        void OnInit() override
        {
        }
        
        void OnShow() override
        {
            
        }


        void OnChangeCustomEncoder(SurfaceElementId surface_element_id, int amount) override
        {
            switch (surface_element_id)
            {
                // case SurfaceElementId::DISPLAY_ENCODER_1:
                //     selectionindex += amount;
                //     lv_buttonmatrix_set_button_ctrl(objects.routingmatrix, selectionindex, LV_BUTTONMATRIX_CTRL_CHECKED);
                //     break;
                default:
                    break;
            }
        }
};