#pragma once

#include "page.h"

using namespace std;

class PageRoutingFpga: public Page
{
    using enum MP_ID;

    private:

        int gui_selected_block = 0;
		int gui_selected_block_before = 0;

        int gui_selected_item = 0;
		int gui_selected_item_before = 0;
        uint gui_items_offset = 0;
        uint gui_items_count = 0;

    public:
        PageRoutingFpga(PageBaseParameter* pagebasepar) : Page(pagebasepar)
        {
            prevPage = X32_PAGE::ROUTING;
            nextPage = X32_PAGE::ROUTING_DSP1;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 1;
            led = X32_BTN_ROUTING;
        }

        void OnInit() override 
        {
            lv_table_set_row_count(objects.table_routing_fpga, NUM_OUTPUT_CHANNEL); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
            lv_table_set_column_count(objects.table_routing_fpga, 3);
            lv_table_set_column_width(objects.table_routing_fpga, 0, 200);
            lv_table_set_column_width(objects.table_routing_fpga, 1, 50);
            lv_table_set_column_width(objects.table_routing_fpga, 2, 200);

            DrawTable();

            BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_2, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_3, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_4, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_5, PAGE_CUSTOM_ENCODER);
        }

        void OnShow() override {

            custom_encoder[DISPLAY_ENCODER_1].label = String(LV_SYMBOL_UP) + " Block " + LV_SYMBOL_DOWN;            
            custom_encoder[DISPLAY_ENCODER_2].label = String(LV_SYMBOL_UP) + " Select " + LV_SYMBOL_DOWN;
			custom_encoder[DISPLAY_ENCODER_3].label = String(LV_SYMBOL_UP) + " Select (Group) " + LV_SYMBOL_DOWN;
			custom_encoder[DISPLAY_ENCODER_4].label = String(LV_SYMBOL_REFRESH) + " Source";
			custom_encoder[DISPLAY_ENCODER_5].label = String(LV_SYMBOL_REFRESH) + " Source (Group)";
        }

        void OnChange(bool force_update) override
        {
            if(gui_selected_block != gui_selected_block_before)
            {
				if (gui_selected_block < 0) {
					// limit list at the top
					gui_selected_block = 0;
				}else if (gui_selected_block >= 7) {
					// limit list at the bottom
					gui_selected_block = 7 - 1;
                }

                lv_roller_set_selected(objects.roller_routing_fpga_block, gui_selected_block, LV_ANIM_OFF);
                
				gui_selected_block_before = gui_selected_block;
                gui_selected_item = 0;

                DrawTable();
            } 

            if(gui_selected_item_before != gui_selected_item)
            {
				if (gui_selected_item < 0) {
					// limit list at the top
					gui_selected_item = 0;
				}else if (gui_selected_item >= gui_items_count) {
					// limit list at the bottom
					gui_selected_item = gui_items_count - 1;
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
                    if (index >= gui_items_offset && index <= gui_items_offset + gui_items_count)
                    {
                        lv_table_set_cell_value(
                            objects.table_routing_fpga,
                            index - gui_items_offset,
                            0,
                            config->GetParameter(ROUTING_FPGA)->GetFormatedValue(index).c_str()
                        );
                    }
                }
            }
        }

        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override
        {
            bool handled = true;

            switch (encoder)
            {
                case X32_ENC_ENCODER1:
                    gui_selected_block += amount;
					OnChange(false);
                    break;
                case X32_ENC_ENCODER2:
                    gui_selected_item += amount;
					OnChange(false);
                    break;
                case X32_ENC_ENCODER3:
                    if (amount < 0) {
						gui_selected_item -= 8;
					}else{
						gui_selected_item += 8;
					}
					OnChange(false);
                    break;
                case X32_ENC_ENCODER4:
                    config->Change(ROUTING_FPGA, amount, gui_selected_item + gui_items_offset);
                    break;
                case X32_ENC_ENCODER5:
					int8_t absoluteChange;
                    if (amount < 0) {
                        absoluteChange = -8;
                    }else{
                        absoluteChange = 8;
                    }
                    for (uint8_t i=gui_selected_item; i<(gui_selected_item+8); i++)
                    {
                        config->Change(ROUTING_FPGA, absoluteChange, i);
                    }
                    break;
                default:  
                    handled = false;                  
                    break;
            }

            return handled;
        }

        void DrawTable()
        {
            switch (gui_selected_block)
            {
                case 0: // DSP Input (Mixer Channels)
                    gui_items_offset = 72;
                    gui_items_count = 40;
                    break;
                case 1: // XLR Outputs
                    gui_items_offset = 0;
                    gui_items_count = 16;
                    break;
                case 2: // AUX Outputs
                    gui_items_offset = 64;
                    gui_items_count = 8;
                    break;
                case 3: // Ultranet
                    gui_items_offset = 16;
                    gui_items_count = 16;
                    break;
                case 4: // CARD Recording
                    gui_items_offset = 32;
                    gui_items_count = 32;
                    break;
                case 5: // AES50 A Outputs
                    gui_items_offset = 112;
                    gui_items_count = 48;
                    break;
                case 6: // AES50 B Outputs
                    gui_items_offset = 160;
                    gui_items_count = 48;
                    break;
            
                default:
                    break;
            }

            lv_table_set_row_count(objects.table_routing_fpga, 0);

            for (uint8_t i = 0; i < gui_items_count; i++)
            {
                lv_table_set_cell_value(objects.table_routing_fpga, i, 0, config->GetParameter(ROUTING_FPGA)->GetFormatedValue(i + gui_items_offset).c_str());
                lv_table_set_cell_value(objects.table_routing_fpga, i, 2, mixer->fpga->GetOutputNameByIndex(i + 1 + gui_items_offset).c_str());
            }

            lv_table_set_cell_value(objects.table_routing_fpga, gui_selected_item, 1, LV_SYMBOL_RIGHT);
        }
};