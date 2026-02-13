#pragma once

#include <stdint.h>
#include <vector>
#include "WString.h"

#include "defines.h"
#include "enum.h"

using namespace std;

/// @brief Mixerparameter contains data and many metadata about a mixer parameter. Def*-functions define, Get*-functions retrieve data, Set*/Change*/Toggle*-functions manipulate data.
class Mixerparameter {

    private:

        MP_ID parameter_id;
        MP_CAT category;        // category of parameter, e.g. setting, channel, fx
        MP_VALUE_TYPE value_type;

        String _name;            // complete long name
        String _name_short;      // short name for thight spaces
        String _name_group;      // logical group the parameter belongs to, e.g. "input" or "bus" for channels, "reverb" or "transientshaper" for fx

        MP_UOM unitOfMeasurement = MP_UOM::NONE;

        float value_min;
        float value_max;
        float value_standard;

        String value_string_standard;

        /// @brief How many instances this Mixerparameter has.
        uint instances = 0;
        
        vector<float> value;
        vector<String> value_string;

        uint decimal_places = 0;
        float stepsize = 1;

        bool value_string_autoincrement_zerobased = false;

        bool readonly = false;

        /// @brief This Mixerparameter must not be included in configfile.
        bool no_config = false;

        /// @brief Hide slider on display encoder.
        bool hide_encoder_slider = false;        

        /// @brief Hide reset label and function on display encoder.
        bool hide_encoder_reset = false;  

        /// @brief Checks if the index is within the specified size of the Mixerparameter.
        /// @param index The index to check. 
        /// @throws std::out_of_range
        void CheckIndex(uint index)
        {
            if (index >= instances)
            {
                __throw_out_of_range((String("The index ") + String(index) + String(" is bigger than the specified instances of ") + String(instances) + String(" (zero based!) of the Mixerparameter ") + GetName() + String(".")).c_str());
            }
        }

        /// @brief Checks if the datatype is the datatype of the Mixerparameter.
        /// @param mp_value_type The datatype to check. 
        /// @throws std::bad_typeid
        void CheckDatatype(MP_VALUE_TYPE mp_value_type)
        {
            if (mp_value_type != value_type)
            {
                __throw_bad_typeid();
            }
        }

        /// @brief Checks if the Mixerparameter is not readonly and his data can be changed.
        /// @throws std::logic_error
        void CheckNotReadonly()
        {
            if (readonly)
            {
                __throw_logic_error((String("The Mixerparameter ") + GetName() + String(" can not be changed, it is readonly.")).c_str());
            }
        }

        String GetUnitOfMesaurement(bool spaceInFront, uint index = 0)
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
                case EQ_TYPE:
                    switch ((uint)value[index])
                    {
                        case 0:
                            result += "Allpass";
                            break;
                        case 1:
                            result += "PEQ";
                            break;
                        case 2:
                            result += "LShv";
                            break;
                        case 3:
                            result += "HShv";
                            break;
                        case 4:
                            result += "Bandp";
                            break;
                        case 5:
                            result += "Notch";
                            break;
                        case 6:
                            result += "HCut";
                            break;
                        case 7:
                            result += "LCut";
                            break;
                        default:
                            result += "???";
                            break;
                    }
                    break;
                default:
                    result += "";
            }	

	    return result;
        }

        String FormatValue_Intern(float value_float)
        {
            using enum MP_UOM;

            if (unitOfMeasurement == KHZ) {
                value_float /= 1000.0f;
            }

            if (unitOfMeasurement == PERCENT) {
               value_float *= 100.0f;
            }

            switch (unitOfMeasurement)
            {
                case EQ_TYPE:
                    return GetUnitOfMesaurement(false);
                case CHANNEL:
                    return String(value_float + 1, 0);
                default:
                    return String(value_float, decimal_places) + GetUnitOfMesaurement(false);
            }            
        }

    public:
 
        Mixerparameter (MP_ID mp, MP_CAT cat, String group, String name, uint count)
        {
            parameter_id = mp;
            category = cat;

            _name_group = group;
            _name = name;

            DefNameShort(name);

            instances = count;
        }

        Mixerparameter* DefNameShort(String name_short) 
        {
            _name_short = name_short;
            if (_name_short.length() > 10) {
                _name_short.substring(0, 9);
            }

            return this;
        }

        Mixerparameter* DefUOM(MP_UOM uom) 
        {
            unitOfMeasurement = uom;

            switch (uom) {
                using enum MP_UOM;

                case PERCENT:
                    stepsize = 0.01f;  // percent in 1% steps
                    decimal_places = 1;
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

        Mixerparameter* DefMinMaxStandard_Float(float min, float max, float standard, uint decimals = 0) {
            value_type = MP_VALUE_TYPE::FLOAT;
            value_min = min;
            value_max = max;
            value_standard = standard;
            decimal_places = decimals;

            // fill with default values
            for (uint i = 0; i < instances; i++)
            {
                value.push_back(value_standard);
            }

            return this;
        }

        Mixerparameter* DefMinMaxStandard_Uint(uint min, uint max, uint standard) {
            value_type = MP_VALUE_TYPE::UINT;       
            value_min = min;
            value_max = max;
            value_standard = standard;
            decimal_places = 0;

            // fill with default values
            for (uint i = 0; i < instances; i++)
            {
                value.push_back(value_standard);
            }

            return this;
        }

        Mixerparameter* DefMinMaxStandard_Int(int min, int max, int standard) {
            value_type = MP_VALUE_TYPE::INT;
            value_min = min;
            value_max = max;
            value_standard = standard;
            decimal_places = 0;

            // fill with default values
            for (uint i = 0; i < instances; i++)
            {
                value.push_back(value_standard);
            }

            return this;
        }

        Mixerparameter* DefStandard_Bool(bool standard) {
            value_type = MP_VALUE_TYPE::BOOL;
            value_min = 0;
            value_max = 1;
            value_standard = standard;
            decimal_places = 0;
            hide_encoder_slider = true;

            // fill with default values
            for (uint i = 0; i < instances; i++)
            {
                value.push_back(value_standard);
            }

            return this;
        }

        Mixerparameter* DefStandard_String(String standard, bool autoincrement_zerobased = false) {
            value_type = MP_VALUE_TYPE::STRING;
            
            value_string_standard = standard;
            value_string_autoincrement_zerobased = autoincrement_zerobased;
            hide_encoder_slider = true;
            hide_encoder_reset = true;

            // fill with default values
            for (uint i = 0; i < instances; i++)
            {
                value_string.push_back(value_string_standard + String(autoincrement_zerobased ? i : i+1));
            }

            return this;
        }

        Mixerparameter* DefStepsize(float steps) {
            stepsize = steps;

            return this;
        }

        /// @brief The data of this Mixerparameter can not be changed, it is readonly.
        Mixerparameter* DefReadonly() {
            readonly = true;

            return this;
        }

        /// @brief Hide the slider on display encoders if this Mixerparameter is bound.
        Mixerparameter* DefHideEncoderSlider() {
            hide_encoder_slider = true;

            return this;
        }

        /// @brief Hide the reset on display encoders if this Mixerparameter is bound.
        Mixerparameter* DefHideEncoderReset() {
            hide_encoder_reset = true;

            return this;
        }

        /// @brief This Mixerparameter must not be included in configfile.
        Mixerparameter* DefNoConfigfile() {
            no_config = true;

            return this;
        }

        //############################################################################################

        /// @brief Get the ID of the Mixerparameter.
        /// @return Mixerparameter ID. 
        MP_ID GetId()
        {
            return parameter_id;
        }

        /// @brief Get the data type of the stored value.
        /// @return The data type.
        MP_VALUE_TYPE GetType()
        {
            return value_type;
        }

        bool GetHideEncoderSlider()
        {
            return hide_encoder_slider;
        }


        bool GetHideEncoderReset()
        {
            return hide_encoder_reset;
        }


        /// @brief Get the category of the Mixerparameter.
        /// @return The category.
        MP_CAT GetCategory()
        {
            return category;
        }

        uint GetDecimaPlaces()
        {
            CheckDatatype(MP_VALUE_TYPE::FLOAT);

	        return decimal_places;
        }

        /// @brief Resets the stored data.
        /// @param index The index of the parameter (usual the vchannel index or FX slot index).
        void Reset(uint index = 0)
        {
            CheckIndex(index);
            CheckNotReadonly();
            
            if (value_type == MP_VALUE_TYPE::STRING)
            {
                value_string[index] = value_string_standard + String(index);
            }
            else
            {
                value[index] = value_standard;
            }
        }

        /// @brief Sets a new value to the Mixerparameter.
        /// @param newValue The new value.
        /// @param index The index of the parameter (usual the vchannel index or FX slot index).
        void Set(float newValue, uint index = 0)
        {
            CheckIndex(index);
            CheckNotReadonly();

            if (newValue > value_max) {
                newValue = value_max;
            } else if (newValue < value_min) {
                newValue = value_min;
            }
            value[index] = newValue;
        }

        /// @brief Sets a new value to the Mixerparameter.
        /// @param newValue The new value.
        /// @param index The index of the parameter (usual the vchannel index or FX slot index).
        void Set(uint newValue, uint index = 0)
        {
            CheckIndex(index);
            CheckNotReadonly();

            if (newValue > value_max) {
                newValue = value_max;
            } else if (newValue < value_min) {
                newValue = value_min;
            }
            value[index] = newValue;
        }

        /// @brief Sets a new value to the Mixerparameter.
        /// @param newValue The new value.
        /// @param index The index of the parameter (usual the vchannel index or FX slot index).
        void Set(int newValue, uint index = 0)
        {
            CheckIndex(index);
            CheckNotReadonly();

            if (newValue > value_max) {
                newValue = value_max;
            } else if (newValue < value_min) {
                newValue = value_min;
            }
            value[index] = newValue;
        }

        /// @brief Sets a new value to the Mixerparameter.
        /// @param newValue The new value.
        /// @param index The index of the parameter (usual the vchannel index or FX slot index).
        void Set(bool newValue, uint index = 0)
        {
            CheckIndex(index);
            CheckNotReadonly();

            if (newValue > value_max) {
                newValue = value_max;
            } else if (newValue < value_min) {
                newValue = value_min;
            }
            value[index] = newValue;
        }

        /// @brief Sets a new value to the Mixerparameter.
        /// @param newValue The new value.
        /// @param index The index of the parameter (usual the vchannel index or FX slot index).
        void Set(String newValue, uint index = 0)
        {
            CheckIndex(index);
            CheckNotReadonly();

            value_string[index] = newValue;
        }

        float Get(uint index = 0)
        {
            CheckIndex(index);
            
            return GetFloat(index);
        }

        float GetFloat(uint index = 0)
        {
            CheckIndex(index);
            
            return value[index];
        }

        int GetInt(uint index = 0)
        {
            CheckIndex(index);
            
            return (int)value[index];
        }

        uint GetUint(uint index = 0)
        {
            CheckIndex(index);
            
            return (uint)value[index];
        }

        String GetString(uint index = 0)
        {
            CheckIndex(index);
            CheckDatatype(MP_VALUE_TYPE::STRING);
            
            return value_string[index];
        }

        uint GetPercent(uint index = 0)
        {
            CheckIndex(index);
            
            float onehunderedpercent = value_max - value_min;
	        float value_normiert = value[index] - value_min;
	        float onepercent = onehunderedpercent / 100.0f;
	        return (uint)(value_normiert / onepercent);
        }

        bool GetBool(uint index = 0)
        {
            CheckIndex(index);
            CheckDatatype(MP_VALUE_TYPE::BOOL);
            
            return (bool)value[index];
        }

        void Change(int amount, uint index = 0)
        {
            CheckIndex(index);
            CheckNotReadonly();

            if (stepsize == 0)
            {
                __throw_logic_error((String("Stepsize of Mixerparameter ") + GetName() + String(" is 0, so no change can happen!")).c_str());
            }
            
            float newValue = value[index] + (amount * stepsize);
            Set(newValue, index);            
        }

        bool Toggle(uint index = 0)
        {
            CheckIndex(index);
            CheckNotReadonly();
            
            if (value_type == MP_VALUE_TYPE::BOOL)
            {
                bool bool_value = value[index] != 0.0f;
                value[index] = !bool_value;
            }            
            return false;
        }

        /// @brief How many instances (Channels or FX-Slots) this parameter has.
        /// @return The number of instances.
        uint GetInstances()
        {        
            return instances;
        }

        String GetConfigGroup()
        { 
            String config_group = _name_group;

            config_group.replace(" ", "_");
            config_group.replace("/", "_");
            config_group.replace("\\", "_");
            config_group.replace(".", "_");
            config_group.toLowerCase();

            return config_group;
        }

        String GetConfigName()
        {            
            String config_name = _name;

            config_name.replace(" ", "_");
            config_name.replace("/", "_");
            config_name.replace("\\", "_");
            config_name.replace(".", "_");
            config_name.toLowerCase();
        
            return config_name;
        }

        String GetName(uint index = 0)
        {
            CheckIndex(index);
            
            return _name;            
        }

        String GetNameShort(uint index = 0)
        {
            CheckIndex(index);
            
            return _name_short;
        }

        String GetLabelAndValue(uint index = 0)
        {
            CheckIndex(index);
            
            return GetName(index) + String(": ") + GetFormatedValue(index);
        }

        String GetShortLabelAndValue(uint index = 0)
        {
            CheckIndex(index);
            
            return GetNameShort(index) + String(": ") + GetFormatedValue(index);
        }

        String GetResetLabel(uint index = 0)
        {
            CheckIndex(index);
            
            return String("Reset: ") + FormatValue_Intern(value_standard);
        }

        String GetFormatedValue(uint index = 0)
        {
            CheckIndex(index);
            
            return FormatValue_Intern(value[index]);
        }

        /// @brief Get wether this Mixerparameter must not be in configfile.
        /// @return True if this Mixerparameter must not be in configfile. 
        bool GetNoConfigfile()
        {            
            return no_config;
        }
};