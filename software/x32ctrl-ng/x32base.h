#pragma once

#include "config.h"
#include "state.h"
#include "helper.h"
#include "x32base-parameter.h"

// The string class
class X32Base
{
    protected:
        Config* config;
        State* state;
        Helper* helper;

    public:
        X32Base(X32BaseParameter* basepar);
};