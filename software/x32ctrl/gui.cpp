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

#include "gui.h"
#include "mixer.h"
#include "surface.h"
#include "hal.h"
#include "fpga.h"
#include "dsp.h"

lv_indev_t *keypad_indev; // will be updated by buttonCallback
lv_indev_t *mouse_indev; // will be polled by LVGL
/*
lv_indev_t *buttons_indev; // will be updated by buttonCallback
lv_indev_t *encoder_indev;
*/

X32_BTN lastButton = X32_BTN_NONE;
bool lastButtonPressed;

// sync mixer state to GUI
void guiSync(void) {
    if (mixerIsModelX32Core()){
        return;
    }

    x32debug("Active Page: %d\n", mixer.activePage);

    sChannel* pChannelSelected = mixerGetSelectedChannel();

    //####################################
    //#         General
    //####################################

    lv_color_t color;
    switch (pChannelSelected->color){
        case SURFACE_COLOR_BLACK:
            color = lv_color_make(0, 0, 0);
            break;
        case SURFACE_COLOR_RED:
            color = lv_color_make(255, 0, 0);
            break;
        case SURFACE_COLOR_GREEN:
            color = lv_color_make(0, 255, 0);
            break;
        case SURFACE_COLOR_YELLOW:
            color = lv_color_make(255, 255, 0);
            break;
        case SURFACE_COLOR_BLUE:
            color = lv_color_make(0, 0, 255);
            break;
        case SURFACE_COLOR_PINK:
            color = lv_color_make(255, 0, 255);
            break;
        case SURFACE_COLOR_CYAN:
            color = lv_color_make(0, 255, 255);
            break;
        case SURFACE_COLOR_WHITE:
            color = lv_color_make(255, 255, 255);
            break;
    }

    lv_label_set_text_fmt(objects.current_channel_number, "vCh%d", pChannelSelected->index);
    lv_label_set_text_fmt(objects.current_channel_name, "%s", pChannelSelected->name);
    lv_obj_set_style_bg_color(objects.current_channel_color, color, 0);

    // //set Encoders to default state
    // const char*  encoderTextMap[] = {"Input", " ", " "," "," ","Output", NULL};
    // lv_btnmatrix_set_map(objects.display_encoders, encoderTextMap);

    //####################################
    //#         Page Home
    //####################################



    bool phantomPower = halGetPhantomPower(pChannelSelected->index);
    
    if (mixer.activePage == X32_PAGE_CONFIG){
    //####################################
    //#         Page Config
    //####################################
        char dspSourceName[5] = "";
        char inputSourceName[10] = "";
        dspGetSourceName(&dspSourceName[0], pChannelSelected->index);
        sprintf(&inputSourceName[0], "%02d: %s", (pChannelSelected->index + 1), dspSourceName);
        lv_label_set_text_fmt(objects.current_channel_source, inputSourceName);

        lv_label_set_text_fmt(objects.current_channel_gain, "%f", (double)halGetGain(pChannelSelected->index));
        lv_label_set_text_fmt(objects.current_channel_phantom, "%d", phantomPower);
        lv_label_set_text_fmt(objects.current_channel_invert, "%d", halGetPhaseInvert(pChannelSelected->index));

        //char outputDestinationName[10] = "";
        //routingGetOutputName(&outputDestinationName[0], mixerGetSelectedChannel());
        //lv_label_set_text_fmt(objects.current_channel_destination, outputDestinationName);

        guiSetEncoderText("Source", "Turn: Gain\nPress: 48V", "-", "-", "-", "-");
    }else if (mixer.activePage == X32_PAGE_ROUTING) {
    //####################################
    //#         Page Routing
    //####################################
        char outputDestinationName[10] = "";
        char inputSourceName[10] = "";
        uint8_t routingIndex = 0;

        // read name of selected output-routing channel
        fpgaRoutingGetOutputNameByIndex(&outputDestinationName[0], mixer.selectedOutputChannelIndex); // mixer.selectedOutputChannelIndex = 1..112
        lv_label_set_text_fmt(objects.hardware_channel_output, outputDestinationName);

        // find name of currently set input-source
		routingIndex = fpgaRoutingGetOutputSourceByIndex(mixer.selectedOutputChannelIndex); // mixer.selectedOutputChannelIndex = 1..112
		fpgaRoutingGetSourceNameByIndex(&inputSourceName[0], routingIndex); // routingIndex = 0..112
        lv_label_set_text_fmt(objects.hardware_channel_source, inputSourceName);

        guiSetEncoderText("Output", "Source", "-", "-", "-", "-");
    }else if (mixer.activePage == X32_PAGE_EQ) {


        if (pChannelSelected->index < 40) {
            // support EQ-channel
            guiSetEncoderText("LowCut\n" + freq2String(mixer.dsp.dspChannel[pChannelSelected->index].lowCutFrequency),
                "Freq\n " + freq2String(mixer.dsp.dspChannel[pChannelSelected->index].peq[mixer.activeEQ].fc),
                "Gain\n" + String(mixer.dsp.dspChannel[pChannelSelected->index].peq[mixer.activeEQ].gain, 1) + " dB",
                "Q\n" + String(mixer.dsp.dspChannel[pChannelSelected->index].peq[mixer.activeEQ].Q, 1),
                "Mode\n" + eqType2String(mixer.dsp.dspChannel[pChannelSelected->index].peq[mixer.activeEQ].type),
                "Select\n" + String(mixer.activeEQ)
            );
        }else{
            // unsupported at the moment
            guiSetEncoderText("-", "-", "-", "-", "-", "-");
        }
    }else if (mixer.activePage == X32_PAGE_METERS) {
    //####################################
    //#         Page Meters
    //####################################

        for(int i=0; i<=15; i++){
            sChannel *chan = &mixer.channel[i];

            if (phantomPower){
                lv_buttonmatrix_set_button_ctrl(objects.phantomindicators, i, LV_BUTTONMATRIX_CTRL_CHECKED);
            } else {
                lv_buttonmatrix_clear_button_ctrl(objects.phantomindicators, i, LV_BUTTONMATRIX_CTRL_CHECKED);
            }

            switch (i){
                case 0:
                    lv_slider_set_value(objects.slider01, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 1:
                    lv_slider_set_value(objects.slider02, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 2:
                    lv_slider_set_value(objects.slider03, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 3:
                    lv_slider_set_value(objects.slider04, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 4:
                    lv_slider_set_value(objects.slider05, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 5:
                    lv_slider_set_value(objects.slider06, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 6:
                    lv_slider_set_value(objects.slider07, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 7:
                    lv_slider_set_value(objects.slider08, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 8:
                    lv_slider_set_value(objects.slider09, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 9:
                    lv_slider_set_value(objects.slider10, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 10:
                    lv_slider_set_value(objects.slider11, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 11:
                    lv_slider_set_value(objects.slider12, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 12:
                    lv_slider_set_value(objects.slider13, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 13:
                    lv_slider_set_value(objects.slider14, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 14:
                    lv_slider_set_value(objects.slider15, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
                case 15:
                    lv_slider_set_value(objects.slider16, dBfs2fader(mixer.dsp.dspChannel[chan->index].volumeLR), LV_ANIM_OFF);
                    break;
            }
        }

        lv_label_set_text_fmt(objects.volumes, "%2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB", 
            (double)mixer.dsp.dspChannel[0].volumeLR,
            (double)mixer.dsp.dspChannel[1].volumeLR,
            (double)mixer.dsp.dspChannel[2].volumeLR,
            (double)mixer.dsp.dspChannel[3].volumeLR,
            (double)mixer.dsp.dspChannel[4].volumeLR,
            (double)mixer.dsp.dspChannel[5].volumeLR,
            (double)mixer.dsp.dspChannel[6].volumeLR,
            (double)mixer.dsp.dspChannel[7].volumeLR
        );
    }else if (mixer.activePage == X32_PAGE_SETUP) {
    //####################################
    //#         Page Setup
    //####################################

        // pChannelSelected->solo ?
        //     lv_imagebutton_set_state(objects.setup_solo, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED):
        //     lv_imagebutton_set_state(objects.setup_solo, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED);

        // pChannelSelected->mute ?
        //     lv_imagebutton_set_state(objects.setup_mute, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED):
        //     lv_imagebutton_set_state(objects.setup_mute, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED);
    }else{
    //####################################
    //#         All other pages
    //####################################
        guiSetEncoderText("-", "-", "-", "-", "-", "-");
    }
}

void guiNewButtonPress(X32_BTN button, bool pressed) {
  lastButton = button;
  lastButtonPressed = pressed;

  // inform LVGL, that we received a new button
  lv_indev_read(keypad_indev);
}

void guiKeypadInputCallback(lv_indev_t* indev, lv_indev_data_t* data){
  data->key = lastButton;            // Get the last pressed or released key

  if (lastButtonPressed) {
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }

  lv_label_set_text_fmt(objects.testbartext, "Button %i pressed!", enum2button(lastButton));
}

// void guiMouseInputCallback(lv_indev_t* indev, lv_indev_data_t* data) {
// /*
//   bool left, middle, right;
//   int16_t x, y;
//   struct input_event ev;

//   int fd_mouse = open("/dev/input/event5", O_RDONLY | O_NONBLOCK);
//   uint8_t bytes = read(fd_mouse, &ev, sizeof(struct input_event));
//   close(fd_mouse);

//   switch (ev.type) {
//     case EV_REL: // Relativbewegung (Maus)
//       switch (ev.code) {
//         case REL_X:
//           if ((ev.value > 0) && (data->point.x < 800) && (ev.value < 800)) {
//             data->point.x += ev.value;
//           }else if ((ev.value < 0) && (data->point.x > 0) && (ev.value > -800)) {
//             data->point.x += ev.value;
//           }
//           break;
//         case REL_Y:
//           if ((ev.value > 0) && (data->point.y < 480) && (ev.value < 480)) {
//             data->point.y += ev.value;
//           }else if ((ev.value < 0) && (data->point.y > 0) && (ev.value > -480)) {
//             data->point.y += ev.value;
//           }
//           break;
//         case REL_WHEEL:
//           // ev.value
//           break;
//       }
//       break;
//     case EV_KEY:
//       left = (ev.code == BTN_LEFT);
//       right = (ev.code == BTN_RIGHT);
//       middle = (ev.code == BTN_MIDDLE);
//       break;
//     default:
//       break;
//   }

//   if (left) {
//     data->state = LV_INDEV_STATE_PRESSED;
//   } else {
//     data->state = LV_INDEV_STATE_RELEASED;
//   }
// */
//   if (lastButtonPressed) {
//     if (lastButton == X32_BTN_HOME) {
//       data->point.x = 50;
//       data->point.y = 15;
//       data->state = LV_INDEV_STATE_PRESSED;
//     }
//     if (lastButton == X32_BTN_METERS) {
//       data->point.x = 400;
//       data->point.y = 15;
//       data->state = LV_INDEV_STATE_PRESSED;
//     }
//     if (lastButton == X32_BTN_UTILITY) {
//       data->point.x = 750;
//       data->point.y = 15;
//       data->state = LV_INDEV_STATE_PRESSED;
//     }
//   }else{
//     data->state = LV_INDEV_STATE_RELEASED;
//   }

// //  printf("Mouse x = %i | y = %i", x, y);
// //  lv_label_set_text_fmt(objects.testbartext, "Mouse Type = %i | Code = %i!", ev.type, ev.code);
// }


// void guiButtonsInputCallback(lv_indev_t* indev, lv_indev_data_t* data) {
//   data->state = LV_INDEV_STATE_PRESSED;
//   data->state = LV_INDEV_STATE_RELEASED;
//   data->btn_id = last_btn;
// }

/*
void guiEncoderInputCallback(lv_indev_t* indev, lv_indev_data_t* data) {
  data->enc_diff = enc_get_new_moves();

  if(enc_pressed()) data->state = LV_INDEV_STATE_PRESSED;
  else data->state = LV_INDEV_STATE_RELEASED;
}
*/

void guiInitInput(void) {
  // initialize keypad
  keypad_indev = lv_indev_create();
  lv_indev_set_type(keypad_indev, LV_INDEV_TYPE_KEYPAD);
  lv_indev_set_read_cb(keypad_indev, guiKeypadInputCallback);
  lv_indev_set_mode(keypad_indev, LV_INDEV_MODE_EVENT); // set to event-driven mode (without would be polling-mode)

  // initialize mouse
  // mouse_indev = lv_indev_create();
  // lv_indev_set_type(mouse_indev, LV_INDEV_TYPE_POINTER);
  // lv_indev_set_read_cb(mouse_indev, guiMouseInputCallback);
//  lv_indev_set_driver(mouse_indev, &my_mouse_driver);
//  lv_indev_set_mode(mouse_indev, LV_INDEV_MODE_EVENT); // set to event-driven mode (without would be polling-mode)

/*
  // intialize buttons
  buttons_indev = lv_indev_create();
  lv_indev_set_type(buttons_indev, LV_INDEV_TYPE_BUTTON);
  lv_indev_set_read_cb(buttons_indev, guiButtonsInputCallback);
  lv_indev_set_mode(buttons_indev, LV_INDEV_MODE_EVENT); // set to event-driven mode (without would be polling-mode)

  // intialize encoder
  encoder_indev = lv_indev_create();
  lv_indev_set_type(encoder_indev, LV_INDEV_TYPE_ENCODER);
  lv_indev_set_read_cb(encoder_indev, guiEncoderInputCallback);
  lv_indev_set_mode(encoder_indev, LV_INDEV_MODE_EVENT); // set to event-driven mode (without would be polling-mode)
*/


/*
  // Create a cursor object (e.g., an image)
  lv_obj_t *cursor_obj = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(cursor_obj, &my_cursor_image);
  lv_indev_set_cursor(mouse_indev, cursor_obj);
*/

/*
  const char* device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
  lv_display_t *disp = lv_linux_fbdev_create();

  lv_linux_fbdev_set_file(disp, device);

  char* mousePath = lv_libinput_find_dev(LV_LIBINPUT_CAPABILITY_POINTER, true);
  lv_indev_t *mouse = lv_libinput_create(LV_INDEV_TYPE_POINTER, mousePath);

  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_display(mouse, disp);
  lv_display_set_default(disp);

  LV_IMAGE_DECLARE(mouse_cursor_icon);
  lv_obj_t *cursor_obj;
  cursor_obj = lv_image_create(lv_screen_active());
  lv_image_set_src(cursor_obj, &mouse_cursor_icon);
  lv_indev_set_cursor(mouse, cursor_obj);
*/
}

void guiInit(void) {
  lv_init();

  driver_backends_register();
  driver_backends_init_backend("FBDEV");

  lv_timer_create(timer10msCallbackLvgl, 10, NULL);
  lv_timer_create(timer100msCallbackLvgl, 100, NULL); // surface/gui sync

  // initialize GUI created by EEZ
  ui_init();

  guiInitInput();

  mixerShowPage((X32_PAGE)-1);   // shows the welcome page
  mixerSetChangeFlags(X32_MIXER_CHANGED_ALL); // trigger first sync to gui/surface

  // start endless loop
  driver_backends_run_loop();
}

void guiSetEncoderText(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6) {
    const char* encoderTextMap[] = {
        mixer.encoderText[0],
        mixer.encoderText[1],
        mixer.encoderText[2],
        mixer.encoderText[3],
        mixer.encoderText[4],
        mixer.encoderText[5],
        NULL
    };

    sprintf(&mixer.encoderText[0][0], "%s", enc1.c_str());
    sprintf(&mixer.encoderText[1][0], "%s", enc2.c_str());
    sprintf(&mixer.encoderText[2][0], "%s", enc3.c_str());
    sprintf(&mixer.encoderText[3][0], "%s", enc4.c_str());
    sprintf(&mixer.encoderText[4][0], "%s", enc5.c_str());
    sprintf(&mixer.encoderText[5][0], "%s", enc6.c_str());

    lv_btnmatrix_set_map(objects.display_encoders, encoderTextMap);
}