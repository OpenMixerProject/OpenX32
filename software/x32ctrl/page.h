#pragma once

#include <map>

#include "defines.h"
#include "enum.h"
#include "base.h"
#include "page-baseparameter.h"
#include "mixer.h"
#include "page-bindings.h"

#include "lv_port_linux/lvgl/lvgl.h"
#include "eez/src/ui/screens.h"
#include "eez/src/ui/styles.h"

using namespace std;

class Page : public X32Base {
    protected:
        Mixer* mixer;
        Surface* surface;

        X32_PAGE nextPage = X32_PAGE::NONE;
        X32_PAGE prevPage = X32_PAGE::NONE;

        X32_BTN led = X32_BTN_NONE;
        bool noLedOnRack = false;

        lv_obj_t* tabLayer0 = nullptr; 
        uint32_t tabIndex0 = 0;
        lv_obj_t* tabLayer1 = nullptr;
        uint32_t tabIndex1 = 0;

        bool hideEncoders = false;

        // encoder binding
        map<uint, PageBindingEncoder*> encoderbinding;
       
        // old, just here to be able to compile
        sDisplayEncoder encoderSliders[MAX_DISPLAY_ENCODER];

        bool initDone = false;

        void BindEncoder(uint encoder, MP_ID mp_id);
        void BindEncoder(uint encoder, MP_ID mp_id, MP_ID mp_id_button);
        
        void SetEncoder(uint encoder, MP_ID mp, String buttonPressLabel);
        void SetEncoder(uint encoder, String label, String buttonPressLabel);
        void SetEncoderValue(uint encoder, float enc1);
        void SetEncoderValue(uint encoder, uint enc1);
        void SetEncoderValue(uint encoder, int enc1);
        void SetEncoderValuesEmpty();
        void UnbindEncoders();
        void UnbindEncoder(uint encoder);
        void SetEncoderHighlight(uint encoder, bool highlight);
        void SyncEncoderWidgets();

        virtual void OnInit();
        virtual void OnUpdateMeters();
        virtual void OnShow();
        virtual void OnChange(bool force_update);
        virtual void OnDisplayEncoderTurned(X32_ENC encoder, int amount);
        virtual void OnDisplayButton(X32_BTN button, bool pressed);

    public:
        Page(PageBaseParameter* pagebasepar);

        void Init();
        void Show();
        void UpdateMeters();
        void Change();

        void DisplayEncoderTurned(X32_ENC encoder, int amount);
        void DisplayButton(X32_BTN button, bool pressed);

        X32_PAGE GetNextPage();
        X32_PAGE GetPrevPage();

        X32_BTN GetLed();
};