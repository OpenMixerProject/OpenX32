#pragma once
#include "page.h"
using namespace std;

class PageEffects: public Page {

    private:
        uint8_t selectedFx = 0;
        uint8_t banking = 0;

    public:
        PageEffects(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 5;
            led = X32_BTN_EFFECTS;
        }

        void OnShow() override {
            lv_table_set_column_count(objects.fxtable, MAX_FX_SLOTS);
            for (uint8_t i = 0; i < MAX_FX_SLOTS; i++) {
                lv_table_set_column_width(objects.fxtable, i, 100);
                // TODO table align center
            }
        }

        void OnChange(bool force_update) override {

            if (state->HasChanged(X32_MIXER_CHANGED_GUI_SELECT) ||
                state->HasChanged(X32_MIXER_CHANGED_FX) ||
                force_update)
            {
                for (uint8_t i = 0; i < MAX_FX_SLOTS; i++) {

                    FxSlot* slot = mixer->dsp->fx_slot[i];
                    
                    if (state->HasChanged(X32_MIXER_CHANGED_GUI_SELECT) || force_update)
                    {
                        if (i == selectedFx) {
                            lv_table_set_cell_value_fmt(objects.fxtable, 0, i, "FX%d <<<", i+1);

                            if (slot->HasFx()) {

                                // reset banking, if FX has less parameters
                                if (slot->fx->GetParameterCount() <= (banking * 6)) {
                                    banking = 0;
                                }

                                for (uint8_t e=0; e < MAX_DISPLAY_ENCODER; e++){
                                    SetEncoder(e, slot->fx->GetParameterDefinition(e + (banking * 6)));
                                    SetEncoderValue(e, slot->fx->GetParameter(e + (banking * 6)));
                                }
                            } else {
                                banking = 0;

                                for (uint8_t e=0; e < MAX_DISPLAY_ENCODER; e++){
                                    SetEncoder(e, MIXERPARAMETER_NONE);
                                }
                            }
                            state->SetChangeFlags(X32_MIXER_CHANGED_GUI);

                        } else {
                            lv_table_set_cell_value_fmt(objects.fxtable, 0, i, "FX%d", i+1);
                        }
                    }

                    if (state->HasChanged(X32_MIXER_CHANGED_FX) || force_update)
                    {
                        // Table - without offset!
                        if (slot->HasFx()) {

                            // Name
                            lv_table_set_cell_value(objects.fxtable, 1, i, mixer->dsp->fx_slot[i]->fx->GetName().c_str());

                            // FX Parameters
                            for (uint8_t p=0; p < slot->fx->GetParameterCount(); p++){
                                uint8_t row_label = (p*2) +3;
                                uint8_t row_data = row_label +1;

                                MixerparameterDefinition mpd = helper->GetMixerparameterDefinition(slot->fx->GetParameterDefinition(p));
                                lv_table_set_cell_value(objects.fxtable, row_label, i, mpd.name.c_str());
                                lv_table_set_cell_value(objects.fxtable, row_data,  i, helper->FormatValue(slot->fx->GetParameter(p), mpd).c_str());
                            }

                           
                        } else {
                            for (uint8_t p=0; p < 10; p++){
                                lv_table_set_cell_value(objects.fxtable, p+2, i, "");
                            }
                        }

                        // Encoders
                        if (i == selectedFx && slot->HasFx()) {
                            for (uint8_t e=0; e < MAX_DISPLAY_ENCODER; e++){
                                SetEncoder(e, slot->fx->GetParameterDefinition(e + (banking * 6)));
                                SetEncoderValue(e, slot->fx->GetParameter(e + (banking * 6)));
                            }
                        }
                    }
                }
            }
        }

        void OnDisplayButton(X32_BTN button, bool pressed) override {
            if (pressed){
                switch (button){
                    case X32_BTN_LEFT:
                        if (selectedFx > 0) {
                            selectedFx--;
                        } else {
                            selectedFx = MAX_FX_SLOTS-1;
                        }
                        state->SetChangeFlags(X32_MIXER_CHANGED_GUI_SELECT);
                        break;
                    case X32_BTN_RIGHT:
                        if (selectedFx < MAX_FX_SLOTS-1) {
                            selectedFx++;
                        } else {
                            selectedFx = 0;
                        }
                        state->SetChangeFlags(X32_MIXER_CHANGED_GUI_SELECT);
                        break;
                    case X32_BTN_UP:
                        if (banking>0)
                        {
                            banking--;
                        }
                        state->SetChangeFlags(X32_MIXER_CHANGED_GUI_SELECT);
                        break;
                    case X32_BTN_DOWN:
                        if (mixer->dsp->fx_slot[selectedFx]->fx->GetParameterCount() > (((banking * 6) + 1) * 6))
                        {
                            banking++;
                        }
                        state->SetChangeFlags(X32_MIXER_CHANGED_GUI_SELECT);
                        break;
                    case X32_BTN_ENCODER1:                      
                    case X32_BTN_ENCODER2:
                    case X32_BTN_ENCODER3:
                    case X32_BTN_ENCODER4:
                    case X32_BTN_ENCODER5:
                    case X32_BTN_ENCODER6:
                        mixer->ResetFxParameter(selectedFx, (banking * 6) + (button - X32_BTN_ENCODER1));
                    default:
                        // just here to avoid compiler warnings
                        break;
                }
            }
        }

        void OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) override {
            switch (encoder){
                case X32_ENC_ENCODER1:
                case X32_ENC_ENCODER2:
                case X32_ENC_ENCODER3:
                case X32_ENC_ENCODER4:
                case X32_ENC_ENCODER5:
                case X32_ENC_ENCODER6:
                    mixer->ChangeFxParameter(selectedFx, (banking * 6) + (encoder - X32_ENC_ENCODER1), amount);
                    break;
                default:  
                    // just here to avoid compiler warnings                  
                    break;
            }   
        }
};