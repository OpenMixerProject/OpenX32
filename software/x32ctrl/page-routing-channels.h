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

    public:
        PageRoutingChannels(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::ROUTING_FPGA;
            nextPage = X32_PAGE::ROUTING_DSP2;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 2;
        }

		void OnInit() override
		{
			// Header
            lv_table_set_column_count(objects.table_routing_dsp_input_header, 5);
            lv_table_set_column_width(objects.table_routing_dsp_input_header, 0, 200);
            lv_table_set_column_width(objects.table_routing_dsp_input_header, 1, 50);
            lv_table_set_column_width(objects.table_routing_dsp_input_header, 2, 100);
			lv_table_set_column_width(objects.table_routing_dsp_input_header, 3, 50);
			lv_table_set_column_width(objects.table_routing_dsp_input_header, 4, 200);
            lv_table_set_cell_value(objects.table_routing_dsp_input_header, 0, 0, "Source");
			lv_table_set_cell_value(objects.table_routing_dsp_input_header, 0, 2, "Tappoint");
            lv_table_set_cell_value(objects.table_routing_dsp_input_header, 0, 4, "Destination");

			lv_table_set_row_count(objects.table_routing_dsp_input, 40); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
			lv_table_set_column_count(objects.table_routing_dsp_input, 5); // Input | # | Source | # | Tap
			lv_table_set_column_width(objects.table_routing_dsp_input, 0, 300);
			lv_table_set_column_width(objects.table_routing_dsp_input, 1, 50);
			lv_table_set_column_width(objects.table_routing_dsp_input, 2, 100);
			lv_table_set_column_width(objects.table_routing_dsp_input, 3, 50);
			lv_table_set_column_width(objects.table_routing_dsp_input, 4, 200);
			for (uint8_t i=0; i < 40; i++)
			{
				String inputChannelName;				
				if ((i+1 >= DSP_BUF_IDX_DSPCHANNEL) && (i+1 < (DSP_BUF_IDX_DSPCHANNEL + 32)))
				{
        			inputChannelName = String("Channel ") + (i + 1);
					
				}
				else if ((i+1 >= DSP_BUF_IDX_AUX) && (i+1 < (DSP_BUF_IDX_AUX + 8)))
				{
        			inputChannelName = String("Aux ") + (i + 1 - 32);
				}

				lv_table_set_cell_value(objects.table_routing_dsp_input, i, 0, (config->GetParameter(ROUTING_FPGA)->GetFormatedValue(72 + i) + " -> " + config->GetParameter(ROUTING_DSP_INPUT)->GetFormatedValue(i)).c_str());
				lv_table_set_cell_value(objects.table_routing_dsp_input, i, 2, config->GetParameter(ROUTING_DSP_INPUT_TAPPOINT)->GetFormatedValue(i).c_str());
				lv_table_set_cell_value(objects.table_routing_dsp_input, i, 4, inputChannelName.c_str());
			}

			lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item, 1, LV_SYMBOL_RIGHT);
			lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item, 3, LV_SYMBOL_RIGHT);

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

		void OnChange(bool force_update) override {
			if(gui_selected_item_before != gui_selected_item) {
				if (gui_selected_item < 0) {
					// limit list at the top
					gui_selected_item = 0;
				}else if (gui_selected_item >= 40) {
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
					config->Change(ROUTING_DSP_INPUT, amount, gui_selected_item);
					break;
				case X32_ENC_ENCODER4:
					int8_t absoluteChange;
					if (amount < 0) {
						absoluteChange = -8;
					}else{
						absoluteChange = 8;
					}
					for (uint8_t i=gui_selected_item; i<(gui_selected_item+8); i++) {
						config->Change(ROUTING_DSP_INPUT, absoluteChange, i);
					}
					break;
				case X32_ENC_ENCODER5:
					config->Change(ROUTING_DSP_INPUT_TAPPOINT, amount, gui_selected_item);
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