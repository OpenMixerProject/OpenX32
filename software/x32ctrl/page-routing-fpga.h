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

        static void draw_event_header_cb(lv_event_t * e) {
            lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
            lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
            lv_obj_t* obj = (lv_obj_t*)lv_event_get_target_obj(e);

            // if the cells are drawn
            if(base_dsc->part == LV_PART_ITEMS) {
                uint32_t row = base_dsc->id1;
                uint32_t col = base_dsc->id2;

                if(row == 0) {
                    lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
                    if(label_draw_dsc) {
                        label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
                    }
                    lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
                    if(fill_draw_dsc) {
                        fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), fill_draw_dsc->color, LV_OPA_20);
                        fill_draw_dsc->opa = LV_OPA_COVER;
                    }
				}
			}
		}

        static void draw_event_cb(lv_event_t * e) {
            lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
            lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
            lv_obj_t* obj = (lv_obj_t*)lv_event_get_target_obj(e);

            // if the cells are drawn
            if(base_dsc->part == LV_PART_ITEMS) {
                uint32_t row = base_dsc->id1;
                uint32_t col = base_dsc->id2;

                /*
                // Make the texts in the first cell center aligned
                if(row == 0) {
                    lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
                    if(label_draw_dsc) {
                        label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
                    }
                    lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
                    if(fill_draw_dsc) {
                        fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), fill_draw_dsc->color, LV_OPA_20);
                        fill_draw_dsc->opa = LV_OPA_COVER;
                    }
                }
                */
                /*
                // In the first column align the texts to the right
                else if(col == 0) {
                    lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
                    if(label_draw_dsc) {
                        label_draw_dsc->align = LV_TEXT_ALIGN_RIGHT;
                    }
                }
                */

                /*
                // Make every 2nd row grayish
                if((row != 0 && row % 2) == 0) {
                    lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
                    if(fill_draw_dsc) {
                        fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), fill_draw_dsc->color, LV_OPA_10);
                        //fill_draw_dsc->opa = LV_OPA_COVER;
                    }
                }
                */

                // highlight selected row
                if (row == (*(int*)lv_obj_get_user_data(obj))) {
                    lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
                    if(fill_draw_dsc) {
                        fill_draw_dsc->color = lv_palette_main(LV_PALETTE_BLUE);
                        //fill_draw_dsc->opa = LV_OPA_20;
                    }

                    // draw all cells center-aligned
                    lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
                    if(label_draw_dsc) {
						label_draw_dsc->color = LV_COLOR_MAKE(0x00, 0x00, 0x00);
                        label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
                    }
                }else{
                    // draw all cells center-aligned
                    lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
                    if(label_draw_dsc) {
						label_draw_dsc->color = LV_COLOR_MAKE(0xFA, 0xFA, 0xFA);
                        label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
                    }
                }
            }
        }

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
            // Header
            lv_table_set_column_count(objects.table_routing_fpga_header, 3);
            lv_table_set_column_width(objects.table_routing_fpga_header, 0, 300);
            lv_table_set_column_width(objects.table_routing_fpga_header, 1, 50);
            lv_table_set_column_width(objects.table_routing_fpga_header, 2, 300);
            lv_table_set_cell_value(objects.table_routing_fpga_header, 0, 0, "Source");
            lv_table_set_cell_value(objects.table_routing_fpga_header, 0, 2, "Destination");

            lv_obj_add_event_cb(objects.table_routing_fpga_header, draw_event_header_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
            lv_obj_add_flag(objects.table_routing_fpga_header, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

            // Selection-Table
            lv_table_set_row_count(objects.table_routing_fpga, NUM_OUTPUT_CHANNEL); // Not required but avoids a lot of memory reallocation lv_table_set_set_value
            lv_table_set_column_count(objects.table_routing_fpga, 3);
            lv_table_set_column_width(objects.table_routing_fpga, 0, 300);
            lv_table_set_column_width(objects.table_routing_fpga, 1, 50);
            lv_table_set_column_width(objects.table_routing_fpga, 2, 300);

            lv_obj_add_event_cb(objects.table_routing_fpga, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
            lv_obj_add_flag(objects.table_routing_fpga, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

            // store config pointer in user data for use in draw callback
            lv_obj_set_user_data(objects.table_routing_fpga, &gui_selected_item);

            DrawTable();
        }

        void OnShow() override
        {
            config->GetParameter(DISPLAY_ENCODER_1_BUTTON)->SetName(String(LV_SYMBOL_UP) + "\nBlock\n" + LV_SYMBOL_DOWN);
            config->GetParameter(DISPLAY_ENCODER_2_BUTTON)->SetName(String(LV_SYMBOL_UP) + "\nSelect\n" + LV_SYMBOL_DOWN);
            config->GetParameter(DISPLAY_ENCODER_3_BUTTON)->SetName(String(LV_SYMBOL_UP) + "\nSelect (Group)\n" + LV_SYMBOL_DOWN);
            config->GetParameter(DISPLAY_ENCODER_4_BUTTON)->SetName(String(LV_SYMBOL_REFRESH) + "\nSource");
            config->GetParameter(DISPLAY_ENCODER_5_BUTTON)->SetName(String(LV_SYMBOL_REFRESH) + "\nSource (Group)");
        }

        void OnChange(bool force_update) override
        {
			if (config->HasParameterChanged(DISPLAY_ENCODER_1_ENCODER))
            {
				gui_selected_block += config->GetInt(DISPLAY_ENCODER_1_ENCODER);
			}

            if (config->HasParameterChanged(DISPLAY_ENCODER_2_ENCODER))
            {
				gui_selected_item += config->GetInt(DISPLAY_ENCODER_2_ENCODER);
			}

            if (config->HasParameterChanged(DISPLAY_ENCODER_3_ENCODER))
            {
				int amount = config->GetInt(DISPLAY_ENCODER_3_ENCODER);

				if (amount < 0) {
					gui_selected_item -= 8;
				}else{
					gui_selected_item += 8;
				}
			}

            if (config->HasParameterChanged(DISPLAY_ENCODER_4_ENCODER))
            {
				int amount = config->GetInt(DISPLAY_ENCODER_4_ENCODER);
                config->Change(ROUTING_FPGA, amount, gui_selected_item + gui_items_offset);
            }

            if (config->HasParameterChanged(DISPLAY_ENCODER_5_ENCODER))
            {
                int amount = config->GetInt(DISPLAY_ENCODER_5_ENCODER);

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
            }

            if((gui_selected_block != gui_selected_block_before) || force_update)
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

            if((gui_selected_item_before != gui_selected_item) || force_update)
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
            
            if((config->HasParameterChanged(ROUTING_FPGA)) || force_update)
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

            try {
                for (uint8_t i = 0; i < gui_items_count; i++)
                {
                    lv_table_set_cell_value(objects.table_routing_fpga, i, 0, config->GetParameter(ROUTING_FPGA)->GetFormatedValue(i + gui_items_offset).c_str());
                    lv_table_set_cell_value(objects.table_routing_fpga, i, 2, mixer->fpga->GetOutputNameByIndex(i + 1 + gui_items_offset).c_str());
                }
            }catch (...){
            }

            lv_table_set_cell_value(objects.table_routing_fpga, gui_selected_item, 1, LV_SYMBOL_RIGHT);
        }
};