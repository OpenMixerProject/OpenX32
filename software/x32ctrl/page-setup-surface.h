#pragma once
#include "page.h"
using namespace std;

class PageSetupSurface: public Page
{
    private:

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
            bank = config->GetAssignBank((X32AssignBankId)config->GetUint(BANKING_ASSIGN));
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
            lv_roller_set_selected(roller_target_group, selected_mixerparameter, LV_ANIM_OFF);
            UpdateMixerparameterIndex();
        }

        void UpdateMixerparameterIndex()
        {
            String roller_target_items;
            for(uint i = 0; i < config->GetParameter(mixerparameters.at(selected_mixerparameter))->GetInstances(); i++)
            {
                roller_target_items += String(i+1) + String("\n");
            }
            lv_roller_set_options(roller_target_item, roller_target_items.c_str(), LV_ROLLER_MODE_NORMAL);

            lv_roller_set_selected(roller_target_item, selected_mixerparameter_index, LV_ANIM_OFF);
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


            // Mixerparameter Actions

            for (uint i = 0; i < (uint)MixerparameterAction::__ELEMENT_COUNTER_DO_NOT_MOVE; i++)
            {
                actions.push_back((MixerparameterAction)(0 + i));
            }

            String roller_actions;
            for(uint i = 0; i < actions.size(); i++)
            {
                roller_actions += helper->MixerparameterAction2String(actions.at(i)) + String("\n");
            }

            lv_roller_set_options(roller_action, roller_actions.c_str(), LV_ROLLER_MODE_NORMAL);

            // Mixerparameter

            // mixerparameters.push_back(NONE);
            // mixerparameters.push_back(CHANNEL_VOLUME);
            // mixerparameters.push_back(CHANNEL_MUTE);
            // mixerparameters.push_back(CHANNEL_SOLO);
            // // mixerparameters.push_back(MUTE_GROUP_1_MUTE);
            // // mixerparameters.push_back(MUTE_GROUP_2_MUTE);
            // // mixerparameters.push_back(MUTE_GROUP_3_MUTE);
            // // mixerparameters.push_back(MUTE_GROUP_4_MUTE);
            // // mixerparameters.push_back(MUTE_GROUP_5_MUTE);
            // // mixerparameters.push_back(MUTE_GROUP_6_MUTE);
            // mixerparameters.push_back(DMX_ARTNET_VALUE);
            // mixerparameters.push_back(DMX_ARTNET_ENABLE);

            for (uint i = 0; i < (uint)MP_ID::__ELEMENT_COUNTER_DO_NOT_MOVE; i++)
            {
                mixerparameters.push_back((MP_ID)(0 + i));
            }

            String roller_target_groups;
            for(uint i = 0; i < mixerparameters.size(); i++)
            {
                roller_target_groups += config->GetParameter(mixerparameters.at(i))->GetName() + String("\n");
            }
            lv_roller_set_options(roller_target_group, roller_target_groups.c_str(), LV_ROLLER_MODE_NORMAL);


        }

        void OnShow() override
        {
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_1, MixerparameterAction::CHANGE, BANKING_ASSIGN);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_2, "Select");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_3, "Select");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_5, "Select");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_6, String("Select\n\n") + LV_SYMBOL_SAVE + String(" Apply"));
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_6);

            UpdateAll();
        }

        void OnChange(bool force_update) override
        {
            if (config->HasParameterChanged(BANKING_ASSIGN))
            {
                UpdateBank();
            }
        }

        void OnChangeCustomEncoder(SurfaceElementId surface_element_id, int amount) override
        {
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_2:
					selected_control = helper->CheckBoundaries(selected_control, amount, 0, controls.size()-1);
                    UpdateControl();
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_3:
                    selected_action = helper->CheckBoundaries(selected_action, amount, 0, actions.size()-1);
                    UpdateAction();
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_5:
                    selected_mixerparameter = helper->CheckBoundaries(selected_mixerparameter, amount, 0, mixerparameters.size()-1);
                    UpdateMixerparameter();    
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
                case SurfaceElementId::DISPLAY_ENCODER_BUTTON_6:
                    {
                        // Update Binding
                        SurfaceBindingParameter* par = bank->bindingMap->at(controls.at(selected_control));
                        par->FillBindingParameter(actions[selected_action], mixerparameters[selected_mixerparameter], selected_mixerparameter_index);

                        // Reload Bank
                        if (config->GetUint(BANKING_ASSIGN) == (uint)(bank->GetID()))
                        {
                            config->Refresh(BANKING_ASSIGN);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        
};