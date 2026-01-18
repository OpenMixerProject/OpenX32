#pragma once

#include "WString.h"
#include "enum.h"

class MixerparameterDefinition {
    public:
        String name;
        MIXERPARAMETER_UOM unitOfMeasurement;
        uint8_t decimal_places;
        float value_min;
        float value_max;
        float value_default;
        bool show_slider;

        MixerparameterDefinition() {
            
        }

        MixerparameterDefinition(String _name, MIXERPARAMETER_UOM uom, float min, float max, float standard, uint8_t _decimal_places, bool _show_slider) {
            name = _name;
            unitOfMeasurement = uom;
            value_min = min;
            value_max = max;
            value_default = standard;
            show_slider = _show_slider;
            decimal_places = _decimal_places;
        }
};