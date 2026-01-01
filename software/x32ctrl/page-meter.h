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
                lv_slider_set_value(objects.slider01, helper->sample2Dbfs(mixer->dsp->rChannel[0].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider02, helper->sample2Dbfs(mixer->dsp->rChannel[1].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider03, helper->sample2Dbfs(mixer->dsp->rChannel[2].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider04, helper->sample2Dbfs(mixer->dsp->rChannel[3].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider05, helper->sample2Dbfs(mixer->dsp->rChannel[4].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider06, helper->sample2Dbfs(mixer->dsp->rChannel[5].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider07, helper->sample2Dbfs(mixer->dsp->rChannel[6].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider08, helper->sample2Dbfs(mixer->dsp->rChannel[7].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider09, helper->sample2Dbfs(mixer->dsp->rChannel[8].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider10, helper->sample2Dbfs(mixer->dsp->rChannel[9].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider11, helper->sample2Dbfs(mixer->dsp->rChannel[10].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider12, helper->sample2Dbfs(mixer->dsp->rChannel[11].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider13, helper->sample2Dbfs(mixer->dsp->rChannel[12].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider14, helper->sample2Dbfs(mixer->dsp->rChannel[13].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider15, helper->sample2Dbfs(mixer->dsp->rChannel[14].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider16, helper->sample2Dbfs(mixer->dsp->rChannel[15].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider17, helper->sample2Dbfs(mixer->dsp->rChannel[16].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider18, helper->sample2Dbfs(mixer->dsp->rChannel[17].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider19, helper->sample2Dbfs(mixer->dsp->rChannel[18].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider20, helper->sample2Dbfs(mixer->dsp->rChannel[19].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider21, helper->sample2Dbfs(mixer->dsp->rChannel[20].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider22, helper->sample2Dbfs(mixer->dsp->rChannel[21].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider23, helper->sample2Dbfs(mixer->dsp->rChannel[22].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider24, helper->sample2Dbfs(mixer->dsp->rChannel[23].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider25, helper->sample2Dbfs(mixer->dsp->rChannel[24].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider26, helper->sample2Dbfs(mixer->dsp->rChannel[25].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider27, helper->sample2Dbfs(mixer->dsp->rChannel[26].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider28, helper->sample2Dbfs(mixer->dsp->rChannel[27].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider29, helper->sample2Dbfs(mixer->dsp->rChannel[28].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider30, helper->sample2Dbfs(mixer->dsp->rChannel[29].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider31, helper->sample2Dbfs(mixer->dsp->rChannel[30].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.slider32, helper->sample2Dbfs(mixer->dsp->rChannel[31].meter6Decay), LV_ANIM_OFF);

                // AUX
                lv_slider_set_value(objects.meter_aux__slider01, helper->sample2Dbfs(mixer->dsp->rChannel[32].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider02, helper->sample2Dbfs(mixer->dsp->rChannel[33].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider03, helper->sample2Dbfs(mixer->dsp->rChannel[34].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider04, helper->sample2Dbfs(mixer->dsp->rChannel[35].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider05, helper->sample2Dbfs(mixer->dsp->rChannel[36].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider06, helper->sample2Dbfs(mixer->dsp->rChannel[37].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider07, helper->sample2Dbfs(mixer->dsp->rChannel[38].meter6Decay), LV_ANIM_OFF);
                lv_slider_set_value(objects.meter_aux__slider08, helper->sample2Dbfs(mixer->dsp->rChannel[39].meter6Decay), LV_ANIM_OFF);

                // FX Ret
                // TODO

                // Bus 1-16
                // TODO
		    }
        }
};