#ifndef GUI_H_
#define GUI_H_

#include "x32ctrl.h"

extern backend_t *sel_display_backend;

void guiSync(void);
void guiInitInput(void);
void guiInit(void);
void guiNewButtonPress(X32_BTN button, bool pressed);
void guiKeypadInputCallback(lv_indev_t* indev, lv_indev_data_t* data);
void guiSetEncoderText(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6);

#endif
