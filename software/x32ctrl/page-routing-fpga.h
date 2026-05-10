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

        uint current_targetblocksize = 0;
        uint current_targetblockoffset = 0;

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

        void OnShow() override
        {
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_1, String(LV_SYMBOL_SHUFFLE) + String("\nSection"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_2, String(LV_SYMBOL_REFRESH) + String("\nSelect"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_5, String(LV_SYMBOL_REFRESH) + String("\nSource"));

            LoadMatrix();
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
                    selection_target = helper->CheckBoundaries(selection_target, amount, 0, 5);
                    lv_roller_set_selected(objects.routing_fpga_target, selection_target, LV_ANIM_OFF);
                    LoadMatrix();
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_2:
                    selection_index = helper->CheckBoundaries(selection_index, amount, 0, current_targetblocksize-1);
                    LoadMatrix();
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_5:
                    {
                        selection_source = helper->CheckBoundaries(selection_source, amount, 0, 4);
                        lv_roller_set_selected(objects.routing_fpga_source, selection_source, LV_ANIM_OFF);

                        uint item_index = 0;

                        switch(selection_source)
                        {
                            case 0: // XLR
                                item_index = FPGA_INPUT_IDX_XLR;
                                break;
                            case 1: // CARD
                                item_index = FPGA_INPUT_IDX_CARD;
                                break;
                            case 2: // AUX
                                item_index = FPGA_INPUT_IDX_AUX;
                                break;
                            case 3: // DSP
                                item_index = FPGA_INPUT_IDX_DSP_RETURN;
                                break;
                            case 4: // AES50 A
                                item_index = FPGA_INPUT_IDX_AES50A;
                                break;
                            default:
                                break;
                        }
                        
                        config->Set(ROUTING_FPGA, item_index, current_targetblockoffset + selection_index);
                    }
                    break;
                default:
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
                default:
                    break;
            }

            current_targetblockoffset = targetblockoffset;
            current_targetblocksize = targetblocksize;           

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

                lv_label_set_text(routing_tile_lbl_header, header.c_str());
                lv_label_set_text(routing_tile_lbl, label.c_str());
                lv_label_set_text(routing_tile_lbl_footer, footer.c_str());

                if (i == selection_index)
                {
                    lv_obj_set_state(routing_tile_lbl_header, LV_STATE_FOCUSED, true);
                    config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_6, MixerparameterAction::CHANGE, ROUTING_FPGA, i + targetblockoffset);

                    // Update source block
                    switch (config->GetUint(ROUTING_FPGA, i + targetblockoffset))
                    {
                        case FPGA_INPUT_IDX_XLR ... FPGA_INPUT_IDX_XLR + 31:
                            selection_source = 0;
                            break;
                        case FPGA_INPUT_IDX_CARD ... FPGA_INPUT_IDX_CARD + 31:
                            selection_source = 1;
                            break;
                        case FPGA_INPUT_IDX_AUX ... FPGA_INPUT_IDX_AUX + 7:
                            selection_source = 2;
                            break;
                        case FPGA_INPUT_IDX_DSP_RETURN ... FPGA_INPUT_IDX_DSP_RETURN + 39:
                            selection_source = 3;
                            break;
                        case FPGA_INPUT_IDX_AES50A ... FPGA_INPUT_IDX_AES50A + 47:
                            selection_source = 4;
                            break;
                        default:
                            break;
                    }                    
                    lv_roller_set_selected(objects.routing_fpga_source, selection_source, LV_ANIM_OFF);
                }
                else
                {
                    lv_obj_set_state(routing_tile_lbl_header, LV_STATE_FOCUSED, false);
                }
            }

            SyncEncoderWidgets(true);
        }
};