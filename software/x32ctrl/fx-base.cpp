/*
    ____                  __   ______ ___  
   / __ \                 \ \ / /___ \__ \ 
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
  | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |                               
         |_|                               
  
  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "fx-base.h"

FxBase::FxBase(X32BaseParameter* basepar) : X32Base(basepar){ }


String FxBase::GetName() {
    return "";
}

void FxBase::InitParameters() {
    helper->DEBUG_FX(DEBUGLEVEL_VERBOSE, "FxBase::InitParameters");
    
    // Load default values
    for (int p = 0; p < parameters.size(); p++) {
        parameter_value.push_back(0.0f); // create parameter_value before using it
        ResetParameter(p);
    }
}

uint8_t FxBase::GetParameterCount() {
    return parameters.size();
}

MIXERPARAMETER FxBase::GetParameterDefinition(uint8_t index) {
    if (index >= parameters.size()) {
        return MIXERPARAMETER_NONE;
    }

    helper->DEBUG_FX(DEBUGLEVEL_VERBOSE, "FxBase::GetParameterDefinition(%d)", index);
    return parameters[index];
}

float FxBase::GetParameter(uint8_t parIdx) {
    if (parIdx >= parameters.size()) {        
        return 0.0f;
    }

    return parameter_value[parIdx];
}

bool FxBase::ChangeParameter(uint8_t parIdx, int8_t amount) {
    if (parIdx < parameters.size()) {        
        float newValue = helper->CalcNewValue(parameter_value[parIdx], amount, parameters[parIdx]);
        return SetParameter(parIdx, newValue);
    }
    return false;
}

bool FxBase::SetParameter(uint8_t parIdx, float value) {
    if (parIdx < parameters.size()) {
        helper->DEBUG_FX(DEBUGLEVEL_VERBOSE, "%s parameterIdx=%d value=%f", GetName().c_str(), parIdx, (double)value);
                
        parameter_value[parIdx] = value;
        state->SetChangeFlags(X32_MIXER_CHANGED_FX);

        return true;
    }
    return false;
}

bool FxBase::ResetParameter(uint8_t parIdx) {
    if (parIdx < parameters.size()) {
        return SetParameter(parIdx, helper->GetMixerparameterDefinition(parameters[parIdx]).float_value_default);
    }
    return false;
}