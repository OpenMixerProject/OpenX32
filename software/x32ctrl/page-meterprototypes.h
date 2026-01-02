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

            // Channel 1-32
            // TODO Phantom, Invert, ...
            lv_led_set_brightness(objects.led_ch01_input, metertoled(mixer->dsp->rChannel[0].meter8Decay));
            

            // Main L/R
            lv_led_set_brightness(objects.led_main_l, metertoled(mixer->dsp->MainChannelLR.meterDecay[0]));
            lv_led_set_brightness(objects.led_main_r, metertoled(mixer->dsp->MainChannelLR.meterDecay[1]));

        }

        private:
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