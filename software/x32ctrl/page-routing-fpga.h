#pragma once

#include "page.h"

using namespace std;

class PageRoutingFpga: public Page
{
    using enum MP_ID;

    private:

        char* button_map[47];
        uint selectionindex = 0;

        int gui_selected_block = 0;
		int gui_selected_block_before = 0;

        int gui_selected_block_item = 0;
		int gui_selected_block_item_before = 0;

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
            lv_buttonmatrix_set_one_checked(objects.routing_fpga_matrix, true);
        }

        void OnShow() override
        {
            uint map_pointer = 0;

            for (uint i=0; i < 40; i++)
            {
                // newline after 8 elements
                if (i > 0 && i % 8 == 0)
                {
                    button_map[map_pointer++] = "\n";
                }

                String text =   config->GetString(CHANNEL_NAME, i) + String("\n") +
                                config->GetParameter(ROUTING_FPGA)->GetFormatedValue(i + 72) + String("\n")  +
                                String("[") + config->GetString(CHANNEL_NAME_INTERN, i) + String("]");

                char* buffer = (char*)lv_malloc(text.length() + 1);                                
                memccpy(buffer, text.c_str(), 0, text.length() + 1);

                button_map[map_pointer++] = buffer;
            }

            // last empty string to end Buttonmatrix
            button_map[map_pointer++] = "";//(char*)lv_malloc_zeroed(1);

            lv_buttonmatrix_set_map(objects.routing_fpga_matrix, button_map);
            
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_1, String(LV_SYMBOL_SHUFFLE) + String("\nCh / In / Out"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_4, String(LV_SYMBOL_REFRESH) + String("\nSelect"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_5, String(LV_SYMBOL_REFRESH) + String("\nSelect"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_6, String(LV_SYMBOL_DOWNLOAD) + String ("\nSelect\n(Save)"));
        }

        void OnChangeCustomEncoder(SurfaceElementId surface_element_id, int amount) override
        {
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_4:
                    selectionindex += amount;
                    lv_buttonmatrix_set_button_ctrl(objects.routing_fpga_matrix, selectionindex, LV_BUTTONMATRIX_CTRL_CHECKED);
                    break;
            }
        }

        void DrawTable()
        {
            // switch (gui_selected_block)
            // {
            //     case 0: // DSP Input (Mixer Channels)
            //         gui_items_offset = 72;
            //         gui_items_count = 40;
            //         break;
            //     case 1: // XLR Outputs
            //         gui_items_offset = 0;
            //         gui_items_count = 16;
            //         break;
            //     case 2: // AUX Outputs
            //         gui_items_offset = 64;
            //         gui_items_count = 8;
            //         break;
            //     case 3: // Ultranet
            //         gui_items_offset = 16;
            //         gui_items_count = 16;
            //         break;
            //     case 4: // CARD Recording
            //         gui_items_offset = 32;
            //         gui_items_count = 32;
            //         break;
            //     case 5: // AES50 A Outputs
            //         gui_items_offset = 112;
            //         gui_items_count = 48;
            //         break;
            //     case 6: // AES50 B Outputs
            //         gui_items_offset = 160;
            //         gui_items_count = 48;
            //         break;
            
            //     default:
            //         break;
            // }

            // lv_table_set_row_count(objects.table_routing_fpga, 0);

            // try {
            //     for (uint8_t i = 0; i < gui_items_count; i++)
            //     {
            //         lv_table_set_cell_value(objects.table_routing_fpga, i, 0, config->GetParameter(ROUTING_FPGA)->GetFormatedValue(i + gui_items_offset).c_str());
            //         lv_table_set_cell_value(objects.table_routing_fpga, i, 2, mixer->fpga->GetOutputNameByIndex(i + 1 + gui_items_offset).c_str());
            //     }
            // }catch (...){
            // }

            // lv_table_set_cell_value(objects.table_routing_fpga, gui_selected_item, 1, LV_SYMBOL_RIGHT);
        }
};