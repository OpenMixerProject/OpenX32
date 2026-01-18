#pragma once

#include "defines.h"
#include "enum.h"
#include "base.h"
#include "page-baseparameter.h"
#include "mixer.h"

#include "lv_port_linux/lvgl/lvgl.h"
#include "eez/src/ui/screens.h"
#include "eez/src/ui/styles.h"

using namespace std;



class Page : public X32Base {
    protected:
        Mixer* mixer;
        Surface* surface;

        X32_PAGE nextPage = X32_PAGE_NONE;
        X32_PAGE prevPage = X32_PAGE_NONE;

        X32_BTN led = X32_BTN_NONE;
        bool noLedOnRack = false;

        lv_obj_t* tabLayer0 = nullptr; 
        uint32_t tabIndex0 = 0;
        lv_obj_t* tabLayer1 = nullptr;
        uint32_t tabIndex1 = 0;

        bool hideEncoders = false;
        MixerparameterDefinition encoderMixerparameterDefinition[6];
        sDisplayEncoderSlider encoderSliders[6];
        lv_obj_t* sliders[6];
        lv_obj_t* encoderButtonLabels[6];

        bool initDone = false;

        void SetEncoder(uint8_t encoder, MIXERPARAMETER mp);
        void SetEncoder(uint8_t encoder, MIXERPARAMETER mp, String buttonPressLabel);
        void SetEncoder(uint8_t encoder, String label, String buttonPressLabel);
        void SetEncoderValue(uint8_t encoder, float enc1);
        void SetEncoderValuesEmpty();
        void SetEncoderButtonLablesHighlight(bool enc1, bool enc2, bool enc3, bool enc4, bool enc5, bool enc6);
        void SyncEncoderWidgets();

        virtual void OnInit();
        virtual void OnUpdateMeters();
        virtual void OnShow();
        virtual void OnChange(bool force_update);

    public:
        Page(PageBaseParameter* pagebasepar);

        void Init();
        void Show();
        void UpdateMeters();
        void Change();

        // a display encoder was turned
        virtual void OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount);

        // a display button was pressed
        virtual void OnDisplayButton(X32_BTN button, bool pressed);

        X32_PAGE GetNextPage();
        X32_PAGE GetPrevPage();

        X32_BTN GetLed();
};