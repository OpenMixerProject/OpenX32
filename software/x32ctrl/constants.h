#ifndef CONSTANTS_H_
#define CONSTANTS_H_

//########################################################
//
//               X32 Generic Stuff
//
//########################################################

#define MAX_BUTTONS 255

// LED Brightness
#define X32_BRIGHTNESS_1 0x0
#define X32_BRIGHTNESS_2 0x40
#define X32_BRIGHTNESS_3 0x80
#define X32_BRIGHTNESS_4 0xC0

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
    X32_PAGE_HOME,
    X32_PAGE_CONFIG,
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
    X32_BTN_BUS_MASTER ,
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

    // X32 Core Only
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

#endif