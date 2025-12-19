#pragma once

#include <stdint.h>
#include <list>
#include <math.h>

#include "base.h"
#include "uart.h"
#include "defines.h"
#include "types.h"
#include "lcd.h"
#include "surface-event.h"
#include "surface-message.h"
#include "surface-fader.h"
#include "helper.h"
#include "vchannel.h"

using namespace std;

class SurfaceButton
{
    private:
        bool blinkState;

    public:
        X32_BTN button;
        bool blink;        
};