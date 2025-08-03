#ifndef X32OSC_REAPER_H_
#define X32OSC_REAPER_H_

// X32 Destination System

//#define X32_MODEL_FULL
#define X32_MODEL_COMPACT
//#define X32_MODEL_PRODUCER
//#define X32_MODEL_RACK
//#define X32_MODEL_CORE
// TODO: read X32-SD-Card and check which device-type we are



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

// our own includes
#include "uart.h"
#include "surface.h"
#include "auxiliary.h"
#include "constants.h"
#include "osc.h"

// function prototypes
void timerCallback(int signum);
void surfaceInit();
void surfaceCallback(uint8_t boardId, uint8_t class, uint8_t index, uint16_t value);
void processMessage(tosc_message *osc);

#endif
