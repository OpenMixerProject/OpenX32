#pragma once
#include "page.h"
using namespace std;

class PagePrototypeGui: public Page
{
    private:

        int gui_selected_destination = 0;
		int gui_selected_destination_before = 0;

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
            BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
           
            custom_encoder[DISPLAY_ENCODER_1].label = "Select";

            SyncEncoderWidgets(true);
        }


        void OnChange(bool force_update) override
        {
            if(gui_selected_destination != gui_selected_destination_before)
            {
				if (gui_selected_destination < 0) {
					// limit list at the top
					gui_selected_destination = 0;
				}else if (gui_selected_destination >= 7) {
					// limit list at the bottom
					gui_selected_destination = 7 - 1;
                }

                lv_roller_set_selected(objects.roller_routing_block, gui_selected_destination, LV_ANIM_OFF);

                

                // // remove old indicator
                // lv_table_set_cell_value(objects.table_routing_fpga, gui_selected_destination, 1, " ");
                
                // // display new indicator
                // lv_table_set_cell_value(objects.table_routing_fpga, gui_selected_item, 1, LV_SYMBOL_RIGHT);
                
                // // set select to scroll table
                // lv_table_set_selected_cell(objects.table_routing_fpga, gui_selected_item, 2);
                
				gui_selected_destination_before = gui_selected_destination;
            } 
        }


        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override
        {
            bool handled = true;

            switch (encoder)
            {
                case X32_ENC_ENCODER1:
                    gui_selected_destination += amount;
					OnChange(false);
                    break;
                case X32_ENC_ENCODER2:
                   
                    break;
                case X32_ENC_ENCODER3:
                    break;
                case X32_ENC_ENCODER4:
                    break;
                case X32_ENC_ENCODER5:
                    break;
                case X32_ENC_ENCODER6:
                    break;
                default:  
                    handled = false;               
                    break;
            }

            return handled;
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
                        break;
                    case X32_BTN_ENCODER2:                      
                        break;
                    case X32_BTN_ENCODER3:  
                        break;
                    case X32_BTN_ENCODER4:
                        break;
                    case X32_BTN_ENCODER5:
                        break;
                    case X32_BTN_ENCODER6:
                        break;
                    default:
                        handled = false;
                }

                if (handled)
                {
                    // update encoders as values have changed
                    SyncEncoderWidgets(true);
                }
            }

            return handled;
        }
};