#pragma once

#include <cstdint>

#define PI 3.14159265358979323846

#define MAX_VCHANNELS 81
#define MAX_NAME_LENGTH 30 + 1 // null termination!
#define VCHANNEL_NOT_SET MAX_VCHANNELS+1
#define SURFACE_MAX_PACKET_LENGTH 32
#define MAX_PAGES 20

#define VOLUME_MIN -100.0f

#define MAX_CHAN_EQS       4
#define NUM_INPUT_CHANNEL  112  // at the moment we are not using AES50A and AES50B
#define NUM_OUTPUT_CHANNEL 112  // at the moment we are not using AES50A and AES50B
#define NUM_DSP_CHANNEL    64   // 32 in + 8 aux + 8 FX return + 16 bus
#define DSP_SAMPLES_IN_BUFFER              16

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
#define X32_VCHANNEL_CHANGED_BALANCE       0b1000000000000000

#define X32_MIXER_CHANGED_ALL              0b1111111111111111
#define X32_MIXER_CHANGED_NONE             0b0000000000000000
// channel selectiong has changed
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
#define SPI_FPGA_SPEED_HZ 10000000 // 10 MHz for the FPGA, seems to be fine
#define SPI_DSP_SPEED_HZ 250000 // 250 kHz for the DSP. Higher and we get problems

//########################################################
//
//               X32 Generic Stuff
//
//########################################################

// Mode of the Banking
typedef enum {
    // Standard Layout like X32 Original
    X32_SURFACE_MODE_BANKING_X32,

    // User Layout
    X32_SURFACE_MODE_BANKING_USER,
} X32_SURFACE_MODE_BANKING;

typedef enum {
    X32_BOARD_MAIN =    0x00,
    X32_BOARD_L =       0x04,
    X32_BOARD_M =       0x05, // only X32 Full
    X32_BOARD_R =       0x08,
} X32_BOARD;

typedef enum {
    X32_MODEL_NONE,
    X32_MODEL_FULL,
    X32_MODEL_COMPACT,
    X32_MODEL_PRODUCER,
    X32_MODEL_RACK,
    X32_MODEL_CORE
} X32_MODEL;

typedef enum {
    X32_PAGE_NONE,
    X32_PAGE_HOME,
        // sub-pages of home
        X32_PAGE_CONFIG,
        X32_PAGE_EQ,
    X32_PAGE_METERS,
    X32_PAGE_ROUTING,
    X32_PAGE_SETUP,
    X32_PAGE_LIBRARY,
    X32_PAGE_EFFECTS,
    X32_PAGE_MUTE_GRP,
    X32_PAGE_UTILITY
} X32_PAGE;

typedef enum  {
    X32_BTN_NONE,

    //###########################################
    //#
    //#    Board Main
    //#
    //#    - X32 Full
    //#    - X32 Compact
    //#    - X32 Producer
    //#    - X32 Rack
    //#    - X32 Core
    //#
    //###########################################

    // Only X32 Core
    X32_BTN_SCENE_SETUP,

    // Only X32 Rack
    X32_BTN_CHANNEL_SOLO,
    X32_BTN_CHANNEL_MUTE,
    X32_BTN_CHANNEL_ENCODER,

    X32_BTN_TALK_A,
    X32_BTN_TALK_B,
    X32_BTN_VIEW_TALK,
    X32_BTN_MONITOR_DIM,
    X32_BTN_VIEW_MONITOR,

    X32_BTN_VIEW_USB,

    X32_BTN_PHANTOM_48V,
    X32_BTN_PHASE_INVERT,
    X32_BTN_LOW_CUT,
    X32_BTN_VIEW_CONFIG,

    X32_BTN_GATE,
    X32_BTN_VIEW_GATE,

    X32_BTN_COMPRESSOR,
    X32_BTN_VIEW_COMPRESSOR,

    X32_BTN_EQ,
    X32_BTN_EQ_MODE,
    X32_BTN_EQ_HIGH,
    X32_BTN_EQ_HIGH_MID,
    X32_BTN_EQ_LOW_MID,
    X32_BTN_EQ_LOW,
    X32_BTN_VIEW_EQ,

    X32_BTN_VIEW_MIX_BUS_SENDS,
    X32_BTN_MONO_BUS,
    X32_BTN_MAIN_LR_BUS,
    X32_BTN_VIEW_BUS_MIXES,
    X32_BTN_BUS_SEND_1_4,
    X32_BTN_BUS_SEND_5_8,
    X32_BTN_BUS_SEND_9_12,
    X32_BTN_BUS_SEND_13_16,

    X32_BTN_ENCODER1,
    X32_BTN_ENCODER2,
    X32_BTN_ENCODER3,
    X32_BTN_ENCODER4,
    X32_BTN_ENCODER5,
    X32_BTN_ENCODER6,

    X32_BTN_HOME,
    X32_BTN_METERS,
    X32_BTN_ROUTING,
    X32_BTN_SETUP,
    X32_BTN_LIBRARY,
    X32_BTN_EFFECTS,
    X32_BTN_MUTE_GRP,
    X32_BTN_UTILITY,

    X32_BTN_UP,
    X32_BTN_DOWN,
    X32_BTN_LEFT,
    X32_BTN_RIGHT,

    //###########################################
    //#
    //#    Board L
    //#
    //#    - X32 Full
    //#    - X32 Compact
    //#    - X32 Producer
    //#
    //###########################################

    // X32 Full
    X32_BTN_CH_1_16,
    X32_BTN_CH_17_32,
    X32_BTN_AUX_IN_EFFECTS,
    X32_BTN_EFFECTS_RETURNS,
    X32_BTN_BUS_MASTER,
    // X32 Compact/Producer
    X32_BTN_CH_1_8,
    X32_BTN_CH_9_16,
    X32_BTN_CH_17_24,
    X32_BTN_CH_25_32,
    X32_BTN_AUX_IN_1_6_USB_REC,
    X32_BTN_BUS_1_8_MASTER,
    X32_BTN_BUS_9_16_MASTER,

    X32_BTN_BOARD_L_CH_1_SELECT,
    X32_BTN_BOARD_L_CH_2_SELECT,
    X32_BTN_BOARD_L_CH_3_SELECT,
    X32_BTN_BOARD_L_CH_4_SELECT,
    X32_BTN_BOARD_L_CH_5_SELECT,
    X32_BTN_BOARD_L_CH_6_SELECT,
    X32_BTN_BOARD_L_CH_7_SELECT,
    X32_BTN_BOARD_L_CH_8_SELECT,

    X32_BTN_BOARD_L_CH_1_SOLO,
    X32_BTN_BOARD_L_CH_2_SOLO,
    X32_BTN_BOARD_L_CH_3_SOLO,
    X32_BTN_BOARD_L_CH_4_SOLO,
    X32_BTN_BOARD_L_CH_5_SOLO,
    X32_BTN_BOARD_L_CH_6_SOLO,
    X32_BTN_BOARD_L_CH_7_SOLO,
    X32_BTN_BOARD_L_CH_8_SOLO,

    X32_BTN_BOARD_L_CH_1_MUTE,
    X32_BTN_BOARD_L_CH_2_MUTE,
    X32_BTN_BOARD_L_CH_3_MUTE,
    X32_BTN_BOARD_L_CH_4_MUTE,
    X32_BTN_BOARD_L_CH_5_MUTE,
    X32_BTN_BOARD_L_CH_6_MUTE,
    X32_BTN_BOARD_L_CH_7_MUTE,
    X32_BTN_BOARD_L_CH_8_MUTE,

    //###########################################
    //#
    //#   Board M
    //#
    //#    - X32 Full
    //#
    //###########################################

    X32_BTN_BOARD_M_CH_1_SELECT,
    X32_BTN_BOARD_M_CH_2_SELECT,
    X32_BTN_BOARD_M_CH_3_SELECT,
    X32_BTN_BOARD_M_CH_4_SELECT,
    X32_BTN_BOARD_M_CH_5_SELECT,
    X32_BTN_BOARD_M_CH_6_SELECT,
    X32_BTN_BOARD_M_CH_7_SELECT,
    X32_BTN_BOARD_M_CH_8_SELECT,

    X32_BTN_BOARD_M_CH_1_SOLO,
    X32_BTN_BOARD_M_CH_2_SOLO,
    X32_BTN_BOARD_M_CH_3_SOLO,
    X32_BTN_BOARD_M_CH_4_SOLO,
    X32_BTN_BOARD_M_CH_5_SOLO,
    X32_BTN_BOARD_M_CH_6_SOLO,
    X32_BTN_BOARD_M_CH_7_SOLO,
    X32_BTN_BOARD_M_CH_8_SOLO,

    X32_BTN_BOARD_M_CH_1_MUTE,
    X32_BTN_BOARD_M_CH_2_MUTE,
    X32_BTN_BOARD_M_CH_3_MUTE,
    X32_BTN_BOARD_M_CH_4_MUTE,
    X32_BTN_BOARD_M_CH_5_MUTE,
    X32_BTN_BOARD_M_CH_6_MUTE,
    X32_BTN_BOARD_M_CH_7_MUTE,
    X32_BTN_BOARD_M_CH_8_MUTE,

    //###########################################
    //#
    //#    Board R
    //#
    //#    - X32 Full
    //#    - X32 Compact
    //#    - X32 Producer
    //#
    //###########################################

    X32_BTN_DAW_REMOTE,
    X32_BTN_SEND_ON_FADERS,
    X32_BTN_GROUP_DCA_1_8,
    X32_BTN_BUS_1_8,
    X32_BTN_BUS_9_16,
    X32_BTN_MATRIX_MAIN_C,
    X32_BTN_CLEAR_SOLO,

    X32_BTN_SCENES_UNDO,
    X32_BTN_SCENES_GO,
    X32_BTN_SCENES_PREV,
    X32_BTN_SCENES_NEXT,
    X32_BTN_VIEW_SCENES,

    X32_BTN_ASSIGN_1,
    X32_BTN_ASSIGN_2,
    X32_BTN_ASSIGN_3,
    X32_BTN_ASSIGN_4,
    X32_BTN_ASSIGN_5,
    X32_BTN_ASSIGN_6,
    X32_BTN_ASSIGN_7,
    X32_BTN_ASSIGN_8,
    X32_BTN_VIEW_ASSIGN,

    // Only X32 Full
    X32_BTN_ASSIGN_9,
    X32_BTN_ASSIGN_10,
    X32_BTN_ASSIGN_11,
    X32_BTN_ASSIGN_12,
    X32_BTN_ASSIGN_A,
    X32_BTN_ASSIGN_B,
    X32_BTN_ASSIGN_C, 

    X32_BTN_MUTE_GROUP_1,
    X32_BTN_MUTE_GROUP_2,
    X32_BTN_MUTE_GROUP_3,
    X32_BTN_MUTE_GROUP_4,
    X32_BTN_MUTE_GROUP_5,
    X32_BTN_MUTE_GROUP_6,

    X32_BTN_BOARD_R_CH_1_SELECT,
    X32_BTN_BOARD_R_CH_2_SELECT,
    X32_BTN_BOARD_R_CH_3_SELECT,
    X32_BTN_BOARD_R_CH_4_SELECT,
    X32_BTN_BOARD_R_CH_5_SELECT,
    X32_BTN_BOARD_R_CH_6_SELECT,
    X32_BTN_BOARD_R_CH_7_SELECT,
    X32_BTN_BOARD_R_CH_8_SELECT,
    X32_BTN_MAIN_SELECT,

    X32_BTN_BOARD_R_CH_1_SOLO,
    X32_BTN_BOARD_R_CH_2_SOLO,
    X32_BTN_BOARD_R_CH_3_SOLO,
    X32_BTN_BOARD_R_CH_4_SOLO,
    X32_BTN_BOARD_R_CH_5_SOLO,
    X32_BTN_BOARD_R_CH_6_SOLO,
    X32_BTN_BOARD_R_CH_7_SOLO,
    X32_BTN_BOARD_R_CH_8_SOLO,
    X32_BTN_MAIN_SOLO,

    X32_BTN_BOARD_R_CH_1_MUTE,
    X32_BTN_BOARD_R_CH_2_MUTE,
    X32_BTN_BOARD_R_CH_3_MUTE,
    X32_BTN_BOARD_R_CH_4_MUTE,
    X32_BTN_BOARD_R_CH_5_MUTE,
    X32_BTN_BOARD_R_CH_6_MUTE,
    X32_BTN_BOARD_R_CH_7_MUTE,
    X32_BTN_BOARD_R_CH_8_MUTE,
    X32_BTN_MAIN_MUTE,

    //###########################################
    //#
    //#    LED only
    //#
    //#    - X32 Full
    //#    - X32 Compact
    //#    - X32 Producer
    //#    - X32 Core
    //#
    //###########################################

    X32_LED_USB_ACCESS,

    X32_LED_BACKLIGHT_CONFIG_GAIN,
    X32_LED_BACKLIGHT_CONFIG_FREQ,
    X32_LED_BACKLIGHT_GATE_THRESHOLD,
    X32_LED_BACKLIGHT_DYNAMICS_THRESHOLD,
    X32_LED_BACKLIGHT_EQ_Q,
    X32_LED_BACKLIGHT_EQ_FREQ,
    X32_LED_BACKLIGHT_EQ_GAIN,
    X32_LED_BACKLIGHT_BUS_MIXES_LEVEL,
    X32_LED_BACKLIGHT_BUS_MIXES_PAN,

    // X32 Rack only
    X32_LED_VIEW_USB_RED,
    X32_LED_MAIN,
    X32_LED_MATRIX,
    X32_LED_BACKLIGHT_CHANNEL_LEVEL,
    X32_LED_BACKLIGHT_MAIN_LEVEL,

    // X32 Core only
    X32_LED_SCENE_SETUP_RED,
    X32_LED_IN,
    X32_LED_AUX,
    X32_LED_BUS,
    X32_LED_MTX,
    X32_LED_DCA,
    X32_LED_AESA_GREEN,
    X32_LED_AESA_RED,
    X32_LED_AESB_GREEN,
    X32_LED_AESB_RED,

    X32_LED_BACKLIGHT_ASSING_1,
    X32_LED_BACKLIGHT_ASSING_2,

} X32_BTN;

typedef enum  {
    X32_ENC_NONE,

    X32_ENC_GAIN,
    X32_ENC_LOWCUT,
    X32_ENC_GATE,
    X32_ENC_DYNAMICS,
    X32_ENC_EQ_Q,
    X32_ENC_EQ_FREQ,
    X32_ENC_EQ_GAIN,
    X32_ENC_MAIN,
    X32_ENC_PAN,

    X32_ENC_BUS_SEND_1,
    X32_ENC_BUS_SEND_2,
    X32_ENC_BUS_SEND_3,
    X32_ENC_BUS_SEND_4,

    X32_ENC_ENCODER1,
    X32_ENC_ENCODER2,
    X32_ENC_ENCODER3,
    X32_ENC_ENCODER4,
    X32_ENC_ENCODER5,
    X32_ENC_ENCODER6,

    // only X32 Full and X32 Core
    X32_ENC_ASSIGN_1,
    // only X32 Full and X32 Core
    X32_ENC_ASSIGN_2,
    // only X32 Full
    X32_ENC_ASSIGN_3,
    // only X32 Full
    X32_ENC_ASSIGN_4,

    // only X32 Rack
    X32_ENC_CHANNEL_SELECT,
    // only X32 Rack
    X32_ENC_CHANNEL_LEVEL,
    // only X32 Rack
    X32_ENC_MAIN_LEVEL,

} X32_ENC;

// (uint32_t)(pow(2, 31) * pow(10, dBfs / 20.0))
const uint32_t vuThresholds[] = {
    2120000000, // index  0 = ~0 dBfs // should be 2147483648, but turn-on CLIP-led on before real clipping
    1914948731, // index  1 = -1 dBfs
    1705600109, // index  2 = -2 dBfs
    1520146747, // index  3 = -3 dBfs
    1355416719, // index  4 = -4 dBfs
    1077708304, // index  5 = -6 dBfs
    856262174,  // index  6 = -8 dBfs
    680373708,  // index  7 = -10 dBfs
    539824673,  // index  8 = -12 dBfs
    382216890,  // index  9 = -15 dBfs
    270387531,  // index 10 = -18 dBfs
    191494873,  // index 11 = -21 dBfs
    135541672,  // index 12 = -24 dBfs
    96009653,   // index 13 = -27 dBfs
    68037371,   // index 14 = -30 dBfs
    48169904,   // index 15 = -33 dBfs
    34105089,   // index 16 = -36 dBfs
    24160410,   // index 17 = -39 dBfs
    17109579,   // index 18 = -42 dBfs
    12117565,   // index 19 = -45 dBfs
    8582730,    // index 20 = -48 dBfs
    6077596,    // index 21 = -51 dBfs
    4300762,    // index 22 = -54 dBfs
    3046757,    // index 23 = -57 dBfs
    2147483     // index 24 = -60 dBfs
};

