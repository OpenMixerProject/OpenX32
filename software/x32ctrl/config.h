#pragma once

#include <algorithm>

#include "types.h"
#include "WString.h"

#include "helper.h"

#include "mixerparameter.h"

class Config {

    private:

        Helper* helper;

        mixerparameter_map_t* mpm = new mixerparameter_map_t();
        mixerparameter_changed_t* mp_changedlist = new mixerparameter_changed_t();

        void CreateMixerparameter(MP_ID mp_type); 
        void CreateMixerparameter(MP_ID mp_type, uint index);    
        
        void SetParameterChanged(MP_ID mp, uint index);

        // old
        X32_MODEL _model;
        X32_SURFACE_MODE_BANKING _activeBankMode;

    public:

        Config(Helper* h);

        void DefineMixerparameters();
        Mixerparameter* DefParameter(MP_ID mp_type, MP_CAT category, String group, String name, uint count = 1);
        Mixerparameter* GetParameter(MP_ID mp);

        mixerparameter_map_t* GetParameterList();
        mixerparameter_changed_t* GetChangedParameterList();
        vector<uint> GetChangedParameterIndexes(MP_CAT parameter_cat);
        vector<uint> GetChangedParameterIndexes(vector<MP_ID> filter_ids);
        bool HasParameterChanged(MP_ID parameter_id);
        bool HasParameterChanged(MP_ID parameter_id, uint index);
        bool HasParametersChanged(vector<MP_ID> parameter_id);
        bool HasParametersChanged(vector<MP_ID> parameter_id, uint index);
        bool HasParametersChanged(MP_CAT parameter_cat);
        bool HasParametersChanged(MP_CAT parameter_cat, uint index);
        bool HasAnyParameterChanged();
        void ResetChangedParameterList();
        
        float GetFloat(MP_ID mp, uint index = 0);
        int GetInt(MP_ID mp, uint index = 0);
        uint GetUint(MP_ID mp, uint index = 0);
        bool GetBool(MP_ID mp, uint index = 0);
        String GetString(MP_ID mp, uint index = 0);
        uint GetPercent(MP_ID mp, uint index = 0);
        void Set(MP_ID mp, float value, uint index = 0);        
        void Change(MP_ID mp, int amount, uint index = 0);
        void Toggle(MP_ID mp, uint index = 0);
        void Refresh(MP_ID mp, uint index = 0);
        void Reset(MP_ID mp, uint index = 0);


        void SetModel(String model);
        bool IsModelX32Full();
        bool IsModelX32FullOrCompact();
        bool IsModelX32FullOrCompactOrProducer();
        bool IsModelX32FullOrCompactOrProducerOrRack();
        bool IsModelX32CompactOrProducer();
        bool IsModelX32ProducerOrRack();
        bool IsModelX32Core();
        bool IsModelX32Rack();
        bool IsModelX32Producer();
        bool IsModelX32Compact();

        void SetBankMode(X32_SURFACE_MODE_BANKING mode);
        X32_SURFACE_MODE_BANKING GetBankMode();
};