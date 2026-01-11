#include "config.h"

void Config::SetModel(String model){

    if (model == "X32CORE") {
        _model = X32_MODEL_CORE;
    }else if (model == "X32RACK") {
        _model = X32_MODEL_RACK;
    }else if (model == "X32Producer") {
        _model = X32_MODEL_PRODUCER;
    }else if (model == "X32C") {
        _model =  X32_MODEL_COMPACT;
    }else if (model == "X32") {
        _model = X32_MODEL_FULL;
    }else{
        //x32log("ERROR: No model detected!\n");
        _model = X32_MODEL_NONE;
    }
};

bool Config::IsModelX32Full() {
    return (_model == X32_MODEL_FULL);
}
bool Config::IsModelX32FullOrCompactOrProducer() {
    return ((_model == X32_MODEL_FULL) || (_model == X32_MODEL_COMPACT) || (_model == X32_MODEL_PRODUCER));
}
bool Config::IsModelX32FullOrCompactOrProducerOrRack() {
    return (IsModelX32FullOrCompactOrProducer() || (_model == X32_MODEL_RACK));
}
bool Config::IsModelX32CompactOrProducer() {
    return ((_model == X32_MODEL_COMPACT) || (_model == X32_MODEL_PRODUCER));
}
bool Config::IsModelX32Core() {
    return (_model == X32_MODEL_CORE);
}
bool Config::IsModelX32Rack() {
    return (_model == X32_MODEL_RACK);
}
bool Config::IsModelX32Producer() {
    return (_model == X32_MODEL_PRODUCER);
}
bool Config::IsModelX32Compact() {
    return (_model == X32_MODEL_COMPACT);
}
bool Config::IsModelX32FullOrCompact() {
    return ((_model == X32_MODEL_FULL) || (_model == X32_MODEL_COMPACT));
}
bool Config::IsModelX32ProducerOrRack() {
    return ((_model == X32_MODEL_PRODUCER) || (_model == X32_MODEL_RACK));
}

void Config::SetSamplerate(uint32_t samplerate) {
    _samplerate = samplerate;
}

uint32_t Config::GetSamplerate() {
    return _samplerate;
}

void Config::SetBankMode(X32_SURFACE_MODE_BANKING mode) {
    _activeBankMode = mode;
}

X32_SURFACE_MODE_BANKING Config::GetBankMode() {
    return _activeBankMode;
}