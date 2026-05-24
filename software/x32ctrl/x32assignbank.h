#pragma once

#include <map>

#include "surfacebindingparameter.h"
#include "mixerparameter.h"

class X32AssignBank
{
    private:

        String _name;
        X32AssignBankId _id;

    public:

        map<SurfaceElementId, SurfaceBindingParameter*>* bindingMap = new map<SurfaceElementId, SurfaceBindingParameter*>();

        X32AssignBank(X32AssignBankId id, String name)
        {
            _id = id;
            _name = name;

            bindingMap->insert({SurfaceElementId::ASSIGN_ENCODER_1, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_ENCODER_2, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_ENCODER_3, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_ENCODER_4, new SurfaceBindingParameter()});

            bindingMap->insert({SurfaceElementId::ASSIGN_LCD_1, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_LCD_2, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_LCD_3, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_LCD_4, new SurfaceBindingParameter()});

            bindingMap->insert({SurfaceElementId::ASSIGN_1, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_2, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_3, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_4, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_5, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_6, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_7, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_8, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_9, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_10, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_11, new SurfaceBindingParameter()});
            bindingMap->insert({SurfaceElementId::ASSIGN_12, new SurfaceBindingParameter()});
        }

        X32AssignBankId GetID()
        {
            return _id;
        }

        String GetName()
        {
            return _name;
        }

        void Reset()
        {
            for (auto const& [id, binding] : *bindingMap)
            {
                binding->FillBindingParameter(MixerparameterAction::NONE, MP_ID::NONE, 0);
            }
        }
};

