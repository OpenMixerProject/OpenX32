#pragma once

#include "x32ctrl_types.h"
#include "WString.h"

class Config {

    private:
        bool _debug;
        X32_MODEL _model;
        float _samplerate;

    public:

        int ReadConfig(const char* filename, const char* key, char* value_buffer, size_t buffer_size);

        void SetDebug(bool debug);
        bool IsDebug(void);

        void SetModel(String model);
        bool IsModelX32Full(void);
        bool IsModelX32FullOrCompactOrProducer(void);
        bool IsModelX32FullOrCompactOrProducerOrRack(void);
        bool IsModelX32CompactOrProducer(void);
        bool IsModelX32Core(void);
        bool IsModelX32Rack(void);
        bool IsModelX32Compact(void);

        void SetSamplerate(float samplerate);
        float GetSamplerate(void);
};