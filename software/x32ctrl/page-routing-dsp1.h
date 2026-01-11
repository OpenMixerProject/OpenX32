#pragma once

#include "page.h"

using namespace std;

class PageRoutingDsp1: public Page {
    public:
        PageRoutingDsp1(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE_ROUTING_FPGA;
            nextPage = X32_PAGE_ROUTING_DSP2;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 2;
        }

		void OnShow() override {
			SetEncoderText("\xEF\x81\xB7 Input \xEF\x81\xB8", "\xEF\x81\xB7 Group \xEF\x81\xB8", "\xEF\x80\xA1 Source", "\xEF\x80\xA1 Group-Source", "\xEF\x80\xA1 Tap", "-");
		}

		void OnInit() override {
			if (state->gui_selected_item >= MAX_FPGA_TO_DSP1_CHANNELS) {
				state->gui_selected_item = 0;
			}else if (state->gui_selected_item < 0) {
				state->gui_selected_item = MAX_FPGA_TO_DSP1_CHANNELS - 1;
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

			lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
			lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_selected_item, 3, LV_SYMBOL_LEFT);
		}

		void OnChange(bool force_update) override {
			if(state->HasChanged(X32_MIXER_CHANGED_GUI_SELECT)) {
				if (state->gui_selected_item >= MAX_FPGA_TO_DSP1_CHANNELS) {
					state->gui_selected_item = 0;
				}else if (state->gui_selected_item < 0) {
					state->gui_selected_item = MAX_FPGA_TO_DSP1_CHANNELS - 1;
				}

				if (state->gui_selected_item != state->gui_old_selected_item ) {
					// remove old indicator
					lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_old_selected_item, 1, " ");
					lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_old_selected_item, 3, " ");
					
					// display new indicator
					lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
					lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_selected_item, 3, LV_SYMBOL_LEFT);
					
					// set select to scroll table
					lv_table_set_selected_cell(objects.table_routing_dsp_input, state->gui_selected_item, 2);
					
					state->gui_old_selected_item = state->gui_selected_item;
				}
			} 
			
			if(state->HasChanged(X32_MIXER_CHANGED_ROUTING)){
				mixer->dsp->RoutingGetTapNameByIndex(&inputSourceName[0], mixer->dsp->Channel[state->gui_selected_item].input, mixer->fpga->fpgaRouting.dsp[mixer->dsp->Channel[state->gui_selected_item].input - 1]);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Channel[state->gui_selected_item].inputTapPoint);

				lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, state->gui_selected_item, 2, "%s", inputSourceName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, state->gui_selected_item, 4, "%s", tapPointName);
			}
		}

        void OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) override {
            switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeGuiSelection(amount);
					break;
				case X32_ENC_ENCODER2:
					if (amount < 0) {
						mixer->ChangeGuiSelection(-8);
					}else{
						mixer->ChangeGuiSelection(8);
					}
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangeDspInput(state->gui_selected_item, amount);
					break;
				case X32_ENC_ENCODER4:
					int8_t absoluteChange;
					if (amount < 0) {
						absoluteChange = -8;
					}else{
						absoluteChange = 8;
					}
					for (uint8_t i=state->gui_selected_item; i<(state->gui_selected_item+8); i++) {
						mixer->ChangeDspInput(i, absoluteChange);
					}
					break;
				case X32_ENC_ENCODER5:
					mixer->ChangeDspInputTapPoint(state->gui_selected_item, amount);
					break;
				case X32_ENC_ENCODER6:
					break;
				default:  
					// just here to avoid compiler warnings                  
					break;
			}
        }

		private:
			char inputChannelName[25] = "";
			char inputSourceName[25] = "";
			char tapPointName[15] = "";
};