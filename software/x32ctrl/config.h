#pragma once

#include "ctrl_types.h"
#include "WString.h"

class Config {

    private:
        X32_MODEL _model;
        uint32_t _samplerate;
        X32_SURFACE_MODE_BANKING _activeBankMode;

    public:
        int ReadConfig(const char* filename, const char* key, char* value_buffer, size_t buffer_size);

        void SetModel(String model);
        bool IsModelX32Full(void);
        bool IsModelX32FullOrCompactOrProducer(void);
        bool IsModelX32FullOrCompactOrProducerOrRack(void);
        bool IsModelX32CompactOrProducer(void);
        bool IsModelX32Core(void);
        bool IsModelX32Rack(void);
        bool IsModelX32Compact(void);

        void SetSamplerate(uint32_t samplerate);
        uint32_t GetSamplerate(void);

        void SetBankMode(X32_SURFACE_MODE_BANKING mode);
        X32_SURFACE_MODE_BANKING GetBankMode(void);
};