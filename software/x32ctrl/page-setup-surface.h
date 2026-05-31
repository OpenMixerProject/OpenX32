#pragma once
#include "page.h"
using namespace std;

class PageSetupSurface: public Page
{
    private:

        uint selected_bank = 0;
        uint selected_control = 0;
        uint selected_action = 0;
        uint selected_group = 0;
        uint selected_mixerparameter = 0;
        uint selected_mixerparameter_index = 0;
        uint selected_mixerparameter_index_upper_bound = 0;

        lv_obj_t* roller_bank = objects.setup_surface_roller_bank;
        lv_obj_t* roller_control = objects.setup_surface_roller_control;
        lv_obj_t* roller_group = objects.setup_surface_roller_group;
        lv_obj_t* roller_mixerparameter = objects.setup_surface_roller_mixerparmeter;
        lv_obj_t* roller_action = objects.setup_surface_roller_action;
        lv_obj_t* roller_index = objects.setup_surface_roller_index;
        
        vector<SurfaceElementId> controls;
        vector<MP_CAT> groups;
        vector<MixerparameterAction> actions;
        vector<MP_ID> mixerparameters;

        X32AssignBank* bank;
        SurfaceBindingParameter* binding_parameter;

        void FillGroups()
        {
            String str_roller_groups;
            for (uint i = 0; i < (uint)MP_CAT::__ELEMENT_COUNTER_DO_NOT_MOVE; i++)
            {
                MP_CAT group = (MP_CAT)(i);
                if (group != MP_CAT::DISPLAY)
                {
                    groups.push_back(group);
                    str_roller_groups += helper->MixerparameterCategoryToString(group) + String("\n");
                }
            }

            lv_roller_set_options(roller_group, str_roller_groups.c_str(), LV_ROLLER_MODE_NORMAL);
        }

        void FillMixerparameter()
        {
            mixerparameters.clear();
            selected_mixerparameter = 0;

            String roller_mixerparameters;
            for (uint i = 0; i < (uint)MP_ID::__ELEMENT_COUNTER_DO_NOT_MOVE; i++)
            {
                MP_ID mixerparameterId = (MP_ID)(0 + i);
                Mixerparameter* mixerparameter = config->GetParameter(mixerparameterId);

                if (mixerparameter->GetType() != MP_VALUE_TYPE::STRING && mixerparameter->GetCategory() == groups.at(selected_group))
                {
                    mixerparameters.push_back(mixerparameterId);
                    roller_mixerparameters += config->GetParameter(mixerparameterId)->GetName() + String("\n");
                }
            }

            lv_roller_set_options(roller_mixerparameter, roller_mixerparameters.c_str(), LV_ROLLER_MODE_NORMAL);
        }

        void FillActions()
        {
            SurfaceElementId surfaceElement = controls.at(selected_control);
            actions.clear();

            if (
                surfaceElement == SurfaceElementId::ASSIGN_LCD_1 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_2 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_3 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_4
            )
            {
                // on LCDs only show the LCD-Actions
                actions.push_back(MixerparameterAction::LCD_Channel);
                actions.push_back(MixerparameterAction::LCD_Assign);
                actions.push_back(MixerparameterAction::LCD_Artnet);
            }
            else
            {
                actions.push_back(MixerparameterAction::NONE);
                actions.push_back(MixerparameterAction::TOGGLE);
                actions.push_back(MixerparameterAction::CHANGE);
                actions.push_back(MixerparameterAction::TOGGLE_SELECTED_CHANNEL);
                actions.push_back(MixerparameterAction::CHANGE_SELECTED_CHANNEL);
            }

            String roller_actions;
            for (uint i = 0; i < actions.size(); i++)
            {
                roller_actions += helper->MixerparameterAction2String(actions.at(i)) + String("\n");
            }

            lv_roller_set_options(roller_action, roller_actions.c_str(), LV_ROLLER_MODE_NORMAL);
        }

        void UpdateBank()
        {
            bank = config->GetAssignBank((X32AssignBankId)selected_bank);
            lv_roller_set_selected(roller_bank, (uint)bank->GetID(), LV_ANIM_OFF);

            UpdateControl();
        }

        void UpdateControl()
        {
            lv_roller_set_selected(roller_control, selected_control, LV_ANIM_OFF);

            binding_parameter = bank->bindingMap->at(controls.at(selected_control));

            if (binding_parameter)
            {
                FillActions();
                uint action_index = 0;
                for (;action_index < actions.size(); action_index++)
                {
                    if (actions[action_index] == binding_parameter->mp_action)
                    {
                        break;
                    }
                }
                selected_action = action_index;
                UpdateAction();

                uint group_idx = 0;
                MP_CAT cat = config->GetParameter(binding_parameter->mp_id)->GetCategory();
                for (uint i = 0;i < groups.size(); i++)
                {
                    if (groups[i] == cat)
                    {
                        group_idx = i;
                        break;
                    }
                }
                selected_group = group_idx;
                UpdateGroup();                
                
                uint parameter_idx = 0;
                for (uint i = 0;i < mixerparameters.size(); i++)
                {
                    if (mixerparameters[i] == binding_parameter->mp_id)
                    {
                        parameter_idx = i;
                        break;
                    }
                }
                selected_mixerparameter = parameter_idx;
                UpdateMixerparameter();

                

                selected_mixerparameter_index = binding_parameter->mp_index;

                UpdateMixerparameterIndex();
            }
            else
            {
                selected_action = 0; // None
                selected_mixerparameter = 0; // None
                selected_mixerparameter_index = 0; // None

                UpdateAction();
                UpdateMixerparameter();
                UpdateMixerparameterIndex();
            }
        }

        void UpdateGroup()
        {
            SurfaceElementId surfaceElement = controls.at(selected_control);
            if (
                surfaceElement == SurfaceElementId::ASSIGN_LCD_1 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_2 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_3 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_4
            )
            {
                lv_obj_set_state(roller_group, LV_STATE_DISABLED, true);
            }
            else
            {
                lv_roller_set_selected(roller_group, selected_group, LV_ANIM_OFF);
                lv_obj_set_state(roller_group, LV_STATE_DISABLED, false);

                FillMixerparameter();
            }

            UpdateMixerparameter();
        }

        void UpdateMixerparameter()
        {
            SurfaceElementId surfaceElement = controls.at(selected_control);
            if (
                surfaceElement == SurfaceElementId::ASSIGN_LCD_1 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_2 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_3 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_4
            )
            {
                lv_obj_set_state(roller_mixerparameter, LV_STATE_DISABLED, true);
            }
            else
            {
                lv_roller_set_selected(roller_mixerparameter, selected_mixerparameter, LV_ANIM_OFF);
                lv_obj_set_state(roller_mixerparameter, LV_STATE_DISABLED, false);
            }

            UpdateMixerparameterIndex();
        }

        void UpdateAction()
        {       
            lv_roller_set_selected(roller_action, selected_action, LV_ANIM_OFF);
        }

        void UpdateMixerparameterIndex()
        {
            String roller_target_items;
            MP_ID mixerparameterId = mixerparameters.at(selected_mixerparameter);
            Mixerparameter* mixerparameter = config->GetParameter(mixerparameterId);
            uint instances = mixerparameter->GetInstances();

            if (actions[selected_action] == MixerparameterAction::LCD_Channel)
            {
                selected_mixerparameter_index_upper_bound = MAX_VCHANNELS;

                for(uint i = 0; i < MAX_VCHANNELS; i++)
                {
                    roller_target_items += config->GetString(CHANNEL_NAME, i) + String("\n");
                }
                lv_roller_set_options(roller_index, roller_target_items.c_str(), LV_ROLLER_MODE_NORMAL);

                lv_roller_set_selected(roller_index, selected_mixerparameter_index, LV_ANIM_OFF);
                lv_obj_set_state(roller_index, LV_STATE_DISABLED, false);
            }
            else if (actions[selected_action] == MixerparameterAction::LCD_Assign)
            {
                selected_group = 0;
                selected_mixerparameter = 0;
                selected_mixerparameter_index = 0;
                selected_mixerparameter_index_upper_bound = 0;
                lv_obj_set_state(roller_index, LV_STATE_DISABLED, true);
            }
            else if (actions[selected_action] == MixerparameterAction::LCD_Artnet)
            {
                selected_mixerparameter_index_upper_bound = MAX_ARTNET_CHANNELS;

                for(uint i = 0; i < MAX_ARTNET_CHANNELS; i++)
                {
                    roller_target_items += String("DMX") + String(i+1) + String("\n");
                }
                lv_roller_set_options(roller_index, roller_target_items.c_str(), LV_ROLLER_MODE_NORMAL);

                lv_roller_set_selected(roller_index, selected_mixerparameter_index, LV_ANIM_OFF);
                lv_obj_set_state(roller_index, LV_STATE_DISABLED, false);
            }
            else if (config->ParameterDependsOn(actions[selected_action]) == SELECTED_CHANNEL)
            {
                selected_mixerparameter_index_upper_bound = 0;
                lv_obj_set_state(roller_index, LV_STATE_DISABLED, true);
                
            }
            else
            {
                selected_mixerparameter_index_upper_bound = mixerparameter->GetInstances()-1;

                for(uint i = 0; i < instances; i++)
                {
                    if (mixerparameter->BelongsToChannel())
                    {
                        roller_target_items += config->GetString(CHANNEL_NAME, i) + String("\n");
                    }
                    else if (mixerparameter->BelongsToFX())
                    {
                        roller_target_items += String("FX Slot ") + String(i+1) + String("\n");
                    }
                    else
                    {
                        roller_target_items += String(i+1) + String("\n");
                    }
                }
                lv_roller_set_options(roller_index, roller_target_items.c_str(), LV_ROLLER_MODE_NORMAL);

                lv_roller_set_selected(roller_index, selected_mixerparameter_index, LV_ANIM_OFF);
                lv_obj_set_state(roller_index, LV_STATE_DISABLED, false);
            }           
        }

    public:

        PageSetupSurface(PageBaseParameter* pagebasepar) : Page(pagebasepar)
        {
            prevPage = X32_PAGE::SETUP_CARD ;
            nextPage = X32_PAGE::ABOUT;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 2;
            led = X32_BTN_SETUP;
        }

        void OnInit() override
        {
            controls.push_back(SurfaceElementId::ASSIGN_ENCODER_1);
            controls.push_back(SurfaceElementId::ASSIGN_ENCODER_2);
            controls.push_back(SurfaceElementId::ASSIGN_ENCODER_3);
            controls.push_back(SurfaceElementId::ASSIGN_ENCODER_4);
            controls.push_back(SurfaceElementId::ASSIGN_LCD_1);
            controls.push_back(SurfaceElementId::ASSIGN_LCD_2);
            controls.push_back(SurfaceElementId::ASSIGN_LCD_3);
            controls.push_back(SurfaceElementId::ASSIGN_LCD_4);
            controls.push_back(SurfaceElementId::ASSIGN_1);
            controls.push_back(SurfaceElementId::ASSIGN_2);
            controls.push_back(SurfaceElementId::ASSIGN_3);
            controls.push_back(SurfaceElementId::ASSIGN_4);
            controls.push_back(SurfaceElementId::ASSIGN_5);
            controls.push_back(SurfaceElementId::ASSIGN_6);
            controls.push_back(SurfaceElementId::ASSIGN_7);
            controls.push_back(SurfaceElementId::ASSIGN_8);
            controls.push_back(SurfaceElementId::ASSIGN_9);
            controls.push_back(SurfaceElementId::ASSIGN_10);
            controls.push_back(SurfaceElementId::ASSIGN_11);
            controls.push_back(SurfaceElementId::ASSIGN_12);

            String roller_options;
            for(uint i = 0; i < controls.size(); i++)
            {
                SurfaceElementId elementId = controls.at(i);

                bool disableControl = false;
                if (config->IsModelX32Full())
                {
                    switch (elementId)
                    {
                        case SurfaceElementId::ASSIGN_1:
                        case SurfaceElementId::ASSIGN_2:
                        case SurfaceElementId::ASSIGN_3:
                        case SurfaceElementId::ASSIGN_4:
                            disableControl = true;
                            break;
                        default:
                            break;
                    }
                }

                String control;
                if (disableControl) control += "[";
                control += config->GetSurfaceElement(elementId)->GetName();
                if (disableControl) control += "]";
                control += "\n";
                
                roller_options += control;
            }

            lv_roller_set_options(roller_control, roller_options.c_str(), LV_ROLLER_MODE_NORMAL);

            FillGroups();
            FillActions();
            FillMixerparameter();
        }

        void OnShow() override
        {
            //config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_1, MixerparameterAction::CHANGE, BANKING_ASSIGN);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_1, LV_SYMBOL_REFRESH);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_2, LV_SYMBOL_REFRESH);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_3, LV_SYMBOL_REFRESH);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_4, String(LV_SYMBOL_REFRESH) + String("\n\n") + LV_SYMBOL_SAVE + String(" Apply"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_4);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_5, String(LV_SYMBOL_REFRESH) + String("\n\n") + LV_SYMBOL_SAVE + String(" Apply"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_5);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_6, String(LV_SYMBOL_REFRESH) + String("\n\n") + LV_SYMBOL_SAVE + String(" Apply"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_6);

            UpdateBank();
        }

        void OnChangeCustomEncoder(SurfaceElementId surface_element_id, int amount) override
        {
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_1:
					selected_bank = helper->CheckBoundaries(selected_bank, amount, 0, 2);
                    UpdateBank();
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_2:
					selected_control = helper->CheckBoundaries(selected_control, amount, 0, controls.size()-1);
                    UpdateControl();
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_3:
					selected_group = helper->CheckBoundaries(selected_group, amount, 0, groups.size()-1);
                    UpdateGroup();
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_4:
                    selected_mixerparameter = helper->CheckBoundaries(selected_mixerparameter, amount, 0, mixerparameters.size()-1);
                    UpdateMixerparameter();    
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_5:
                    selected_action = helper->CheckBoundaries(selected_action, amount, 0, actions.size()-1);
                    UpdateAction();
                    UpdateMixerparameterIndex();
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_6:
                    {
                        Mixerparameter* parameter = config->GetParameter(mixerparameters.at(selected_mixerparameter));
                        if (parameter)
                        {
                            selected_mixerparameter_index = helper->CheckBoundaries(selected_mixerparameter_index, amount, 0, selected_mixerparameter_index_upper_bound);                           
                        }
                        else
                        {
                            selected_mixerparameter_index = 0;
                        }

                        UpdateMixerparameterIndex();    
                    }
                    break;
                default:
                    break;
            }
        }

        void OnChangeCustomButton(SurfaceElementId surface_element_id) override
        {
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_BUTTON_4:
                case SurfaceElementId::DISPLAY_ENCODER_BUTTON_5:
                case SurfaceElementId::DISPLAY_ENCODER_BUTTON_6:
                    {
                        // Apply Binding
                        SurfaceBindingParameter* par = bank->bindingMap->at(controls.at(selected_control));
                        par->FillBindingParameter(actions[selected_action], mixerparameters[selected_mixerparameter], selected_mixerparameter_index);

                        // Reload Bank
                        config->Refresh(BANKING_ASSIGN);
                    }
                    break;
                default:
                    break;
            }
        }
};