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
    surface = pagebasepar->surface;
}

void Page::Init() {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::Init()");

    // Prepare Encoder Bindingssystem
    encoderbinding[DISPLAY_ENCODER_1] = new PageBindingEncoder(objects.widget1__label, objects.widget1__value, objects.widget1__slider, objects.widget1__label_buttonpress);
    encoderbinding[DISPLAY_ENCODER_2] = new PageBindingEncoder(objects.widget2__label, objects.widget2__value, objects.widget2__slider, objects.widget2__label_buttonpress);
    encoderbinding[DISPLAY_ENCODER_3] = new PageBindingEncoder(objects.widget3__label, objects.widget3__value, objects.widget3__slider, objects.widget3__label_buttonpress);
    encoderbinding[DISPLAY_ENCODER_4] = new PageBindingEncoder(objects.widget4__label, objects.widget4__value, objects.widget4__slider, objects.widget4__label_buttonpress);
    encoderbinding[DISPLAY_ENCODER_5] = new PageBindingEncoder(objects.widget5__label, objects.widget5__value, objects.widget5__slider, objects.widget5__label_buttonpress);
    encoderbinding[DISPLAY_ENCODER_6] = new PageBindingEncoder(objects.widget6__label, objects.widget6__value, objects.widget6__slider, objects.widget6__label_buttonpress);

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
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::UpdateMeters()");
    if (initDone) {
        OnUpdateMeters();
    }
}

void Page::OnUpdateMeters() {
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::OnUpdateMeters()");
}

void Page::DisplayEncoderTurned(X32_ENC encoder, int amount) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::DisplayEncoderTurned()");
    if (initDone) {
        //if (!
        // // let page handle encoder message first (if it wants)
        OnDisplayEncoderTurned(encoder, amount);
        //) {
            mixer->Change(encoderbinding[encoder]->mp_id_encoder, amount);
        //}
    }
}

void Page::OnDisplayEncoderTurned(X32_ENC encoder, int amount) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::OnDisplayEncoderTurned()");
}

void Page::DisplayButton(X32_BTN button, bool pressed) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::DisplayButton()");
    if (initDone) {
        OnDisplayButton(button, pressed);
    }
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

/// @brief Binds an encoder to a mixerparameter
/// @param encoder the encoder
/// @param mp parameter to bind to the encoder, button press resets the parameter
void Page::BindEncoder(uint encoder, MP_ID mp_id) {
    if (mp_id == MP_ID::NONE) {
        UnbindEncoder(encoder);
    }
    else 
    {
        encoderbinding[encoder]->mp_id_encoder = mp_id;
        encoderbinding[encoder]->mp_id_button = MP_ID::NONE;
    }
}

/// @brief Binds an encoder to a mixerparameter
/// @param encoder the encoder
/// @param mp main parameter to bind to the encoder itself
/// @param mp_button secondary parameter (only type bool) to bind to button press instead of a reset function
void Page::BindEncoder(uint encoder, MP_ID mp_id, MP_ID mp_id_button) {
    if (mp_id == MP_ID::NONE) {
        UnbindEncoder(encoder);
    }
    else 
    {
        encoderbinding[encoder]->mp_id_encoder = mp_id;
        encoderbinding[encoder]->mp_id_button = mp_id_button;
    }
}

void Page::SetEncoder(uint encoder, String label, String buttonPressLabel){
    encoderSliders[encoder].mp = MP_ID::NONE;

    encoderSliders[encoder].label = label;
    encoderSliders[encoder].label_buttonpress = buttonPressLabel;
    encoderSliders[encoder].slider_hidden = true;
}

void Page::SetEncoderValue(uint encoder, float float_value)  {
    if (encoderSliders[encoder].mp == MP_ID::NONE) {
        return;
    }

    // Mixerparameter* parameter = mixer->GetParameter(encoderSliders[encoder].mp);
    // encoderSliders[encoder].label_value = parameter->getl helper->FormatValue(float_value, mpd);
    // encoderSliders[encoder].percent = helper->value2percent(float_value, mpd->float_value_min, mpd->float_value_max);
}

void Page::SetEncoderValue(uint encoder, uint uint8_t_value)  {
    if (encoderSliders[encoder].mp == MP_ID::NONE) {
        return;
    }

    // Mixerparameter* mpd = helper->GetMixerparameterDefinition(encoderSliders[encoder].mp);
    // encoderSliders[encoder].label_value = helper->FormatValue(uint8_t_value, mpd);
    // encoderSliders[encoder].percent = helper->value2percent(uint8_t_value, mpd->uint_value_min, mpd->uint_value_max);
}

void Page::SetEncoderValue(uint encoder, int int8_t_value)  {
    if (encoderSliders[encoder].mp == MP_ID::NONE) {
        return;
    }

    // Mixerparameter* mpd = helper->GetMixerparameterDefinition(encoderSliders[encoder].mp);
    // encoderSliders[encoder].label_value = helper->FormatValue(int8_t_value, mpd);
    // encoderSliders[encoder].percent = helper->value2percent(int8_t_value, mpd->int_value_min, mpd->int_value_max);
}

void Page::UnbindEncoder(uint encoder) {
    encoderbinding[encoder]->mp_id_encoder = MP_ID::NONE;
    encoderbinding[encoder]->mp_id_button = MP_ID::NONE;
}

void Page::UnbindEncoders() {
    for(int8_t s = 0; s < MAX_DISPLAY_ENCODER; s++) {
        UnbindEncoder(s);
    }
}

void Page::SetEncoderValuesEmpty(){
    for(int8_t s = 0; s < MAX_DISPLAY_ENCODER; s++) {
        encoderSliders[s].label_value = "";
        encoderSliders[s].percent = 0;
    }
}

void Page::SetEncoderHighlight(uint encoder, bool highlight) {
    encoderSliders[encoder].label_buttonpress_highlighted = highlight;
}

void Page::SyncEncoderWidgets() {
    if (hideEncoders) {
        return;
    }

    // TODO: Just update, if parameter has really changed (new Eventsystem)

    uint8_t chanIndex = config->selectedVChannel;

    for(int8_t s = 0; s < MAX_DISPLAY_ENCODER; s++) {

        PageBindingEncoder* binding = encoderbinding[s];
        Mixerparameter* parameter = mixer->GetParameter(binding->mp_id_encoder);

        
        if (binding->mp_id_encoder == MP_ID::NONE ||  // encoder is not bound 
            parameter->GetId() == MP_ID::NONE)        // parameter is not (yet) defined
        {
            // clear all labels and hide slider

            lv_label_set_text(binding->Label, "");
            lv_label_set_text(binding->ValueLabel, "");
            lv_obj_set_flag(binding->Slider, LV_OBJ_FLAG_HIDDEN, true);
            lv_label_set_text(binding->ButtonLabel, "");

            continue;
        }

        lv_label_set_text(binding->Label, parameter->GetName(chanIndex).c_str());
        lv_label_set_text(binding->ValueLabel, parameter->FormatValue(chanIndex).c_str());

        // Hide Slider for Boolean and String
        if (parameter->GetType() == MP_VALUE_TYPE::BOOL ||
            parameter->GetType() == MP_VALUE_TYPE::STRING)
        {
            lv_obj_set_flag(binding->Slider, LV_OBJ_FLAG_HIDDEN, true);
        } else {
            lv_obj_set_flag(binding->Slider, LV_OBJ_FLAG_HIDDEN, false);
            lv_slider_set_value(binding->Slider, parameter->GetPercent(chanIndex), LV_ANIM_OFF);          
        }

        if (binding->mp_id_button == MP_ID::NONE)
        {
            // button resets the main parameter

            lv_label_set_text(binding->ButtonLabel, parameter->GetResetLabel(chanIndex).c_str());
        }
        else
        {
            // button is bound to its own parameter

            Mixerparameter* parameter_button = mixer->GetParameter(binding->mp_id_button);

            lv_label_set_text(binding->ButtonLabel, parameter_button->GetName(chanIndex).c_str());

            // Button is highlighted if value is true
            if (parameter_button->GetBool(chanIndex)) {
                add_style_label_bg_yellow(binding->ButtonLabel);
            } else {
                remove_style_label_bg_yellow(binding->ButtonLabel);
            }
        }
    }
}