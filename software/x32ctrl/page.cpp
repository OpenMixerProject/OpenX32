/*
    ____                  __   ______ ___  
   / __ \                 \ \ / /___ \__ \ 
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
  | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |                               
         |_|                               
  
  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "page.h"

Page::Page(PageBaseParameter* pagebasepar) : X32Base(pagebasepar) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::Page()");
    mixer = pagebasepar->mixer;
}

void Page::Init() {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::Init()");

    sliders[0] = objects.widget1__slider;
    sliders[1] = objects.widget2__slider;
    sliders[2] = objects.widget3__slider;
    sliders[3] = objects.widget4__slider;
    sliders[4] = objects.widget5__slider;
    sliders[5] = objects.widget6__slider;

    encoderButtonLabels[0] = objects.widget1__label_buttonpress;
    encoderButtonLabels[1] = objects.widget2__label_buttonpress;
    encoderButtonLabels[2] = objects.widget3__label_buttonpress;
    encoderButtonLabels[3] = objects.widget4__label_buttonpress;
    encoderButtonLabels[4] = objects.widget5__label_buttonpress;
    encoderButtonLabels[5] = objects.widget6__label_buttonpress;

    SetEncoderLables("-", "-", "-", "-", "-", "-");
    OnInit();
    initDone = true;
}

void Page::OnInit() {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::OnInit()");
}

void Page::Show() {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::Show()");

    // open tab on Layer0
	if (tabLayer0 != nullptr) {
		lv_tabview_set_active(tabLayer0, tabIndex0, LV_ANIM_OFF);
	}

	// open tab on Layer1
	if (tabLayer1 != nullptr) {
		lv_tabview_set_active(tabLayer1, tabIndex1, LV_ANIM_OFF);
	}

    lv_obj_set_flag(objects.display_encoder_sliders, LV_OBJ_FLAG_HIDDEN, hideEncoders);

    // Show Page and trigger a complete update
    OnShow();
    OnChange(true);
}

void Page::OnShow() {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::OnShow()");
}

void Page::Change() {
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::Change()");
    if (initDone) {
        OnChange(false);
        SyncEncoderWidgets();
    }
}

void Page::OnChange(bool force_update) {
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::OnChange(force_update: %d)", force_update);
}

void Page::UpdateMeters() {
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::Time10ms()");
    if (initDone) {
        OnUpdateMeters();
    }
}

void Page::OnUpdateMeters() {
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::OnTime10ms()");
}

void Page::OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::OnDisplayEncoderTurned()");
}

void Page::OnDisplayButton(X32_BTN button, bool pressed) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::OnDisplayButton()");
}

X32_PAGE Page::GetNextPage() {
    return nextPage;
}

X32_PAGE Page::GetPrevPage() {
    return prevPage;
}

X32_BTN Page::GetLed() {
    if (!(config->IsModelX32Rack() && noLedOnRack))
    {
        return led;
    }

    return X32_BTN_NONE;
}

void Page::SetEncoderLables(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6) {
    encoderSliders[0].label = enc1;
    encoderSliders[1].label = enc2;
    encoderSliders[2].label = enc3;
    encoderSliders[3].label = enc4;
    encoderSliders[4].label = enc5;
    encoderSliders[5].label = enc6;
}

void Page::SetEncoderValues(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6)  {
    encoderSliders[0].label_value = enc1;
    encoderSliders[1].label_value = enc2;
    encoderSliders[2].label_value = enc3;
    encoderSliders[3].label_value = enc4;
    encoderSliders[4].label_value = enc5;
    encoderSliders[5].label_value = enc6;
}

void Page::SetEncoderSliderHidden(bool enc1, bool enc2, bool enc3, bool enc4, bool enc5, bool enc6) {
    encoderSliders[0].slider_hidden = enc1;
    encoderSliders[1].slider_hidden = enc2;
    encoderSliders[2].slider_hidden = enc3;
    encoderSliders[3].slider_hidden = enc4;
    encoderSliders[4].slider_hidden = enc5;
    encoderSliders[5].slider_hidden = enc6;
}

void Page::SetEncoderPercent(uint8_t enc1, uint8_t enc2, uint8_t enc3, uint8_t enc4, uint8_t enc5, uint8_t enc6) {
    encoderSliders[0].percent = enc1;
    encoderSliders[1].percent = enc2;
    encoderSliders[2].percent = enc3;
    encoderSliders[3].percent = enc4;
    encoderSliders[4].percent = enc5;
    encoderSliders[5].percent = enc6;
}

void Page::SetEncoderButtonLables(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6) {
    encoderSliders[0].label_buttonpress = enc1;
    encoderSliders[1].label_buttonpress = enc2;
    encoderSliders[2].label_buttonpress = enc3;
    encoderSliders[3].label_buttonpress = enc4;
    encoderSliders[4].label_buttonpress = enc5;
    encoderSliders[5].label_buttonpress = enc6;
}

void Page::SetEncoderButtonLablesHighlight(bool enc1, bool enc2, bool enc3, bool enc4, bool enc5, bool enc6) {
    encoderSliders[0].label_buttonpress_highlighted = enc1;
    encoderSliders[1].label_buttonpress_highlighted = enc2;
    encoderSliders[2].label_buttonpress_highlighted = enc3;
    encoderSliders[3].label_buttonpress_highlighted = enc4;
    encoderSliders[4].label_buttonpress_highlighted = enc5;
    encoderSliders[5].label_buttonpress_highlighted = enc6;
}

void Page::SyncEncoderWidgets() {
    if (hideEncoders) {
        return;
    }

    lv_label_set_text(objects.widget1__label, encoderSliders[0].label.c_str());
    lv_label_set_text(objects.widget2__label, encoderSliders[1].label.c_str());
    lv_label_set_text(objects.widget3__label, encoderSliders[2].label.c_str());
    lv_label_set_text(objects.widget4__label, encoderSliders[3].label.c_str());
    lv_label_set_text(objects.widget5__label, encoderSliders[4].label.c_str());
    lv_label_set_text(objects.widget6__label, encoderSliders[5].label.c_str());

    lv_label_set_text(objects.widget1__value, encoderSliders[0].label_value.c_str());
    lv_label_set_text(objects.widget2__value, encoderSliders[1].label_value.c_str());
    lv_label_set_text(objects.widget3__value, encoderSliders[2].label_value.c_str());
    lv_label_set_text(objects.widget4__value, encoderSliders[3].label_value.c_str());
    lv_label_set_text(objects.widget5__value, encoderSliders[4].label_value.c_str());
    lv_label_set_text(objects.widget6__value, encoderSliders[5].label_value.c_str());

    for(int8_t s = 0; s < 6; s++) {
        if (encoderSliders[s].slider_hidden) {
            lv_obj_set_flag(sliders[s], LV_OBJ_FLAG_HIDDEN, true);
        } else {
            lv_obj_set_flag(sliders[s], LV_OBJ_FLAG_HIDDEN, false);
            lv_slider_set_value(sliders[s], encoderSliders[s].percent, LV_ANIM_OFF);          
        }

        if (encoderSliders[s].label_buttonpress_highlighted) {
            add_style_label_bg_yellow(encoderButtonLabels[s]);
        } else {
            remove_style_label_bg_yellow(encoderButtonLabels[s]);
        }
    }

    lv_label_set_text(objects.widget1__label_buttonpress, encoderSliders[0].label_buttonpress.c_str());
    lv_label_set_text(objects.widget2__label_buttonpress, encoderSliders[1].label_buttonpress.c_str());
    lv_label_set_text(objects.widget3__label_buttonpress, encoderSliders[2].label_buttonpress.c_str());
    lv_label_set_text(objects.widget4__label_buttonpress, encoderSliders[3].label_buttonpress.c_str());
    lv_label_set_text(objects.widget5__label_buttonpress, encoderSliders[4].label_buttonpress.c_str());
    lv_label_set_text(objects.widget6__label_buttonpress, encoderSliders[5].label_buttonpress.c_str());
}