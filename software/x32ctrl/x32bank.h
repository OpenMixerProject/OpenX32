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
        }

        X32BankId GetID()
        {
            return _id;
        }

        String GetName()
        {
            return _name;
        }
        
};