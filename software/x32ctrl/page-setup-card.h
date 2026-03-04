#pragma once
#include "page.h"
using namespace std;

class PageSetupCard: public Page {
    private:
        uint8_t card_channelmode;
        int gui_selected_item = 0;
		int gui_selected_item_before = 0;
        uint numberOfEntries = 0;
        String TOC;

        void Encodertext() {
            String cardmode = String("Channelmode\n"); // TODO: + mixer->GetCardChannelModeString(card_channelmode);
            custom_encoder[0].label = cardmode;
        }

        static void draw_event_header_cb(lv_event_t * e) {
            lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
            lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
            lv_obj_t* obj = (lv_obj_t*)lv_event_get_target_obj(e);

            // if the cells are drawn
            if(base_dsc->part == LV_PART_ITEMS) {
                uint32_t row = base_dsc->id1;
                uint32_t col = base_dsc->id2;

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
			}
		}

        static void draw_event_cb(lv_event_t * e) {
            lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
            lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
            lv_obj_t* obj = (lv_obj_t*)lv_event_get_target_obj(e);

            // if the cells are drawn
            if(base_dsc->part == LV_PART_ITEMS) {
                uint32_t row = base_dsc->id1;
                uint32_t col = base_dsc->id2;

                /*
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
                */
                /*
                // In the first column align the texts to the right
                else if(col == 0) {
                    lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
                    if(label_draw_dsc) {
                        label_draw_dsc->align = LV_TEXT_ALIGN_RIGHT;
                    }
                }
                */
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

                    // draw all cells center-aligned
                    lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
                    if(label_draw_dsc) {
						label_draw_dsc->color = LV_COLOR_MAKE(0x00, 0x00, 0x00);
                        label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
                    }
                }else{
                    // draw all cells center-aligned
                    lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
                    if(label_draw_dsc) {
						label_draw_dsc->color = LV_COLOR_MAKE(0xFA, 0xFA, 0xFA);
                        label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
                    }
                }
            }
        }

    public:
        PageSetupCard(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::SETUP;
            nextPage = X32_PAGE::ABOUT;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 1;
            led = X32_BTN_SETUP;
        }

        void OnInit() override {
            UpdateEncoderBinding();
            lv_label_set_text_fmt(objects.setup_card_detected, "%s", mixer->GetCardModelString().c_str());
        }

        void OnShow() override {
            // get all files from CARD
            numberOfEntries = 0;
            TOC = mixer->card->XLIVE_RequestToc(&numberOfEntries);
            
            // Header
            lv_table_set_column_count(objects.setup_card_toc_header, 3);
            lv_table_set_column_width(objects.setup_card_toc_header, 0, 25); // selection marker
            lv_table_set_column_width(objects.setup_card_toc_header, 1, 125); // entries (HEX)
            lv_table_set_column_width(objects.setup_card_toc_header, 2, 300); // entries (TimeCode)
            lv_table_set_cell_value(objects.setup_card_toc_header, 0, 1, "File-ID");
            lv_table_set_cell_value(objects.setup_card_toc_header, 0, 2, "Date and Time");

            lv_obj_add_event_cb(objects.setup_card_toc_header, draw_event_header_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
            lv_obj_add_flag(objects.setup_card_toc_header, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

            // Selection-Table
            lv_table_set_row_count(objects.setup_card_toc, numberOfEntries); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
            lv_table_set_column_count(objects.setup_card_toc, 3);
            lv_table_set_column_width(objects.setup_card_toc, 0, 25); // selection marker
            lv_table_set_column_width(objects.setup_card_toc, 1, 125); // entries (HEX)
            lv_table_set_column_width(objects.setup_card_toc, 2, 300); // entries (TimeCode)
            for (uint8_t i=0; i < numberOfEntries; i++)
            {
                lv_table_set_cell_value_fmt(objects.setup_card_toc, i, 1, "%s", helper->split(TOC, ',', i).c_str());
                lv_table_set_cell_value_fmt(objects.setup_card_toc, i, 2, "%s", mixer->card->XLIVE_SessionNameToString(helper->split(TOC, ',', i)).c_str());
            }
            lv_table_set_cell_value(objects.setup_card_toc, gui_selected_item, 0, LV_SYMBOL_RIGHT);

            lv_obj_add_event_cb(objects.setup_card_toc, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
            lv_obj_add_flag(objects.setup_card_toc, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

            // store config pointer in user data for use in draw callback
            lv_obj_set_user_data(objects.setup_card_toc, &gui_selected_item);

            // logic for the icons
            if (mixer->card->XLIVE_Playing) {
                lv_image_set_offset_x(objects.setup_card_sdcard, (2 + (config->GetUint(CARD_SDCARD) * 3)) * -lv_obj_get_width(objects.setup_card_sdcard));
            }else if (mixer->card->XLIVE_Recording) {
                lv_image_set_offset_x(objects.setup_card_sdcard, (3 + (config->GetUint(CARD_SDCARD) * 3)) * -lv_obj_get_width(objects.setup_card_sdcard));
            }else{
                lv_image_set_offset_x(objects.setup_card_sdcard, (1 + (config->GetUint(CARD_SDCARD) * 3)) * -lv_obj_get_width(objects.setup_card_sdcard));
            }
        }

        void UpdateEncoderBinding() {
            BindEncoder(DISPLAY_ENCODER_1, MP_ID::CARD_NUMBER_OF_CHANNELS, MP_ID::CARD_AUDIO_SOURCE); // Channelnumber and Source-Mode
            BindEncoder(DISPLAY_ENCODER_2, MP_ID::CARD_SDCARD);

            BindEncoder(DISPLAY_ENCODER_3, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_4, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_5, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_6, PAGE_CUSTOM_ENCODER);
            custom_encoder[DISPLAY_ENCODER_3].label = "Stop";
            custom_encoder[DISPLAY_ENCODER_4].label = "Play/Pause";
            custom_encoder[DISPLAY_ENCODER_5].label = "Record";
            custom_encoder[DISPLAY_ENCODER_6].label = "Select Track";

            SyncEncoderWidgets(true);
        }

        void OnChange(bool force_update) override
        {
            if(gui_selected_item_before != gui_selected_item) {

                if (numberOfEntries == 0) {
                    gui_selected_item = 0;
                }else{
                    if (gui_selected_item < 0) {
                        // limit list at the top
                        gui_selected_item = 0;
                    }else if (gui_selected_item >= numberOfEntries) {
                        // limit list at the bottom
                        gui_selected_item = numberOfEntries - 1;
                    }
                }

                // remove old indicator
                lv_table_set_cell_value(objects.setup_card_toc, gui_selected_item_before, 0, " ");
                
                // display new indicator
                lv_table_set_cell_value(objects.setup_card_toc, gui_selected_item, 0, LV_SYMBOL_RIGHT);
                
                // set select to scroll table
                lv_table_set_selected_cell(objects.setup_card_toc, gui_selected_item, 0);
                
				gui_selected_item_before = gui_selected_item;
            }

            if (config->HasParameterChanged(CARD_NUMBER_OF_CHANNELS) || force_update)
            {
                lv_label_set_text(objects.setup_card_channelmode, config->GetParameter(CARD_NUMBER_OF_CHANNELS)->GetFormatedValue().c_str());
            }
            
            if (config->HasParameterChanged(CARD_AUDIO_SOURCE) || force_update)
            {
                lv_label_set_text(objects.setup_card_sourcemode, config->GetParameter(CARD_AUDIO_SOURCE)->GetFormatedValue().c_str());
                lv_image_set_offset_x(objects.setup_card_sdusb, config->GetUint(CARD_AUDIO_SOURCE) * -lv_obj_get_width(objects.setup_card_sdusb));
            }

            if (config->HasParameterChanged(CARD_SDCARD) || force_update) {
                OnShow(); // read TOC again and reset UI as we have changed the SD-Card
            }

            // logic for the icons
            if (mixer->card->XLIVE_Playing) {
                lv_image_set_offset_x(objects.setup_card_sdcard, (2 + (config->GetUint(CARD_SDCARD) * 3)) * -lv_obj_get_width(objects.setup_card_sdcard));
            }else if (mixer->card->XLIVE_Recording) {
                lv_image_set_offset_x(objects.setup_card_sdcard, (3 + (config->GetUint(CARD_SDCARD) * 3)) * -lv_obj_get_width(objects.setup_card_sdcard));
            }else{
                lv_image_set_offset_x(objects.setup_card_sdcard, (1 + (config->GetUint(CARD_SDCARD) * 3)) * -lv_obj_get_width(objects.setup_card_sdcard));
            }
        }

        void OnUpdateMeters() override {
            if (mixer->card->XLIVE_Playing) {
                // update text-fields and progressbar
                lv_label_set_text_fmt(objects.setup_card_currentposition, helper->secondsToHmsHuman(mixer->card->currentSongPositionSeconds).c_str());
                lv_label_set_text_fmt(objects.setup_card_totaltime, helper->secondsToHmsHuman(mixer->card->currentSongTotalSeconds).c_str());
                int32_t percentage = (mixer->card->currentSongPositionSeconds * 100) / mixer->card->currentSongTotalSeconds;
                lv_bar_set_value(objects.setup_card_progress, percentage, LV_ANIM_OFF);
            }
        }

        bool OnDisplayButton(X32_BTN button, bool pressed) override
        {
            bool handled = false;

        	if (pressed)
            {
                handled = true;

				switch (button)
                {
                    //case X32_BTN_ENCODER1:
                    //    break;
                    //case X32_BTN_ENCODER2: 
                    //    break;
                    case X32_BTN_ENCODER3: // Stop
                        mixer->card->XLIVE_Stop();
                        lv_delay_ms(100);
                        mixer->card->FlushRxBuffer(); // purge all commands send by Expansion-Card (several *9N24 and *9N00 commands)
                        OnShow(); // refresh list as content could have changed
                        break;
                    case X32_BTN_ENCODER4: // Play/Pause
                        mixer->card->XLIVE_PlayPause();
                        OnChange(false);
                        break;
                    case X32_BTN_ENCODER5: // Record
                        mixer->card->XLIVE_RecordNewSession();
                        OnChange(false);
                        break;
                    case X32_BTN_ENCODER6: // Select
                        lv_label_set_text(objects.setup_card_debugtext, helper->split(TOC, ',', gui_selected_item).c_str());
                        mixer->card->XLIVE_SelectSession(helper->split(TOC, ',', gui_selected_item));
                        break;
                    default:
                        handled = false;
                        // dummy
                }

                // update encoders as values have changed
                SyncEncoderWidgets(true);
            }

            return handled;
        }

        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount)
        {
            bool handled = true;

            switch (encoder)
            {
                //case X32_ENC_ENCODER1:
                //    break;
                //case X32_ENC_ENCODER2: // Stop
                //    break;
                //case X32_ENC_ENCODER3: // Play/Pause
                //    break;
                //case X32_ENC_ENCODER4: // 
                //    break;
                //case X32_ENC_ENCODER5: // Record
                //    break;
                case X32_ENC_ENCODER6: // Select
                    gui_selected_item += amount;
                    OnChange(false);
                    break;
                default:
                    handled = false;
                    // dummy
            }

            return handled;
        }
};