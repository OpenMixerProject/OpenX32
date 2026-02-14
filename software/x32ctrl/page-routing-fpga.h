#pragma once

#include "page.h"

using namespace std;

class PageRoutingFpga: public Page
{
    using enum MP_ID;

    private:

        uint gui_selected_item = 0;
		uint gui_selected_item_before = 0;
        bool page_routing_fpga_table_drawn = false;

    public:
        PageRoutingFpga(PageBaseParameter* pagebasepar) : Page(pagebasepar)
        {
            prevPage = X32_PAGE::ROUTING;
            nextPage = X32_PAGE::ROUTING_DSP1;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 1;
        }

        void OnInit() override 
        {
            // Header
            lv_table_set_column_count(objects.table_routing_fpga_header, 3);
            lv_table_set_column_width(objects.table_routing_fpga_header, 0, 200);
            lv_table_set_column_width(objects.table_routing_fpga_header, 1, 50);
            lv_table_set_column_width(objects.table_routing_fpga_header, 2, 200);
            lv_table_set_cell_value(objects.table_routing_fpga_header, 0, 0, "Source");
            lv_table_set_cell_value(objects.table_routing_fpga_header, 0, 2, "Destination");


            lv_table_set_row_count(objects.table_routing_fpga, NUM_OUTPUT_CHANNEL); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
            lv_table_set_column_count(objects.table_routing_fpga, 3);
            lv_table_set_column_width(objects.table_routing_fpga, 0, 200);
            lv_table_set_column_width(objects.table_routing_fpga, 1, 50);
            lv_table_set_column_width(objects.table_routing_fpga, 2, 200);
            for (uint8_t i=0; i < NUM_OUTPUT_CHANNEL; i++)
            {
                lv_table_set_cell_value_fmt(objects.table_routing_fpga, i, 0, "%s", config->GetParameter(ROUTING_FPGA)->GetFormatedValue(i).c_str());
                lv_table_set_cell_value_fmt(objects.table_routing_fpga, i, 2, "%s", mixer->fpga->GetOutputNameByIndex(i+1).c_str());
            }
            lv_table_set_cell_value(objects.table_routing_fpga, gui_selected_item, 1, LV_SYMBOL_RIGHT);
            page_routing_fpga_table_drawn = true;

            BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_2, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_3, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_4, PAGE_CUSTOM_ENCODER);
        }

        void OnShow() override {
            custom_encoder[DISPLAY_ENCODER_3].label = String(LV_SYMBOL_UP) + "Destination" + LV_SYMBOL_DOWN;
			custom_encoder[DISPLAY_ENCODER_4].label = String(LV_SYMBOL_UP) + "Group" + LV_SYMBOL_DOWN;
			custom_encoder[DISPLAY_ENCODER_1].label = String(LV_SYMBOL_REFRESH) + "Source";
			custom_encoder[DISPLAY_ENCODER_2].label = String(LV_SYMBOL_REFRESH) + "Group-Source";
        }

        void OnChange(bool force_update) override
        {
            if(gui_selected_item_before != gui_selected_item)
            {
                if (gui_selected_item >= NUM_OUTPUT_CHANNEL)
                {
                    gui_selected_item = NUM_OUTPUT_CHANNEL -1;
                }

                // remove old indicator
                lv_table_set_cell_value(objects.table_routing_fpga, gui_selected_item_before, 1, " ");
                
                // display new indicator
                lv_table_set_cell_value(objects.table_routing_fpga, gui_selected_item, 1, LV_SYMBOL_RIGHT);
                
                // set select to scroll table
                lv_table_set_selected_cell(objects.table_routing_fpga, gui_selected_item, 2);
                
				gui_selected_item_before = gui_selected_item;
            } 
            
            if(config->HasParameterChanged(ROUTING_FPGA))
            {
                for(auto const& index : config->GetChangedParameterIndexes({ROUTING_FPGA}))
                {
                    lv_table_set_cell_value_fmt(objects.table_routing_fpga, index, 0, "%s", config->GetParameter(ROUTING_FPGA)->GetFormatedValue(index).c_str());
                }
            }
        }

        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override {
            switch (encoder){
                case X32_ENC_ENCODER1:
                    config->Change(ROUTING_FPGA, amount, gui_selected_item);
                    break;
                case X32_ENC_ENCODER2:
					int8_t absoluteChange;
                    if (amount < 0) {
                        absoluteChange = -8;
                    }else{
                        absoluteChange = 8;
                    }
                    for (uint8_t i=gui_selected_item; i<(gui_selected_item+8); i++)
                    {
                        config->Change(ROUTING_FPGA, amount, i);
                    }
                    break;
                case X32_ENC_ENCODER3:
                    gui_selected_item += amount;
					OnChange(false);
                    break;
                case X32_ENC_ENCODER4:
                    if (amount < 0) {
						gui_selected_item -= 8;
					}else{
						gui_selected_item += 8;
					}
					OnChange(false);
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