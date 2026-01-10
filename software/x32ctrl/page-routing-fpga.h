#pragma once

#include "page.h"

using namespace std;

class PageRoutingFpga: public Page {
    public:
        PageRoutingFpga(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE_ROUTING;
            nextPage = X32_PAGE_ROUTING_DSP1;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 1;
        }

        void OnInit() override {
            if (state->gui_selected_item >= NUM_OUTPUT_CHANNEL) {
                state->gui_selected_item = 0;
            }else if (state->gui_selected_item < 0) {
                state->gui_selected_item = NUM_OUTPUT_CHANNEL - 1;
            }

            lv_table_set_row_count(objects.table_routing_fpga, NUM_OUTPUT_CHANNEL); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
            lv_table_set_column_count(objects.table_routing_fpga, 3);
            lv_table_set_column_width(objects.table_routing_fpga, 0, 200);
            lv_table_set_column_width(objects.table_routing_fpga, 1, 50);
            lv_table_set_column_width(objects.table_routing_fpga, 2, 200);
            for (uint8_t i=0; i < NUM_OUTPUT_CHANNEL; i++){
                routingIndex = mixer->fpga->GetOutputByIndex(i+1);
                lv_table_set_cell_value_fmt(objects.table_routing_fpga, i, 0, "%s", mixer->fpga->GetOutputNameByIndex(i+1).c_str());
                lv_table_set_cell_value_fmt(objects.table_routing_fpga, i, 2, "%s", mixer->fpga->GetInputNameByIndex(routingIndex).c_str());
            }
            lv_table_set_cell_value(objects.table_routing_fpga, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
            state->page_routing_fpga_table_drawn = true;
        }

        void OnShow() override {
            SetEncoderText("\xEF\x81\xB7 Target \xEF\x81\xB8", "\xEF\x81\xB7 Group \xEF\x81\xB8", "\xEF\x80\xA1 Source", "\xEF\x80\xA1 Group-Source", "-", "-");
        }

        void OnChange() override {
            
            if(state->HasChanged(X32_MIXER_CHANGED_GUI_SELECT)) {
                if (state->gui_selected_item >= NUM_OUTPUT_CHANNEL) {
                    state->gui_selected_item = 0;
                }else if (state->gui_selected_item < 0) {
                    state->gui_selected_item = NUM_OUTPUT_CHANNEL - 1;
                }

                if (state->gui_selected_item != state->gui_old_selected_item ) {
                    // remove old indicator
                    lv_table_set_cell_value(objects.table_routing_fpga, state->gui_old_selected_item, 1, " ");
                    
                    // display new indicator
                    lv_table_set_cell_value(objects.table_routing_fpga, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
                    
                    // set select to scroll table
                    lv_table_set_selected_cell(objects.table_routing_fpga, state->gui_selected_item, 2);
                    
                    state->gui_old_selected_item = state->gui_selected_item;
                }
            } 
            
            if(state->HasChanged(X32_MIXER_CHANGED_ROUTING)){
                routingIndex = mixer->fpga->GetOutputByIndex(state->gui_selected_item+1);
                lv_table_set_cell_value_fmt(objects.table_routing_fpga, state->gui_selected_item, 2, "%s", mixer->fpga->GetInputNameByIndex(routingIndex).c_str());
            }
        }

        void OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) {
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
                    mixer->ChangeHardwareInput(state->gui_selected_item, amount);
                    break;
                case X32_ENC_ENCODER4:
                    int8_t absoluteChange;
                    if (amount < 0) {
                        absoluteChange = -8;
                    }else{
                        absoluteChange = 8;
                    }
                    for (uint8_t i=state->gui_selected_item; i<(state->gui_selected_item+8); i++) {
                        mixer->ChangeHardwareInput(i, absoluteChange);
                    }
                    break;
                case X32_ENC_ENCODER5:
                    break;
                case X32_ENC_ENCODER6:
                    break;
                default:  
                    // just here to avoid compiler warnings                  
                    break;
            }
        }

        private:
            char outputDestinationName[15] = "";
            char inputSourceName[15] = "";
            uint8_t routingIndex = 0;
};