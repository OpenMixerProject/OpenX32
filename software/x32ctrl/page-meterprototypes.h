#pragma once
#include "page.h"
using namespace std;

class PageMeterPrototypes : public Page {
    public:
        PageMeterPrototypes(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE_METERS;
            tabLayer0 = objects.maintab;
            tabIndex0 = 1;
            tabLayer1 = objects.metertab;
            tabIndex1 = 1;
            hideEncoders = true;
        }

        void OnShow() override {
            lv_led_off(objects.led_xlr01_phantom);
            lv_led_off(objects.led_xlr01_invert);
            lv_led_off(objects.led_xlr01_input_level);

            lv_led_off(objects.led_ch01_input);
            lv_led_off(objects.led_ch01_output);

            lv_led_off(objects.led_main_l);
            lv_led_off(objects.led_main_r);
        }

        void OnChange() override {

            if (state->HasChanged(X32_MIXER_CHANGED_METER))
            {
                // Channel 1-32
                // TODO Phantom, Invert, ...
                if(debug1)
                {
                    lv_led_set_brightness(objects.led_ch01_input, metertoled(mixer->dsp->rChannel[0].meterDecay));
                }

                // Main L/R
                if(debug1)
                {
                    lv_led_set_brightness(objects.led_main_l, metertoled(mixer->dsp->MainChannelLR.meterDecay[0]));
                    lv_led_set_brightness(objects.led_main_r, metertoled(mixer->dsp->MainChannelLR.meterDecay[1]));
                }

                // LED-Style VU-Meters with faders
                if(debug2)
                {
                                
                    lv_obj_t * parent = objects.meterstrip_ch01;
                    srDspChannel rDspChan = mixer->dsp->rChannel[0];
                    for(int i = 0; i < lv_obj_get_child_count_by_type(parent, &lv_led_class); i++) {
                        lv_obj_t * child = lv_obj_get_child_by_type(parent, i, &lv_led_class);

                        if (lv_obj_get_class(child) == &lv_led_class){                    
                            lv_led_set_brightness(child, ((rDspChan.meter6Info << i) & 0b100000) ? 255: 0);
                        }
                    }
                    lv_slider_set_value(objects.meterstrip_ch01__fader, mixer->dsp->Channel[0].volumeLR, LV_ANIM_OFF);

                    parent = objects.meterstrip_ch02;
                    rDspChan = mixer->dsp->rChannel[1];
                    for(int i = 0; i < lv_obj_get_child_count_by_type(parent, &lv_led_class); i++) {
                        lv_obj_t * child = lv_obj_get_child_by_type(parent, i, &lv_led_class);

                        if (lv_obj_get_class(child) == &lv_led_class){                    
                            lv_led_set_brightness(child, ((rDspChan.meter6Info << i) & 0b100000) ? 255: 0);
                        }
                    }
                    lv_slider_set_value(objects.meterstrip_ch02__fader, mixer->dsp->Channel[1].volumeLR, LV_ANIM_OFF);

                    parent = objects.meterstrip_ch03;
                    rDspChan = mixer->dsp->rChannel[2];
                    for(int i = 0; i < lv_obj_get_child_count_by_type(parent, &lv_led_class); i++) {
                        lv_obj_t * child = lv_obj_get_child_by_type(parent, i, &lv_led_class);

                        if (lv_obj_get_class(child) == &lv_led_class){                    
                            lv_led_set_brightness(child, ((rDspChan.meter6Info << i) & 0b100000) ? 255: 0);
                        }
                    }
                    lv_slider_set_value(objects.meterstrip_ch03__fader, mixer->dsp->Channel[2].volumeLR, LV_ANIM_OFF);

                    parent = objects.meterstrip_aux1;
                    rDspChan = mixer->dsp->rChannel[32];

                    for(int i = 0; i < lv_obj_get_child_count_by_type(parent, &lv_led_class); i++) {
                        lv_obj_t * child = lv_obj_get_child_by_type(parent, i, &lv_led_class);

                        if (lv_obj_get_class(child) == &lv_led_class){                    
                            lv_led_set_brightness(child, ((rDspChan.meter6Info << i) & 0b100000) ? 255: 0);
                        }
                    }
                    lv_slider_set_value(objects.meterstrip_aux1__fader, mixer->dsp->Channel[32].volumeLR, LV_ANIM_OFF);

                    parent = objects.meterstrip_aux2;
                    rDspChan = mixer->dsp->rChannel[33];

                    for(int i = 0; i < lv_obj_get_child_count_by_type(parent, &lv_led_class); i++) {
                        lv_obj_t * child = lv_obj_get_child_by_type(parent, i, &lv_led_class);

                        if (lv_obj_get_class(child) == &lv_led_class){                    
                            lv_led_set_brightness(child, ((rDspChan.meter6Info << i) & 0b100000) ? 255: 0);
                        }
                    }
                    lv_slider_set_value(objects.meterstrip_aux2__fader, mixer->dsp->Channel[33].volumeLR, LV_ANIM_OFF);
                }



            }
        }

        void OnDisplayButton(X32_BTN button, bool pressed) override {
            if (pressed){
                switch (button){
                    case X32_BTN_ENCODER1:
                        debug1 = !debug1;
                        break;
                    case X32_BTN_ENCODER2:
                        debug2 = !debug2;
                        break;
                    case X32_BTN_ENCODER3:
                        break;
                    case X32_BTN_ENCODER4:
                        break;
                    case X32_BTN_ENCODER5:
                        break;
                    case X32_BTN_ENCODER6:
                        break;
                    default:
                        // just here to avoid compiler warnings
                        break;
                }
            }
        }

        private:

            bool debug1 = true;
            bool debug2 = true;

            uint8_t metertoled(uint32_t metervalue) {
                
                // TODO: find good looking values :-)
                // TODO2: color -> green, yellow, red (Clip)

                if (metervalue >= VUTRESH_00_DBFS_CLIP) return 255;
                if (metervalue >= VUTRESH_MINUS_06_DBFS) return 200;
                if (metervalue >= VUTRESH_MINUS_12_DBFS) return 180;
                if (metervalue >= VUTRESH_MINUS_18_DBFS) return 150;
                if (metervalue >= VUTRESH_MINUS_30_DBFS) return 100;
                if (metervalue >= VUTRESH_MINUS_60_DBFS) return 50;
                return 0;
            }
};