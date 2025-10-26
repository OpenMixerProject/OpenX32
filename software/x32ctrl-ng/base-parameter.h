#pragma once

#include "config.h"
#include "state.h"

class X32BaseParameter{
    public:
        Config* config;    
        State* state;
        X32BaseParameter(Config* c, State* s){
            this->config = c;
            this->state = s;
        }
};