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
            lv_label_set_text(objects.setup_card_debugtext, String(mixer->card->type).c_str());

            // get all files from CARD
            numberOfEntries = 0;
            TOC = mixer->card->XLIVE_RequestToc(&numberOfEntries);
            
            lv_table_set_row_count(objects.setup_card_toc, numberOfEntries); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
            lv_table_set_column_count(objects.setup_card_toc, 3);
            lv_table_set_column_width(objects.setup_card_toc, 0, 50); // selection marker
            lv_table_set_column_width(objects.setup_card_toc, 1, 150); // entries (HEX)
            lv_table_set_column_width(objects.setup_card_toc, 2, 200); // entries (TimeCode)
            for (uint8_t i=0; i < numberOfEntries; i++)
            {
                lv_table_set_cell_value_fmt(objects.setup_card_toc, i, 1, "%s", helper->split(TOC, ',', i).c_str());
                lv_table_set_cell_value_fmt(objects.setup_card_toc, i, 2, "%s", mixer->card->XLIVE_SessionNameToString(helper->split(TOC, ',', i)).c_str());
            }
            lv_table_set_cell_value(objects.setup_card_toc, gui_selected_item, 0, LV_SYMBOL_RIGHT);
        }

        void UpdateEncoderBinding() {
            BindEncoder(DISPLAY_ENCODER_1, MP_ID::CARD_NUMBER_OF_CHANNELS, MP_ID::CARD_AUDIO_SOURCE); // Channelnumber and Source-Mode
            BindEncoder(DISPLAY_ENCODER_2, MP_ID::CARD_SOURCE);

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
				if (gui_selected_item < 0) {
					// limit list at the top
					gui_selected_item = 0;
				}else if (gui_selected_item >= numberOfEntries) {
					// limit list at the bottom
					gui_selected_item = numberOfEntries - 1;
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
                        OnShow(); // refresh list as content could have changed
                        break;
                    case X32_BTN_ENCODER4: // Play/Pause
                        mixer->card->XLIVE_PlayPause();
                        break;
                    case X32_BTN_ENCODER5: // Record
                        mixer->card->XLIVE_RecordNewSession();
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