#pragma once

#include "page.h"

using namespace std;

class PageRoutingFpga: public Page
{
    using enum MP_ID;

    private:
        char outputDestinationName[15] = "";
        char inputSourceName[15] = "";
        uint8_t routingIndex = 0;

        uint gui_selected_item = 0;
		uint gui_selected_item_before = 0;
        bool page_routing_fpga_table_drawn = false;

    public:
        PageRoutingFpga(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::ROUTING;
            nextPage = X32_PAGE::ROUTING_DSP1;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 1;
        }

        void OnInit() override {
            if (gui_selected_item >= NUM_OUTPUT_CHANNEL) {
                gui_selected_item = 0;
            }else if (gui_selected_item < 0) {
                gui_selected_item = NUM_OUTPUT_CHANNEL - 1;
            }

            lv_table_set_row_count(objects.table_routing_fpga, NUM_OUTPUT_CHANNEL); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
            lv_table_set_column_count(objects.table_routing_fpga, 3);
            lv_table_set_column_width(objects.table_routing_fpga, 0, 200);
            lv_table_set_column_width(objects.table_routing_fpga, 1, 50);
            lv_table_set_column_width(objects.table_routing_fpga, 2, 200);
            for (uint8_t i=0; i < NUM_OUTPUT_CHANNEL; i++){
                routingIndex = mixer->fpga->GetOutputByIndex(i+1);
                lv_table_set_cell_value_fmt(objects.table_routing_fpga, i, 0, "%s", mixer->fpga->GetOutputNameByIndex(i+1).c_str());
                lv_table_set_cell_value_fmt(objects.table_routing_fpga, i, 2, "%s", mixer->fpga->GetInputNameByIndex(routingIndex).c_str());
            }
            lv_table_set_cell_value(objects.table_routing_fpga, gui_selected_item, 1, LV_SYMBOL_LEFT);
            page_routing_fpga_table_drawn = true;

            BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_2, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_3, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_4, PAGE_CUSTOM_ENCODER);
        }

        void OnShow() override {
            custom_encoder[0].label = "\xEF\x81\xB7 Target \xEF\x81\xB8";
			custom_encoder[1].label = "\xEF\x81\xB7 Group \xEF\x81\xB8";
			custom_encoder[2].label =  "\xEF\x80\xA1 Source";
			custom_encoder[3].label =  "\xEF\x80\xA1 Group-Source";
        }

        void OnChange(bool force_update) override
        {
            if(gui_selected_item_before != gui_selected_item)
            {
                if (gui_selected_item >= NUM_OUTPUT_CHANNEL)
                {
                    gui_selected_item = 0;
                }
                else if (gui_selected_item < 0)
                {
                    gui_selected_item = NUM_OUTPUT_CHANNEL - 1;
                }

                // remove old indicator
                lv_table_set_cell_value(objects.table_routing_fpga, gui_selected_item_before, 1, " ");
                
                // display new indicator
                lv_table_set_cell_value(objects.table_routing_fpga, gui_selected_item, 1, LV_SYMBOL_LEFT);
                
                // set select to scroll table
                lv_table_set_selected_cell(objects.table_routing_fpga, gui_selected_item, 2);
                
				gui_selected_item_before = gui_selected_item;
            } 
            
            if(config->HasParameterChanged(ROUTING_FPGA))
            {
                routingIndex = mixer->fpga->GetOutputByIndex(gui_selected_item+1);
                lv_table_set_cell_value_fmt(objects.table_routing_fpga, gui_selected_item, 2, "%s", mixer->fpga->GetInputNameByIndex(routingIndex).c_str());
            }
        }

        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override {
            switch (encoder){
                case X32_ENC_ENCODER1:
                    gui_selected_item += amount;
					OnChange(false);
                    break;
                case X32_ENC_ENCODER2:
					if (amount < 0) {
						gui_selected_item -= 8;
					}else{
						gui_selected_item += 8;
					}
					OnChange(false);
                    break;
                case X32_ENC_ENCODER3:
                    mixer->ChangeHardwareInput(gui_selected_item, amount);
                    break;
                case X32_ENC_ENCODER4:
                    int8_t absoluteChange;
                    if (amount < 0) {
                        absoluteChange = -8;
                    }else{
                        absoluteChange = 8;
                    }
                    for (uint8_t i=gui_selected_item; i<(gui_selected_item+8); i++) {
                        mixer->ChangeHardwareInput(i, absoluteChange);
                    }
                    break;
                case X32_ENC_ENCODER5:
                    break;
                case X32_ENC_ENCODER6:
                    break;
                default:  
                    // just here to avoid compiler warnings                  
                    break;
            }

            return true;
        }
};