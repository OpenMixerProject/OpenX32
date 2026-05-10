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

    // build list of lvgl encoder widgets for easy enumeration
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
    if (config->GetBool(DISPLAY_UTILITY))
    {
        config->Set(DISPLAY_UTILITY, false);
    }
    // Turn off Mute Group Assign Mode
    if (config->GetBool(DISPLAY_MUTE_GROUP))
    {
        config->Set(DISPLAY_MUTE_GROUP, false);
    }

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

    ResetEncoderBinding();

    // show page and trigger a complete update
    OnShow();
    Change(true);
}

void Page::ResetEncoderBinding()
{
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
}

void Page::Change(bool syncAll)
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

        syncAll |= config->HasParameterChanged(SELECTED_CHANNEL);
        OnChange(syncAll);

        // Mute Groups
        if (config->HasParameterChanged(DISPLAY_MUTE_GROUP))
        {
            if (config->GetBool(DISPLAY_MUTE_GROUP))
            {
                // Show Display Encoders
                lv_obj_set_flag(objects.display_encoder_sliders, LV_OBJ_FLAG_HIDDEN, false);

                // Bind Encoder Widgets to Mute Groups
                for (uint i = 0; i < MUTE_GROUPS; i++)
                {
                    config->SurfaceBindCustom(config->CalcSurfaceElementId(SurfaceElementId::DISPLAY_ENCODER_1, i), String("Assign to\n"));
                }
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_1, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, MUTE_GROUP_1);
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_2, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, MUTE_GROUP_2);
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_3, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, MUTE_GROUP_3);
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_4, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, MUTE_GROUP_4);
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_5, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, MUTE_GROUP_5);
                config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_6, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, MUTE_GROUP_6);
            }
            else
            {
                // Rebind encoders from page
                ResetEncoderBinding();
                OnShow();
                OnChange(true);
            }
        }

        if (!config->GetBool(DISPLAY_MUTE_GROUP))
        {
            // hide encoders if wanted
            lv_obj_set_flag(objects.display_encoder_sliders, LV_OBJ_FLAG_HIDDEN, hideEncoders);
        }

        SyncEncoderWidgets(syncAll);
    }
}

void Page::ChangeCustomButton(SurfaceElementId surface_element_id)
{
    OnChangeCustomButton(surface_element_id);
    SyncEncoderWidgets(false);
}

void Page::ChangeCustomEncoder(SurfaceElementId surface_element_id, int amount)
{
    OnChangeCustomEncoder(surface_element_id, amount);
    SyncEncoderWidgets(false);
}

void Page::UpdateMeters() {
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::UpdateMeters()");
    if (initDone) {
        OnUpdateMeters();
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

    // if Surface binding has changed -> refresh all
    if (config->HasSurfaceBindingChanged(elementIdEncoder) || config->HasSurfaceBindingChanged(elementIdButton))
    {
        force = true;
    }
    
    // encoder is not bound
    if (surface_binding_encoder == 0 || surface_binding_encoder->mp_action == MixerparameterAction::NONE)
    {
        ClearEncoder(lvgl_encoder_widget);
    }
    else
    {
        // Custom Display Encoder -> just show the label and clear slider and button
        if (surface_binding_encoder->mp_action == MixerparameterAction::CUSTOM)
        {
            // set label
            lv_label_set_text(lvgl_encoder_widget->Label, surface_binding_encoder->custom_label.c_str());

            // hide slider
            lv_obj_set_flag(lvgl_encoder_widget->Slider, LV_OBJ_FLAG_HIDDEN, true);
        }
        else
        {
            uint index = config->ParameterCalcIndex(surface_binding_encoder);
            MP_ID id = config->ParameterCalcId(surface_binding_encoder);

            if(config->HasParameterChanged(id, index) || force)
            {
                if (id == MP_ID::NONE)
                {
                    ClearEncoder(lvgl_encoder_widget);
                } 
                else
                { 
                    Mixerparameter* parameter = config->GetParameter(id);

                    lv_label_set_text(lvgl_encoder_widget->Label, parameter->GetLabelAndValue(index).c_str());

                    // Hide or Show Slider 
                    if (parameter->GetHideEncoderSlider())
                    {
                        lv_obj_set_flag(lvgl_encoder_widget->Slider, LV_OBJ_FLAG_HIDDEN, true);
                    }
                    else 
                    {
                        lv_obj_set_flag(lvgl_encoder_widget->Slider, LV_OBJ_FLAG_HIDDEN, false);
                        lv_slider_set_value(lvgl_encoder_widget->Slider, parameter->GetPercent(index), LV_ANIM_OFF);          
                    }
                }
            }
        }
    }
    
    // button is not bound
    if (surface_binding_button == 0 || surface_binding_button->mp_action == MixerparameterAction::NONE)
    {
        ClearEncoderButton(lvgl_encoder_widget);
    }
    else
    {
        MP_ID id = config->ParameterCalcId(surface_binding_button);
        uint index = config->ParameterCalcIndex(surface_binding_button);
            
        if (config->HasParameterChanged(id, index) || force)
        {
            if (id == MP_ID::NONE)
            {
                ClearEncoderButton(lvgl_encoder_widget);
            }
            else
            {
                MixerparameterAction action = surface_binding_button->mp_action;
                Mixerparameter* parameter_button = config->GetParameter(id);
                String text;

                // Button Text
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

                // Button Highlight
                if (parameter_button->GetBool(index))
                {
                    add_style_label_bg_yellow(lvgl_encoder_widget->ButtonLabel);
                }
                else
                {
                    remove_style_label_bg_yellow(lvgl_encoder_widget->ButtonLabel);
                }
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