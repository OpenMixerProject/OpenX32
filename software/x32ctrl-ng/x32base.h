#pragma once

#include "config.h"
#include "helper.h"

// The string class
class X32Base
{
    protected:
        Config* config;
        Helper* helper;

    public:
        X32Base(Config* config);
};