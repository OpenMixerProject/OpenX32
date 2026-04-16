#pragma once

#include "mixerparameter.h"

using namespace std;

class SurfaceBindingParameter
{
    public:

        SurfaceBindingAction sb_action = SurfaceBindingAction::NONE;  

        // Mixerparameter IDs
        MP_ID mp_id = MP_ID::NONE;
        uint mp_index = 0;

        SurfaceBindingParameter(SurfaceBindingAction action, MP_ID mixerparameter_id, uint mixerparameter_index)
        {
            sb_action = action;
	        mp_id = mixerparameter_id;
	        mp_index = mixerparameter_index;
        }
};