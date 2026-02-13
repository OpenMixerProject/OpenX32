#pragma once

#include "page.h"

using namespace std;

class PageRoutingDsp1: public Page
{
	using enum MP_ID;

	private:
		char inputChannelName[25] = "";
		char inputSourceName[25] = "";
		char tapPointName[15] = "";

		uint gui_selected_item = 0;
		uint gui_selected_item_before = 0;
        bool page_routing_dsp1_table_drawn = false;

    public:
        PageRoutingDsp1(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::ROUTING_FPGA;
            nextPage = X32_PAGE::ROUTING_DSP2;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 2;
        }

		void OnShow() override {
			custom_encoder[0].label = "\xEF\x81\xB7 Input \xEF\x81\xB8";
			custom_encoder[1].label = "\xEF\x81\xB7 Group \xEF\x81\xB8";
			custom_encoder[2].label =  "\xEF\x80\xA1 Source";
			custom_encoder[3].label =  "\xEF\x80\xA1 Group-Source";
			custom_encoder[4].label = "\xEF\x80\xA1 Tap";
		}

		void OnInit() override {
			if (gui_selected_item >= MAX_FPGA_TO_DSP1_CHANNELS) {
				gui_selected_item = 0;
			}else if (gui_selected_item < 0) {
				gui_selected_item = MAX_FPGA_TO_DSP1_CHANNELS - 1;
			}

			lv_table_set_row_count(objects.table_routing_dsp_input, MAX_FPGA_TO_DSP1_CHANNELS); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
			lv_table_set_column_count(objects.table_routing_dsp_input, 5); // Input | # | Source | # | Tap
			lv_table_set_column_width(objects.table_routing_dsp_input, 0, 200);
			lv_table_set_column_width(objects.table_routing_dsp_input, 1, 50);
			lv_table_set_column_width(objects.table_routing_dsp_input, 2, 200);
			lv_table_set_column_width(objects.table_routing_dsp_input, 3, 50);
			lv_table_set_column_width(objects.table_routing_dsp_input, 4, 100);
			for (uint8_t i=0; i < MAX_FPGA_TO_DSP1_CHANNELS; i++){
				mixer->dsp->RoutingGetInputNameByIndex(&inputChannelName[0], i+1);
				mixer->dsp->RoutingGetTapNameByIndex(&inputSourceName[0], mixer->dsp->Channel[i].input, mixer->fpga->fpgaRouting.dsp[mixer->dsp->Channel[i].input - 1]);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Channel[i].inputTapPoint);

				lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, i, 0, "%s", inputChannelName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, i, 2, "%s", inputSourceName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, i, 4, "%s", tapPointName);
			}

			lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item, 1, LV_SYMBOL_LEFT);
			lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item, 3, LV_SYMBOL_LEFT);
		}

		void OnChange(bool force_update) override {


			if(gui_selected_item_before != gui_selected_item) {

				if (gui_selected_item >= MAX_FPGA_TO_DSP1_CHANNELS) {
					gui_selected_item = 0;
				}else if (gui_selected_item < 0) {
					gui_selected_item = MAX_FPGA_TO_DSP1_CHANNELS - 1;
				}

				// remove old indicator
				lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item_before, 1, " ");
				lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item_before, 3, " ");
				
				// display new indicator
				lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item, 1, LV_SYMBOL_LEFT);
				lv_table_set_cell_value(objects.table_routing_dsp_input, gui_selected_item, 3, LV_SYMBOL_LEFT);
				
				// set select to scroll table
				lv_table_set_selected_cell(objects.table_routing_dsp_input, gui_selected_item, 2);
				
				gui_selected_item_before = gui_selected_item;
			} 
			
			if(config->HasParameterChanged(ROUTING_DSP1_SOURCE))
			{
				mixer->dsp->RoutingGetTapNameByIndex(&inputSourceName[0], mixer->dsp->Channel[gui_selected_item].input, mixer->fpga->fpgaRouting.dsp[mixer->dsp->Channel[gui_selected_item].input - 1]);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Channel[gui_selected_item].inputTapPoint);

				lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, gui_selected_item, 2, "%s", inputSourceName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, gui_selected_item, 4, "%s", tapPointName);
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
					mixer->ChangeDspInput(gui_selected_item, amount);
					break;
				case X32_ENC_ENCODER4:
					int8_t absoluteChange;
					if (amount < 0) {
						absoluteChange = -8;
					}else{
						absoluteChange = 8;
					}
					for (uint8_t i=gui_selected_item; i<(gui_selected_item+8); i++) {
						mixer->ChangeDspInput(i, absoluteChange);
					}
					break;
				case X32_ENC_ENCODER5:
					mixer->ChangeDspInputTapPoint(gui_selected_item, amount);
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