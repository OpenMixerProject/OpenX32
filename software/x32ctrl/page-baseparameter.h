#pragma once

#include "base-parameter.h"
#include "CLI11.hpp"
#include "config.h"
#include "state.h"
#include "helper.h"
#include "mixer.h"
#include "surface.h"

class PageBaseParameter : public X32BaseParameter {
    public:
        Mixer* mixer;
        Surface* surface;


        PageBaseParameter(CLI::App* a, Config* c, State* s, Helper* h, Mixer* m, Surface* su) : X32BaseParameter(a, c, s, h){        
            mixer = m;
            surface = su;
        }
};