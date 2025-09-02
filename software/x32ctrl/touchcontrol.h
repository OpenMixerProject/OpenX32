#ifndef TOUCHCONTROL_H_
#define TOUCHCONTROL_H_

#include "x32ctrl.h"

void mixerTouchControlTick(void);
bool mixerTouchcontrolCanSetFader(X32_BOARD p_board, uint8_t p_faderIndex);

#endif