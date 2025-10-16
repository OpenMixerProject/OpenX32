#pragma once

#include "x32ctrl_types.h"
#include "WString.h"

class X32Config {

    static bool _debug;
    static X32_MODEL _model;
    static float _samplerate;

    public:

        static void SetDebug(bool debug);
        static bool IsDebug(void);

        static void SetModel(String model);
        static bool IsModelX32Full(void);
        static bool IsModelX32FullOrCompactOrProducer(void);
        static bool IsModelX32FullOrCompactOrProducerOrRack(void);
        static bool IsModelX32CompactOrProducer(void);
        static bool IsModelX32Core(void);
        static bool IsModelX32Rack(void);
        static bool IsModelX32Compact(void);

        static void SetSamplerate(float samplerate);
        static float GetSamplerate(void);
};