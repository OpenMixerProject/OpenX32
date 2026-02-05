#pragma once

#include "WString.h"
#include <list>

#include "defines.h"
#include "enum.h"


class MixerparameterDefinition {

    private:
        uint8_t decimal_places;

    public:
        String name;
        MIXERPARAMETER_UOM unitOfMeasurement;
        MIXERPARAMETER_VALUE_TYPE value_type;
        
        float float_value_min;
        float float_value_max;
        float float_value_default;
        float float_stepsize;
        uint8_t uint8_t_value_min;
        uint8_t uint8_t_value_max;
        uint8_t uint8_t_value_default;
        int8_t int8_t_value_min;
        int8_t int8_t_value_max;
        int8_t int8_t_value_default;
        x32_changeflag changeflag_mixer;
        x32_changeflag changeflag_vchannel;
        bool show_slider;

        MixerparameterDefinition() {}  // just to avoid compiler errors

        MixerparameterDefinition(String _name, MIXERPARAMETER_UOM _uom, x32_changeflag _changeflag_mixer, x32_changeflag _changeflag_vchannel, bool _show_slider) {
            name = _name;
            unitOfMeasurement = _uom;
            changeflag_mixer = _changeflag_mixer;
            changeflag_vchannel = _changeflag_vchannel;
            show_slider = _show_slider;
        }

        void SetMinMaxStandardDecimals_Float(float _min, float _max, float _standard, uint8_t _decimal_places) {
            value_type = MIXERPARAMETER_VALUE_TYPE_FLOAT;
            float_value_min = _min;
            float_value_max = _max;
            float_value_default = _standard;
            decimal_places = _decimal_places;

            switch (unitOfMeasurement) {
                case MIXERPARAMETER_UOM_PERCENT:
                    float_stepsize = 0.01f;  // percent in 1% steps
                    break;
                case MIXERPARAMETER_UOM_S:
                    float_stepsize = 0.2f;  // seconds in 200ms steps
                    break;
                default:
                    float_stepsize = 1.0f;
                    break;
            }
        }

        void SetMinMaxStandard_Uint8(uint8_t _min, uint8_t _max, uint8_t _standard) {
            value_type = MIXERPARAMETER_VALUE_TYPE_UINT8;       
            uint8_t_value_min = _min;
            uint8_t_value_max = _max;
            uint8_t_value_default = _standard;
        }

        void SetInt8Definition(int8_t _min, int8_t _max, int8_t _standard) {
            value_type = MIXERPARAMETER_VALUE_TYPE_INT8;
            int8_t_value_min = _min;
            int8_t_value_max = _max;
            int8_t_value_default = _standard;
        }

        uint8_t GetDecimaPlaces() {
	        return decimal_places;
        }
};