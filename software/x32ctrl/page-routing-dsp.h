#pragma once

#include "page.h"

using namespace std;
using enum MP_ID;

class PageRoutingDsp: public Page
{
	private:

		uint gui_selected_item = 0;
		uint gui_selected_item_before = 0;
        bool page_routing_dsp2_table_drawn = false;

    public:
        PageRoutingDsp(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::ROUTING_DSP2;
            nextPage = X32_PAGE::NONE;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 3;
        }

		void OnInit() override {
			if (gui_selected_item >= (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS)) {
				gui_selected_item = 0;
			}

			lv_table_set_row_count(objects.table_routing_dsp_output, (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS)); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
			lv_table_set_column_count(objects.table_routing_dsp_output, 5); // Input | # | Source | # | Tap | #
			lv_table_set_column_width(objects.table_routing_dsp_output, 0, 200);
			lv_table_set_column_width(objects.table_routing_dsp_output, 1, 50);
			lv_table_set_column_width(objects.table_routing_dsp_output, 2, 200);
			lv_table_set_column_width(objects.table_routing_dsp_output, 3, 50);
			lv_table_set_column_width(objects.table_routing_dsp_output, 4, 100);
			for (uint8_t i=0; i < MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS; i++)
			{
				lv_table_set_cell_value(objects.table_routing_dsp_output, i, 4, mixer->dsp->RoutingGetOutputNameByIndex(i+1).c_str());
				lv_table_set_cell_value(objects.table_routing_dsp_output, i, 0, config->GetParameter(ROUTING_DSP)->GetFormatedValue(i).c_str());
				lv_table_set_cell_value(objects.table_routing_dsp_output, i, 2, config->GetParameter(ROUTING_DSP_TAPPOINT)->GetFormatedValue(i).c_str());
			}

			lv_table_set_cell_value(objects.table_routing_dsp_output, gui_selected_item, 1, LV_SYMBOL_RIGHT);
			lv_table_set_cell_value(objects.table_routing_dsp_output, gui_selected_item, 3, LV_SYMBOL_RIGHT);

			BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_2, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_3, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_4, PAGE_CUSTOM_ENCODER);
			BindEncoder(DISPLAY_ENCODER_5, PAGE_CUSTOM_ENCODER);
		}

		void OnShow() override {
			custom_encoder[DISPLAY_ENCODER_4].label = "\xEF\x81\xB7 Output \xEF\x81\xB8";
			custom_encoder[DISPLAY_ENCODER_5].label = "\xEF\x81\xB7 Group \xEF\x81\xB8";
			custom_encoder[DISPLAY_ENCODER_1].label =  "\xEF\x80\xA1 Source";
			custom_encoder[DISPLAY_ENCODER_2].label =  "\xEF\x80\xA1 Group-Source";
			custom_encoder[DISPLAY_ENCODER_3].label = "\xEF\x80\xA1 Tap";
		}

		void OnChange(bool force_update) override
		{
			if(gui_selected_item_before != gui_selected_item)
			{
				if (gui_selected_item >= (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS))
				{
					gui_selected_item = (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS) - 1;
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
			
			if(config->HasParametersChanged({ROUTING_DSP, ROUTING_DSP_TAPPOINT}) || force_update)
			{
				for(auto const& index : config->GetChangedParameterIndexes({ROUTING_DSP, ROUTING_DSP_TAPPOINT}))
                {
					lv_table_set_cell_value(objects.table_routing_dsp_output, index, 0, config->GetParameter(ROUTING_DSP)->GetFormatedValue(index).c_str());
					lv_table_set_cell_value(objects.table_routing_dsp_output, index, 2, config->GetParameter(ROUTING_DSP_TAPPOINT)->GetFormatedValue(index).c_str());
				}
			}
		}

        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override {
            switch (encoder){
				case X32_ENC_ENCODER4:
					gui_selected_item += amount;
					OnChange(false);
					break;
				case X32_ENC_ENCODER5:
					if (amount < 0) {
						gui_selected_item -= 8;
					}else{
						gui_selected_item += 8;
					}
					OnChange(false);
					break;
				case X32_ENC_ENCODER1:
					config->Change(ROUTING_DSP, amount, gui_selected_item);
					break;
				case X32_ENC_ENCODER2:
					int8_t absoluteChange;
					if (amount < 0) {
						absoluteChange = -8;
					}else{
						absoluteChange = 8;
					}
					for (uint8_t i=gui_selected_item; i<(gui_selected_item+8); i++) {
						config->Change(ROUTING_DSP, amount, i);
					}
					break;
				case X32_ENC_ENCODER3:
					config->Change(ROUTING_DSP_TAPPOINT, amount, gui_selected_item);
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