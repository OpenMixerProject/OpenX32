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

        void OnShow() override
        {
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_1, MixerparameterAction::SET, DISPLAY_ENCODER_1_ENCODER);
            config->GetParameter(DISPLAY_ENCODER_1_ENCODER)->SetName(String(LV_SYMBOL_REFRESH) + "\nSelect FX");

            lv_table_set_column_count(objects.fxtable, 8);
            for (uint8_t i = 0; i < MAX_FX_SLOTS; i++) {
                lv_table_set_column_width(objects.fxtable, i, DISPLAY_RESOLUTION_X / 8);
            }
        }

        void OnChange(bool force_update) override
        {
            if (config->HasParameterChanged(DISPLAY_ENCODER_1_ENCODER))
            {
                // ENCODER 1 -> select new FX
                // get the selected FX-slot
                FxSlot* slot = mixer->dsp->fx_slot[selectedFx];

                // calculate new FX-Type based on current FX-Type and encoder turn amount
                FX_TYPE newFxType = (FX_TYPE)((int)slot->GetFxType() + config->GetUint(DISPLAY_ENCODER_1_ENCODER));
                if (newFxType < FX_TYPE::NONE) {
                    newFxType = (FX_TYPE)((int)FX_TYPE::FX_COUNT - 1);
                }else if (newFxType >= FX_TYPE::FX_COUNT) {
                    newFxType = FX_TYPE::NONE;
                }

                // install new effect
                mixer->dsp->DSP2_SetFx(selectedFx, newFxType, 2);
            }

            if (config->HasParameterChanged(DISPLAY_LEFT))
            {
                prevFX();
            }

            if (config->HasParameterChanged(DISPLAY_RIGHT))
            {
                nextFX();
            }

            if (config->HasParameterChanged(DISPLAY_UP))
            {
                prevParameterBank();
            }

            if (config->HasParameterChanged(DISPLAY_DOWN))
            {
                nextParameterBank();
            }


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
                                if (slot->fx->GetParameterCount() <= (banking * 5))
                                {
                                    banking = 0;
                                }

                                for (uint8_t e = 0; e < (MAX_DISPLAY_ENCODER - 1); e++)
                                {
                                    config->SurfaceBind((SurfaceElementId)(((uint)(SurfaceElementId::DISPLAY_ENCODER_2)) + e), MixerparameterAction::SET, slot->fx->GetParameterDefinition(e + (banking * 5)), selectedFx);
                                }

                            } else {
                                // slot has no FX -> show nothing

                                banking = 0;

                                for (uint8_t e = 0; e < (MAX_DISPLAY_ENCODER - 1); e++)
                                {
                                    config->SurfaceUnbind((SurfaceElementId)(((uint)(SurfaceElementId::DISPLAY_ENCODER_2)) + e));
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
                            if (slot->fx->GetParameterCount() == 0) {
                                // clear name and parameters
                                for (uint8_t p = 0; p < 12; p++){
                                    lv_table_set_cell_value(objects.fxtable, p + 2, (slotIndex * 2), "");
                                    lv_table_set_cell_value(objects.fxtable, p + 2, (slotIndex * 2) + 1, "");
                                }
                            }else{
                                for (uint8_t p = 0; p < slot->fx->GetParameterCount(); p++)
                                {
                                    Mixerparameter *parameter = config->GetParameter(slot->fx->GetParameterDefinition(p));
                                    lv_table_set_cell_value(objects.fxtable, p + 3, (slotIndex * 2), parameter->GetName().c_str());
                                    lv_table_set_cell_value(objects.fxtable, p + 3, (slotIndex * 2) + 1, parameter->GetFormatedValue(slotIndex).c_str());
                                }
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

        void nextParameterBank()
        {
            if (mixer->dsp->fx_slot[selectedFx]->fx->GetParameterCount() > ((banking + 1) * 5))
            {
                banking++;
            }
        }

        void prevParameterBank()
        {
            if (banking > 0)
            {
                banking--;
            }
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
        }
};