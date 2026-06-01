#pragma once

#include "surfacebindingparameter.h"
#include "mixerparameter.h"

class X32ChannelStrip
{
    public:

        SurfaceBindingParameter* select;
        SurfaceBindingParameter* vumeter;
        SurfaceBindingParameter* solo;
        SurfaceBindingParameter* lcd;
        SurfaceBindingParameter* mute;
        SurfaceBindingParameter* fader;
};

class X32FaderBank
{
    private:

        String _name;
        X32BankId _id;

    public:

        X32ChannelStrip* channelstrip[8];

        X32FaderBank(X32BankId id, String name)
        {
            _id = id;
            _name = name;

            for(uint i = 0; i < 8; i++)
            {
                channelstrip[i] = new X32ChannelStrip();

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

enum class WingBankId
{
    None = -1,
    WING_1_12,
    WING_13_24,
    WING_25_36,
    WING_37_48,
    WING_BUS_1_12,
    WING_BUS_13_24,
    WING_DCA,
    WING_USER1,
    WING_USER2,
    __ELEMENT_COUNTER_DO_NOT_MOVE
};

class WingFaderBank
{
    private:

        String _name;
        WingBankId _id;

    public:

        X32ChannelStrip* channelstrip[12];

        WingFaderBank(WingBankId id, String name)
        {
            _id = id;
            _name = name;

            for(uint i = 0; i < 12; i++)
            {
                channelstrip[i] = new X32ChannelStrip();

                channelstrip[i]->select = new SurfaceBindingParameter();
                channelstrip[i]->vumeter = new SurfaceBindingParameter();
                channelstrip[i]->solo = new SurfaceBindingParameter();
		        channelstrip[i]->lcd = new SurfaceBindingParameter();
                channelstrip[i]->mute = new SurfaceBindingParameter();
                channelstrip[i]->fader = new SurfaceBindingParameter();
            }
        }

        WingBankId GetID()
        {
            return _id;
        }

        String GetName()
        {
            return _name;
        }

        void Reset()
        {
            for(uint i = 0; i < 12; i++)
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