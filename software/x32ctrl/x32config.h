#pragma once

#include <algorithm>
#include <map>
#include <set>

#include "types.h"
#include "WString.h"
#include "glaze/glaze.hpp"

#include "helper.h"

#include "mixerparameter.h"
#include "surfaceelement.h"
#include "x32faderbank.h"
#include "x32assignbank.h"


class X32Config
{
    using enum MP_ID;

    private:

        Helper* helper;

        // Mixerparameter

        Mixerparameter* mpm[(uint)MP_ID::__ELEMENT_COUNTER_DO_NOT_MOVE];
        map<MP_ID, set<uint>>* mp_changedlist = new map<MP_ID, set<uint>>();
        bool MixerParameterChangelistFreeze = false;
        map<MP_ID, set<uint>>* mp_changedlist_temp = new map<MP_ID, set<uint>>();
        
        void SetParameterChanged(MP_ID mp, uint index);

        // Surfaceelements
        SurfaceElement* sem[(uint)SurfaceElementId::__ELEMENT_COUNTER_DO_NOT_MOVE];

        // surface binding
        map<SurfaceElementId, SurfaceBindingParameter*>* surface_binding;
        set<SurfaceElementId> surface_binding_changed;

        X32AssignBank* assingBanks[(uint)X32AssignBankId::__ELEMENT_COUNTER_DO_NOT_MOVE];


        // old
        X32_MODEL _model;

    public:

        X32Config(Helper* h);

        bool LoadConfig(uint scene);
        void Save(uint scene);

        void DefineMixerparameters();
        Mixerparameter* DefParameter(MP_ID mp_type, MP_CAT category, String name, uint count = 1);
        Mixerparameter* GetParameter(MP_ID mp);

        Mixerparameter** GetParameterList();
        map<MP_ID, set<uint>>* GetChangedParameterList();

        MP_ID MpCalcId(MP_ID mp_id, int amount);
        
        vector<uint> GetChangedParameterIndexes(MP_CAT parameter_cat);
        vector<uint> GetChangedParameterIndexes(vector<MP_ID> filter_ids);
        bool HasParameterChanged(MP_ID parameter_id);
        bool HasParameterChanged(MP_ID parameter_id, uint index);
        bool HasBoundParameterChanged(SurfaceElementId id);
        bool HasParametersChanged(vector<MP_ID> parameter_id);
        bool HasParametersChanged(vector<MP_ID> parameter_id, uint index);
        bool HasParametersChanged(MP_CAT parameter_cat);
        bool HasParametersChanged(MP_CAT parameter_cat, uint index);
        bool HasAnyParameterChanged();
        void FreezeParameterList();
        void SetParameterUnchanged(MP_ID mp);
        void SaveResetAndUnfreezeChangedParameterList();
        
        float GetFloat(MP_ID mp, uint index = 0);
        int GetInt(MP_ID mp, uint index = 0);
        uint GetUint(MP_ID mp, uint index = 0);
        bool GetBool(MP_ID mp, uint index = 0);
        String GetString(MP_ID mp, uint index = 0);
        uint GetPercent(MP_ID mp, uint index = 0);
        bool GetBlink(MP_ID mp);
        void Set(MP_ID mp, float value, uint index = 0);    
        void Set(MP_ID mp, String value_string, uint index = 0
        );
        void Change(MP_ID mp, int amount, uint index = 0);
        void Toggle(MP_ID mp, uint index = 0);
        void Refresh(MP_ID mp, uint index = 0);
        void Reset(MP_ID mp, uint index = 0);

        MP_ID ParameterCalcId(SurfaceBindingParameter* binding_parameter);
        uint ParameterCalcIndex(SurfaceBindingParameter* binding_parameter);
        MP_ID ParameterDependsOn(SurfaceBindingParameter* binding_parameter);
        MP_ID ParameterDependsOn(MixerparameterAction mp_action);

        void DefineSurfaceElements();
        SurfaceElement* DefSurfaceElements(SurfaceElementId element_id, String name);

        SurfaceElementId CalcSurfaceElementId(SurfaceElementId id, int amount);

        bool HasSurfaceElement(SurfaceElementId id);
        SurfaceElement* GetSurfaceElement(SurfaceElementId);
        SurfaceElement* GetSurfaceElementButton(X32_BOARD board, uint16_t value);
        SurfaceElement* GetSurfaceElementEncoder(X32_BOARD board, uint8_t index);
        SurfaceElement* GetSurfaceElementFader(X32_BOARD board, uint8_t index);
        
        map<SurfaceElementId, SurfaceBindingParameter*>* GetSurfaceBinding();
        SurfaceBindingParameter* GetSurfaceBinding(SurfaceElementId elementId);
        void SurfaceBindParameter(SurfaceElementId surfaceelement_id, SurfaceBindingParameter* binding_parameter);
        void SurfaceBind(SurfaceElementId surfaceelement_id, MixerparameterAction action, MP_ID mixerparaemter_id, uint mixerparameter_index = 0, uint extra_value = 0);
        // void SurfaceBind(SurfaceElementId surfaceelement_id, X32Action action);
        void SurfaceUnbind(SurfaceElementId surfaceelement_id);
        void SurfaceBindCustom(SurfaceElementId surfaceelement_id, String labeltext = "");

        bool HasAnySurfaceBindingChanged();
        bool HasSurfaceBindingChanged(SurfaceElementId elementId);
        void RemoveSurfaceBindingChanged(SurfaceElementId elementId);

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

        void InitAssignBanks();
        X32AssignBank* GetAssignBank(X32AssignBankId id);
};

class X32ConfigFileEntry
{
    public:
        MP_ID MixerparameterId;
        String MixerparameterName;
        vector<float> value;
        vector<String> string_value;
};

class X32ConfigFile
{
    public:
        vector<X32ConfigFileEntry> entries;
};

