#pragma once

#include "page.h"

using namespace std;

class PageRoutingChannels: public Page
{
	using enum MP_ID;

	private:
		// char inputChannelName[25] = "";
		// char inputSourceName[25] = "";
		// char tapPointName[15] = "";

		int gui_selected_item = 0;
		int gui_selected_item_before = 0;
        bool page_routing_dsp1_table_drawn = false;

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
        PageRoutingChannels(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::ROUTING_FPGA;
            nextPage = X32_PAGE::ROUTING_DSP2;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 2;
			led = X32_BTN_ROUTING;
        }

		void OnInit() override
		{
			// Header
            lv_table_set_column_count(objects.table_routing_dsp_input_header, 5);
            lv_table_set_column_width(objects.table_routing_dsp_input_header, 0, 400);
            lv_table_set_column_width(objects.table_routing_dsp_input_header, 1, 50);
            lv_table_set_column_width(objects.table_routing_dsp_input_header, 2, 100);
			lv_table_set_column_width(objects.table_routing_dsp_input_header, 3, 50);
			lv_table_set_column_width(objects.table_routing_dsp_input_header, 4, 200);
            lv_table_set_cell_value(objects.table_routing_dsp_input_header, 0, 0, "Source");
			lv_table_set_cell_value(objects.table_routing_dsp_input_header, 0, 2, "Tappoint");
            lv_table_set_cell_value(objects.table_routing_dsp_input_header, 0, 4, "Destination");

            lv_obj_add_event_cb(objects.table_routing_dsp_input_header, draw_event_header_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
            lv_obj_add_flag(objects.table_routing_dsp_input_header, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

			// Selection-Table
			lv_table_set_row_count(objects.table_routing_dsp_input, 40); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
			lv_table_set_column_count(objects.table_routing_dsp_input, 5); // Input | # | Source | # | Tap
			lv_table_set_column_width(objects.table_routing_dsp_input, 0, 400);
			lv_table_set_column_width(objects.table_routing_dsp_input, 1, 50);
			lv_table_set_column_width(objects.table_routing_dsp_input, 2, 100);
			lv_table_set_column_width(objects.table_routing_dsp_input, 3, 50);
			lv_table_set_column_width(objects.table_routing_dsp_input, 4, 200);
			for (uint8_t i = 0; i < 40; i++)
			{
				String inputChannelName;				
				if (((i + 1) >= DSP_BUF_IDX_DSPCHANNEL) && ((i + 1) < (DSP_BUF_IDX_DSPCHANNEL + 32)))
				{
        			inputChannelName = String("Channel ") + (i + 1);
					
				}
				else if (((i + 1) >= DSP_BUF_IDX_AUX) && ((i + 1) < (DSP_BUF_IDX_AUX + 8)))
				{
        			inputChannelName = String("Aux ") + (i + 1 - 32);
				}

				lv_table_set_cell_value(objects.table_routing_dsp_input, i, 0, (config->GetParameter(ROUTING_FPGA)->GetFormatedValue(72 + i) + " -> " + config->GetParameter(ROUTING_DSP_INPUT)->GetFormatedValue(i)).c_str());
				lv_table_set_cell_value(objects.table_routing_dsp_input, i, 2, config->GetParameter(ROUTING_DSP_INPUT_TAPPOINT)->GetFormatedValue(i).c_str());
				lv_table_set_cell_value(objects.table_routing_dsp_input, i, 4, inputChannelName.c_str());
			}

			lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item, 1, LV_SYMBOL_RIGHT);
			lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item, 3, LV_SYMBOL_RIGHT);

            lv_obj_add_event_cb(objects.table_routing_dsp_input, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
            lv_obj_add_flag(objects.table_routing_dsp_input, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

            // store config pointer in user data for use in draw callback
            lv_obj_set_user_data(objects.table_routing_dsp_input, &gui_selected_item);
		}

		void OnShow() override
		{
			config->GetParameter(DISPLAY_ENCODER_1_BUTTON)->SetName(String(LV_SYMBOL_UP) + "\nSelect\n" + LV_SYMBOL_DOWN);
            config->GetParameter(DISPLAY_ENCODER_2_BUTTON)->SetName(String(LV_SYMBOL_UP) + "\nSelect (Group)\n" + LV_SYMBOL_DOWN);
            config->GetParameter(DISPLAY_ENCODER_3_BUTTON)->SetName(String(LV_SYMBOL_REFRESH) + "\nSource");
            config->GetParameter(DISPLAY_ENCODER_4_BUTTON)->SetName(String(LV_SYMBOL_REFRESH) + "\nSource (Group)");
            config->GetParameter(DISPLAY_ENCODER_5_BUTTON)->SetName(String(LV_SYMBOL_REFRESH) + "\nTAP");
		}

		void OnChange(bool force_update) override 
		{
			if (config->HasParameterChanged(DISPLAY_ENCODER_1_ENCODER))
            {
				gui_selected_item += config->GetInt(DISPLAY_ENCODER_1_ENCODER);
			}

			if (config->HasParameterChanged(DISPLAY_ENCODER_2_ENCODER))
            {
				int amount = config->GetInt(DISPLAY_ENCODER_2_ENCODER);

				if (amount < 0) {
					gui_selected_item -= 8;
				}else{
					gui_selected_item += 8;
				}
			}

			if (config->HasParameterChanged(DISPLAY_ENCODER_3_ENCODER))
            {
				int amount = config->GetInt(DISPLAY_ENCODER_3_ENCODER);
				config->Change(ROUTING_DSP_INPUT, amount, gui_selected_item);
			}

			if (config->HasParameterChanged(DISPLAY_ENCODER_4_ENCODER))
            {	
				int amount = config->GetInt(DISPLAY_ENCODER_4_ENCODER);

				int8_t absoluteChange;
				if (amount < 0) {
					absoluteChange = -8;
				}else{
					absoluteChange = 8;
				}
				for (uint8_t i = (gui_selected_item); i < (gui_selected_item + 8); i++) {
					config->Change(ROUTING_DSP_INPUT, absoluteChange, i);
				}
			}

			if (config->HasParameterChanged(DISPLAY_ENCODER_5_ENCODER))
            {	
				int amount = config->GetInt(DISPLAY_ENCODER_5_ENCODER);
				config->Change(ROUTING_DSP_INPUT_TAPPOINT, amount, gui_selected_item);
			}

			if(gui_selected_item_before != gui_selected_item)
			{
				if (gui_selected_item < 0)
				{
					// limit list at the top
					gui_selected_item = 0;
				}
				else if (gui_selected_item >= 40) 
				{
					// limit list at the bottom
					gui_selected_item = 40 - 1;
				}

				// remove old indicator
				lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item_before, 1, " ");
				lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item_before, 3, " ");
				
				// display new indicator
				lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item, 1, LV_SYMBOL_RIGHT);
				lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item, 3, LV_SYMBOL_RIGHT);
				
				// set select to scroll table
				lv_table_set_selected_cell(objects.table_routing_dsp_input, gui_selected_item, 2);
				
				gui_selected_item_before = gui_selected_item;
			} 
			
			if(config->HasParametersChanged({ROUTING_DSP_INPUT, ROUTING_DSP_INPUT_TAPPOINT}) || force_update)
			{
				for(auto const& index : config->GetChangedParameterIndexes({ROUTING_DSP_INPUT, ROUTING_DSP_INPUT_TAPPOINT}))
                {
					if ((config->GetUint(ROUTING_DSP_INPUT, index) >= 1) && (config->GetUint(ROUTING_DSP_INPUT, index) <= 40)) {
						// external signal from FPGA
						lv_table_set_cell_value(objects.table_routing_dsp_input, index, 0, (config->GetParameter(ROUTING_FPGA)->GetFormatedValue(FPGA_OUTPUT_IDX_DSP - 1 + index) + " -> " + config->GetParameter(ROUTING_DSP_INPUT)->GetFormatedValue(index)).c_str());
					}else{
						// internal signal within DSP
						lv_table_set_cell_value(objects.table_routing_dsp_input, index, 0, config->GetParameter(ROUTING_DSP_INPUT)->GetFormatedValue(index).c_str());
					}
					lv_table_set_cell_value(objects.table_routing_dsp_input, index, 2, config->GetParameter(ROUTING_DSP_INPUT_TAPPOINT)->GetFormatedValue(index).c_str());
				}
			}
		}
};