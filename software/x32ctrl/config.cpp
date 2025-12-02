#include "config.h"

void Config::SetModel(String model){

    if (model == "X32Core") {
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

bool Config::IsModelX32Full(void){
    return (_model == X32_MODEL_FULL);
}
bool Config::IsModelX32FullOrCompactOrProducer(void){
    return ((_model == X32_MODEL_FULL) || (_model == X32_MODEL_COMPACT) || (_model == X32_MODEL_PRODUCER));
}
bool Config::IsModelX32FullOrCompactOrProducerOrRack(void){
    return (IsModelX32FullOrCompactOrProducer() || (_model == X32_MODEL_RACK));
}
bool Config::IsModelX32CompactOrProducer(void){
    return ((_model == X32_MODEL_COMPACT) || (_model == X32_MODEL_PRODUCER));
}
bool Config::IsModelX32Core(void){
    return (_model == X32_MODEL_CORE);
}
bool Config::IsModelX32Rack(void){
    return (_model == X32_MODEL_RACK);
}
bool Config::IsModelX32Compact(void){
    return (_model == X32_MODEL_COMPACT);
}

void Config::SetSamplerate(uint32_t samplerate){
    _samplerate = samplerate;
}

uint32_t Config::GetSamplerate(void){
    return _samplerate;
}

void Config::SetBankMode(X32_SURFACE_MODE_BANKING mode){
    _activeBankMode = mode;
}

X32_SURFACE_MODE_BANKING Config::GetBankMode(void){
    return _activeBankMode;
}