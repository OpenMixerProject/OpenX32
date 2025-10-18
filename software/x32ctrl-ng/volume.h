#pragma once

#include <stdint.h>
#include "constants.h"
#include "x32base.h"

class Volume : public X32Base {
    private:
        float _volumeDbfs;
    public:
        void SetDbfs(float volume);
        void SetFadervalue(float volume);
        float GetDbfs(void);
        float GetFadervalue(void);
};