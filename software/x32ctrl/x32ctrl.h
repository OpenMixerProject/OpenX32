#pragma once

#include "external.h"
#include "x32ctrl_types.h"

// our own includes used by all parts
#include "x32ctrl_types.h"
#include "WString.h"
#include "auxiliary.h"
#include "mixer.h"
#include "uart.h"
#include "hal.h"
#include "touchcontrol.h"
#include "demo.h"
#include "surface.h"
#include "dsp.h"
#include "demo.h"
#include "fpga.h"
#include "gui.h"
#include "fx.h"
#include "xremote.h"


extern Mixer* mixer;

// function prototypes
void timer100msCallbackLvgl(_lv_timer_t* lv_timer);
void timer100msCallbackLinux(int timer);
void timer100msCallback();
void timer10msCallbackLvgl(_lv_timer_t* lv_timer);
void timer10msCallbackLinux(int timer);
void timer10msCallback();
void callbackSurface(X32_BOARD boardId, uint8_t classId, uint8_t index, uint16_t value);
void callbackAdda(char* msg);
void callbackFpga(char* buf, uint8_t len);
void callbackDsp1(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values);
void callbackDsp2(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values);
void syncAll(void);
int init100msTimer(void); // only for Non-GUI systems
