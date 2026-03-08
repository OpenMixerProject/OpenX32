#pragma once

#include "mixerparameter.h"

using namespace std;

class SurfaceBinding_Fader
{
    public:

        // Mixerparameter IDs
        MP_ID mp_id = MP_ID::NONE;
        uint mp_index = 0;

        SB_ACTION action = SB_ACTION::NONE;       

        SurfaceBinding_Fader(MP_ID mixerparameter_id, uint mixerparameter_index, SB_ACTION sb_action)
        {
            this->mp_id = mixerparameter_id;
            this->mp_index = mixerparameter_index;
            this->action = sb_action;
        }

};