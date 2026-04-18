#pragma once

#include <algorithm>
#include <map>
#include <set>

#include "types.h"
#include "WString.h"

#include "helper.h"

#include "mixerparameter.h"
#include "surfaceelement.h"
#include "x32bank.h"

class X32Config
{
    using enum MP_ID;

    private:

        Helper* helper;

        // Mixerparameter

        Mixerparameter* mpm[(uint)MP_ID::__ELEMENT_COUNTER_DO_NOT_MOVE];
        map<MP_ID, set<uint>>* mp_changedlist = new map<MP_ID, set<uint>>();
        
        void SetParameterChanged(MP_ID mp, uint index);

        // Surfaceelements
        SurfaceElement* sem[(uint)SurfaceElementId::__ELEMENT_COUNTER_DO_NOT_MOVE];

        // surface binding
        map<SurfaceElementId, SurfaceBindingParameter*>* surface_binding;
        set<SurfaceElementId> surface_binding_changed;




        // old
        X32_MODEL _model;

    public:

        X32Config(Helper* h);

        void DefineMixerparameters();
        Mixerparameter* DefParameter(MP_ID mp_type, MP_CAT category, String name, uint count = 1);
        Mixerparameter* GetParameter(MP_ID mp);

        Mixerparameter** GetParameterList();
        map<MP_ID, set<uint>>* GetChangedParameterList();
        
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
        void Set(MP_ID mp, String value_string, uint index = 0
        );
        void Change(MP_ID mp, int amount, uint index = 0);
        void Toggle(MP_ID mp, uint index = 0);
        void Refresh(MP_ID mp, uint index = 0);
        void Reset(MP_ID mp, uint index = 0);

        void DefineSurfaceElements();
        SurfaceElement* DefSurfaceElements(SurfaceElementId element_id, String name);

        bool HasSurfaceElement(SurfaceElementId id);
        SurfaceElement* GetSurfaceElement(SurfaceElementId);
        SurfaceElement* GetSurfaceElementButton(X32_BOARD board, uint16_t value);
        SurfaceElement* GetSurfaceElementFader(X32_BOARD board, uint8_t index);
        
        map<SurfaceElementId, SurfaceBindingParameter*>* GetSurfaceBinding();
        SurfaceBindingParameter* GetSurfaceBinding(SurfaceElementId elementId);
        void SurfaceBindParameter(SurfaceElementId surfaceelement_id, SurfaceBindingParameter* binding_parameter);
        void SurfaceBind(SurfaceElementId surfaceelement_id, MixerparameterAction action, MP_ID mixerparaemter_id, uint mixerparameter_index = 0);
        // void SurfaceBind(SurfaceElementId surfaceelement_id, X32Action action);
        void SurfaceUnbind(SurfaceElementId surfaceelement_id);

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
};