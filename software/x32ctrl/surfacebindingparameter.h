#pragma once

#include "mixerparameter.h"

using namespace std;

class SurfaceBindingParameter
{
    public:
    
        MixerparameterAction mp_action = MixerparameterAction::NONE;  
        MP_ID mp_id = MP_ID::NONE;
        uint mp_index = 0;

        X32Action x32_action = X32Action::None;

        SurfaceBindingParameter(MixerparameterAction action, MP_ID mixerparameter_id, uint mixerparameter_index)
        {
            mp_action = action;
	        mp_id = mixerparameter_id;
	        mp_index = mixerparameter_index;
        }

        SurfaceBindingParameter(X32Action action)
        {
	        x32_action = action;
        }
};