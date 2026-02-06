#pragma once

#include "WString.h"

#include "defines.h"
#include "enum.h"


class MixerparameterDefinition {

    public:
 
        x32_changeflag changeflag_mixer;
        x32_changeflag changeflag_vchannel;

        MP_CAT category;        // category of parameter, e.g. setting, channel, fx

        String name;            // complete long name
        String name_short;      // short name for thight spaces
        String config_prefix;   // prefix (or name) in config files
    
        MP_VALUE_TYPE value_type;

        float float_value_min;
        float float_value_max;
        float float_value_standard;

        uint decimal_places;
        float float_stepsize;

        uint uint_value_min;
        uint uint_value_max;
        uint uint_value_standard;

        int int_value_min;
        int int_value_max;
        int int_value_standard;

        MP_UOM unitOfMeasurement = MP_UOM::NONE;

        MixerparameterDefinition* UoM(MP_UOM uom) 
        {
            unitOfMeasurement = uom;

            switch (uom) {
                case MP_UOM::PERCENT:
                    float_stepsize = 0.01f;  // percent in 1% steps
                    break;
                case MP_UOM::SECONDS:
                    float_stepsize = 0.2f;  // seconds in 200ms steps
                    break;
                default:
                    float_stepsize = 1.0f;
                    break;
            }

            return this;
        }

        MixerparameterDefinition* ChangeFlags(x32_changeflag cf_mixer, x32_changeflag cf_vchannel) 
        {
            changeflag_mixer = cf_mixer;
            changeflag_vchannel = cf_vchannel;

            return this;
        }

        MixerparameterDefinition* MinMaxStandard_Float(float min, float max, float standard, uint decimals = 0) {
            value_type = MP_VALUE_TYPE::FLOAT;
            float_value_min = min;
            float_value_max = max;
            float_value_standard = standard;
            decimal_places = decimals;

            return this;
        }

        MixerparameterDefinition* MinMaxStandard_Uint(uint8_t min, uint8_t max, uint8_t standard) {
            value_type = MP_VALUE_TYPE::UINT;       
            uint_value_min = min;
            uint_value_max = max;
            uint_value_standard = standard;
            decimal_places = 0;

            return this;
        }

        MixerparameterDefinition* MinMaxStandard_Int(int8_t min, int8_t max, int8_t standard) {
            value_type = MP_VALUE_TYPE::INT;
            int_value_min = min;
            int_value_max = max;
            int_value_standard = standard;
            decimal_places = 0;

            return this;
        }

        uint8_t GetDecimaPlaces() {
	        return decimal_places;
        }
};