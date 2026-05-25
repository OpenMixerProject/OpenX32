#pragma once
#include "page.h"
using namespace std;

class PageSetupSurface: public Page
{
    private:

        uint selected_bank = 0;
        uint selected_control = 0;
        uint selected_action = 0;
        uint selected_mixerparameter = 0;
        uint selected_mixerparameter_index = 0;

        lv_obj_t* roller_bank = objects.setup_surface_roller_bank;
        lv_obj_t* roller_control = objects.setup_surface_roller_control;
        lv_obj_t* roller_action = objects.setup_surface_roller_action;
        lv_obj_t* roller_target_group = objects.setup_surface_roller_target_group;
        lv_obj_t* roller_target_item = objects.setup_surface_roller_target_item;
        
        vector<SurfaceElementId> controls;
        vector<MixerparameterAction> actions;
        vector<MP_ID> mixerparameters;

        X32AssignBank* bank;
        SurfaceBindingParameter* binding_parameter;

        void UpdateAll()
        {
            UpdateBank();
        }

        void UpdateBank()
        {
            bank = config->GetAssignBank((X32AssignBankId)selected_bank);
            lv_roller_set_selected(roller_bank, (uint)bank->GetID(), LV_ANIM_OFF);

            selected_control = 0;
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

                uint parameter_idx = 0;
                for (;parameter_idx < mixerparameters.size(); parameter_idx++)
                {
                    if (mixerparameters[parameter_idx] == binding_parameter->mp_id)
                    {
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

        void UpdateAction()
        {       
            lv_roller_set_selected(roller_action, selected_action, LV_ANIM_OFF);
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
                lv_obj_set_state(roller_target_group, LV_STATE_DISABLED, true);
            }
            else
            {
                lv_roller_set_selected(roller_target_group, selected_mixerparameter, LV_ANIM_OFF);
                lv_obj_set_state(roller_target_group, LV_STATE_DISABLED, false);
            }

            UpdateMixerparameterIndex();
        }

        void UpdateMixerparameterIndex()
        {
            String roller_target_items;
            MP_ID mixerparameterId = mixerparameters.at(selected_mixerparameter);
            Mixerparameter* mixerparameter = config->GetParameter(mixerparameterId);
            uint instances = mixerparameter->GetInstances();

            if (instances == 1)
            {
                lv_obj_set_state(roller_target_item, LV_STATE_DISABLED, true);
            }
            else if (config->ParameterDependsOn(actions[selected_action]) == SELECTED_CHANNEL)
            {
                lv_obj_set_state(roller_target_item, LV_STATE_DISABLED, true);
            }
            else
            {
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
                lv_roller_set_options(roller_target_item, roller_target_items.c_str(), LV_ROLLER_MODE_NORMAL);

                lv_roller_set_selected(roller_target_item, selected_mixerparameter_index, LV_ANIM_OFF);
                lv_obj_set_state(roller_target_item, LV_STATE_DISABLED, false);
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
            // Surface Elements

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
                roller_options += config->GetSurfaceElement(controls.at(i))->GetName() + String("\n");
            }

            lv_roller_set_options(roller_control, roller_options.c_str(), LV_ROLLER_MODE_NORMAL);

            FillActions();
            FillMixerparameter();
        }

        void FillMixerparameter()
        {
            // Mixerparameter

            for (uint i = 0; i < (uint)MP_ID::__ELEMENT_COUNTER_DO_NOT_MOVE; i++)
            {
                MP_ID mixerparameterId = (MP_ID)(0 + i);
                Mixerparameter* mixerparameter = config->GetParameter(mixerparameterId);

                if (mixerparameter->GetType() != MP_VALUE_TYPE::STRING)
                {
                    mixerparameters.push_back((MP_ID)(0 + i));
                }
            }

            String roller_target_groups;
            for (uint i = 0; i < mixerparameters.size(); i++)
            {
                roller_target_groups += config->GetParameter(mixerparameters.at(i))->GetName() + String("\n");
            }
            lv_roller_set_options(roller_target_group, roller_target_groups.c_str(), LV_ROLLER_MODE_NORMAL);
        }

        void FillActions()
        {
            // Mixerparameter Actions

            // on LCDs only show the LCD-Actions
            SurfaceElementId surfaceElement = controls.at(selected_control);

            actions.clear();

            if (
                surfaceElement == SurfaceElementId::ASSIGN_LCD_1 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_2 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_3 ||
                surfaceElement == SurfaceElementId::ASSIGN_LCD_4
            )
            {
                actions.push_back(MixerparameterAction::LCD_Channel);
                actions.push_back(MixerparameterAction::LCD_Assign);
                actions.push_back(MixerparameterAction::LCD_Artnet);
            }
            else
            {
                actions.push_back(MixerparameterAction::NONE);
                actions.push_back(MixerparameterAction::TOGGLE);
                actions.push_back(MixerparameterAction::TOGGLE_SELECTED_CHANNEL);
                actions.push_back(MixerparameterAction::CHANGE);
                actions.push_back(MixerparameterAction::CHANGE_SELECTED_CHANNEL);
                

                // // on the rest only show non-LCD-Actions
                // for (uint i = 0; i < (uint)MixerparameterAction::__ELEMENT_COUNTER_DO_NOT_MOVE; i++)
                // {
                //     MixerparameterAction a = (MixerparameterAction)(0 + i);
                //     switch (a)
                //     {
                //         case MixerparameterAction::LCD_Channel:
                //         case MixerparameterAction::LCD_Assign:
                //         case MixerparameterAction::LCD_Artnet:
                //         case MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL:
                //         case MixerparameterAction::CUSTOM:
                //         case MixerparameterAction::SET__MP_INDIRECT__SELECTED_CHANNEL:
                //         case MixerparameterAction::SET_ON_INDEX:
                //         case MixerparameterAction::SET_TO_INDEX:
                //         case MixerparameterAction::VUMETER:
                //             // do nothing
                //             break;
                //         default:
                //             actions.push_back(a);
                //             break;
                //     }
                // }
            }
            

            String roller_actions;
            for (uint i = 0; i < actions.size(); i++)
            {
                roller_actions += helper->MixerparameterAction2String(actions.at(i)) + String("\n");
            }

            lv_roller_set_options(roller_action, roller_actions.c_str(), LV_ROLLER_MODE_NORMAL);
        }

        void OnShow() override
        {
            //config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_1, MixerparameterAction::CHANGE, BANKING_ASSIGN);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_1, "Bank");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_2, "Select");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_3, "Select");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_5, String("Select\n\n") + LV_SYMBOL_SAVE + String(" Apply"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_5);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_6, String("Select\n\n") + LV_SYMBOL_SAVE + String(" Apply"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_6);

            UpdateAll();
        }

        // void OnChange(bool force_update) override
        // {
        //     if (config->HasParameterChanged(BANKING_ASSIGN))
        //     {
        //         UpdateBank();
        //     }
        // }

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
                            selected_mixerparameter_index = helper->CheckBoundaries(selected_mixerparameter_index, amount, 0, parameter->GetInstances()-1);                           
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