#pragma once
#include "page.h"

using namespace std;
using enum MP_ID;

class PageMeters : public Page 
{
    public:
        PageMeters(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 1;
            led = X32_BTN_METERS;
            hideEncoders = true;            
        }

        void OnInit() override {
            meterBlocks[0] = objects.meters_1_8;
            meterBlocks[1] = objects.meters_9_16;
            meterBlocks[2] = objects.meters_17_24;
            meterBlocks[3] = objects.meters_25_32;
            meterBlocks[4] = objects.meters_aux;
            meterBlocks[5] = objects.meters_fxret;
            meterBlocks[6] = objects.meters_bus_1_8;
            meterBlocks[7] = objects.meters_bus_9_16;
            meterBlocks[8] = objects.meters_matrix;
        }

        void OnUpdateMeters() override {

            for(int m = 0; m < 9; m++) {
                lv_obj_t* parent = meterBlocks[m];
                for(int i = 0; i < lv_obj_get_child_count(parent); i++) {

                    uint8_t index = i+(m*8);

                    lv_obj_t * strip = lv_obj_get_child(parent, i);
                    lv_obj_t * meter = lv_obj_get_child(strip, 0); 

                    // CH1-32 + AUX
                    if (m < 5) { 
                        lv_bar_set_value(meter, helper->sample2Dbfs(mixer->dsp->rChannel[index].meterDecay), LV_ANIM_OFF);
                    } else 

                    // FX Ret
                    if (m == 5) {
                        // TODO Meter
                    } else 
                    
                    // Bus 1-16
                    if ((m == 6) || (m == 7)) {
                        // TODO Meter
                    } else
                    
                    // Matrix
                    if (m == 8) {
                        if (i < 6) {
                            //lv_bar_set_value(meter, helper->sample2Dbfs(mixer->dsp->Matrix[i]. MainChannelSub.meterDecay[0]), LV_ANIM_OFF);
                        } else if (i == 7) {
                            lv_bar_set_value(meter, helper->sample2Dbfs(mixer->dsp->MainChannelSub.meterDecay[0]), LV_ANIM_OFF);
                        }
                    }
                }
            }

            // Main L/R
            // TODO use both channels!
            lv_bar_set_value(objects.ms_main_lr__meter, helper->sample2Dbfs(mixer->dsp->MainChannelLR.meterDecay[0]), LV_ANIM_OFF);
        }

        void OnChange(bool force_update) override
        {
            // Maybe TODO: Implement with loop over changed Mixerparameter

            if (config->HasParametersChanged({CHANNEL_SOLO, CHANNEL_MUTE, CHANNEL_VOLUME}) || force_update)
            {
                for(int m = 0; m < 9; m++)
                {
                    lv_obj_t* parent = meterBlocks[m];

                    for(int i = 0; i < lv_obj_get_child_count(parent); i++)
                    {
                        uint8_t index = i+(m*8);

                        lv_obj_t * strip;
                        lv_obj_t * fader;
                        bool isMainFader = index == (uint)X32_VCHANNEL_BLOCK::MAIN;

                        if (isMainFader)
                        {
                            fader = objects.ms_main_lr__fader;
                        }
                        else
                        {
                            strip = lv_obj_get_child(parent, i);
                            fader = lv_obj_get_child(strip, 2);   
                        }

                        UpdateSlider(index, force_update, fader);
                    } 
                } 
                
                // Main L/R
                if (config->HasParametersChanged({CHANNEL_SOLO, CHANNEL_MUTE, CHANNEL_VOLUME}, (uint)X32_VCHANNEL_BLOCK::MAIN) || force_update)
                {
                    UpdateSlider((uint)X32_VCHANNEL_BLOCK::MAIN, force_update, objects.ms_main_lr__fader);
                }
            }
        }

        void UpdateSlider(uint index, bool force_update, lv_obj_t *fader)
        {
            if (config->HasParameterChanged(CHANNEL_SOLO, index) || force_update)
            {
                config->GetBool(CHANNEL_SOLO, index) ? add_style_slider_fader_solo(fader) : remove_style_slider_fader_solo(fader);
            }
            if (config->HasParameterChanged(CHANNEL_MUTE, index) || force_update)
            {
                config->GetBool(CHANNEL_MUTE, index) ? add_style_slider_fader_mute(fader) : remove_style_slider_fader_mute(fader);
            }
            if (config->HasParameterChanged(CHANNEL_VOLUME, index) || force_update)
            {
                lv_slider_set_value(fader, config->GetFloat(CHANNEL_VOLUME, index), LV_ANIM_OFF);
            }
        }

        bool OnDisplayButton(X32_BTN button, bool pressed) override {
            if (pressed){
                switch (button){
                    case X32_BTN_ENCODER1:
                        break;
                    case X32_BTN_ENCODER2:
                        break;
                    case X32_BTN_ENCODER3:
                        break;
                    case X32_BTN_ENCODER4:
                        break;
                    case X32_BTN_ENCODER5:
                        break;
                    case X32_BTN_ENCODER6:
                        {
							for (int i = 0; i < 16; i++) {
                                config->Set(MP_ID::CHANNEL_PHANTOM, 1.0f, i);
                                config->Set(MP_ID::CHANNEL_GAIN, 47.0f, i);
							}
    					}
                        break;
                    default:
                        // just here to avoid compiler warnings
                        break;
                }
            }

            return true;
        }        

    private:
        lv_obj_t* meterBlocks[9];

};