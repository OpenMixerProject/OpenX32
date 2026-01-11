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

        X32_PAGE nextPage = X32_PAGE_NONE;
        X32_PAGE prevPage = X32_PAGE_NONE;

        X32_BTN led = X32_BTN_NONE;
        bool noLedOnRack = false;

        lv_obj_t* tabLayer0 = nullptr; 
        uint32_t tabIndex0 = 0;
        lv_obj_t* tabLayer1 = nullptr;
        uint32_t tabIndex1 = 0;

        bool hideEncoders = false;
        char displayEncoderText[6][30];
        const char* displayEncoderButtonMap[7] = {
            displayEncoderText[0],
            displayEncoderText[1],
            displayEncoderText[2],
            displayEncoderText[3],
            displayEncoderText[4],
            displayEncoderText[5],
        NULL};

        bool initDone = false;

        void SetEncoderText(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6);

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