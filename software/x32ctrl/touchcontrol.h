#ifndef TOUCHCONTROL_H_
#define TOUCHCONTROL_H_

#include "x32ctrl.h"

void touchcontrolTick(void);
bool touchcontrolCanSetFader(X32_BOARD p_board, uint8_t p_faderIndex);

#endif