#pragma once

#include <stdint.h>
#include <vector>
#include "WString.h"

#include "defines.h"
#include "enum.h"

using namespace std;

class Mixerparameter {

    private:

        MP_ID parameter_id;
        MP_CAT category;        // category of parameter, e.g. setting, channel, fx
        MP_VALUE_TYPE value_type;

        String _name;            // complete long name
        String _name_short;      // short name for thight spaces
        String _config_prefix;   // prefix (or name) in config files

        MP_UOM unitOfMeasurement = MP_UOM::NONE;

        float value_min;
        float value_max;
        float value_standard;

        uint size = 0;
        vector<float> value;

        uint decimal_places;
        float stepsize;

        String GetUnitOfMesaurement(bool spaceInFront = false)
        {
            String result = spaceInFront ? " " : "";

            switch(unitOfMeasurement)
            {
                using enum MP_UOM;
                
                case DB:
                    result += "dB";
                    break;
                case DBFS:
                    result += "dbFS";
                    break;
                case HZ:
                    result += "Hz";
                    break;
                case KHZ:
                    result += "kHz";
                    break;
                case MS:
                    result += "ms";
                    break;
                case PERCENT:
                    result += "%";
                    break;
                case SECONDS:
                    result += "s";
                    break;
                default:
                    result += "";
            }	

	    return result;
        }

    public:
 
        x32_changeflag changeflag_mixer;
        x32_changeflag changeflag_vchannel;

        Mixerparameter (MP_ID mp, MP_CAT cat, String name, uint count)
        {
            parameter_id = mp;
            category = cat;

            _name = name;

            _name_short = name;
            if (_name_short.length() > 10) {
                _name_short.substring(0, 9);
            }

            _config_prefix = name;
            _config_prefix.replace(" ", "_");
            _config_prefix.replace("/", "_");
            _config_prefix.replace("\\", "_");
            _config_prefix.replace(".", "_");
            _config_prefix.toLowerCase();

            size = count;
        }

        Mixerparameter* UoM(MP_UOM uom) 
        {
            unitOfMeasurement = uom;

            switch (uom) {
                using enum MP_UOM;

                case PERCENT:
                    stepsize = 0.01f;  // percent in 1% steps
                    break;
                case SECONDS:
                    stepsize = 0.2f;  // seconds in 200ms steps
                    break;
                default:
                    stepsize = 1.0f;
                    break;
            }

            return this;
        }

        Mixerparameter* ChangeFlags(x32_changeflag cf_mixer, x32_changeflag cf_vchannel) 
        {
            changeflag_mixer = cf_mixer;
            changeflag_vchannel = cf_vchannel;

            return this;
        }

        Mixerparameter* MinMaxStandard_Float(float min, float max, float standard, uint decimals = 0) {
            value_type = MP_VALUE_TYPE::FLOAT;
            value_min = min;
            value_max = max;
            value_standard = standard;
            decimal_places = decimals;

            // fill with default values
            for (uint i = 0; i < size; i++)
            {
                value.push_back(value_standard);
            }

            return this;
        }

        Mixerparameter* MinMaxStandard_Uint(uint8_t min, uint8_t max, uint8_t standard) {
            value_type = MP_VALUE_TYPE::UINT;       
            value_min = min;
            value_max = max;
            value_standard = standard;
            decimal_places = 0;

            // fill with default values
            for (uint i = 0; i < size; i++)
            {
                value.push_back(value_standard);
            }

            return this;
        }

        Mixerparameter* MinMaxStandard_Int(int8_t min, int8_t max, int8_t standard) {
            value_type = MP_VALUE_TYPE::INT;
            value_min = min;
            value_max = max;
            value_standard = standard;
            decimal_places = 0;

            // fill with default values
            for (uint i = 0; i < size; i++)
            {
                value.push_back(value_standard);
            }

            return this;
        }

        Mixerparameter* Stepsize(float steps) {
            stepsize = steps;

            return this;
        }

        //############################################################################################

        MP_ID GetId()
        {
            return parameter_id;
        }

        MP_VALUE_TYPE GetType()
        {
            return value_type;
        }

        uint GetDecimaPlaces()
        {
	        return decimal_places;
        }

        void Reset(uint index = 0)
        {
            value[index] = value_standard;
        }

        void Set(float newValue, uint index = 0)
        {
            if (newValue > value_max) {
                newValue = value_max;
            } else if (newValue < value_min) {
                newValue = value_min;
            }
            
            value[index] = newValue;
        }

        float Get(uint index = 0)
        {
            return GetFloat(index);
        }

        float GetFloat(uint index = 0)
        {
            return value[index];
        }

        int GetInt(uint index = 0)
        {
            return (int)value[index];
        }

        uint GetUint(uint index = 0)
        {
            return (uint)value[index];
        }

        uint GetPercent(uint index = 0)
        {
            float onehunderedpercent = value_max - value_min;
	        float value_normiert = value[index] - value_min;
	        float onepercent = onehunderedpercent / 100.0f;
	        return (uint)(value_normiert / onepercent);
        }

        bool GetBool(uint index = 0)
        {
            return (bool)value[index];
        }

        void Change(int amount, uint index = 0)
        {
            float newValue = value[index] + (amount * stepsize);
            Set(newValue, index);            
        }

        bool Toggle(uint index = 0)
        {
            if (value_type == MP_VALUE_TYPE::BOOL)
            {
                bool bool_value = value[index] != 0.0f;
                value[index] = !bool_value;
            }            
            return false;
        }

        String GetConfigPrefix(uint index = 0)
        {
            if (size == 1)
            {
                return _config_prefix;
            } else {
                return _config_prefix + String(index);
            }
        }

        String GetName(uint index = 0)
        {
            return _name;            
        }

        String GetLabelAndValue(uint index = 0)
        {
            return GetName(index) + String(": ") + FormatValue(index);
        }

        String GetResetLabel(uint index = 0)
        {
            return String("Reset: ") + FormatValue(index);
        }

        String FormatValue(uint index = 0)
        {
            float tempValue = value[index];
            
            if (unitOfMeasurement == MP_UOM::KHZ) {
                tempValue /= 1000;
            }

            if (unitOfMeasurement == MP_UOM::PERCENT) {
               tempValue *= 100;
            }
            return String(tempValue, decimal_places) + GetUnitOfMesaurement();
        }
};