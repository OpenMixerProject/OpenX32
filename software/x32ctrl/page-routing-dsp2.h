#pragma once

#include "page.h"

using namespace std;

class PageRoutingDsp2: public Page {
    public:
        PageRoutingDsp2(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE_ROUTING_DSP2;
            nextPage = X32_PAGE_NONE;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 3;
        }

		void OnInit() override {
			if (state->gui_selected_item >= (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS)) {
				state->gui_selected_item = 0;
			}else if (state->gui_selected_item < 0) {
				state->gui_selected_item = (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS) - 1;
			}

			lv_table_set_row_count(objects.table_routing_dsp_output, (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS)); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
			lv_table_set_column_count(objects.table_routing_dsp_output, 5); // Input | # | Source | # | Tap | #
			lv_table_set_column_width(objects.table_routing_dsp_output, 0, 200);
			lv_table_set_column_width(objects.table_routing_dsp_output, 1, 50);
			lv_table_set_column_width(objects.table_routing_dsp_output, 2, 200);
			lv_table_set_column_width(objects.table_routing_dsp_output, 3, 50);
			lv_table_set_column_width(objects.table_routing_dsp_output, 4, 100);
			for (uint8_t i=0; i < MAX_DSP1_TO_FPGA_CHANNELS; i++){
				mixer->dsp->RoutingGetOutputNameByIndex(&outputChannelName[0], i+1);
				mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], mixer->dsp->Dsp1toFpga[i].input, mixer->dsp->Channel[i].input);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Dsp1toFpga[i].tapPoint);

				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i, 0, "%s", outputChannelName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i, 2, "%s", outputSourceName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i, 4, "%s", tapPointName);
			}

			for (uint8_t i=0; i < MAX_DSP1_TO_DSP2_CHANNELS; i++){
				mixer->dsp->RoutingGetOutputNameByIndex(&outputChannelName[0], i+MAX_DSP1_TO_FPGA_CHANNELS+1);
				mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], mixer->dsp->Dsp1toDsp2Routing[i].input, 0);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Dsp1toDsp2Routing[i].tapPoint);

				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i+MAX_DSP1_TO_FPGA_CHANNELS, 0, "%s", outputChannelName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i+MAX_DSP1_TO_FPGA_CHANNELS, 2, "%s", outputSourceName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i+MAX_DSP1_TO_FPGA_CHANNELS, 4, "%s", tapPointName);
			}

			lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
			lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_selected_item, 3, LV_SYMBOL_LEFT);
		}

		void OnShow() override {
			SetEncoderText("\xEF\x81\xB7 Output \xEF\x81\xB8", "\xEF\x81\xB7 Group \xEF\x81\xB8", "\xEF\x80\xA1 Source", "\xEF\x80\xA1 Group-Source", "\xEF\x80\xA1 Tap", "-");
		}

		void OnChange() override {
			if(state->HasChanged(X32_MIXER_CHANGED_GUI_SELECT)) {
				if (state->gui_selected_item >= (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS)) {
					state->gui_selected_item = 0;
				}else if (state->gui_selected_item < 0) {
					state->gui_selected_item = (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS) - 1;
				}

				if (state->gui_selected_item != state->gui_old_selected_item ) {
					// remove old indicator
					lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_old_selected_item, 1, " ");
					lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_old_selected_item, 3, " ");
					
					// display new indicator
					lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
					lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_selected_item, 3, LV_SYMBOL_LEFT);
					
					// set select to scroll table
					lv_table_set_selected_cell(objects.table_routing_dsp_output, state->gui_selected_item, 2);
					
					state->gui_old_selected_item = state->gui_selected_item;
				}
			} 
			
			if(state->HasChanged(X32_MIXER_CHANGED_ROUTING)){
				if (state->gui_selected_item < MAX_DSP1_TO_FPGA_CHANNELS) {
					mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], mixer->dsp->Dsp1toFpga[state->gui_selected_item].input, mixer->dsp->Channel[state->gui_selected_item].input);
					mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Dsp1toFpga[state->gui_selected_item].tapPoint);
				}else if ((state->gui_selected_item >= MAX_DSP1_TO_FPGA_CHANNELS) && (state->gui_selected_item < (MAX_DSP1_TO_FPGA_CHANNELS+MAX_DSP1_TO_DSP2_CHANNELS))) {
					/*
						// dspSource-Signals
						// 0		DSP_BUF_IDX_OFF
						// 1-33		DSP-Input 1-32 from FPGA
						// 33-40	AUX-Input 1-8 from FPGA
						// 41-56	FX-Return 1-8 from DSP2
						// 57-72	Mixbus 1-16 (internal)
						// 73-75	Main Left, Right, Sub (internal)
						// 76-81	Matrix 1-6 (internal)
						// 82-89	FX-Aux-Channel 1-8 from DSP2
						// 90-92	Monitor Left, Right, Talkback (internal)
					*/
					uint8_t dspOutputSourceIndex = mixer->dsp->Dsp1toDsp2Routing[state->gui_selected_item-MAX_DSP1_TO_FPGA_CHANNELS].input;

					if (dspOutputSourceIndex <= 40) {
						// we are using one of the DSP-channels -> check if it is routed from FPGA
						if (mixer->dsp->Channel[dspOutputSourceIndex - 1].input < 40) {
							// routed from FPGA
							mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], dspOutputSourceIndex, mixer->fpga->fpgaRouting.dsp[dspOutputSourceIndex - 1]); // stringBuffer, dspSource, fpgaSource
						}else{
							// routed to internal DSP-signal
							mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], dspOutputSourceIndex, mixer->dsp->Channel[dspOutputSourceIndex - 1].input + MAX_DSP1_TO_FPGA_CHANNELS); // stringBuffer, dspSource, dspSource
						}
					}else{
						mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], dspOutputSourceIndex, 0); // DSP-internal signal without an FPGA-source
					}
					mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Dsp1toDsp2Routing[state->gui_selected_item-MAX_DSP1_TO_FPGA_CHANNELS].tapPoint);
				}

				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, state->gui_selected_item, 2, "%s", outputSourceName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, state->gui_selected_item, 4, "%s", tapPointName);
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
					if (state->gui_selected_item < MAX_DSP1_TO_FPGA_CHANNELS) {
						mixer->ChangeDspOutput(state->gui_selected_item, amount);
					}else if ((state->gui_selected_item >= MAX_DSP1_TO_FPGA_CHANNELS) && (state->gui_selected_item < (MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS))) {
						mixer->ChangeDspFxOutput(state->gui_selected_item - MAX_DSP1_TO_FPGA_CHANNELS, amount);
					}
					break;
				case X32_ENC_ENCODER4:
					int8_t absoluteChange;
					if (amount < 0) {
						absoluteChange = -8;
					}else{
						absoluteChange = 8;
					}
					for (uint8_t i=state->gui_selected_item; i<(state->gui_selected_item+8); i++) {
						if (i < MAX_DSP1_TO_FPGA_CHANNELS) {
							mixer->ChangeDspOutput(i, absoluteChange);
						}else if ((i >= MAX_DSP1_TO_FPGA_CHANNELS) && (i < (MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS))) {
							mixer->ChangeDspFxOutput(i - MAX_DSP1_TO_FPGA_CHANNELS, absoluteChange);
						}
					}
					break;
				case X32_ENC_ENCODER5:
					if (state->gui_selected_item < MAX_DSP1_TO_FPGA_CHANNELS) {
						mixer->ChangeDspOutputTapPoint(state->gui_selected_item, amount);
					}else if ((state->gui_selected_item >= MAX_DSP1_TO_FPGA_CHANNELS) && (state->gui_selected_item < (MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS))) {
						mixer->ChangeDspFxOutputTapPoint(state->gui_selected_item - MAX_DSP1_TO_FPGA_CHANNELS, amount);
					}
					break;
				case X32_ENC_ENCODER6:
					break;
				default:  
					// just here to avoid compiler warnings                  
					break;
			}
        }

		private:
			char outputChannelName[25] = "";
			char outputSourceName[25] = "";
			char tapPointName[15] = "";
};