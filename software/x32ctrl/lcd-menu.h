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

        LcdData* d;

    public:
        LcdMenu(X32BaseParameter* basepar, Mixer* _mixer, Surface* _surface);

        void OnInit();
        void OnShow();
        void OnChange();

        // the encoder was turned
        void OnLcdEncoderTurned(int8_t amount);

        // the encode button was pressed
        void OnLcdButton(bool pressed);
};