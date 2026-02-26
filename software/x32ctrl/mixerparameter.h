#pragma once

#include <stdint.h>
#include <vector>
#include "WString.h"
#include "math.h"

#include "defines.h"
#include "enum.h"

using namespace std;

/// @brief Mixerparameter contains data and many metadata about a mixer parameter. Def*-functions define, Get*-functions retrieve data, Set*/Change*/Toggle*-functions manipulate data.
class Mixerparameter
{
    private:

        MP_ID parameter_id;
        MP_CAT category;        // category of parameter, e.g. setting, channel, fx
        MP_VALUE_TYPE value_type;

        String _name;           // complete long name
        String _name_short;     // short name for thight spaces

        String _config_section;   // logical group the parameter belongs to, e.g. "input" or "bus" for channels, "reverb" or "transientshaper" for fx
        String _config_entry;   // the entry in that group

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
        uint stepmode = 0; // 0: linear, 1: frequency, 2: boolean toggle between 0 and 1
        float stepsize = 1;

        bool value_string_autoincrement_zerobased = false;

        bool readonly = false;

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

        String GetUnitOfMesaurement(bool spaceInFront, uint index, bool isResetLabel)
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
                    result += "Hz"; // this will automatically be converted to kHz for values >= 1000 in FormatValue_Intern()
                    break;
                case PERCENT:
                    result += "%";
                    break;
                case MS:
                    result += "ms";
                    break;
                case SECONDS:
                    result += "s"; // this will automatically be converted to ms or minutes
                    break;
                case EQ_TYPE:
                    switch ((uint) (isResetLabel ? value_standard : value[index]))
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
                case FPGA_ROUTING:
                    {
                        uint chan = (uint) (isResetLabel ? value_standard : value[index]);
                        switch (chan)
                        {
                            case FPGA_INPUT_IDX_OFF:
                                result += "OFF";
                                break;
                            case FPGA_INPUT_IDX_XLR ... (FPGA_INPUT_IDX_CARD - 1):
                                result += String("XLR In ") + chan;
                                break;
                            case FPGA_INPUT_IDX_CARD ... (FPGA_INPUT_IDX_AUX - 1):
                                result += String("Card In ") + (chan - FPGA_INPUT_IDX_CARD + 1) ;
                                break;
                            case FPGA_INPUT_IDX_AUX ... (FPGA_INPUT_IDX_TALKBACK_INT - 1):
                                result += String("AUX In ") + (chan - FPGA_INPUT_IDX_AUX + 1);
                                break;
                            case FPGA_INPUT_IDX_TALKBACK_INT:
                                result += String("Talkback Internal");
                                break;
                            case FPGA_INPUT_IDX_TALKBACK_EXT:
                                result += String("Talkback External");
                                break;
                            case FPGA_INPUT_IDX_DSP_RETURN ... (FPGA_INPUT_IDX_AES50A - 8 - 1):
                                result += String("DSP Out ") + (chan - FPGA_INPUT_IDX_DSP_RETURN + 1);
                                break;
                            case FPGA_INPUT_IDX_DSP_RETURN + 32 ... (FPGA_INPUT_IDX_AES50A - 1):
                                result += String("DSP AuxOut ") + (chan - (FPGA_INPUT_IDX_DSP_RETURN + 32) + 1);
                                break;
                            case FPGA_INPUT_IDX_AES50A ... (FPGA_INPUT_IDX_AES50B - 1):
                                result += String("AES50A In ") + (chan - FPGA_INPUT_IDX_AES50A + 1);
                                break;
                            case FPGA_INPUT_IDX_AES50B ... (FPGA_INPUT_IDX_AES50B + 48 - 1):
                                result += String("AES50B In ") + (chan - FPGA_INPUT_IDX_AES50B + 1);
                                break;
                            default:
                                result += "???";
                        }
                    }
                    break;
                case DSP_ROUTING:
                    {
                        uint chan = (uint) (isResetLabel ? value_standard : value[index]);
                        switch (chan)
                        {
                            case DSP_BUF_IDX_OFF:
                                result += "OFF";
                                break;
                            case DSP_BUF_IDX_DSPCHANNEL ... (DSP_BUF_IDX_AUX - 1):
                                result += String("FPGA -> DSP In ") + chan;
                                break;
                            case DSP_BUF_IDX_AUX ... (DSP_BUF_IDX_DSP2_FX - 1):
                                result += String("FPGA -> DSP In ") + chan;
                                break;
                            case DSP_BUF_IDX_DSP2_FX ... (DSP_BUF_IDX_MIXBUS - 1):
                                result += String("DSP2 -> FX Return ") + (chan - DSP_BUF_IDX_DSP2_FX + 1);
                                break;
                            case DSP_BUF_IDX_MIXBUS ... (DSP_BUF_IDX_MAINLEFT - 1):
                                result += String("Mixbus ") + (chan - DSP_BUF_IDX_MIXBUS + 1);
                                break;
                            case DSP_BUF_IDX_MAINLEFT:
                                result += String("Main L");
                                break;
                            case DSP_BUF_IDX_MAINRIGHT:
                                result += String("Main R");
                                break;
                            case DSP_BUF_IDX_MAINSUB:
                                result += String("Sub");
                                break;
                            case DSP_BUF_IDX_MATRIX ... (DSP_BUF_IDX_DSP2_AUX - 1):
                                result += String("Matrix ") + (chan - DSP_BUF_IDX_MATRIX + 1);
                                break;
                            case DSP_BUF_IDX_DSP2_AUX:
                                result += String("Linux Audio L");
                                break;
                            case DSP_BUF_IDX_DSP2_AUX + 1:
                                result += String("Linux Audio R");
                                break;
                            case DSP_BUF_IDX_DSP2_AUX + 2:
                                result += String("Oscillator 1");
                                break;
                            case DSP_BUF_IDX_DSP2_AUX + 3:
                                result += String("Oscillator 2");
                                break;
                            case DSP_BUF_IDX_DSP2_AUX + 4:
                                result += String("Unused DSP2 Aux 5");
                                break;
                            case DSP_BUF_IDX_DSP2_AUX + 5:
                                result += String("Unused DSP2 Aux 6");
                                break;
                            case DSP_BUF_IDX_DSP2_AUX + 6:
                                result += String("Unused DSP2 Aux 7");
                                break;
                            case DSP_BUF_IDX_DSP2_AUX + 7:
                                result += String("Unused DSP2 Aux 8");
                                break;
                            case DSP_BUF_IDX_MONLEFT:
                                result += String("Monitor L");
                                break;
                            case DSP_BUF_IDX_MONRIGHT:
                                result += String("Monitor R");
                                break;
                            case DSP_BUF_IDX_TALKBACK:
                                result += String("Talkback");
                                break;
                            default:
                                result += "???";
                        }
                    }
                    break;
                case TAPPOINT:
                    switch ((DSP_TAP) (isResetLabel ? value_standard : value[index]))
                    {
                        case DSP_TAP::INPUT:
                            result += "Input";
                            break;
                        case DSP_TAP::POST_EQ:
                            result += "Post EQ";
                            break;
                        case DSP_TAP::POST_FADER:
                            result += "Post Fader";
                            break;
                        case DSP_TAP::PRE_EQ:
                            result += "Pre EQ";
                            break;
                        case DSP_TAP::PRE_FADER:
                            result += "Pre Fader";
                            break;
                    }
                    break;
                case CHANNEL_LCD_MODE:
                    switch((uint) (isResetLabel ? value_standard : value[index]))
                    {
                        case 0:
                            result += "Lite";
                            break;
                        case 1:
                            result += "Detail";
                            break;
                    }
                    break;
                case CARD_NUMBER_OF_CHANNELS:
                    switch((uint) (isResetLabel ? value_standard : value[index]))
                    {
                        case CARD_CHANNELMODE_32IN_32OUT:
                            result += String("32/32");
                            break;
                        case CARD_CHANNELMODE_16IN_16OUT:
                            result += String("16/16");
                            break;
                        case CARD_CHANNELMODE_32IN_8OUT:
                            result += String("32/8");
                            break;
                        case CARD_CHANNELMODE_8IN_32OUT:
                            result += String("8/32");
                            break;
                        case CARD_CHANNELMODE_8IN_8OUT:
                            result += String("8/8");
                            break;
                        case CARD_CHANNELMODE_2IN_2OUT:
                            result += String("2/2");
                            break;
                    }
                    break;
                default:
                    result += "";
            }	

	    return result;
        }

        String FormatValue_Intern(float value_float, uint index, bool isResetLabel)
        {
            using enum MP_UOM;

            if (unitOfMeasurement == PERCENT) {
               value_float *= 100.0f;
            }

            switch (unitOfMeasurement)
            {
                case TAPPOINT:
                case EQ_TYPE:
                case FPGA_ROUTING:
                case DSP_ROUTING:
                case CHANNEL_LCD_MODE:
                case CARD_NUMBER_OF_CHANNELS:
                    return GetUnitOfMesaurement(false, index, isResetLabel);
                case ZERO_BASED_INDEX__START_BY_ONE:
                    return String(value_float + 1, 0);
                case HZ:
                    if (value_float >= 1000.0f) {
                        return String(value_float/1000.0f, 2) + " kHz";
                    }else{
                        return String(value_float, decimal_places) + " Hz";
                    }
                case SECONDS:
                    if (value_float >= 60.0f) {
                        return String(value_float/60.0f, 0) + " min " + String(fmod(value_float, 60.0f), 0) + " s";
                    }else if (value_float >= 1.0f) {
                        return String(value_float, 0) + " s";
                    }else{
                        return String(value_float * 1000.0f, 0) + " ms";
                    }
                default:
                    return String(value_float, decimal_places) + GetUnitOfMesaurement(false, index, isResetLabel);
            }            
        }

    public:
 
        Mixerparameter (MP_ID mp, MP_CAT cat, String name, uint count)
        {
            parameter_id = mp;
            category = cat;

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

        Mixerparameter* DefStepmode(uint mode) {
            stepmode = mode;

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

        /// @brief This Mixerparameter is included in the configfile.
        Mixerparameter* DefConfig(String section, String entry)
        {
            _config_section = section;
            _config_section.replace(" ", "_");
            _config_section.replace("/", "_");
            _config_section.replace("\\", "_");
            _config_section.replace(".", "_");
            _config_section.toLowerCase();

            _config_entry = entry;
            _config_entry.replace(" ", "_");
            _config_entry.replace("/", "_");
            _config_entry.replace("\\", "_");
            _config_entry.replace(".", "_");
            _config_entry.toLowerCase();

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
            if (index < instances)
            {
                return GetFloat(index);
            }

            return 0;
        }

        float GetFloat(uint index = 0)
        {
            if (index < instances)
            {
                return value[index];
            }

            return 0.0f;
        }

        int GetInt(uint index = 0)
        {
            if (index < instances)
            {            
                return (int)value[index];
            }

            return 0;
        }

        uint GetUint(uint index = 0)
        {
            if (index < instances)
            {
                return (uint)value[index];
            }

            return 0;
        }

        String GetString(uint index = 0)
        {
            if (index < instances && value_type == MP_VALUE_TYPE::STRING)
            {
                return value_string[index];
            }

            return "";
        }

        uint GetPercent(uint index = 0)
        {
            if (index < instances)
            {
                float onehunderedpercent = value_max - value_min;
                float value_normiert = value[index] - value_min;
                float onepercent = onehunderedpercent / 100.0f;
                return (uint)(value_normiert / onepercent);
            }

            return 0;
        }

        bool GetBool(uint index = 0)
        {
            if (index < instances && value_type == MP_VALUE_TYPE::BOOL)
            {
                return (bool)value[index];
            }

            return false;
        }

        float GetMin()
        {
            return value_min;
        }

        float GetMax()
        {
            return value_max;
        }

        void Change(int amount, uint index = 0)
        {
            CheckIndex(index);
            CheckNotReadonly();

            if (stepsize == 0)
            {
                __throw_logic_error((String("Stepsize of Mixerparameter ") + GetName() + String(" is 0, so no change can happen!")).c_str());
            }

            float newValue;
            switch (stepmode)
            {
                case 0: // linear
                    newValue = value[index] + (amount * stepsize);
                    break;
                case 1: // frequency
                    newValue = value[index] * powf(2.0f, (amount * stepsize) / 12.0f); // semitone steps
                    break;
                case 2: // boolean toggle
                    newValue = (amount > 0) ? 1.0f : 0.0f;
                    break;
                default:
                    newValue = value[index] + (amount * stepsize);
                    break;
            }
            
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
            return _config_section;
        }

        String GetConfigEntry()
        {            
            return _config_entry;
        }

        String GetName(uint index = 0)
        {
            if (index < instances)
            {
                return _name;
            }
            
            return "";
        }

        String GetNameShort(uint index = 0)
        {
            if (index < instances)
            {
                return _name_short;
            }
            
            return "";
        }

        String GetLabelAndValue(uint index = 0)
        {
            if (index < instances)
            {
                return GetName(index) + String(": ") + GetFormatedValue(index);
            }

            return "";
        }

        String GetShortLabelAndValue(uint index = 0)
        {
            if (index < instances)
            {
               return GetNameShort(index) + String(": ") + GetFormatedValue(index);
            }

            return "";
        }

        String GetResetLabel(uint index = 0)
        {
            if (index < instances)
            {
                return String("Reset: ") + FormatValue_Intern(value_standard, index, true);
            }

            return "";
        }

        String GetFormatedValue(uint index = 0)
        {
            if (index < instances)
            {
                if (value_type == MP_VALUE_TYPE::STRING)
                {
                    return value_string[index];
                }
                else
                {
                    return FormatValue_Intern(value[index], index, false);
                }
            }
            
            return "";
        }

        /// @brief Get wether this Mixerparameter must not be in configfile.
        /// @return True if this Mixerparameter must not be in configfile. 
        bool IsNoConfig()
        {            
            return _config_section == "" || _config_entry == "";
        }

        bool IsReadonly()
        {
            return readonly;
        }
};