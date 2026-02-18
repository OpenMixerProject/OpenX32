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

Page::Page(PageBaseParameter* pagebasepar) : X32Base(pagebasepar)
{
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::Page()");

    mixer = pagebasepar->mixer;
    surface = pagebasepar->surface;
}

void Page::Init()
{
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::Init()");

    // Prepare Encoder Bindingssystem
    // -> put labels and sliders in binding struct
    encoderbinding[DISPLAY_ENCODER_1] = new PageBinding_Encoder(objects.widget1__label, objects.widget1__value, objects.widget1__slider, objects.widget1__label_buttonpress);
    encoderbinding[DISPLAY_ENCODER_2] = new PageBinding_Encoder(objects.widget2__label, objects.widget2__value, objects.widget2__slider, objects.widget2__label_buttonpress);
    encoderbinding[DISPLAY_ENCODER_3] = new PageBinding_Encoder(objects.widget3__label, objects.widget3__value, objects.widget3__slider, objects.widget3__label_buttonpress);
    encoderbinding[DISPLAY_ENCODER_4] = new PageBinding_Encoder(objects.widget4__label, objects.widget4__value, objects.widget4__slider, objects.widget4__label_buttonpress);
    encoderbinding[DISPLAY_ENCODER_5] = new PageBinding_Encoder(objects.widget5__label, objects.widget5__value, objects.widget5__slider, objects.widget5__label_buttonpress);
    encoderbinding[DISPLAY_ENCODER_6] = new PageBinding_Encoder(objects.widget6__label, objects.widget6__value, objects.widget6__slider, objects.widget6__label_buttonpress);

    OnInit();
    initDone = true;
}

void Page::Show()
{
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::Show()");

    // open tab on Layer0
	if (tabLayer0 != nullptr)
    {
		lv_tabview_set_active(tabLayer0, tabIndex0, LV_ANIM_OFF);
	}

	// open tab on Layer1
	if (tabLayer1 != nullptr)
    {
		lv_tabview_set_active(tabLayer1, tabIndex1, LV_ANIM_OFF);
	}

    // clear encoders from previous page
    for (uint i = 0; i < MAX_DISPLAY_ENCODER; i++)
    {
        ClearEncoder(encoderbinding[i]);
    }

    // hide encoders if wanted
    lv_obj_set_flag(objects.display_encoder_sliders, LV_OBJ_FLAG_HIDDEN, hideEncoders);

    // show page and trigger a complete update
    OnShow();
    OnChange(true);
    SyncEncoderWidgets(true);
}

void Page::Change()
{
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::Change()");

    if (initDone) {
        OnChange(false);
        SyncEncoderWidgets(false);
    }
}

void Page::UpdateMeters() {
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::UpdateMeters()");
    if (initDone) {
        OnUpdateMeters();
    }
}

void Page::DisplayEncoderTurned(X32_ENC encoder, int amount) 
{
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::DisplayEncoderTurned()");

    if (initDone) {

        // let page handle encoder message
        if (!OnDisplayEncoderTurned(encoder, amount))
        {
            // page did not want it, so handle it here
            
            uint encoderIndex = encoder - X32_ENC_ENCODER1;
            if (encoderbinding[encoderIndex]->mp_id_encoder != MP_ID::NONE)
            {
                // save encoder change to Mixerparameter
                config->Change(
                    encoderbinding[encoderIndex]->mp_id_encoder,
                    amount,
                    encoderbinding[encoderIndex]->mp_index);
            }
        }
    }
}

void Page::DisplayButton(X32_BTN button, bool pressed)
{
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::DisplayButton()");

    if (initDone) {
        
        // let page handle button message
        if (!OnDisplayButton(button, pressed))
        {
            // page did not want it, so handle it here

            if (pressed)
            {
                if (button >= X32_BTN_ENCODER1 && button <= X32_BTN_ENCODER6)
                {
                    uint encoderIndex = button - X32_BTN_ENCODER1;

                    if (encoderbinding[encoderIndex]->mp_id_encoder != MP_ID::NONE)
                    {
                        if (config->GetParameter(encoderbinding[encoderIndex]->mp_id_encoder)->GetHideEncoderReset())
                        {
                            // encoder reset funtion is hidden -> do nothing
                        }
                        else if (encoderbinding[encoderIndex]->mp_id_button == MP_ID::NONE) 
                        {
                            // reset data of the Mixerparameter
                            config->Reset(
                                encoderbinding[encoderIndex]->mp_id_encoder,
                                encoderbinding[encoderIndex]->mp_index);
                        }
                        else 
                        {
                            // toggle data of the Mixerparameter
                            config->Toggle(
                                encoderbinding[encoderIndex]->mp_id_button,
                                encoderbinding[encoderIndex]->mp_index);
                        }
                    }
                }
            }
        }
    }
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

/// @brief Binds an encoder automagically to a Mixerparameter.
/// @param encoder The encoder, e.g. DISPLAY_ENCODER_1.
/// @param mp_id Mixerparameter to bind to the encoder, pressing the encoder button resets the parameter to its standard value.
/// @param mp_index The index of the Mixerparameter to bind to. Usually 0 for system settings, otherwise e.g. the channel index or fx slot index.
void Page::BindEncoder(uint encoder, MP_ID mp_id, uint mp_index) {
    if (mp_id == MP_ID::NONE) {
        UnbindEncoder(encoder);
    }
    else 
    {
        encoderbinding[encoder]->mp_index = mp_index;
        encoderbinding[encoder]->mp_id_encoder = mp_id;
        encoderbinding[encoder]->mp_id_button = MP_ID::NONE;
    }
}

/// @brief Binds an encoder automagically to a Mixerparameter.
/// @param encoder The encoder, e.g. DISPLAY_ENCODER_1.
/// @param mp_id Main parameter to bind to the encoder.
/// @param mp_id_button Secondary parameter (only type bool!) to bind to the button of the encoder instead of a reset function.
/// @param mp_index The index of the Mixerparameter to bind to. Usually 0 for system settings, otherwise e.g. the channel index or fx slot index.
void Page::BindEncoder(uint encoder, MP_ID mp_id, MP_ID mp_id_button, uint mp_index) {
    if (mp_id == MP_ID::NONE) {
        UnbindEncoder(encoder);
    }
    else 
    {
        encoderbinding[encoder]->mp_index = mp_index;
        encoderbinding[encoder]->mp_id_encoder = mp_id;
        encoderbinding[encoder]->mp_id_button = mp_id_button;
    }
}

void Page::UnbindEncoder(uint encoder) {
    encoderbinding[encoder]->mp_index = 0;
    encoderbinding[encoder]->mp_id_encoder = MP_ID::NONE;
    encoderbinding[encoder]->mp_id_button = MP_ID::NONE;
}

void Page::UnbindEncoders() {
    for(int8_t s = 0; s < MAX_DISPLAY_ENCODER; s++) {
        UnbindEncoder(s);
    }
}

void Page::SyncEncoderWidgets(bool force) {
    if (hideEncoders) {
        return;
    }

    // if (config->HasParameterChanged(MP_ID::SELECTED_CHANNEL))
    // {
    //     force = true;
    // }

    for(int8_t encoder_index = 0; encoder_index < MAX_DISPLAY_ENCODER; encoder_index++) {

        PageBinding_Encoder* binding = encoderbinding[encoder_index];
        uint targetIndex = binding->mp_index;
                
        // encoder is not bound
        if (binding->mp_id_encoder == MP_ID::NONE)
        {
            // clear all labels and hide slider

            ClearEncoder(binding);
        }
        // encoder is handled entirely by page
        else if (binding->mp_id_encoder == MP_ID::PAGE_CUSTOM_ENCODER)
        {
            lv_label_set_text(binding->Label, custom_encoder[encoder_index].label.c_str());
        }
        // encoder is handled by this:
        else
        {    
            if (config->HasParameterChanged(binding->mp_id_encoder, targetIndex) || force) 
            {
                Mixerparameter* parameter = config->GetParameter(binding->mp_id_encoder);

                if (parameter->GetId() == MP_ID::NONE)
                {
                    __throw_invalid_argument((String("Mixerparameter with enum id ") + String(to_underlying(binding->mp_id_encoder)) + String(" is not defined!")).c_str());
                }        

                lv_label_set_text(binding->Label, parameter->GetName(targetIndex).c_str());
                lv_label_set_text(binding->ValueLabel, parameter->GetFormatedValue(targetIndex).c_str());

                // Hide or Show Slider 
                if (parameter->GetHideEncoderSlider())
                {
                    lv_obj_set_flag(binding->Slider, LV_OBJ_FLAG_HIDDEN, true);
                } else {
                    lv_obj_set_flag(binding->Slider, LV_OBJ_FLAG_HIDDEN, false);
                    lv_slider_set_value(binding->Slider, parameter->GetPercent(targetIndex), LV_ANIM_OFF);          
                }

                if (parameter->GetHideEncoderReset())
                {
                    // empty reset label
                    lv_label_set_text(binding->ButtonLabel, "");
                } 
                else if (binding->mp_id_button == MP_ID::NONE)
                {
                    // button resets the main parameter

                    lv_label_set_text(binding->ButtonLabel, parameter->GetResetLabel(targetIndex).c_str());
                }
            }

            
            if (config->HasParameterChanged(binding->mp_id_button, targetIndex) || force)
            {
                if (binding->mp_id_button != MP_ID::NONE)
                {
                    // button is bound to its own parameter

                    Mixerparameter* parameter_button = config->GetParameter(binding->mp_id_button);

                    lv_label_set_text(binding->ButtonLabel, parameter_button->GetName(targetIndex).c_str());

                    // Button is highlighted if value is true
                    if (parameter_button->GetBool(targetIndex)) {
                        add_style_label_bg_yellow(binding->ButtonLabel);
                    } else {
                        remove_style_label_bg_yellow(binding->ButtonLabel);
                    }
                } else {
                    remove_style_label_bg_yellow(binding->ButtonLabel);
                }
            }
        }
    }
}

void Page::ClearEncoder(PageBinding_Encoder *binding)
{
    lv_label_set_text(binding->Label, "");
    lv_label_set_text(binding->ValueLabel, "");
    lv_obj_set_flag(binding->Slider, LV_OBJ_FLAG_HIDDEN, true);
    lv_label_set_text(binding->ButtonLabel, "");
}

// old
void Page::SetEncoderHighlight(uint encoder, bool highlight) { }
void Page::SetEncoder(uint encoder, String label, String buttonPressLabel){ }
void Page::SetEncoderValue(uint encoder, float float_value)  {}
void Page::SetEncoderValue(uint encoder, uint uint8_t_value)  { }
void Page::SetEncoderValue(uint encoder, int int8_t_value)  { }
void Page::SetEncoderValuesEmpty() { }
