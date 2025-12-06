/*
    ____                  __   ______ ___  
   / __ \                 \ \ / /___ \__ \ 
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
  | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |                               
         |_|                               
  
  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "surface.h"

Surface::Surface(X32BaseParameter* basepar): X32Base(basepar){
    uart = new Uart(basepar);

    buttonDefinitionIndex = 0;
    encoderDefinitionIndex = 0;
}

void Surface::Init(void) {
    uart->Open("/dev/ttymxc1", 115200, true);

    Reset();
    InitDefinitions();

    // set brightness and contrast
    SetBrightness(0, 255); // brightness of LEDs
    SetBrightness(1, 255);
    SetBrightness(4, 255);
    SetBrightness(5, 255);
    SetBrightness(8, 255);
    SetContrast(0, 0x24); // contrast of LCDs
    SetContrast(4, 0x24);
    SetContrast(5, 0x24);
    SetContrast(8, 0x24);
}

void Surface::Reset(void) {
    int fd = open("/sys/class/leds/reset_surface/brightness", O_WRONLY);
    write(fd, "1", 1);
    usleep(100 * 1000);
    write(fd, "0", 1);
    close(fd);
    usleep(2000 * 1000);
}

void Surface::AddButtonDefinition(X32_BTN p_button, uint16_t p_buttonNr) {
    if (buttonDefinitionIndex >= MAX_BUTTONS)
    {
        //TODO: Error Message
        helper->Error("ERROR: MAX_BUTTONS\n");
        return;
    }
    x32_btn_def[buttonDefinitionIndex].button = p_button;
    x32_btn_def[buttonDefinitionIndex].buttonNr = p_buttonNr;
    buttonDefinitionIndex++;
    helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE, "added button definition: Button %d -> ButtonNr %d", p_button, p_buttonNr);
}

void Surface::AddEncoderDefinition(X32_ENC p_encoder, uint16_t p_encoderNr) {
    if (encoderDefinitionIndex >= MAX_ENCODERS)
    {
        //TODO: Error Message
        helper->Error("MAX_ENCODERS");
        return;
    }
    x32_enc_def[encoderDefinitionIndex].encoder = p_encoder;
    x32_enc_def[encoderDefinitionIndex].encoderNr = p_encoderNr;
    encoderDefinitionIndex++;
    helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE, "added encoder definition: Encoder %d -> EncoderNr %d", p_encoder, p_encoderNr);
}


void Surface::InitDefinitions(void) {
    if(config->IsModelX32Full()) {
            AddButtonDefinition(X32_BTN_TALK_A, 0x012E);
            AddButtonDefinition(X32_BTN_TALK_B, 0x012F);
            AddButtonDefinition(X32_BTN_VIEW_TALK, 0x0130);
            AddButtonDefinition(X32_BTN_MONITOR_DIM, 0x012C);
            AddButtonDefinition(X32_BTN_VIEW_MONITOR, 0x012D);

            AddButtonDefinition(X32_BTN_VIEW_USB, 0x0131);

            AddButtonDefinition(X32_BTN_PHANTOM_48V, 0x0101);
            AddButtonDefinition(X32_BTN_PHASE_INVERT, 0x0102);
            AddButtonDefinition(X32_BTN_LOW_CUT, 0x0103);
            AddButtonDefinition(X32_BTN_VIEW_CONFIG, 0x0100);

            AddButtonDefinition(X32_BTN_GATE, 0x0106);
            AddButtonDefinition(X32_BTN_VIEW_GATE, 0x0104);

            AddButtonDefinition(X32_BTN_COMPRESSOR, 0x0107);
            AddButtonDefinition(X32_BTN_VIEW_COMPRESSOR, 0x0105);

            AddButtonDefinition(X32_BTN_EQ, 0x0109);
            AddButtonDefinition(X32_BTN_EQ_MODE, 0x0108);
            AddButtonDefinition(X32_BTN_EQ_HIGH, 0x010B);
            AddButtonDefinition(X32_BTN_EQ_HIGH_MID, 0x010C);
            AddButtonDefinition(X32_BTN_EQ_LOW_MID, 0x010D);
            AddButtonDefinition(X32_BTN_EQ_LOW, 0x010E);
            AddButtonDefinition(X32_BTN_VIEW_EQ, 0x010A);

            AddButtonDefinition(X32_BTN_VIEW_MIX_BUS_SENDS, 0x010F);
            AddButtonDefinition(X32_BTN_MONO_BUS, 0x0115);
            AddButtonDefinition(X32_BTN_MAIN_LR_BUS, 0x0116);
            AddButtonDefinition(X32_BTN_VIEW_BUS_MIXES, 0x0114);
            AddButtonDefinition(X32_BTN_BUS_SEND_1_4, 0x0110);
            AddButtonDefinition(X32_BTN_BUS_SEND_5_8, 0x0111);
            AddButtonDefinition(X32_BTN_BUS_SEND_9_12, 0x0112);
            AddButtonDefinition(X32_BTN_BUS_SEND_13_16, 0x0113);

            AddButtonDefinition(X32_BTN_ENCODER1, 0x0117);
            AddButtonDefinition(X32_BTN_ENCODER2, 0x0118);
            AddButtonDefinition(X32_BTN_ENCODER3, 0x0119);
            AddButtonDefinition(X32_BTN_ENCODER4, 0x011A);
            AddButtonDefinition(X32_BTN_ENCODER5, 0x011B);
            AddButtonDefinition(X32_BTN_ENCODER6, 0x011C);

            AddButtonDefinition(X32_BTN_HOME, 0x0122);
            AddButtonDefinition(X32_BTN_METERS, 0x0123);
            AddButtonDefinition(X32_BTN_ROUTING, 0x0124);
            AddButtonDefinition(X32_BTN_SETUP, 0x0125);
            AddButtonDefinition(X32_BTN_LIBRARY, 0x0126);
            AddButtonDefinition(X32_BTN_EFFECTS, 0x0127);
            AddButtonDefinition(X32_BTN_MUTE_GRP, 0x0128);
            AddButtonDefinition(X32_BTN_UTILITY, 0x0129);

            AddButtonDefinition(X32_BTN_UP, 0x011D);
            AddButtonDefinition(X32_BTN_DOWN, 0x0120);
            AddButtonDefinition(X32_BTN_LEFT, 0x011E);
            AddButtonDefinition(X32_BTN_RIGHT, 0x011F);

            // Board, L);

            AddButtonDefinition(X32_BTN_DAW_REMOTE, 0x400);
            AddButtonDefinition(X32_BTN_CH_1_16, 0x401);
            AddButtonDefinition(X32_BTN_CH_17_32, 0x402);
            AddButtonDefinition(X32_BTN_AUX_IN_EFFECTS, 0x403);
            AddButtonDefinition(X32_BTN_BUS_MASTER, 0x404);

            AddButtonDefinition(X32_BTN_BOARD_L_CH_1_SELECT, 0x420);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_2_SELECT, 0x421);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_3_SELECT, 0x422);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_4_SELECT, 0x423);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_5_SELECT, 0x424);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_6_SELECT, 0x425);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_7_SELECT, 0x426);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_8_SELECT, 0x427);

            AddButtonDefinition(X32_BTN_BOARD_L_CH_1_SOLO, 0x430);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_2_SOLO, 0x431);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_3_SOLO, 0x432);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_4_SOLO, 0x433);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_5_SOLO, 0x434);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_6_SOLO, 0x435);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_7_SOLO, 0x436);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_8_SOLO, 0x437);

            AddButtonDefinition(X32_BTN_BOARD_L_CH_1_MUTE, 0x440);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_2_MUTE, 0x441);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_3_MUTE, 0x442);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_4_MUTE, 0x443);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_5_MUTE, 0x444);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_6_MUTE, 0x445);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_7_MUTE, 0x446);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_8_MUTE, 0x447);

            // Board, M);

            AddButtonDefinition(X32_BTN_BOARD_M_CH_1_SELECT, 0x520);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_2_SELECT, 0x521);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_3_SELECT, 0x522);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_4_SELECT, 0x523);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_5_SELECT, 0x524);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_6_SELECT, 0x525);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_7_SELECT, 0x526);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_8_SELECT, 0x527);

            AddButtonDefinition(X32_BTN_BOARD_M_CH_1_SOLO, 0x530);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_2_SOLO, 0x531);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_3_SOLO, 0x532);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_4_SOLO, 0x533);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_5_SOLO, 0x534);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_6_SOLO, 0x535);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_7_SOLO, 0x536);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_8_SOLO, 0x537);

            AddButtonDefinition(X32_BTN_BOARD_M_CH_1_MUTE, 0x540);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_2_MUTE, 0x541);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_3_MUTE, 0x542);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_4_MUTE, 0x543);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_5_MUTE, 0x544);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_6_MUTE, 0x545);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_7_MUTE, 0x546);
            AddButtonDefinition(X32_BTN_BOARD_M_CH_8_MUTE, 0x547);

            // Board, R);

            AddButtonDefinition(X32_BTN_SEND_ON_FADERS, 0x800);
            AddButtonDefinition(X32_BTN_GROUP_DCA_1_8, 0x801);
            AddButtonDefinition(X32_BTN_BUS_1_8, 0x802);
            AddButtonDefinition(X32_BTN_BUS_9_16, 0x803);
            AddButtonDefinition(X32_BTN_MATRIX_MAIN_C, 0x804);
            AddButtonDefinition(X32_BTN_CLEAR_SOLO, 0x806);

            AddButtonDefinition(X32_BTN_SCENES_UNDO, 0x03);
            AddButtonDefinition(X32_BTN_SCENES_GO, 0x04);
            AddButtonDefinition(X32_BTN_SCENES_PREV, 0x01);
            AddButtonDefinition(X32_BTN_SCENES_NEXT, 0x02);
            AddButtonDefinition(X32_BTN_VIEW_SCENES, 0x00);

            AddButtonDefinition(X32_BTN_ASSIGN_5, 0x06);
            AddButtonDefinition(X32_BTN_ASSIGN_6, 0x07);
            AddButtonDefinition(X32_BTN_ASSIGN_7, 0x08);
            AddButtonDefinition(X32_BTN_ASSIGN_8, 0x09);
            AddButtonDefinition(X32_BTN_ASSIGN_9, 0x0A);
            AddButtonDefinition(X32_BTN_ASSIGN_10, 0x0B);
            AddButtonDefinition(X32_BTN_ASSIGN_11, 0x0C);
            AddButtonDefinition(X32_BTN_ASSIGN_12, 0x0D);
            AddButtonDefinition(X32_BTN_ASSIGN_A, 0x0E);
            AddButtonDefinition(X32_BTN_ASSIGN_B, 0x0F);
            AddButtonDefinition(X32_BTN_ASSIGN_C, 0x10);
            AddButtonDefinition(X32_BTN_VIEW_ASSIGN, 0x05);

            AddButtonDefinition(X32_BTN_MUTE_GROUP_1, 0x11);
            AddButtonDefinition(X32_BTN_MUTE_GROUP_2, 0x12);
            AddButtonDefinition(X32_BTN_MUTE_GROUP_3, 0x13);
            AddButtonDefinition(X32_BTN_MUTE_GROUP_4, 0x14);
            AddButtonDefinition(X32_BTN_MUTE_GROUP_5, 0x15);
            AddButtonDefinition(X32_BTN_MUTE_GROUP_6, 0x16);

            AddButtonDefinition(X32_BTN_BOARD_R_CH_1_SELECT, 0x820);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_2_SELECT, 0x821);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_3_SELECT, 0x822);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_4_SELECT, 0x823);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_5_SELECT, 0x824);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_6_SELECT, 0x825);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_7_SELECT, 0x826);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_8_SELECT, 0x827);
            AddButtonDefinition(X32_BTN_MAIN_SELECT, 0x828);

            AddButtonDefinition(X32_BTN_BOARD_R_CH_1_SOLO, 0x830);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_2_SOLO, 0x831);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_3_SOLO, 0x832);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_4_SOLO, 0x833);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_5_SOLO, 0x834);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_6_SOLO, 0x835);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_7_SOLO, 0x836);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_8_SOLO, 0x837);
            AddButtonDefinition(X32_BTN_MAIN_SOLO, 0x838);

            AddButtonDefinition(X32_BTN_BOARD_R_CH_1_MUTE, 0x840);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_2_MUTE, 0x841);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_3_MUTE, 0x842);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_4_MUTE, 0x843);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_5_MUTE, 0x844);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_6_MUTE, 0x845);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_7_MUTE, 0x846);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_8_MUTE, 0x847);
            AddButtonDefinition(X32_BTN_MAIN_MUTE, 0x848);

            // LED, only);

            AddButtonDefinition(X32_LED_EQ_HCUT, 0x0118);
            AddButtonDefinition(X32_LED_EQ_HSHV, 0x0119);
            AddButtonDefinition(X32_LED_EQ_VEQ, 0x011A);
            AddButtonDefinition(X32_LED_EQ_PEQ, 0x011B);
            AddButtonDefinition(X32_LED_EQ_LSHV, 0x011C);
            AddButtonDefinition(X32_LED_EQ_LCUT, 0x011D);

            AddButtonDefinition(X32_LED_USB_ACCESS, 0x0122);

            AddButtonDefinition(X32_LED_BACKLIGHT_CONFIG_GAIN, 0x012A);
            AddButtonDefinition(X32_LED_BACKLIGHT_CONFIG_FREQ, 0x012B);
            AddButtonDefinition(X32_LED_BACKLIGHT_GATE_THRESHOLD, 0x012C);
            AddButtonDefinition(X32_LED_BACKLIGHT_DYNAMICS_THRESHOLD, 0x012D);
            AddButtonDefinition(X32_LED_BACKLIGHT_EQ_Q, 0x012E);
            AddButtonDefinition(X32_LED_BACKLIGHT_EQ_FREQ, 0x012F);
            AddButtonDefinition(X32_LED_BACKLIGHT_EQ_GAIN, 0x0130);
            AddButtonDefinition(X32_LED_BACKLIGHT_BUS_MIXES_LEVEL, 0x0131);
            AddButtonDefinition(X32_LED_BACKLIGHT_BUS_MIXES_PAN, 0x0132);

            // Encoder

            AddEncoderDefinition(X32_ENC_GAIN, 0x0100);
            AddEncoderDefinition(X32_ENC_LOWCUT, 0x0101);
            AddEncoderDefinition(X32_ENC_GATE, 0x0102);
            AddEncoderDefinition(X32_ENC_DYNAMICS, 0x0103);

            AddEncoderDefinition(X32_ENC_EQ_Q, 0x0104);
            AddEncoderDefinition(X32_ENC_EQ_FREQ, 0x0105);
            AddEncoderDefinition(X32_ENC_EQ_GAIN, 0x0106);

            AddEncoderDefinition(X32_ENC_MAIN, 0x010B);
            AddEncoderDefinition(X32_ENC_PAN, 0x010C);

            AddEncoderDefinition(X32_ENC_ENCODER1, 0x010D);
            AddEncoderDefinition(X32_ENC_ENCODER2, 0x010E);
            AddEncoderDefinition(X32_ENC_ENCODER3, 0x010F);
            AddEncoderDefinition(X32_ENC_ENCODER4, 0x0110);
            AddEncoderDefinition(X32_ENC_ENCODER5, 0x0111);
            AddEncoderDefinition(X32_ENC_ENCODER6, 0x0112);

    }

    if (config->IsModelX32Compact()){

            AddButtonDefinition(X32_BTN_TALK_A,         0x0100);
            AddButtonDefinition(X32_BTN_TALK_B,         0x0101);
            AddButtonDefinition(X32_BTN_MONITOR_DIM,    0x0102);
            AddButtonDefinition(X32_BTN_VIEW_MONITOR,   0x0103);

            AddButtonDefinition(X32_BTN_VIEW_USB,      0x0104);

            AddButtonDefinition(X32_BTN_PHANTOM_48V, 0x0105);
            AddButtonDefinition(X32_BTN_PHASE_INVERT, 0x0106);
            AddButtonDefinition(X32_BTN_LOW_CUT, 0x0107);
            AddButtonDefinition(X32_BTN_VIEW_CONFIG, 0x0108);

            AddButtonDefinition(X32_BTN_GATE, 0x0109);
            AddButtonDefinition(X32_BTN_VIEW_GATE, 0x010A);

            AddButtonDefinition(X32_BTN_COMPRESSOR, 0x010B);
            AddButtonDefinition(X32_BTN_VIEW_COMPRESSOR, 0x010C);

            AddButtonDefinition(X32_BTN_EQ, 0x010D);
            AddButtonDefinition(X32_BTN_EQ_MODE, 0x010E);
            AddButtonDefinition(X32_BTN_EQ_HIGH, 0x010F);
            AddButtonDefinition(X32_BTN_EQ_HIGH_MID, 0x0110);
            AddButtonDefinition(X32_BTN_EQ_LOW_MID, 0x0111);
            AddButtonDefinition(X32_BTN_EQ_LOW, 0x0112);
            AddButtonDefinition(X32_BTN_VIEW_EQ, 0x0113);

            AddButtonDefinition(X32_BTN_VIEW_MIX_BUS_SENDS, 0x0114);
            AddButtonDefinition(X32_BTN_MONO_BUS, 0x0115);
            AddButtonDefinition(X32_BTN_MAIN_LR_BUS, 0x0116);
            AddButtonDefinition(X32_BTN_VIEW_BUS_MIXES, 0x0117);

            AddButtonDefinition(X32_BTN_ENCODER1, 0x0118);
            AddButtonDefinition(X32_BTN_ENCODER2, 0x0119);
            AddButtonDefinition(X32_BTN_ENCODER3, 0x011A);
            AddButtonDefinition(X32_BTN_ENCODER4, 0x011B);
            AddButtonDefinition(X32_BTN_ENCODER5, 0x011C);
            AddButtonDefinition(X32_BTN_ENCODER6, 0x011D);

            AddButtonDefinition(X32_BTN_HOME,           0x011E);
            AddButtonDefinition(X32_BTN_METERS,         0x011F);
            AddButtonDefinition(X32_BTN_ROUTING,        0x0120);
            AddButtonDefinition(X32_BTN_SETUP,          0x0121);
            AddButtonDefinition(X32_BTN_LIBRARY,        0x0122);
            AddButtonDefinition(X32_BTN_EFFECTS,        0x0123);
            AddButtonDefinition(X32_BTN_MUTE_GRP,       0x0124);
            AddButtonDefinition(X32_BTN_UTILITY,        0x0125);

            AddButtonDefinition(X32_BTN_UP,             0x0126);
            AddButtonDefinition(X32_BTN_DOWN,           0x0127);
            AddButtonDefinition(X32_BTN_LEFT,           0x0128);
            AddButtonDefinition(X32_BTN_RIGHT,          0x0129);

            // Board L

            AddButtonDefinition(X32_BTN_CH_1_8, 0x400);
            AddButtonDefinition(X32_BTN_CH_9_16, 0x401);
            AddButtonDefinition(X32_BTN_CH_17_24, 0x402);
            AddButtonDefinition(X32_BTN_CH_25_32, 0x403);
            AddButtonDefinition(X32_BTN_AUX_IN_1_6_USB_REC, 0x404);
            AddButtonDefinition(X32_BTN_EFFECTS_RETURNS, 0x405);
            AddButtonDefinition(X32_BTN_BUS_1_8_MASTER, 0x406);
            AddButtonDefinition(X32_BTN_BUS_9_16_MASTER, 0x407);

            AddButtonDefinition(X32_BTN_BOARD_L_CH_1_SELECT, 0x420);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_2_SELECT, 0x421);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_3_SELECT, 0x422);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_4_SELECT, 0x423);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_5_SELECT, 0x424);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_6_SELECT, 0x425);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_7_SELECT, 0x426);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_8_SELECT, 0x427);

            AddButtonDefinition(X32_BTN_BOARD_L_CH_1_SOLO, 0x430);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_2_SOLO, 0x431);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_3_SOLO, 0x432);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_4_SOLO, 0x433);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_5_SOLO, 0x434);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_6_SOLO, 0x435);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_7_SOLO, 0x436);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_8_SOLO, 0x437);

            AddButtonDefinition(X32_BTN_BOARD_L_CH_1_MUTE, 0x440);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_2_MUTE, 0x441);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_3_MUTE, 0x442);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_4_MUTE, 0x443);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_5_MUTE, 0x444);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_6_MUTE, 0x445);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_7_MUTE, 0x446);
            AddButtonDefinition(X32_BTN_BOARD_L_CH_8_MUTE, 0x447);

            // Board R);

            AddButtonDefinition(X32_BTN_DAW_REMOTE, 0x800);
            AddButtonDefinition(X32_BTN_SEND_ON_FADERS, 0x801);
            AddButtonDefinition(X32_BTN_GROUP_DCA_1_8, 0x802);
            AddButtonDefinition(X32_BTN_BUS_1_8, 0x803);
            AddButtonDefinition(X32_BTN_BUS_9_16, 0x804);
            AddButtonDefinition(X32_BTN_MATRIX_MAIN_C, 0x805);
            AddButtonDefinition(X32_BTN_CLEAR_SOLO, 0x806);

            AddButtonDefinition(X32_BTN_SCENES_UNDO, 0x807);
            AddButtonDefinition(X32_BTN_SCENES_GO, 0x808);
            AddButtonDefinition(X32_BTN_SCENES_PREV, 0x809);
            AddButtonDefinition(X32_BTN_SCENES_NEXT, 0x80A);
            AddButtonDefinition(X32_BTN_VIEW_SCENES, 0x80B);

            AddButtonDefinition(X32_BTN_ASSIGN_1, 0x80C);
            AddButtonDefinition(X32_BTN_ASSIGN_2, 0x80D);
            AddButtonDefinition(X32_BTN_ASSIGN_3, 0x80E);
            AddButtonDefinition(X32_BTN_ASSIGN_4, 0x80F);
            AddButtonDefinition(X32_BTN_ASSIGN_5, 0x810);
            AddButtonDefinition(X32_BTN_ASSIGN_6, 0x811);
            AddButtonDefinition(X32_BTN_ASSIGN_7, 0x812);
            AddButtonDefinition(X32_BTN_ASSIGN_8, 0x813);
            AddButtonDefinition(X32_BTN_VIEW_ASSIGN, 0x814);

            AddButtonDefinition(X32_BTN_MUTE_GROUP_1, 0x815);
            AddButtonDefinition(X32_BTN_MUTE_GROUP_2, 0x816);
            AddButtonDefinition(X32_BTN_MUTE_GROUP_3, 0x817);
            AddButtonDefinition(X32_BTN_MUTE_GROUP_4, 0x818);
            AddButtonDefinition(X32_BTN_MUTE_GROUP_5, 0x819);
            AddButtonDefinition(X32_BTN_MUTE_GROUP_6, 0x81A);

            AddButtonDefinition(X32_BTN_BOARD_R_CH_1_SELECT, 0x820);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_2_SELECT, 0x821);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_3_SELECT, 0x822);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_4_SELECT, 0x823);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_5_SELECT, 0x824);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_6_SELECT, 0x825);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_7_SELECT, 0x826);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_8_SELECT, 0x827);
            AddButtonDefinition(X32_BTN_MAIN_SELECT, 0x828);

            AddButtonDefinition(X32_BTN_BOARD_R_CH_1_SOLO, 0x830);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_2_SOLO, 0x831);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_3_SOLO, 0x832);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_4_SOLO, 0x833);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_5_SOLO, 0x834);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_6_SOLO, 0x835);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_7_SOLO, 0x836);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_8_SOLO, 0x837);
            AddButtonDefinition(X32_BTN_MAIN_SOLO, 0x838);

            AddButtonDefinition(X32_BTN_BOARD_R_CH_1_MUTE, 0x840);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_2_MUTE, 0x841);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_3_MUTE, 0x842);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_4_MUTE, 0x843);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_5_MUTE, 0x844);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_6_MUTE, 0x845);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_7_MUTE, 0x846);
            AddButtonDefinition(X32_BTN_BOARD_R_CH_8_MUTE, 0x847);
            AddButtonDefinition(X32_BTN_MAIN_MUTE, 0x848);

            // LED, only);

            AddButtonDefinition(X32_LED_EQ_HCUT, 0x0118);
            AddButtonDefinition(X32_LED_EQ_HSHV, 0x0119);
            AddButtonDefinition(X32_LED_EQ_VEQ, 0x011A);
            AddButtonDefinition(X32_LED_EQ_PEQ, 0x011B);
            AddButtonDefinition(X32_LED_EQ_LSHV, 0x011C);
            AddButtonDefinition(X32_LED_EQ_LCUT, 0x011D);

            AddButtonDefinition(X32_LED_USB_ACCESS, 0x0126);

            AddButtonDefinition(X32_LED_BACKLIGHT_CONFIG_GAIN, 0x012A);
            AddButtonDefinition(X32_LED_BACKLIGHT_CONFIG_FREQ, 0x012B);
            AddButtonDefinition(X32_LED_BACKLIGHT_GATE_THRESHOLD, 0x012C);
            AddButtonDefinition(X32_LED_BACKLIGHT_DYNAMICS_THRESHOLD, 0x012D);
            AddButtonDefinition(X32_LED_BACKLIGHT_EQ_Q, 0x012E);
            AddButtonDefinition(X32_LED_BACKLIGHT_EQ_FREQ, 0x012F);
            AddButtonDefinition(X32_LED_BACKLIGHT_EQ_GAIN, 0x0130);
            AddButtonDefinition(X32_LED_BACKLIGHT_BUS_MIXES_LEVEL, 0x0131);
            AddButtonDefinition(X32_LED_BACKLIGHT_BUS_MIXES_PAN, 0x0132);

            // Encoder

            AddEncoderDefinition(X32_ENC_GAIN, 0x0100);
            AddEncoderDefinition(X32_ENC_LOWCUT, 0x0101);
            AddEncoderDefinition(X32_ENC_GATE, 0x0102);
            AddEncoderDefinition(X32_ENC_DYNAMICS, 0x0103);

            AddEncoderDefinition(X32_ENC_EQ_Q, 0x0104);
            AddEncoderDefinition(X32_ENC_EQ_FREQ, 0x0105);
            AddEncoderDefinition(X32_ENC_EQ_GAIN, 0x0106);

            AddEncoderDefinition(X32_ENC_MAIN, 0x0107);
            AddEncoderDefinition(X32_ENC_PAN, 0x0108);

            AddEncoderDefinition(X32_ENC_ENCODER1, 0x0109);
            AddEncoderDefinition(X32_ENC_ENCODER2, 0x010A);
            AddEncoderDefinition(X32_ENC_ENCODER3, 0x010B);
            AddEncoderDefinition(X32_ENC_ENCODER4, 0x010C);
            AddEncoderDefinition(X32_ENC_ENCODER5, 0x010D);
            AddEncoderDefinition(X32_ENC_ENCODER6, 0x010E);
    }

    if (config->IsModelX32Rack()){

            AddButtonDefinition(X32_BTN_VIEW_USB, 0x0000);
            AddButtonDefinition(X32_BTN_CHANNEL_SOLO, 0x0001);
            AddButtonDefinition(X32_BTN_CHANNEL_MUTE, 0x0002);
            AddButtonDefinition(X32_BTN_TALK_A, 0x0003);
            AddButtonDefinition(X32_BTN_CLEAR_SOLO, 0x0004);
            AddButtonDefinition(X32_BTN_CHANNEL_ENCODER, 0x0005);

            AddButtonDefinition(X32_BTN_HOME, 0x0006);
            AddButtonDefinition(X32_BTN_METERS, 0x0007);
            AddButtonDefinition(X32_BTN_ROUTING, 0x0008);
            AddButtonDefinition(X32_BTN_LIBRARY, 0x0009);
            AddButtonDefinition(X32_BTN_EFFECTS, 0x000A);
            AddButtonDefinition(X32_BTN_SETUP, 0x000B);
            AddButtonDefinition(X32_BTN_VIEW_MONITOR, 0x000C);
            AddButtonDefinition(X32_BTN_VIEW_SCENES, 0x000D);
            AddButtonDefinition(X32_BTN_MUTE_GRP, 0x000E);
            AddButtonDefinition(X32_BTN_UTILITY, 0x000F);

            AddButtonDefinition(X32_BTN_ENCODER1, 0x0010);
            AddButtonDefinition(X32_BTN_ENCODER2, 0x0011);
            AddButtonDefinition(X32_BTN_ENCODER3, 0x0012);
            AddButtonDefinition(X32_BTN_ENCODER4, 0x0013);
            AddButtonDefinition(X32_BTN_ENCODER5, 0x0014);
            AddButtonDefinition(X32_BTN_ENCODER6, 0x0015);

            AddButtonDefinition(X32_BTN_UP, 0x0016);
            AddButtonDefinition(X32_BTN_LEFT, 0x0017);
            AddButtonDefinition(X32_BTN_RIGHT, 0x0018);
            AddButtonDefinition(X32_BTN_DOWN, 0x0019);

            // Encoder

            AddEncoderDefinition(X32_ENC_CHANNEL_LEVEL, 0x0000);
            AddEncoderDefinition(X32_ENC_MAIN_LEVEL, 0x0001);
            AddEncoderDefinition(X32_ENC_CHANNEL_SELECT, 0x0002);
            AddEncoderDefinition(X32_ENC_ENCODER1, 0x0003);
            AddEncoderDefinition(X32_ENC_ENCODER2, 0x0004);
            AddEncoderDefinition(X32_ENC_ENCODER3, 0x0005);
            AddEncoderDefinition(X32_ENC_ENCODER4, 0x0006);
            AddEncoderDefinition(X32_ENC_ENCODER5, 0x0007);
            AddEncoderDefinition(X32_ENC_ENCODER6, 0x0008);

            // LED only

            AddButtonDefinition(X32_LED_VIEW_USB_RED, 0x0005);

            AddButtonDefinition(X32_LED_IN, 0x0010);
            AddButtonDefinition(X32_LED_AUX, 0x0011);
            AddButtonDefinition(X32_LED_BUS, 0x0012);
            AddButtonDefinition(X32_LED_DCA, 0x0013);
            AddButtonDefinition(X32_LED_MAIN, 0x0014);
            AddButtonDefinition(X32_LED_MATRIX, 0x0015);

            AddButtonDefinition(X32_LED_BACKLIGHT_CHANNEL_LEVEL, 0x0016);
            AddButtonDefinition(X32_LED_BACKLIGHT_CHANNEL_LEVEL, 0x0017);

    }

    if (config->IsModelX32Core()){

            AddButtonDefinition(X32_BTN_SCENE_SETUP,    0x0000);
            AddButtonDefinition(X32_BTN_TALK_A,         0x0001);
            AddButtonDefinition(X32_BTN_ASSIGN_3,       0x0002);
            AddButtonDefinition(X32_BTN_ASSIGN_4,       0x0003);
            AddButtonDefinition(X32_BTN_ASSIGN_5,       0x0004);
            AddButtonDefinition(X32_BTN_ASSIGN_6,       0x0005);
            AddButtonDefinition(X32_BTN_ENCODER1,       0x0006);
            //AddButtonDefinition(X32_BTN_ENCODER1,       0x0006);

            // LED only
            AddButtonDefinition(X32_LED_SCENE_SETUP_RED,0x0006);
            AddButtonDefinition(X32_LED_IN,             0x0007);
            AddButtonDefinition(X32_LED_AUX,            0x0008);
            AddButtonDefinition(X32_LED_BUS,            0x0009);
            AddButtonDefinition(X32_LED_MTX,            0x000A);
            AddButtonDefinition(X32_LED_DCA,            0x000B);
            AddButtonDefinition(X32_LED_AESA_GREEN,     0x000C);
            AddButtonDefinition(X32_LED_AESA_RED,       0x000D);
            AddButtonDefinition(X32_LED_AESB_GREEN,     0x000E);
            AddButtonDefinition(X32_LED_AESB_RED,       0x000F);

    }
}

uint16_t Surface::Enum2Button(X32_BTN button) {
    //x32debug("DEBUG: enum2button: Button %d -> ", button);
    for(int i = 0; i < buttonDefinitionIndex; i++) {
        if (x32_btn_def[i].button == button) {
            //x32debug("gefunden: Button %d\n", x32_btn_def[i].buttonNr);
            return x32_btn_def[i].buttonNr;
        }
    }
    //x32debug(" NICHT gefunden!\n");
    return 0;
}

X32_BTN Surface::Button2Enum(uint16_t buttonNr) {
    //x32debug("DEBUG: button2enum: ButtonNr %d -> ", buttonNr);
    for(int i = 0; i < buttonDefinitionIndex; i++) {
        if (x32_btn_def[i].buttonNr == buttonNr) {
            //x32debug("gefunden: Button %d\n", x32_btn_def[i].button);
            return x32_btn_def[i].button;
        }
    }
    //x32debug(" NICHT gefunden!\n");
    return X32_BTN_NONE;
}

uint16_t Surface::Enum2Encoder(X32_ENC encoder) {
    //x32debug("DEBUG: enum2button: Encoder %d -> ", encoder);
    for(int i = 0; i < encoderDefinitionIndex; i++) {
        if (x32_enc_def[i].encoder == encoder) {
            //x32debug("gefunden: Encoder %d\n", x32_enc_def[i].encoderNr);
            return x32_enc_def[i].encoderNr;
        }
    }
    //x32debug(" NICHT gefunden!\n");
    return 0;
}

X32_ENC Surface::Encoder2Enum(uint16_t encoderNr) {
    //x32debug("DEBUG: encoder2enum: EncoderNr %d -> ", encoderNr);
    for(int i = 0; i < encoderDefinitionIndex; i++) {
        if (x32_enc_def[i].encoderNr == encoderNr) {
            //x32debug("gefunden: Encoder %d\n", x32_enc_def[i].encoder);
            return x32_enc_def[i].encoder;
        }
    }
    //x32debug(" NICHT gefunden!\n");
    return X32_ENC_NONE;
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t Surface::CalcEncoderRingLedIncrement(uint8_t pct) {
    uint8_t num_leds_to_light = 0;
/*
    if (pct <= 50) {
        // Scale 0-50 to 0-6 LEDs
        num_leds_to_light = (uint8_t)((float)pct / 50.0f * 6.0f);
    } else {
        // Scale 51-100 to 7-12 LEDs (6 more LEDs)
        // From 51 to 100, there are 50 steps.
        // We need to add (pct - 50) steps mapped to the remaining 6 LEDs.
        // (pct - 50) / 50.0f * 6.0f
        num_leds_to_light = 6 + (uint8_t)(((float)(pct - 50) / 50.0f) * 6.0f);

        if (num_leds_to_light > 12) {
            num_leds_to_light = 12;
        }
    }
*/
    num_leds_to_light = (uint8_t)((float)pct / 100.0f * 12.5f);
    if (num_leds_to_light > 13) {
        num_leds_to_light = 13;
    }

    uint16_t led_mask = 0;
    if (num_leds_to_light > 0) {
        led_mask = (1U << num_leds_to_light) - 1;
    }

    return led_mask;
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t Surface::CalcEncoderRingLedDecrement(uint8_t pct) {
    uint8_t num_leds_to_light = 0;
/*
    if (pct <= 50) {
        // Scale 0-50 to 0-6 LEDs
        num_leds_to_light = (uint8_t)((float)pct / 50.0f * 6.0f);
    } else {
        // Scale 51-100 to 7-12 LEDs (6 more LEDs)
        // From 51 to 100, there are 50 steps.
        // We need to add (pct - 50) steps mapped to the remaining 6 LEDs.
        // (pct - 50) / 50.0f * 6.0f
        num_leds_to_light = 6 + (uint8_t)(((float)(pct - 50) / 50.0f) * 6.0f);

        if (num_leds_to_light > 12) {
            num_leds_to_light = 12;
        }
    }
*/
    num_leds_to_light = (uint8_t)((float)pct / 100.0f * 12.5f);
    if (num_leds_to_light > 13) {
        num_leds_to_light = 13;
    }

    uint16_t led_mask = 0;
    
    // Bits von rechts nach links setzen
    for (uint8_t i = 0; i < num_leds_to_light; ++i) {
        led_mask |= (1U << (12 - i));
    }

    return led_mask;
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t Surface::CalcEncoderRingLedPosition(uint8_t pct) {
    uint8_t led_index = (uint8_t)(((float)pct / 100.0f) * 12.0f + 0.5f); // +0.5f für Rundung

    if (led_index > 12) {
        led_index = 12;
    }

    return (1U << led_index);
}

uint16_t Surface::CalcEncoderRingLedDbfs(float dbfs, bool onlyPosition) {
    uint16_t led_mask = 0;

    if (config->IsModelX32Rack()){
        // X32Rack: Channel Level, Mail LR Level

        // LEDs dBfs
        // 1 -50
        // 2 -40
        // 3 -30
        // 4 -24
        // 5 -18
        // 6 -12
        // 7 -9
        // 8 -6
        // 9 -3
        // 10 0
        // 11 3
        // 12 6
        // 13 10


        if (dbfs > -60) { 
            uint8_t led_index = 0;

            if (dbfs >= 10) led_index = 12;
            else if (dbfs >= 6) led_index = 11;
            else if (dbfs >= 3) led_index = 10;
            else if (dbfs >= 0) led_index = 9;
            else if (dbfs >= -3) led_index = 8;
            else if (dbfs >= -6) led_index = 7;
            else if (dbfs >= -9) led_index = 6;
            else if (dbfs >= -12) led_index = 5;
            else if (dbfs >= -18) led_index = 4;
            else if (dbfs >= -24) led_index = 3;
            else if (dbfs >= -30) led_index = 2;
            else if (dbfs >= -40) led_index = 1;
            else if (dbfs > -50) led_index = 0;

            if (onlyPosition){
                led_mask = (1U << led_index);
            } else {
                led_mask = (1U << (led_index + 1)) -1;
            }
        } 

    } else {
    
        // led_index = (uint8_t)(((float)pct / 100.0f) * 12.0f + 0.5f); // +0.5f für Rundung

        // if (led_index > 12) {
        //     led_index = 12;
        // }
    }

    return led_mask;
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t Surface::CalcEncoderRingLedBalance(uint8_t pct) {
    uint16_t led_mask = 0;

    if (pct < 50) {
        float scale = (float)pct / 50.0f; // Skaliert 0-49 auf 0-0.98
        // (scale * 6) = Anzahl der LEDs, die an sind.
        uint8_t num_on_left_side = (uint8_t)roundf(scale * 6.5f);

        // Sicherstellen, dass mindestens Bit 6 an ist, wenn pct < 50
        if (num_on_left_side < 1) num_on_left_side = 1;

        // Setze die Bits von Bit 0 bis zum berechneten Index
        for (int i = 0; i < num_on_left_side; ++i) {
            if (i <= 6) { // Sicherstellen, dass wir im Bereich Bits 0-6 bleiben
                led_mask |= (1U << i);
            }
        }

        // invert LED-mask
        led_mask ^= 0xFFFF;
        led_mask &= 0b0000000001111111;

    } else { // pct >= 50
        // Skaliere (pct - 50) von 1-50 auf die Anzahl der LEDs, die von Bit 6 nach rechts zusätzlich an sein sollen.
        float scale = (float)(pct - 50) / 50.0f; // Skaliert 51-100 auf 0.02-1
        uint8_t num_on_right_side = (uint8_t)roundf(1.0f + (scale * 6.5f)); // 1 für Bit 6, plus 6 weitere LEDs

        // Sicherstellen, dass mindestens Bit 6 an ist, wenn pct > 50
        if (num_on_right_side < 1) num_on_right_side = 1;

        // Setze die Bits von Bit 6 bis zum berechneten Index
        for (int i = 0; i < num_on_right_side; ++i) {
            if ((6 + i) <= 12) { // Sicherstellen, dass wir im Bereich Bits 6-12 bleiben
                led_mask |= (1U << (6 + i));
            }
        }
    }

    return led_mask;
}

// bit 0=CCW, bit 6=center, bit 12 = CW, bit 15=encoder-backlight
// CCW <- XXXXXX X XXXXXX -> CW
uint16_t Surface::CalcEncoderRingLedWidth(uint8_t pct) {
    if (pct == 0) {
        return (1U << 6); // Setzt nur Bit 6
    }

    float scaled_value = (float)(pct - 1) / 99.0f; // Skaliert 1-100 auf 0-1

    // Anzahl der zusätzlichen LEDs, die auf der linken Seite von Bit 6 aus eingeschaltet werden sollen.
    // Max 6 LEDs (Bits 0-5).
    uint8_t num_left_additional_leds = (uint8_t)roundf(scaled_value * 6.0f);
    // Anzahl der zusätzlichen LEDs, die auf der rechten Seite von Bit 6 aus eingeschaltet werden sollen.
    // Max 7 LEDs (Bits 7-12).
    uint8_t num_right_additional_leds = (uint8_t)roundf(scaled_value * 6.0f);

    uint16_t led_mask = (1U << 6); // Starte mit Bit 6 gesetzt

    // Schalte die zusätzlichen LEDs auf der linken Seite ein
    for (int i = 0; i < num_left_additional_leds; ++i) {
        if ((6 - (i + 1)) >= 0) { // Sicherstellen, dass der Index nicht negativ wird
            led_mask |= (1U << (6 - (i + 1)));
        }
    }

    // Schalte die zusätzlichen LEDs auf der rechten Seite ein
    for (int i = 0; i < num_right_additional_leds; ++i) {
        if ((6 + (i + 1)) <= 12) { // Sicherstellen, dass der Index nicht über 15 hinausgeht
            led_mask |= (1U << (6 + (i + 1)));
        }
    }

    // Bei 100% sollen alle Bits gesetzt sein (0x1FFF).
    // Die Berechnung oben sollte dies bereits erreichen, aber eine explizite Prüfung schadet nicht.
    if (pct == 100) {
        return 0x1FFF; // Alle 13 Bits setzen
    }

    return led_mask;
}

// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// brightness = 0 ... 255
void Surface::SetBrightness(uint8_t boardId, uint8_t brightness) {
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('C'); // class: C = Controlmessage
    message.AddDataByte('B'); // index
    message.AddDataByte(brightness);
    uart->Tx(&message, true);
}

// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// brightness = 0 ... 255
void Surface::SetContrast(uint8_t boardId, uint8_t contrast) {
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('C'); // class: C = Controlmessage
    message.AddDataByte('C'); // index
    message.AddDataByte(contrast & 0x3F);
    uart->Tx(&message, true);
}

void Surface::SetLed(uint8_t boardId, uint8_t ledId, bool ledState){
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId);
    message.AddDataByte('L'); // class: L = LED
    message.AddDataByte(0x80); // index - fixed at 0x80 for LEDs
    if (ledState > 0) {
        message.AddDataByte(ledId + 0x80); // turn LED on
    }else{
        message.AddDataByte(ledId); // turn LED off
    }
    uart->Tx(&message, true);
}

// position = 0x0000 ... 0x0FFF
void Surface::SetFader(uint8_t boardId, uint8_t index, uint16_t position) {
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('F'); // class: F = Fader
    message.AddDataByte(index); // index
    message.AddDataByte((position & 0xFF)); // LSB
    message.AddDataByte((char)((position & 0x0F00) >> 8)); // MSB
    uart->Tx(&message, true);
    BlockFader(boardId, index);
}

// set 7-Segment display on X32 Rack
// dot = 128
void Surface::SetX32RackDisplayRaw(uint8_t p_value2, uint8_t p_value1){
    SurfaceMessage message;
    message.AddDataByte(0x80);
    message.AddDataByte('D'); // Display
    message.AddDataByte(0x80);
    message.AddDataByte(p_value2); 
    message.AddDataByte(p_value1);
    uart->Tx(&message, true);
}

// set 7-Segment display on X32 Rack
// dot = 128 or 256
void Surface::SetX32RackDisplay(uint8_t vChannelIndex){
    uint8_t vChannelNumber = vChannelIndex + 1;
    if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK_NORMAL)) {
        SetX32RackDisplayRaw(int2segment((uint8_t)(vChannelNumber/10)), int2segment(vChannelNumber % 10));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK_AUX)) {
        SetX32RackDisplayRaw(int2segment('A'), int2segment(vChannelNumber - X32_VCHANNEL_BLOCK_AUX));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK_FXRET)) {
        SetX32RackDisplayRaw(int2segment('F'), int2segment(vChannelNumber - X32_VCHANNEL_BLOCK_FXRET));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK_BUS)) {
        uint8_t number = vChannelNumber - X32_VCHANNEL_BLOCK_BUS;
        SetX32RackDisplayRaw(int2segment((uint8_t)(number/10)), int2segment(number % 10)+128);
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK_MATRIX)) {
        SetX32RackDisplayRaw(int2segment('M'), int2segment(vChannelNumber - X32_VCHANNEL_BLOCK_MATRIX));
    } else if (helper->IsInChannelBlock(vChannelIndex, X32_VCHANNEL_BLOCK_DCA)) {
        SetX32RackDisplayRaw(int2segment('D'), int2segment(vChannelNumber - X32_VCHANNEL_BLOCK_DCA));
    }
}

uint8_t Surface::int2segment(int8_t p_value){
    switch (p_value){
        case 0:
            return 63;
        case 1:
            return 6;
        case 2:
            return 91;
        case 3:
            return 79;
        case 4:
            return 102;
        case 5:
            return 109;
        case 6:
            return 125;
        case 7:
            return 7;
        case 8:
            return 127;
        case 9:
            return 111;
        case 'a':
        case 'A':
            return 119;
        case 'b':
        case 'B':
            return 124;
        case 'd':
        case 'D':
            return 94;
        case 'f':
        case 'F':
            return 1 + 32 + 64 + 16;
        case 'm':
        case 'M':
            return 55;
        default:
            return 0;
    }
}

// ledNr = Nr of the LED from constants.h (contains BoardId and LED-Nr)
// state = 0 / 1
void Surface::SetLedByNr(uint16_t ledNr, bool ledState) {
  uint8_t boardId = (uint8_t)((ledNr & 0xFF00) >> 8);
  uint8_t ledId = (uint8_t)(ledNr & 0x7F);

  //x32debug("LedNr: 0x%04X -> BoardId: 0x%02X, LED: 0x%02X\n", ledNr, boardId, ledId);

  SetLed(boardId, ledId, ledState);
}

// ledNr = LED from X32_BTN enum
// state = 0 / 1
void Surface::SetLedByEnum(X32_BTN led, bool ledState) {
    SetLedByNr(Enum2Button(led), ledState);
}

// boardId = 0, 1, 4, 5, 8
// index = 0 ... 8
// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
void Surface::SetMeterLed(uint8_t boardId, uint8_t index, uint8_t leds) {
  // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
  // 0x4C, index, leds.b[]
  SurfaceMessage message;
  message.AddDataByte(0x80 + boardId); // start message for specific boardId
  message.AddDataByte('M'); // class: M = Meter
  message.AddDataByte(index); // index
  message.AddDataByte(leds);
  uart->Tx(&message, true);
}
// preamp = 8-bit bitwise (bit 0=Sig, 1=-30dB ... 6=-3dB, 7=Clip)
// meter = 32-bit bitwise (bit 0=-45dB ... 15=-4, 16=-2, 19=Clip, 20+=unused)
void Surface::SetMeterLedMain_Rack(uint8_t preamp, uint32_t meterL, uint32_t meterR, uint32_t meterSolo) {
    SurfaceMessage message;
    message.AddDataByte(0x80); // start message for specific boardId
    message.AddDataByte('M'); // class: M = Meter
    message.AddDataByte(0); // index
    message.AddDataByte(preamp);

    // Example for Big Meters on X32Rack (!different scale to X32Full/Compact/Producer!)
    //message.AddDataByte(0b11110000);  // first nibble shifted 4 to left
    //message.AddDataByte(0b11111111);  // bit 4..12 shifted 4 to right
    //message.AddDataByte(0b10110111);  // last bits are crazy splitted :-/

    message.AddDataByte((uint8_t)(meterL<<4));
    message.AddDataByte((uint8_t)(meterL>>4));
    message.AddDataByte((((uint8_t)(meterL>>12))&0b10000111) | (((uint8_t)(meterL>>11))&0b00110000));
    message.AddDataByte(0);
    message.AddDataByte((uint8_t)(meterR<<4));
    message.AddDataByte((uint8_t)(meterR>>4));
    message.AddDataByte((((uint8_t)(meterR>>12))&0b10000111) | (((uint8_t)(meterR>>11))&0b00110000));
    message.AddDataByte(0x00);
    message.AddDataByte((uint8_t)(meterSolo<<4));
    message.AddDataByte((uint8_t)(meterSolo>>4));
    message.AddDataByte((((uint8_t)(meterSolo>>12))&0b10000111) | (((uint8_t)(meterSolo>>11))&0b00110000));
    message.AddDataByte(0x00);
    uart->Tx(&message, true);
}

// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
// leds = 32-bit bitwise (bit 0=-57dB ... 22=-2, 23=-1, 24=Clip)
void Surface::SetMeterLedMain_FullCompactProducer(uint8_t preamp, uint8_t dynamics, uint32_t meterL, uint32_t meterR, uint32_t meterSolo) {
    // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
    // 0x4C, index, leds.b[]
    SurfaceMessage message;
    message.AddDataByte(0x80 + 1); // start message for specific boardId
    message.AddDataByte('M'); // class: M = Meter
    message.AddDataByte(0); // index
    message.AddDataByte(preamp);
    message.AddDataByte(dynamics);
    message.AddDataByte((uint8_t)meterL);
    message.AddDataByte((uint8_t)(meterL>>8));
    message.AddDataByte((uint8_t)(meterL>>16));
    message.AddDataByte(0x00);
    message.AddDataByte((uint8_t)meterR);
    message.AddDataByte((uint8_t)(meterR>>8));
    message.AddDataByte((uint8_t)(meterR>>16));
    message.AddDataByte(0x00);
    message.AddDataByte((uint8_t)meterSolo);
    message.AddDataByte((uint8_t)(meterSolo>>8));
    message.AddDataByte((uint8_t)(meterSolo>>16));
    message.AddDataByte(0x00);
    uart->Tx(&message, true);
}

// boardId = 0, 1, 4, 5, 8
// index
// ledMode = 0=increment, 1=absolute position, 2=balance l/r, 3=width/spread, 4=decrement
// ledPct = percentage 0...100
// backlight = enable or disable
void Surface::SetEncoderRing(uint8_t boardId, uint8_t index, uint8_t ledMode, uint8_t ledPct, bool backlight) {
    // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
    // 0x52, index, leds.w[]
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('R'); // class: R = Ring
    message.AddDataByte(index); // index

    uint16_t leds = 0;
    switch (ledMode) {
        case 0: // standard increment-method
            leds = CalcEncoderRingLedIncrement(ledPct);
            break;
        case 1: // absolute position
            leds = CalcEncoderRingLedPosition(ledPct);
            break;
        case 2: // balance left/right
            leds = CalcEncoderRingLedBalance(ledPct);
            break;
        case 3: // spread/width
            leds = CalcEncoderRingLedWidth(ledPct);
            break;
        case 4: // decrement-method
            leds = CalcEncoderRingLedDecrement(ledPct);
            break;
    }
    message.AddDataByte(leds & 0xFF);
    if (backlight) {
        message.AddDataByte(((leds & 0x7F00) >> 8) + 0x80); // turn backlight on
    }else{
        message.AddDataByte(((leds & 0x7F00) >> 8)); // turn backlight off
    }
    uart->Tx(&message, true);
}

void Surface::SetEncoderRingDbfs(uint8_t boardId, uint8_t index, float dbfs, bool muted, bool backlight) {
    // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
    // 0x52, index, leds.w[]
    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('R'); // class: R = Ring
    message.AddDataByte(index); // index

    uint16_t leds = CalcEncoderRingLedDbfs(dbfs, muted);

    message.AddDataByte(leds & 0xFF);
    if (backlight) {
        message.AddDataByte(((leds & 0x7F00) >> 8) + 0x80); // turn backlight on
    }else{
        message.AddDataByte(((leds & 0x7F00) >> 8)); // turn backlight off
    }
    uart->Tx(&message, true);
}

// boardId = 0, 4, 5, 8
// index = 0 ... 8
// color = 0=BLACK, 1=RED, 2=GREEN, 3=YELLOW, 4=BLUE, 5=PINK, 6=CYAN, 7=WHITE
// icon = 0xA0 (none), 0xA1 ... 0xE9
// sizeA/B = 0x00 (small) or 0x20 (large)
// xA/B = horizontal position in pixel
// yA/B = vertical position in pixel
// strA/B = String of Text to be displayed
void Surface::SetLcd(
    uint8_t boardId, uint8_t index, uint8_t color,
    uint8_t xicon, uint8_t yicon, uint8_t icon, 
    uint8_t sizeA, uint8_t xA, uint8_t yA, const char* strA,
    uint8_t sizeB, uint8_t xB, uint8_t yB, const char* strB
    ) {
    // 0xFE, 0x8i, class, index, data[], 0xFE, chksum
    // 0x44, i, color.b, script[]

    SurfaceMessage message;
    message.AddDataByte(0x80 + boardId); // start message for specific boardId
    message.AddDataByte('D'); // class: D = Display
    message.AddDataByte(index); // index
    message.AddDataByte(color & 0x0F); // use only 4 bits (bit 0=R, 1=G, 2=B, 3=Inverted)

    // begin of script
    // transmit icon
    message.AddDataByte(icon);
    message.AddDataByte(xicon);
    message.AddDataByte(yicon);

    // transmit first text
    message.AddDataByte(sizeA + strlen(strA)); // size + textLength
    message.AddDataByte(xA);
    message.AddDataByte(yA);
    message.AddString(strA); // this is ASCII, so we can omit byte-stuffing

    message.AddDataByte(sizeB + strlen(strB)); // size + textLength
    message.AddDataByte(xB);
    message.AddDataByte(yB);
    message.AddString(strB); // this is ASCII, so we can omit byte-stuffing
    uart->Tx(&message, true);
}

void Surface::SetLcdX(LcdData* p_data, uint8_t p_textCount) {
    SurfaceMessage message;
    message.AddDataByte(0x80 + p_data->boardId);
    message.AddDataByte('D'); // class: D = Display
    message.AddDataByte(p_data->lcdIndex); 
    message.AddDataByte((p_data->color) & 0x0F);
    message.AddDataByte(p_data->icon.icon);
    message.AddDataByte(p_data->icon.x);
    message.AddDataByte(p_data->icon.y);
    for (int i=0;i<p_textCount;i++){
        message.AddDataByte(p_data->texts[i].size + strlen(p_data->texts[i].text.c_str())); // size + textLength
        message.AddDataByte(p_data->texts[i].x);
        message.AddDataByte(p_data->texts[i].y);
        message.AddString(p_data->texts[i].text.c_str()); // this is ASCII, so we can omit byte-stuffing  
    }
    uart->Tx(&message, true);
}

void Surface::ProcessUartData() {
    //uint8_t currentByte = 0;
    uint8_t receivedClass = 0;
    uint8_t receivedIndex = 0;
    uint16_t receivedValue = 0;
    //uint8_t receivedChecksum = 0;
    bool lastPackageIncomplete = false;

    int bytesToProcess = uart->Rx(&surfaceBufferUart[0], sizeof(surfaceBufferUart));

    if (bytesToProcess <= 0) {
        return;
	}

    // first init package buffer with 0x00s
    // start at surfacePacketCurrentIndex to not overwrite saved data from last incomplete package
    for (uint8_t package=0; package<SURFACE_MAX_PACKET_LENGTH;package++){
        for (int i = surfacePacketCurrentIndex; i < 6; i++) {
            surfacePacketBuffer[package][i]=0x00;
        }
        surfacePacketCurrentIndex=0;
    }



    if (helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE)) {
        printf("DEBUG_SURFACE: ");

        // print received values on one row
        bool divide_after_next_dbg = false;
        for (int i = 0; i < bytesToProcess; i++) {
            if (divide_after_next_dbg && ((uint8_t)surfaceBufferUart[i] == 0xFE)) {
                printf("| ");
                divide_after_next_dbg = false;
            }
            printf("%02X ", (uint8_t)surfaceBufferUart[i]); // empfangene Bytes als HEX-Wert ausgeben
            if (divide_after_next_dbg){
                printf("| ");
                divide_after_next_dbg = false;
            } 
            if ((uint8_t)surfaceBufferUart[i] == 0xFE) {
                divide_after_next_dbg=true;
            }
        }
        printf("\n");
    }

    // break up received data into packages
    bool divide_after_next = false;
    for (int i = 0; i < bytesToProcess; i++) {

        if (divide_after_next && ((uint8_t)surfaceBufferUart[i] == 0xFE)) {
            // previous package had no checksum
            surfacePacketCurrent++;
            surfacePacketCurrentIndex=0;
            divide_after_next = false;
        }

        surfacePacketBuffer[surfacePacketCurrent][surfacePacketCurrentIndex++] = (uint8_t)surfaceBufferUart[i];

        if (divide_after_next) {
            surfacePacketCurrent++;
            surfacePacketCurrentIndex=0;
            divide_after_next = false;
        }

        // use 0xFE as package divider
        if (((uint8_t)surfaceBufferUart[i] == 0xFE))
        {
            divide_after_next = true;
        }
    }

    if (divide_after_next){
        // divide_after_next got no usage, because the uartBuffer was emptied out -> reason: no checksum was send
        // clean up this situation
        surfacePacketCurrent++;
        while (surfacePacketCurrentIndex < 6){  
            // fill with zero - maybe not needed
            surfacePacketBuffer[surfacePacketCurrent][surfacePacketCurrentIndex++]=0x00;
        }
        surfacePacketCurrentIndex=0;
    }

    if (
        (surfacePacketCurrentIndex!=0) &&
        !((surfacePacketBuffer[surfacePacketCurrent][3]==0xFE) | (surfacePacketBuffer[surfacePacketCurrent][4]==0xFE))
    ){
        // last package was incomplete, save it for next run
        /*
            Example1:                                  _____ incomplete, has no 0xFE (and is too short)
                                                      /  
            this run         66 01 FB 00 FE 12 | 66 02

            next run         46 02 FE 44 | 66 03 D6 02 FE 33 | 66 04 73 02 FE 15 | 66 05 4E 03 FE 38 | 66 06 21 02 FE 65 |
                             \
                              \____ take the bytes from the last incomplete package and glue it together


            Example2:                                        _____ incomplete, has no 0xFE
                                                            / 
            this run         66 05 EF 0E FE 0C | 66 06 52 0D

            next run         FE 29 | 66 07 C2 0C FE 39
                             \
                              \____ take the bytes from the last incomplete package and glue it together
            
        */

        helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE, "surfacePacketCurrent=%d seems incomplete? surfacePacketCurrentIndex=%d", surfacePacketCurrent, surfacePacketCurrentIndex);
        lastPackageIncomplete = true;
    }


    if (helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE)) {
        printf("DEBUG_SURFACE: ");
        
        // print packages, one in a row    
        uint8_t packagesToPrint = surfacePacketCurrent;
        if (lastPackageIncomplete){
            packagesToPrint++;
        }
        printf("surfacePacketCurrent=%d\n", surfacePacketCurrent);

        for (int package=0; package < packagesToPrint; package++) {
            printf("surfaceProcessUartData(): Package %d: ", package);
            for (uint8_t i = 0; i<6; i++){
                printf("%02X ", surfacePacketBuffer[package][i]);
            }
            if (surfacePacketBuffer[package][0] == 0xFE){
                printf("  <--- Board %d", surfacePacketBuffer[package][1] & 0x7F);
            } else if (lastPackageIncomplete){
                printf("  <--- incomplete, saved for next run");
            }
            printf("\n");
        } 
    }   


    for (int8_t package=0; package < surfacePacketCurrent;package++){

        if (surfacePacketBuffer[package][0] == 0xFE){
            // received BoardId
            uint8_t receivedBoardIdtemp = surfacePacketBuffer[package][1] & 0x7F;
            switch(receivedBoardIdtemp){
                case 0:
                case 1:
                case 4:
                case 5:
                case 8:
                    receivedBoardId = receivedBoardIdtemp;
                    break;
            }
        } else {
            receivedClass = surfacePacketBuffer[package][0];
            receivedIndex = surfacePacketBuffer[package][1];
            if (surfacePacketBuffer[package][3] == 0xFE){
                // short package
                receivedValue = surfacePacketBuffer[package][2];
                //receivedChecksum = surfacePacketBuffer[package][4];
            } else if (surfacePacketBuffer[package][4] == 0xFE){
                // long package
                receivedValue = ((uint16_t)surfacePacketBuffer[package][3] << 8) | (uint16_t)surfacePacketBuffer[package][2];
                //receivedChecksum = surfacePacketBuffer[package][5];
            }
        

            // only process valid packages
            bool valid = true;

            switch (receivedClass){
                case 'f':
                case 'b':
                case 'e':
                    break;
                default:
                    valid = false;
                    break;
            }       

            if (valid){
                helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE, "Callback(%d, %02X, %02X, %04X)", receivedBoardId, receivedClass, receivedIndex, receivedValue);
                eventBuffer.push_back(new SurfaceEvent((X32_BOARD)receivedBoardId, receivedClass, receivedIndex, receivedValue));
            } 
        }
    }

    // all packages are processed
    // now clean up for next run

    if (lastPackageIncomplete){
        // copy last incomplete package to package0 for next run
        for (uint8_t i=0; i < surfacePacketCurrentIndex; i++){
            surfacePacketBuffer[0][i] = surfacePacketBuffer[surfacePacketCurrent][i];
        }

        // reset index for next run
        lastPackageIncomplete=false;
        surfacePacketCurrent=0;
        // do NOT touch surfacePacketCurrentIndex!
    }else {
        // reset index for next run
        surfacePacketCurrent=0;
        surfacePacketCurrentIndex=0;
    }
}

bool Surface::HasNextEvent(void){
    return eventBuffer.size() > 0;
}

SurfaceEvent* Surface::GetNextEvent(void){
    SurfaceEvent* event = eventBuffer.back();
    eventBuffer.pop_back();
    return event;
}


void Surface::BlockFader(uint8_t boardId, uint8_t faderIndex){
    faderBlockList.push_front(new SurfaceFaderblock(boardId, faderIndex));
}

bool Surface::IsFaderBlocked(uint8_t boardId, uint8_t faderIndex){
    for (auto const& i : faderBlockList) {
        if ((i->_boardId == boardId)&&(i->_index == faderIndex))
        {
            return true;
        }
    }
    return false;
}