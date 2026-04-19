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
    lvgl_encoder_widgets[SurfaceElementId::DISPLAY_ENCODER_1] = new LVGLEncoderWidget(objects.widget1__label, objects.widget1__slider, objects.widget1__label_buttonpress);
    lvgl_encoder_widgets[SurfaceElementId::DISPLAY_ENCODER_2] = new LVGLEncoderWidget(objects.widget2__label, objects.widget2__slider, objects.widget2__label_buttonpress);
    lvgl_encoder_widgets[SurfaceElementId::DISPLAY_ENCODER_3] = new LVGLEncoderWidget(objects.widget3__label, objects.widget3__slider, objects.widget3__label_buttonpress);
    lvgl_encoder_widgets[SurfaceElementId::DISPLAY_ENCODER_4] = new LVGLEncoderWidget(objects.widget4__label, objects.widget4__slider, objects.widget4__label_buttonpress);
    lvgl_encoder_widgets[SurfaceElementId::DISPLAY_ENCODER_5] = new LVGLEncoderWidget(objects.widget5__label, objects.widget5__slider, objects.widget5__label_buttonpress);
    lvgl_encoder_widgets[SurfaceElementId::DISPLAY_ENCODER_6] = new LVGLEncoderWidget(objects.widget6__label, objects.widget6__slider, objects.widget6__label_buttonpress);

    OnInit();
    initDone = true;
}

void Page::Show()
{
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::Show()");

    // Turn off Utility
    config->Set(DISPLAY_UTILITY, false);

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

    // hide encoders if wanted
    lv_obj_set_flag(objects.display_encoder_sliders, LV_OBJ_FLAG_HIDDEN, hideEncoders);

    // Reset Surfacebinding of Display Encoders
    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_1);
    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_2);
    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_3);
    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_4);
    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_5);
    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_6);

    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_1);
    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_2);
    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_3);
    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_4);
    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_5);
    config->SurfaceUnbind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_6);

    config->Reset(DISPLAY_ENCODER_1_ENCODER);
    config->Reset(DISPLAY_ENCODER_2_ENCODER);
    config->Reset(DISPLAY_ENCODER_3_ENCODER);
    config->Reset(DISPLAY_ENCODER_4_ENCODER);
    config->Reset(DISPLAY_ENCODER_5_ENCODER);
    config->Reset(DISPLAY_ENCODER_6_ENCODER);

    config->Reset(DISPLAY_ENCODER_1_BUTTON);
    config->Reset(DISPLAY_ENCODER_2_BUTTON);
    config->Reset(DISPLAY_ENCODER_3_BUTTON);
    config->Reset(DISPLAY_ENCODER_4_BUTTON);
    config->Reset(DISPLAY_ENCODER_5_BUTTON);
    config->Reset(DISPLAY_ENCODER_6_BUTTON);

    config->GetParameter(DISPLAY_ENCODER_1_ENCODER)->SetName("");
    config->GetParameter(DISPLAY_ENCODER_2_ENCODER)->SetName("");
    config->GetParameter(DISPLAY_ENCODER_3_ENCODER)->SetName("");
    config->GetParameter(DISPLAY_ENCODER_4_ENCODER)->SetName("");
    config->GetParameter(DISPLAY_ENCODER_5_ENCODER)->SetName("");
    config->GetParameter(DISPLAY_ENCODER_6_ENCODER)->SetName("");

    config->GetParameter(DISPLAY_ENCODER_1_BUTTON)->SetName("");
    config->GetParameter(DISPLAY_ENCODER_2_BUTTON)->SetName("");
    config->GetParameter(DISPLAY_ENCODER_3_BUTTON)->SetName("");
    config->GetParameter(DISPLAY_ENCODER_4_BUTTON)->SetName("");
    config->GetParameter(DISPLAY_ENCODER_5_BUTTON)->SetName("");
    config->GetParameter(DISPLAY_ENCODER_6_BUTTON)->SetName("");

    // show page and trigger a complete update
    OnShow();
    OnChange(true);
    SyncEncoderWidgets(true);
}

void Page::Change()
{
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::Change()");

    if (initDone)
    {
        // Go to next page
        if (config->HasParameterChanged(DISPLAY_RIGHT) && this->GetNextPage() != X32_PAGE::NONE)
        {
            config->SetParameterUnchanged(DISPLAY_RIGHT);
            config->Set(ACTIVE_PAGE, (uint)(this->GetNextPage()));
        }

        // Go to previous page
        if (config->HasParameterChanged(DISPLAY_LEFT) && this->GetPrevPage() != X32_PAGE::NONE)
        {
            config->SetParameterUnchanged(DISPLAY_LEFT);
            config->Set(ACTIVE_PAGE, (uint)(this->GetPrevPage()));
        }

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

// void Page::DisplayEncoderTurned(X32_ENC encoder, int amount) 
// {
//     helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::DisplayEncoderTurned()");

//     if (initDone) {

//         // let page handle encoder message
//         if (!OnDisplayEncoderTurned(encoder, amount))
//         {
//             // page did not want it, so handle it here
            
//             uint encoderIndex = encoder - X32_ENC_ENCODER1;
//             MP_ID encoder_mp_id = lvgl_encoder_widgets[encoderIndex]->mp_id_encoder;
//             if (encoder_mp_id != NONE && encoder_mp_id != PAGE_CUSTOM_ENCODER)
//             {
//                 // save encoder change to Mixerparameter
//                 config->Change(
//                     lvgl_encoder_widgets[encoderIndex]->mp_id_encoder,
//                     amount,
//                     lvgl_encoder_widgets[encoderIndex]->mp_index);
//             }
//         }
//     }
// }

// void Page::DisplayButton(X32Action action, bool pressed)
// {
//     helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::DisplayButton()");

//     if (initDone) {
        
//         // let page handle button message
//         if (!OnDisplayButton(action, pressed))
//         {
//             // page did not want it, so handle it here

//             if (pressed)
//             {
//                 if (action >= X32Action::Display_EncoderButton_1 && action <= X32Action::Display_EncoderButton_6)
//                 {
//                     uint encoderIndex = action - X32Action::Display_EncoderButton_1;
//                     MP_ID encoder_mp_id = lvgl_encoder_widgets[encoderIndex]->mp_id_encoder;

//                     if (encoder_mp_id != NONE && encoder_mp_id != PAGE_CUSTOM_ENCODER)
//                     {
//                         if (config->GetParameter(lvgl_encoder_widgets[encoderIndex]->mp_id_encoder)->GetHideEncoderReset())
//                         {
//                             // encoder reset funtion is hidden -> do nothing
//                         }
//                         else if (lvgl_encoder_widgets[encoderIndex]->mp_id_button == MP_ID::NONE) 
//                         {
//                             // reset data of the Mixerparameter
//                             config->Reset(
//                                 lvgl_encoder_widgets[encoderIndex]->mp_id_encoder,
//                                 lvgl_encoder_widgets[encoderIndex]->mp_index);
//                         }
//                         else 
//                         {
//                             // toggle data of the Mixerparameter (bool or any type of number)
//                             config->Toggle(
//                                 lvgl_encoder_widgets[encoderIndex]->mp_id_button,
//                                 lvgl_encoder_widgets[encoderIndex]->mp_index);
//                         }
//                     }
//                 }
//             }
//         }
//     }
// }

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

// /// @brief Binds an encoder automagically to a Mixerparameter.
// /// @param encoder The encoder, e.g. DISPLAY_ENCODER_1.
// /// @param mp_id Mixerparameter to bind to the encoder, pressing the encoder button resets the parameter to its standard value.
// /// @param mp_index The index of the Mixerparameter to bind to. Usually 0 for system settings, otherwise e.g. the channel index or fx slot index.
// void Page::BindEncoder(uint encoder, MP_ID mp_id, uint mp_index) {
//     if (mp_id == MP_ID::NONE) {
//         UnbindEncoder(encoder);
//     }
//     else 
//     {
//         encoderbinding[encoder]->mp_index = mp_index;
//         encoderbinding[encoder]->mp_id_encoder = mp_id;
//         encoderbinding[encoder]->mp_id_button = MP_ID::NONE;
//     }
// }

// /// @brief Binds an encoder automagically to a Mixerparameter.
// /// @param encoder The encoder, e.g. DISPLAY_ENCODER_1.
// /// @param mp_id Main parameter to bind to the encoder.
// /// @param mp_id_button Secondary parameter (only type bool!) to bind to the button of the encoder instead of a reset function.
// /// @param mp_index The index of the Mixerparameter to bind to. Usually 0 for system settings, otherwise e.g. the channel index or fx slot index.
// void Page::BindEncoder(uint encoder, MP_ID mp_id, MP_ID mp_id_button, uint mp_index) {
//     if (mp_id == MP_ID::NONE) {
//         UnbindEncoder(encoder);
//     }
//     else 
//     {
//         encoderbinding[encoder]->mp_index = mp_index;
//         encoderbinding[encoder]->mp_id_encoder = mp_id;
//         encoderbinding[encoder]->mp_id_button = mp_id_button;
//     }
// }

// void Page::UnbindEncoder(uint encoder) {
//     encoderbinding[encoder]->mp_index = 0;
//     encoderbinding[encoder]->mp_id_encoder = MP_ID::NONE;
//     encoderbinding[encoder]->mp_id_button = MP_ID::NONE;
// }

// void Page::UnbindEncoders() {
//     for(int8_t s = 0; s < MAX_DISPLAY_ENCODER; s++) {
//         UnbindEncoder(s);
//     }
// }

void Page::SyncEncoderWidgets(bool force)
{
    if (hideEncoders) {
        return;
    }

    SyncEncoderWidget(SurfaceElementId::DISPLAY_ENCODER_1, SurfaceElementId::DISPLAY_ENCODER_BUTTON_1, force);
    SyncEncoderWidget(SurfaceElementId::DISPLAY_ENCODER_2, SurfaceElementId::DISPLAY_ENCODER_BUTTON_2, force);
    SyncEncoderWidget(SurfaceElementId::DISPLAY_ENCODER_3, SurfaceElementId::DISPLAY_ENCODER_BUTTON_3, force);
    SyncEncoderWidget(SurfaceElementId::DISPLAY_ENCODER_4, SurfaceElementId::DISPLAY_ENCODER_BUTTON_4, force);
    SyncEncoderWidget(SurfaceElementId::DISPLAY_ENCODER_5, SurfaceElementId::DISPLAY_ENCODER_BUTTON_5, force);
    SyncEncoderWidget(SurfaceElementId::DISPLAY_ENCODER_6, SurfaceElementId::DISPLAY_ENCODER_BUTTON_6, force);
}

void Page::SyncEncoderWidget(SurfaceElementId elementIdEncoder, SurfaceElementId elementIdButton, bool force)
{
    SurfaceBindingParameter* surface_binding_encoder = config->GetSurfaceBinding(elementIdEncoder);
    SurfaceBindingParameter* surface_binding_button = config->GetSurfaceBinding(elementIdButton);
    LVGLEncoderWidget* lvgl_encoder_widget = lvgl_encoder_widgets[elementIdEncoder];
            
    // encoder is not bound
    if (surface_binding_encoder == 0)
    {
        ClearEncoder(lvgl_encoder_widget);
    }

    // button is not bound
    if (surface_binding_button == 0)
    {
        ClearEncoderButton(lvgl_encoder_widget);
    }
    
    if (surface_binding_encoder != 0)
    {
        uint index = (surface_binding_encoder->mp_action == MixerparameterAction::CHANGE_SELECTED_CHANNEL) ?
            config->GetUint(SELECTED_CHANNEL) : surface_binding_encoder->mp_index;

        if(config->HasParameterChanged(surface_binding_encoder->mp_id, index) || force)
        {
            if (surface_binding_encoder->mp_id == MP_ID::NONE)
            {
                __throw_invalid_argument((String("Mixerparameter with enum id ") + String(to_underlying(surface_binding_encoder->mp_id)) + String(" is not defined!")).c_str());
            }  

            Mixerparameter* parameter = config->GetParameter(surface_binding_encoder->mp_id);

            lv_label_set_text(lvgl_encoder_widget->Label, parameter->GetLabelAndValue(index).c_str());

            // Hide or Show Slider 
            if (parameter->GetHideEncoderSlider())
            {
                lv_obj_set_flag(lvgl_encoder_widget->Slider, LV_OBJ_FLAG_HIDDEN, true);
            } else {
                lv_obj_set_flag(lvgl_encoder_widget->Slider, LV_OBJ_FLAG_HIDDEN, false);
                lv_slider_set_value(lvgl_encoder_widget->Slider, parameter->GetPercent(index), LV_ANIM_OFF);          
            }
        }
    }
    
    if (surface_binding_button != 0)
    {
        MixerparameterAction action = surface_binding_button->mp_action;
        uint index = 0;

        switch (action)
        {
            case MixerparameterAction::TOGGLE_SELECTED_CHANNEL:
            case MixerparameterAction::RESET_SELECTED_CHANNEL:
                index = config->GetUint(SELECTED_CHANNEL);
                break;
            default:
                index = surface_binding_encoder->mp_index;
                break;
        }
            
        if (config->HasParameterChanged(surface_binding_button->mp_id, index) || force)
        {
            if (surface_binding_button->mp_id != MP_ID::NONE)
            {
                Mixerparameter* parameter_button = config->GetParameter(surface_binding_button->mp_id);
                String text;
                if (action == MixerparameterAction::RESET ||
                    action == MixerparameterAction::RESET_SELECTED_CHANNEL)
                {
                    text = parameter_button->GetResetLabel(index);
                }
                else
                {
                    text = parameter_button->GetName();
                }
                lv_label_set_text(lvgl_encoder_widget->ButtonLabel, text.c_str());

                // Button is highlighted if value is true
                if (parameter_button->GetBool(index))
                {
                    add_style_label_bg_yellow(lvgl_encoder_widget->ButtonLabel);
                }
                else
                {
                    remove_style_label_bg_yellow(lvgl_encoder_widget->ButtonLabel);
                }
            }
            else
            {
                remove_style_label_bg_yellow(lvgl_encoder_widget->ButtonLabel);
            }
        }
    }
}

void Page::ClearEncoder(LVGLEncoderWidget *binding)
{
    lv_label_set_text(binding->Label, "");
    lv_obj_set_flag(binding->Slider, LV_OBJ_FLAG_HIDDEN, true);
}

void Page::ClearEncoderButton(LVGLEncoderWidget *binding)
{
    lv_label_set_text(binding->ButtonLabel, "");
    remove_style_label_bg_yellow(binding->ButtonLabel);
}