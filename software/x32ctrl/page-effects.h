#pragma once
#include "page.h"
using namespace std;

class PageEffects: public Page {

    private:
        
        uint selectedFx = 0;
        uint selectedFxBefore = 0;

        uint banking = 0;
        uint bankingBefore = 0;

    public:
        PageEffects(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 5;
            led = X32_BTN_EFFECTS;
        }

        void OnShow() override {
            // encoder 1
            BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER, MP_ID::NONE);
            custom_encoder[DISPLAY_ENCODER_1].label = "Select FX";

            lv_table_set_column_count(objects.fxtable, 8);
            for (uint8_t i = 0; i < MAX_FX_SLOTS; i++) {
                lv_table_set_column_width(objects.fxtable, i, DISPLAY_RESOLUTION_X / 8);
            }
        }

        void OnChange(bool force_update) override {

            bool parameterChanged = config->HasParametersChanged(MP_CAT::FX);
            bool selectionChanged = selectedFx != selectedFxBefore;
            bool bankingChanged = banking != bankingBefore;

            if(parameterChanged || selectionChanged || bankingChanged || force_update)
            {
                for (uint slotIndex = 0; slotIndex < MAX_FX_SLOTS; slotIndex++) {

                    FxSlot* slot = mixer->dsp->fx_slot[slotIndex];
                    
                    if (selectionChanged || bankingChanged || force_update)
                    {
                        if (slotIndex == selectedFx) {
                            
                            // FX number (selected)
                            lv_table_set_cell_value_fmt(objects.fxtable, 0, selectedFx*2, "FX%d <<<", selectedFx+1);
                            lv_table_set_selected_cell(objects.fxtable, 0, selectedFx*2);

                            if (slot->HasFx()) {

                                // reset banking, if FX has less parameters
                                if (slot->fx->GetParameterCount() <= (banking * 5)) {
                                    banking = 0;
                                }

                                for (uint8_t e = 0; e < (MAX_DISPLAY_ENCODER - 1); e++){
                                    BindEncoder(DISPLAY_ENCODER_2 + e, slot->fx->GetParameterDefinition(e + (banking * 5)), selectedFx);
                                }

                            } else {
                                // slot has no FX -> show nothing

                                banking = 0;

                                for (uint8_t e = 0; e < (MAX_DISPLAY_ENCODER - 1); e++){
                                    UnbindEncoder(DISPLAY_ENCODER_2 + e);
                                }
                            }

                        } else {
                            // FX number (not selected)
                            lv_table_set_cell_value_fmt(objects.fxtable, 0, slotIndex * 2, "FX%d", slotIndex+1);
                        }
                    }
                    
                    if(parameterChanged || force_update)
                    {
                        // Table
                        if (slot->HasFx()) {
                            // FX Name
                            lv_table_set_cell_value(objects.fxtable, 1, slotIndex * 2, mixer->dsp->fx_slot[slotIndex]->fx->GetName().c_str());

                            // FX Parameters
                            for (uint8_t p = 0; p < slot->fx->GetParameterCount(); p++)
                            {
                                Mixerparameter *parameter = config->GetParameter(slot->fx->GetParameterDefinition(p));
                                lv_table_set_cell_value(objects.fxtable, p + 3, (slotIndex * 2), parameter->GetName().c_str());
                                lv_table_set_cell_value(objects.fxtable, p + 3, (slotIndex * 2) + 1, parameter->GetFormatedValue(slotIndex).c_str());
                            }
                        } else {
                            // FX Name
                            lv_table_set_cell_value(objects.fxtable, 1, slotIndex * 2, "No FX");

                            // clear name and parameters
                            for (uint8_t p = 0; p < 12; p++){
                                lv_table_set_cell_value(objects.fxtable, p + 2, (slotIndex * 2), "");
                                lv_table_set_cell_value(objects.fxtable, p + 2, (slotIndex * 2) + 1, "");
                            }
                        }   
                    }
                }

                selectedFxBefore = selectedFx;
                bankingBefore = banking;
            }
        }

        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override {

            if (encoder == X32_ENC_ENCODER1) {
                // ENCODER 1 -> select new FX
                // get the selected FX-slot
                FxSlot* slot = mixer->dsp->fx_slot[selectedFx];

                // calculate new FX-Type based on current FX-Type and encoder turn amount
                FX_TYPE newFxType = (FX_TYPE)((int)slot->GetFxType() + amount);
                if (newFxType < FX_TYPE::NONE) {
                    newFxType = (FX_TYPE)((int)FX_TYPE::FX_COUNT - 1);
                }else if (newFxType >= FX_TYPE::FX_COUNT) {
                    newFxType = FX_TYPE::NONE;
                }

                // install new effect
                mixer->dsp->DSP2_SetFx(selectedFx, newFxType, 2);

                // update UI
                OnChange(true);
                SyncEncoderWidgets(true);
            }else{
                // ENCODER 2 ... 6 -> change parameter of selected FX
                uint encoderIndex = encoder - X32_ENC_ENCODER1; // must be X32_ENC_ENCODER1 here to calculate correct index for encoderbinding
                if (encoderbinding[encoderIndex]->mp_id_encoder != MP_ID::NONE)
                {
                    config->Change(encoderbinding[encoderIndex]->mp_id_encoder, amount, selectedFx);
                }
            }
            
            return true;
        }

        bool OnDisplayButton(X32_BTN button, bool pressed) override {
            bool message_handled = false;

            if (pressed){
                message_handled = true;

                switch (button){
                    case X32_ENC_ENCODER1:
                        // do nothing when encoder 1 is pressed
                        break;
                    case X32_BTN_LEFT:
                        prevFX();
                        break;
                    case X32_BTN_RIGHT:
                        nextFX();
                        break;
                    case X32_BTN_UP:
                        prevParameterBank();
                        break;
                    case X32_BTN_DOWN:
                        nextParameterBank();
                        break;
                    default:
                        message_handled = false;
                        break;
                }
            }

            return message_handled;
        }

        void nextParameterBank()
        {
            if (mixer->dsp->fx_slot[selectedFx]->fx->GetParameterCount() > ((banking + 1) * 5))
            {
                banking++;
            }

            OnChange(true);
            SyncEncoderWidgets(true);
        }

        void prevParameterBank()
        {
            if (banking > 0)
            {
                banking--;
            }

            OnChange(true);
            SyncEncoderWidgets(true);
        }

        void nextFX()
        {
            if (selectedFx < MAX_FX_SLOTS - 1)
            {
                selectedFx++;
            }
            else
            {
                selectedFx = 0;
            }

            OnChange(true);
            SyncEncoderWidgets(true);
        }

        void prevFX()
        {
            if (selectedFx > 0)
            {
                selectedFx--;
            }
            else
            {
                selectedFx = MAX_FX_SLOTS - 1;
            }

            OnChange(true);
            SyncEncoderWidgets(true);
        }
};