#pragma once

#include "external.h"
#include "x32ctrl_types.h"
#include "WString.h"


// includes for lvgl
#include "lv_port_linux/lvgl/lvgl.h"
#include "lv_port_linux/src/lib/backends.h"
#include "lv_port_linux/src/lib/driver_backends.h"

// including the GUI built by EEZ-Studio
#include "eez/src/ui/actions.h"
#include "eez/src/ui/fonts.h"
#include "eez/src/ui/images.h"
#include "eez/src/ui/screens.h"
#include "eez/src/ui/structs.h"
#include "eez/src/ui/styles.h"
#include "eez/src/ui/ui.h"
#include "eez/src/ui/vars.h"

extern backend_t *sel_display_backend;

void guiSync(void);
void guiInitInput(void);
void guiInit(void);
void guiNewButtonPress(X32_BTN button, bool pressed);
void guiKeypadInputCallback(lv_indev_t* indev, lv_indev_data_t* data);
void guiSetEncoderText(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6);
static void guiDisplayEncoderEventHandler(lv_event_t* e);
void guiDrawEq();
