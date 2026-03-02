#pragma once

#include "page.h"

using namespace std;
using enum MP_ID;

class PageRoutingDsp: public Page
{
	private:

		int gui_selected_item = 1;
		int gui_selected_item_before = 1;
        bool page_routing_dsp2_table_drawn = false;

        static void draw_event_cb(lv_event_t * e) {
            lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
            lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
            lv_obj_t* obj = (lv_obj_t*)lv_event_get_target_obj(e);

            // if the cells are drawn
            if(base_dsc->part == LV_PART_ITEMS) {
                uint32_t row = base_dsc->id1;
                uint32_t col = base_dsc->id2;

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
                // In the first column align the texts to the right
                else if(col == 0) {
                    lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
                    if(label_draw_dsc) {
                        label_draw_dsc->align = LV_TEXT_ALIGN_RIGHT;
                    }
                }

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
                }
            }
        }

    public:
        PageRoutingDsp(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::ROUTING_DSP2;
            nextPage = X32_PAGE::NONE;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 3;
			led = X32_BTN_ROUTING;
        }

		void OnInit() override {
			if (gui_selected_item > (MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS)) {
				gui_selected_item = 1;
			}

			lv_table_set_row_count(objects.table_routing_dsp_output, (MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS + 1)); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
			lv_table_set_column_count(objects.table_routing_dsp_output, 5); // Input | # | Source | # | Tap | #
			lv_table_set_column_width(objects.table_routing_dsp_output, 0, 300);
			lv_table_set_column_width(objects.table_routing_dsp_output, 1, 50);
			lv_table_set_column_width(objects.table_routing_dsp_output, 2, 200);
			lv_table_set_column_width(objects.table_routing_dsp_output, 3, 50);
			lv_table_set_column_width(objects.table_routing_dsp_output, 4, 200);
			for (uint8_t i=0; i < MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS; i++)
			{
				lv_table_set_cell_value(objects.table_routing_dsp_output, i, 0, config->GetParameter(ROUTING_DSP_OUTPUT)->GetFormatedValue(i).c_str());
				lv_table_set_cell_value(objects.table_routing_dsp_output, i, 2, config->GetParameter(ROUTING_DSP_OUTPUT_TAPPOINT)->GetFormatedValue(i).c_str());
				lv_table_set_cell_value(objects.table_routing_dsp_output, i, 4, mixer->dsp->RoutingGetOutputNameByIndex(i+1).c_str());
			}

            lv_table_set_cell_value(objects.table_routing_dsp_output, 0, 0, "Source");
			lv_table_set_cell_value(objects.table_routing_dsp_output, 0, 2, "Tappoint");
            lv_table_set_cell_value(objects.table_routing_dsp_output, 0, 4, "DSP-Output");
			lv_table_set_cell_value(objects.table_routing_dsp_output, gui_selected_item, 1, LV_SYMBOL_RIGHT);
			lv_table_set_cell_value(objects.table_routing_dsp_output, gui_selected_item, 3, LV_SYMBOL_RIGHT);

            lv_obj_add_event_cb(objects.table_routing_dsp_output, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
            lv_obj_add_flag(objects.table_routing_dsp_output, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

            // store config pointer in user data for use in draw callback
            lv_obj_set_user_data(objects.table_routing_dsp_output, &gui_selected_item);

			BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_2, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_3, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_4, PAGE_CUSTOM_ENCODER);
			BindEncoder(DISPLAY_ENCODER_5, PAGE_CUSTOM_ENCODER);
		}

		void OnShow() override {
			custom_encoder[DISPLAY_ENCODER_1].label = "\xEF\x81\xB7 Select \xEF\x81\xB8";
			custom_encoder[DISPLAY_ENCODER_2].label = "\xEF\x81\xB7 Select (Group) \xEF\x81\xB8";
			custom_encoder[DISPLAY_ENCODER_3].label = "\xEF\x80\xA1 Source";
			custom_encoder[DISPLAY_ENCODER_4].label = "\xEF\x80\xA1 Source (Group)";
			custom_encoder[DISPLAY_ENCODER_5].label = "\xEF\x80\xA1 Tap";
		}

		void OnChange(bool force_update) override
		{
			if(gui_selected_item_before != gui_selected_item)
			{
				if (gui_selected_item < 1) {
					// limit list at the top
					gui_selected_item = 1;
				}else if (gui_selected_item > (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS)) {
					// limit list at the bottom
					gui_selected_item = (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS);
				}

				// remove old indicator
				lv_table_set_cell_value(objects.table_routing_dsp_output, gui_selected_item_before, 1, " ");
				lv_table_set_cell_value(objects.table_routing_dsp_output, gui_selected_item_before, 3, " ");
				
				// display new indicator
				lv_table_set_cell_value(objects.table_routing_dsp_output, gui_selected_item, 1, LV_SYMBOL_RIGHT);
				lv_table_set_cell_value(objects.table_routing_dsp_output, gui_selected_item, 3, LV_SYMBOL_RIGHT);
				
				// set select to scroll table
				lv_table_set_selected_cell(objects.table_routing_dsp_output, gui_selected_item, 2);
				
				gui_selected_item_before = gui_selected_item;
			} 
			
			if(config->HasParametersChanged({ROUTING_DSP_OUTPUT, ROUTING_DSP_OUTPUT_TAPPOINT}) || force_update)
			{
				for(auto const& index : config->GetChangedParameterIndexes({ROUTING_DSP_OUTPUT, ROUTING_DSP_OUTPUT_TAPPOINT}))
                {
					if ((config->GetUint(ROUTING_DSP_OUTPUT, index) >= 1) && (config->GetUint(ROUTING_DSP_OUTPUT, index) <= 40)) {
						// external signal from FPGA
						lv_table_set_cell_value(objects.table_routing_dsp_output, index, 0, (config->GetParameter(ROUTING_FPGA)->GetFormatedValue(FPGA_OUTPUT_IDX_DSP - 1 + config->GetUint(ROUTING_DSP_OUTPUT, index) - 1) + " -> " + config->GetParameter(ROUTING_DSP_OUTPUT)->GetFormatedValue(index)).c_str());
					}else{
						// internal signal within DSP
						lv_table_set_cell_value(objects.table_routing_dsp_output, index, 0, config->GetParameter(ROUTING_DSP_OUTPUT)->GetFormatedValue(index).c_str());
					}
					lv_table_set_cell_value(objects.table_routing_dsp_output, index, 2, config->GetParameter(ROUTING_DSP_OUTPUT_TAPPOINT)->GetFormatedValue(index).c_str());
				}
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
					config->Change(ROUTING_DSP_OUTPUT, amount, gui_selected_item - 1);
					break;
				case X32_ENC_ENCODER4:
					int8_t absoluteChange;
					if (amount < 0) {
						absoluteChange = -8;
					}else{
						absoluteChange = 8;
					}
					for (uint8_t i = (gui_selected_item - 1); i < (gui_selected_item - 1 + 8); i++) {
						config->Change(ROUTING_DSP_OUTPUT, absoluteChange, i);
					}
					break;
				case X32_ENC_ENCODER5:
					config->Change(ROUTING_DSP_OUTPUT_TAPPOINT, amount, gui_selected_item - 1);
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