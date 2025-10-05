#include "x32config.h"

void X32Config::SetDebug(bool debug){
    _debug = debug;
}

bool X32Config::IsDebug(void){
    return _debug;
}

void X32Config::SetModel(String model){

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

bool X32Config::IsModelX32Full(void){
    return (_model == X32_MODEL_FULL);
}
bool X32Config::IsModelX32FullOrCompactOrProducer(void){
    return ((_model == X32_MODEL_FULL) || (_model == X32_MODEL_COMPACT) || (_model == X32_MODEL_PRODUCER));
}
bool X32Config::IsModelX32FullOrCompactOrProducerOrRack(void){
    return (IsModelX32FullOrCompactOrProducer() || (_model == X32_MODEL_RACK));
}
bool X32Config::IsModelX32CompactOrProducer(void){
    return ((_model == X32_MODEL_COMPACT) || (_model == X32_MODEL_PRODUCER));
}
bool X32Config::IsModelX32Core(void){
    return (_model == X32_MODEL_CORE);
}
bool X32Config::IsModelX32Rack(void){
    return (_model == X32_MODEL_RACK);
}
bool X32Config::IsModelX32Compact(void){
    return (_model == X32_MODEL_COMPACT);
}

void X32Config::SetSamplerate(float samplerate){
    _samplerate = samplerate;
}

float X32Config::GetSamplerate(void){
    return _samplerate;
}