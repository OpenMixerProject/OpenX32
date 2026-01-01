#pragma once

#include "types.h"
#include "WString.h"

class Config {

    private:
        X32_MODEL _model;
        uint32_t _samplerate;
        X32_SURFACE_MODE_BANKING _activeBankMode;

    public:

        uint8_t selectedVChannel;

        int ReadConfig(const char* filename, const char* key, char* value_buffer, size_t buffer_size);

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

        void SetSamplerate(uint32_t samplerate);
        uint32_t GetSamplerate();

        void SetBankMode(X32_SURFACE_MODE_BANKING mode);
        X32_SURFACE_MODE_BANKING GetBankMode();
};