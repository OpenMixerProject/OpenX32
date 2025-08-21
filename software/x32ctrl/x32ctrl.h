#ifndef X32CTRL_H_
#define X32CTRL_H_

#define DEBUG 1

// standard includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <linux/input.h>

// special includes
#include <string.h>

// includes for timer
#include <time.h>
#include <signal.h>

// includes for uart
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h> // for FIONREAD

// includes for lvgl
#include "lv_port_linux/lvgl/lvgl.h"
#include "lv_port_linux/src/lib/backends.h"

// including the GUI built by EEZ-Studio
#include "eez/src/ui/actions.h"
#include "eez/src/ui/fonts.h"
#include "eez/src/ui/images.h"
#include "eez/src/ui/screens.h"
#include "eez/src/ui/structs.h"
#include "eez/src/ui/styles.h"
#include "eez/src/ui/ui.h"
#include "eez/src/ui/vars.h"

// our own includes
#include "x32ctrl_types.h"
#include "constants.h"
#include "WString.h"
#include "gui.h"
#include "uart.h"
#include "surface.h"
#include "auxiliary.h"
#include "adda.h"
#include "mixing.h"
#include "mixer.h"
#include "fpga.h"

// function prototypes
void timer100msCallback(struct _lv_timer_t *);
void timer10msCallback(struct _lv_timer_t *);
void surfaceDemo(void);
void surfaceCallback(uint8_t boardId, uint8_t class, uint8_t index, uint16_t value);
void addaCallback(char *msg);
void fpgaCallback(char *buf, uint8_t len);
void syncGui(void);
void syncSurface(void);
void syncSurfaceBoard(X32_BOARD board);
void syncSurfaceBankIndicator(void);
void showPage(X32_PAGE page);
void x32log(const char *format, ...);
void x32debug(const char *format, ...);

#endif
