#pragma once

#include "mixerparameter.h"

using namespace std;



class PageBinding_Encoder {
    public:

        // Mixerparameter IDs
        MP_ID mp_id_encoder;
        MP_ID mp_id_button;

        // GUI Handles
        lv_obj_t* Label;
        lv_obj_t* ValueLabel;
        lv_obj_t* Slider;
        lv_obj_t* ButtonLabel;

        PageBinding_Encoder(lv_obj_t* encoder_label, lv_obj_t* encoder_value_label, lv_obj_t* encoder_slider, lv_obj_t* encoder_button_label) {
            mp_id_encoder = MP_ID::NONE;
            mp_id_button = MP_ID::NONE;
            
            Label = encoder_label;
            ValueLabel = encoder_value_label;
            Slider = encoder_slider;
            ButtonLabel = encoder_button_label;
        }

};