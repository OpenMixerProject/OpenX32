#pragma once

#include "mixer.h"
#include "external.h"
#include "x32ctrl_types.h"
#include "x32config.h"


// our own includes used by all parts
// #include "WString.h"
// #include "auxiliary.h"
// #include "uart.h"
// #include "hal.h"
// #include "demo.h"
#include "surface.h"
// #include "dsp.h"
// #include "demo.h"
#include "fpga.h"
#include "gui.h"
// #include "fx.h"
#include "xremote.h"


extern X32Config config;

// function prototypes
void timer100msCallbackLvgl(_lv_timer_t* lv_timer);
void timer100msCallbackLinux(int timer);
void timer100msCallback();
void timer10msCallbackLvgl(_lv_timer_t* lv_timer);
void timer10msCallbackLinux(int timer);
void timer10msCallback();
void callbackSurface(X32_BOARD boardId, uint8_t classId, uint8_t index, uint16_t value);
void callbackFpga(char* buf, uint8_t len);

int init100msTimer(void); // only for Non-GUI systems