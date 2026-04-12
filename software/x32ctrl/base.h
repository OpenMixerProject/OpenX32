#pragma once

#include "x32config.h"
#include "state.h"
#include "helper.h"
#include "base-parameter.h"

// The string class
class X32Base
{
    protected:
        CLI::App* app;
        X32Config* config;
        State* state;
        Helper* helper;

    public:
        X32Base(X32BaseParameter* basepar);
};