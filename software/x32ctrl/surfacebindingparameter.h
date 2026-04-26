#pragma once

#include "mixerparameter.h"

using namespace std;

class SurfaceBindingParameter
{
    public:
    
        MixerparameterAction mp_action = MixerparameterAction::NONE;  
        MP_ID mp_id = MP_ID::NONE;
        uint mp_index = 0;
        uint led_value;

        SurfaceBindingParameter(MixerparameterAction action, MP_ID mixerparameter_id, uint mixerparameter_index, uint led_value = 0)
        {
            mp_action = action;
	        mp_id = mixerparameter_id;
	        mp_index = mixerparameter_index;
            this->led_value = led_value;
        }
};