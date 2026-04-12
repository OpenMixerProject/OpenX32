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

        // Banking Parameter
        X32BankId bank_id = X32BankId::None;
        X32BankTarget bank_target = X32BankTarget::None;

        SurfaceBindingParameter(SurfaceBindingAction action, MP_ID mixerparameter_id, uint mixerparameter_index)
        {
            sb_action = action;
	        mp_id = mixerparameter_id;
	        mp_index = mixerparameter_index;
        }

        SurfaceBindingParameter(X32BankId id, X32BankTarget target)
        {
            sb_action = SurfaceBindingAction::Banking;
            bank_id = id;
	        bank_target = target;
        }
};