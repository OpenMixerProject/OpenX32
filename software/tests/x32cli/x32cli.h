#ifndef X32CLI_H_
#define X32CLI_H_

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

// function prototypes
void timer10msCallback(int signum);
void surfaceInit();
void surfaceCallback(uint8_t boardId, uint8_t class, uint8_t index, uint16_t value);

#endif
