#pragma once

#include "surfacebindingparameter.h"
#include "mixerparameter.h"

class X32BankParameter
{
    public:

        SurfaceBindingParameter* select;
        SurfaceBindingParameter* vumeter;
        SurfaceBindingParameter* solo;
        SurfaceBindingParameter* lcd;
        SurfaceBindingParameter* mute;
        SurfaceBindingParameter* fader;
};

class X32Bank 
{      
    private:
        String _name;
        X32BankId _id;
    
    public:

        X32BankParameter* channelstrip[8];

        X32Bank(X32BankId id, String name)
        {
            _id = id;
            _name = name;

            for(uint i = 0; i < 8; i++)
            {
                channelstrip[i] = new X32BankParameter();

                channelstrip[i]->select = new SurfaceBindingParameter();
                channelstrip[i]->vumeter = new SurfaceBindingParameter();
                channelstrip[i]->solo = new SurfaceBindingParameter();
		        channelstrip[i]->lcd = new SurfaceBindingParameter();
                channelstrip[i]->mute = new SurfaceBindingParameter();
                channelstrip[i]->fader = new SurfaceBindingParameter();
            }
        }

        X32BankId GetID()
        {
            return _id;
        }

        String GetName()
        {
            return _name;
        }

        void Reset()
        {
            for(uint i = 0; i < 8; i++)
            {
                channelstrip[i]->select->FillBindingParameter(MixerparameterAction::NONE, MP_ID::NONE, 0);
                channelstrip[i]->vumeter->FillBindingParameter(MixerparameterAction::NONE, MP_ID::NONE, 0);
                channelstrip[i]->solo->FillBindingParameter(MixerparameterAction::NONE, MP_ID::NONE, 0);
		        channelstrip[i]->lcd->FillBindingParameter(MixerparameterAction::NONE, MP_ID::NONE, 0);
                channelstrip[i]->mute->FillBindingParameter(MixerparameterAction::NONE, MP_ID::NONE, 0);
                channelstrip[i]->fader->FillBindingParameter(MixerparameterAction::NONE, MP_ID::NONE, 0);
            }
        }
        
};