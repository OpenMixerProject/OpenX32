#pragma once
#include "page.h"
using namespace std;

class PageMeter : public Page {
    public:
        PageMeter(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 1;
            led = X32_BTN_METERS;
            hideEncoders = true;
        }

        void OnChange() {
            if (config->IsModelX32FullOrCompactOrProducerOrRack()) {

                // Channel 1-32
                lv_slider_set_value(objects.slider01, helper->sample2Dbfs(mixer->dsp->rChannel[0].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider02, helper->sample2Dbfs(mixer->dsp->rChannel[1].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider03, helper->sample2Dbfs(mixer->dsp->rChannel[2].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider04, helper->sample2Dbfs(mixer->dsp->rChannel[3].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider05, helper->sample2Dbfs(mixer->dsp->rChannel[4].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider06, helper->sample2Dbfs(mixer->dsp->rChannel[5].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider07, helper->sample2Dbfs(mixer->dsp->rChannel[6].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider08, helper->sample2Dbfs(mixer->dsp->rChannel[7].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider09, helper->sample2Dbfs(mixer->dsp->rChannel[8].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider10, helper->sample2Dbfs(mixer->dsp->rChannel[9].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider11, helper->sample2Dbfs(mixer->dsp->rChannel[10].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider12, helper->sample2Dbfs(mixer->dsp->rChannel[11].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider13, helper->sample2Dbfs(mixer->dsp->rChannel[12].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider14, helper->sample2Dbfs(mixer->dsp->rChannel[13].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider15, helper->sample2Dbfs(mixer->dsp->rChannel[14].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider16, helper->sample2Dbfs(mixer->dsp->rChannel[15].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider17, helper->sample2Dbfs(mixer->dsp->rChannel[16].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider18, helper->sample2Dbfs(mixer->dsp->rChannel[17].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider19, helper->sample2Dbfs(mixer->dsp->rChannel[18].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider20, helper->sample2Dbfs(mixer->dsp->rChannel[19].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider21, helper->sample2Dbfs(mixer->dsp->rChannel[20].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider22, helper->sample2Dbfs(mixer->dsp->rChannel[21].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider23, helper->sample2Dbfs(mixer->dsp->rChannel[22].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider24, helper->sample2Dbfs(mixer->dsp->rChannel[23].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider25, helper->sample2Dbfs(mixer->dsp->rChannel[24].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider26, helper->sample2Dbfs(mixer->dsp->rChannel[25].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider27, helper->sample2Dbfs(mixer->dsp->rChannel[26].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider28, helper->sample2Dbfs(mixer->dsp->rChannel[27].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider29, helper->sample2Dbfs(mixer->dsp->rChannel[28].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider30, helper->sample2Dbfs(mixer->dsp->rChannel[29].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider31, helper->sample2Dbfs(mixer->dsp->rChannel[30].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider32, helper->sample2Dbfs(mixer->dsp->rChannel[31].meterDecay), LV_ANIM_OFF);

                // AUX
                lv_slider_set_value(objects.meter_aux__slider01, helper->sample2Dbfs(mixer->dsp->rChannel[32].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider02, helper->sample2Dbfs(mixer->dsp->rChannel[33].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider03, helper->sample2Dbfs(mixer->dsp->rChannel[34].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider04, helper->sample2Dbfs(mixer->dsp->rChannel[35].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider05, helper->sample2Dbfs(mixer->dsp->rChannel[36].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider06, helper->sample2Dbfs(mixer->dsp->rChannel[37].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider07, helper->sample2Dbfs(mixer->dsp->rChannel[38].meterDecay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider08, helper->sample2Dbfs(mixer->dsp->rChannel[39].meterDecay), LV_ANIM_OFF);

                // FX Ret
                // TODO

                // Bus 1-16
                // TODO
		    }
        }
};