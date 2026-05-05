#pragma once

#include "page.h"

using namespace std;

class PageRoutingFpga: public Page
{
    using enum MP_ID;

    private:

        uint selection_target = 0;
        uint selection_index = 0;
        int selection_source = 0;
        int selection_source_item = 0;

    public:

        PageRoutingFpga(PageBaseParameter* pagebasepar) : Page(pagebasepar)
        {
            prevPage = X32_PAGE::ROUTING;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 1;
            led = X32_BTN_ROUTING;
        }

        void OnShow() override
        {
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_1, String(LV_SYMBOL_SHUFFLE) + String("\nSection"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_2, String(LV_SYMBOL_REFRESH) + String("\nSelect"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_5, String(LV_SYMBOL_REFRESH) + String("\nSource"));
        }

        void OnChange(bool force)
        {
            if (config->HasParameterChanged(ROUTING_FPGA))
            {
                LoadMatrix();
            }
        }

        void OnChangeCustomEncoder(SurfaceElementId surface_element_id, int amount) override
        {
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_1:
                    selection_target += amount;
                    lv_roller_set_selected(objects.routing_fpga_target, selection_target, LV_ANIM_OFF);
                    LoadMatrix();
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_2:
                    selection_index += amount;
                    LoadMatrix();
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_5:
                    selection_source += amount;
                    lv_roller_set_selected(objects.routing_fpga_source, selection_source, LV_ANIM_OFF);
                    break;
            }
        }

        void LoadMatrix()
        {
            uint targetblockoffset = 0;
            uint targetblocksize = 0;

            switch (selection_target)
            {
                case 0: // CHANNELS
                    targetblockoffset = 72;
                    targetblocksize = 40;
                    break;
                case 1: // AUX Out
                    targetblockoffset = 65;
                    targetblocksize = 6;
                    break;
                case 2: // XLR Out
                    targetblockoffset = 0;
                    targetblocksize = 16;
                    break;
                case 3: // AES50 Out
                    targetblockoffset = 112;
                    targetblocksize = 48;
                    break;
                case 4: // Card Out
                    targetblockoffset = 32;
                    targetblocksize = 32;
                    break;
                case 5: // Ultranet
                    targetblockoffset = 16;
                    targetblocksize = 16;
                    break;
            }

            for (uint i = 0; i < lv_obj_get_child_count(objects.routing_matrix); i++)
            {
                lv_obj_t* routing_tile = lv_obj_get_child(objects.routing_matrix, i);

                lv_obj_t* routing_tile_lbl_header = lv_obj_get_child(routing_tile, 0);
                lv_obj_t* routing_tile_lbl = lv_obj_get_child(routing_tile, 1);
                lv_obj_t* routing_tile_lbl_footer = lv_obj_get_child(routing_tile, 2);

                String header = "";
                String label = "";
                String footer = "";

                if (i < targetblocksize)
                {
                    switch (selection_target)
                    {
                        case 0: // CHANNELS
                            header = config->GetString(CHANNEL_NAME_INTERN, i);
                            label = config->GetParameter(ROUTING_FPGA)->GetFormatedValue(i + targetblockoffset);
                            footer = config->GetString(CHANNEL_NAME, i);
                            break;
                        default:
                            header = String(i+1);
                            label = config->GetParameter(ROUTING_FPGA)->GetFormatedValue(i + targetblockoffset);
                            break;
                    }
                }
                
                if (i == selection_index)
                {
                    lv_obj_set_state(routing_tile_lbl_header, LV_STATE_FOCUSED, true);
                    config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_6, MixerparameterAction::CHANGE, ROUTING_FPGA, i + targetblockoffset);
                }
                else
                {
                    lv_obj_set_state(routing_tile_lbl_header, LV_STATE_FOCUSED, false);
                }

                lv_label_set_text(routing_tile_lbl_header, header.c_str());
                lv_label_set_text(routing_tile_lbl, label.c_str());
                lv_label_set_text(routing_tile_lbl_footer, footer.c_str());
            }

            SyncEncoderWidgets(true);
        }

        // void GenerateItemList()
        // {
        //     String itemName;
        //     uint count = 0;

        //     switch (gui_selected_block)
        //     {
        //         case 0:
        //             itemName = "XLR";
        //             count = (uint)X32_VCHANNEL_BLOCK_SIZE::NORMAL;
        //             break;
        //         case 1:
        //             itemName = "AUX";
        //             count = (uint)X32_VCHANNEL_BLOCK_SIZE::AUX;
        //             break;
        //         case 2:
        //             itemName = "CARD";
        //             count = 32;
        //             break;
        //         case 3:
        //             itemName = "AES50 A";
        //             count = 48;
        //             break;
        //     }

        //     String itemstring;
        //     for (uint i = 0; i < count; i++)
        //     {
        //         if (i != 0)
        //         {
        //             itemstring += "\n";
        //         }

        //         itemstring += String("XLR ") + String(i+1);
        //     }
        //     lv_roller_set_options(objects.routing_fpga_source_item, itemstring.c_str(), LV_ROLLER_MODE_NORMAL);
        // }



        // uint RoutingFPGA_BlockToOffset(uint block)
        // {
        //     uint offset = 0;
        //     switch (gui_selected_block)
        //     {
        //         case 0: // XLR Input
        //             offset = 0;
        //             break;
        //         case 1: // AUX Input
        //             offset = 64;
        //             break;
        //         case 2: // Card input
        //             offset = 32;
        //             break;
        //         case 3: // AES50 A
        //             offset = 112;
        //             break;
        //     }

        //     return offset;
        // }
};