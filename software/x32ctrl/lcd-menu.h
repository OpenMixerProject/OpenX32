#pragma once

#include "defines.h"
#include "enum.h"
#include "base.h"
#include "base-parameter.h"
#include "mixer.h"

using namespace std;

class LcdMenu : public X32Base {
    private:
        Mixer* mixer;
        Surface* surface;
        bool initDone = false;

        bool encoderModeSelection = true;

        uint selectedItem = 0;
        uint selectedItemBefore = 0;

        vector<MP_ID> menuItems;

        LcdData* d;

    public:
        LcdMenu(X32BaseParameter* basepar, Mixer* _mixer, Surface* _surface);

        void OnInit();
        void OnShow();
        void OnChange(bool force_update);

        // the encoder was turned
        void OnLcdEncoderTurned(int8_t amount);
        void OnLcdEncoderPressed();
};