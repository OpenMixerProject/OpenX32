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

        void Update()
        {
            lv_roller_set_selected(roller_bank, config->GetUint(BANKING_ASSIGN), LV_ANIM_OFF);
            lv_roller_set_selected(roller_control, selected_control, LV_ANIM_OFF);

            SurfaceBindingParameter* binding_parameter = config->GetSurfaceBinding(controls.at(selected_control));
            if (binding_parameter)
            {
                // Action

                uint action_index = 0;
                for (;action_index < actions.size(); action_index++)
                {
                    if (actions[action_index] == binding_parameter->mp_action)
                    {
                        break;
                    }
                }
                selected_action = action_index;
                
                // Mixerparameter

                uint parameter_idx = 0;
                for (;parameter_idx < mixerparameters.size(); parameter_idx++)
                {
                    if (mixerparameters[parameter_idx] == binding_parameter->mp_id)
                    {
                        break;
                    }
                }
                selected_mixerparameter = parameter_idx;

                // Mixerparameter Index

                String roller_target_items;
                for(uint i = 0; i < config->GetParameter(binding_parameter->mp_id)->GetInstances(); i++)
                {
                    roller_target_items += String(i+1) + String("\n");
                }
                lv_roller_set_options(roller_target_item, roller_target_items.c_str(), LV_ROLLER_MODE_NORMAL);

                selected_mixerparameter_index = binding_parameter->mp_index;
                
            }
            else
            {
                selected_action = 0; // None
                selected_mixerparameter = 0; // None
                selected_mixerparameter_index = 0; // None
            }

            lv_roller_set_selected(roller_action, selected_action, LV_ANIM_OFF);
            lv_roller_set_selected(roller_target_group, selected_mixerparameter, LV_ANIM_OFF);
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
            // DEBUG
            config->SurfaceBind(SurfaceElementId::ASSIGN_ENCODER_2, MixerparameterAction::CHANGE, CHANNEL_VOLUME, 2);
            //config->SurfaceBind(SurfaceElementId::, MixerparameterAction::LCD_Channel, NONE, 2);
            config->SurfaceBind(SurfaceElementId::ASSIGN_6, MixerparameterAction::TOGGLE, CHANNEL_SOLO, 2);
            config->SurfaceBind(SurfaceElementId::ASSIGN_8, MixerparameterAction::TOGGLE, MUTE_GROUP_1_MUTE, 0);
            config->SurfaceBind(SurfaceElementId::ASSIGN_10, MixerparameterAction::TOGGLE, CHANNEL_MUTE, 2);

            // DEBUG X32 Compact
            config->SurfaceBind(SurfaceElementId::ASSIGN_1, MixerparameterAction::TOGGLE, CHANNEL_SOLO, 2);

            // Surface Elements

            if (config->IsModelX32Full())
            {
                controls.push_back(SurfaceElementId::ASSIGN_ENCODER_1);
                controls.push_back(SurfaceElementId::ASSIGN_ENCODER_2);
                controls.push_back(SurfaceElementId::ASSIGN_ENCODER_3);
                controls.push_back(SurfaceElementId::ASSIGN_ENCODER_4);
                controls.push_back(SurfaceElementId::ASSIGN_5);
                controls.push_back(SurfaceElementId::ASSIGN_6);
                controls.push_back(SurfaceElementId::ASSIGN_7);
                controls.push_back(SurfaceElementId::ASSIGN_8);
                controls.push_back(SurfaceElementId::ASSIGN_9);
                controls.push_back(SurfaceElementId::ASSIGN_10);
                controls.push_back(SurfaceElementId::ASSIGN_11);
                controls.push_back(SurfaceElementId::ASSIGN_12);
            }
            else if (config->IsModelX32Compact())
            {
                controls.push_back(SurfaceElementId::ASSIGN_1);
                controls.push_back(SurfaceElementId::ASSIGN_2);
                controls.push_back(SurfaceElementId::ASSIGN_3);
                controls.push_back(SurfaceElementId::ASSIGN_4);
                controls.push_back(SurfaceElementId::ASSIGN_5);
                controls.push_back(SurfaceElementId::ASSIGN_6);
                controls.push_back(SurfaceElementId::ASSIGN_7);
                controls.push_back(SurfaceElementId::ASSIGN_8);
            }

            if (config->IsModelX32FullOrCompactOrProducer())
            {
                controls.push_back(SurfaceElementId::MUTE_GROUP_1);
                controls.push_back(SurfaceElementId::MUTE_GROUP_2);
                controls.push_back(SurfaceElementId::MUTE_GROUP_3);
                controls.push_back(SurfaceElementId::MUTE_GROUP_4);
                controls.push_back(SurfaceElementId::MUTE_GROUP_5);
                controls.push_back(SurfaceElementId::MUTE_GROUP_6);
            }

            String roller_options;
            for(uint i = 0; i < controls.size(); i++)
            {
                roller_options += config->GetSurfaceElement(controls.at(i))->GetName() + String("\n");
            }

            lv_roller_set_options(roller_control, roller_options.c_str(), LV_ROLLER_MODE_NORMAL);


            // Mixerparameter Actions

            actions.push_back(MixerparameterAction::NONE);
            actions.push_back(MixerparameterAction::CHANGE);
            actions.push_back(MixerparameterAction::TOGGLE);

            String roller_actions;
            for(uint i = 0; i < actions.size(); i++)
            {
                roller_actions += helper->MixerparameterAction2String(actions.at(i)) + String("\n");
            }

            lv_roller_set_options(roller_action, roller_actions.c_str(), LV_ROLLER_MODE_NORMAL);

            // Mixerparameter

            mixerparameters.push_back(NONE);
            mixerparameters.push_back(CHANNEL_VOLUME);
            mixerparameters.push_back(CHANNEL_MUTE);
            mixerparameters.push_back(CHANNEL_SOLO);
            mixerparameters.push_back(MUTE_GROUP_1_MUTE);
            mixerparameters.push_back(MUTE_GROUP_2_MUTE);
            mixerparameters.push_back(MUTE_GROUP_3_MUTE);
            mixerparameters.push_back(MUTE_GROUP_4_MUTE);
            mixerparameters.push_back(MUTE_GROUP_5_MUTE);
            mixerparameters.push_back(MUTE_GROUP_6_MUTE);
            mixerparameters.push_back(DMX_ARTNET_VALUE);
            mixerparameters.push_back(DMX_ARTNET_ENABLE);

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
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_6, "Select");
        }

        void OnChange(bool force_update) override
        {
            if (config->HasParameterChanged(BANKING_ASSIGN))
            {
                selected_control = 0;
                Update();
            }
        }

        void OnChangeCustomEncoder(SurfaceElementId surface_element_id, int amount) override
        {
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_2:
					selected_control = helper->CheckBoundaries(selected_control, amount, 0, controls.size()-1);
                    Update();
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_3:
                    {
                        selected_action = helper->CheckBoundaries(selected_action, amount, 0, actions.size()-1);

                        SurfaceBindingParameter* binding_parameter = config->GetSurfaceBinding(controls.at(selected_control));
                        if (binding_parameter)
                        {
                            binding_parameter->mp_action = actions[selected_action];
                        }

                        Update();
                    }
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_5:
                    {
                        selected_mixerparameter = helper->CheckBoundaries(selected_mixerparameter, amount, 0, mixerparameters.size()-1);

                        SurfaceBindingParameter* binding_parameter = config->GetSurfaceBinding(controls.at(selected_control));
                        if (binding_parameter)
                        {
                            binding_parameter->mp_id = mixerparameters.at(selected_mixerparameter);
                        }

                        Update();    
                    }
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

                        SurfaceBindingParameter* binding_parameter = config->GetSurfaceBinding(controls.at(selected_control));
                        if (binding_parameter)
                        {
                            binding_parameter->mp_index = selected_mixerparameter_index;
                        }

                        Update();    
                    }
                    break;
                default:
                    break;
            }
        }

        
};