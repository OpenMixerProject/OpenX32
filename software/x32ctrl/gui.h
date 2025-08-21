#ifndef GUI_H_
#define GUI_H_

#include "x32ctrl.h"

extern backend_t *sel_display_backend;

void guiInitInput(void);
void guiInit(void);
void guiNewButtonPress(X32_BTN button, bool pressed);
void guiKeypadInputCallback(lv_indev_t * indev, lv_indev_data_t * data);

#endif
