#pragma once

#include "mixerparameter.h"

using namespace std;


class MixerparameterChanged {

    public:

        MP_ID changed_mp_id;
        uint changed_index;

        MixerparameterChanged(MP_ID mp_id, uint index)
        {
            changed_mp_id = mp_id;
            changed_index = index;
        }
};