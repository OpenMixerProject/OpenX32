#ifndef X32CTRL_H_
#define X32CTRL_H_

#define DEBUG 0

#define MAX_VCHANNELS 81
#define MAX_NAME_LENGTH 30 + 1 // null termination!
#define VCHANNEL_NOT_SET MAX_VCHANNELS+1
#define VCHANNEL_IDX_MAIN 32
#define SURFACE_MAX_PACKET_LENGTH 32
#define MAX_PAGES 20

#define MAX_CHAN_EQS       2
#define NUM_INPUT_CHANNEL  112  // at the moment we are not using AES50A and AES50B
#define NUM_OUTPUT_CHANNEL 112  // at the moment we are not using AES50A and AES50B
#define NUM_DSP_CHANNEL    64   // 32 in + 8 aux + 8 FX return + 16 bus

#define X32_VCHANNEL_CHANGED_ALL           0b1111111111111111
#define X32_VCHANNEL_CHANGED_NONE          0b0000000000000000
#define X32_VCHANNEL_CHANGED_VOLUME        0b0000000000000010
#define X32_VCHANNEL_CHANGED_SELECT        0b0000000000000100
#define X32_VCHANNEL_CHANGED_SOLO          0b0000000000001000
#define X32_VCHANNEL_CHANGED_MUTE          0b0000000000010000
#define X32_VCHANNEL_CHANGED_PHANTOM       0b0000000000100000
#define X32_VCHANNEL_CHANGED_PHASE_INVERT  0b0000000001000000
#define X32_VCHANNEL_CHANGED_COLOR         0b0000000010000000
#define X32_VCHANNEL_CHANGED_NAME          0b0000000100000000
#define X32_VCHANNEL_CHANGED_INPUT         0b0000001000000000
#define X32_VCHANNEL_CHANGED_GAIN          0b0000010000000000
#define X32_VCHANNEL_CHANGED_GATE          0b0000100000000000
#define X32_VCHANNEL_CHANGED_EQ            0b0001000000000000
#define X32_VCHANNEL_CHANGED_DYNAMIC       0b0010000000000000
#define X32_VCHANNEL_CHANGED_SENDS         0b0100000000000000

#define X32_MIXER_CHANGED_ALL              0b1111111111111111
#define X32_MIXER_CHANGED_NONE             0b0000000000000000
// vChannel selectiong has changed
#define X32_MIXER_CHANGED_SELECT           0b0000000000000001
// routing was changed
#define X32_MIXER_CHANGED_ROUTING          0b0000000000000010
// bank was changed
#define X32_MIXER_CHANGED_BANKING          0b0000000000000100
#define X32_MIXER_CHANGED_LCD              0b0000000000001000
// value in vChannel has changed
#define X32_MIXER_CHANGED_VCHANNEL         0b0000000000010000
// displayed page changed
#define X32_MIXER_CHANGED_PAGE             0b0000000000100000
// gui parameter has changed (general screen-refresh requested)
#define X32_MIXER_CHANGED_GUI              0b1000000000000000


#define SURFACE_COLOR_BLACK 0
#define SURFACE_COLOR_RED 1
#define SURFACE_COLOR_GREEN 2
#define SURFACE_COLOR_YELLOW 3
#define SURFACE_COLOR_BLUE 4
#define SURFACE_COLOR_PINK 5
#define SURFACE_COLOR_CYAN 6
#define SURFACE_COLOR_WHITE 7
#define SURFACE_COLOR_INVERTED 8

#define MAX_BUTTONS 255
#define MAX_ENCODERS 23 // counted on X32 Full

// LED Brightness
#define X32_BRIGHTNESS_1 0x0
#define X32_BRIGHTNESS_2 0x40
#define X32_BRIGHTNESS_3 0x80
#define X32_BRIGHTNESS_4 0xC0

// Maximal mögliche Paketlänge inklusive Startbyte, Endbyte und Checksumme
// Langes Paket: 0xFE (1) + BoardID (1) + ID (1) + Value_MSB (1) + Value_LSB (1) + 0xFE (1) + CHECKSUM (1) = 7 Bytes
#define MAX_MESSAGE_SIZE 200

#define SPI_DEVICE_DSP1 "/dev/spidev0.0"
#define SPI_DEVICE_DSP2 "/dev/spidev0.1"
#define SPI_DEVICE_FPGA "/dev/spidev2.0"
#define SPI_SPEED_HZ 10000000 // 10 MHz, but seems to be ignored by Linux-driver

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

// includes for UDP-communication
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

// includes for reading IP-Address
#include <sys/stat.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// includes for spi-communication
#include <linux/spi/spidev.h>

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

// our own includes used by all parts
#include "constants.h"
#include "x32ctrl_types.h"
#include "WString.h"
#include "auxiliary.h"

// function prototypes
void timer100msCallbackLvgl(_lv_timer_t* lv_timer);
void timer100msCallbackLinux(int timer);
void timer100msCallback();
void timer10msCallbackLvgl(_lv_timer_t* lv_timer);
void timer10msCallbackLinux(int timer);
void timer10msCallback();
void surfaceDemo(void);
void surfaceCallback(X32_BOARD boardId, uint8_t classId, uint8_t index, uint16_t value);
void addaCallback(char* msg);
void fpgaCallback(char* buf, uint8_t len);
void syncAll(void);
void syncGuiAndSurface(void);
void syncGui(void);
void syncSurface(void);
void syncSurfaceBoardMain();
void syncSurfaceBoard(X32_BOARD board);
void syncSurfaceBankIndicator(void);
void showPage(X32_PAGE page);
void x32log(const char* format, ...);
void x32debug(const char* format, ...);

#endif
