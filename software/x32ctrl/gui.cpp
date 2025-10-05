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

lv_indev_t *keypad_indev; // will be updated by buttonCallback
lv_indev_t *mouse_indev; // will be polled by LVGL
/*
lv_indev_t *buttons_indev; // will be updated by buttonCallback
lv_indev_t *encoder_indev;
*/

char displayEncoderText[6][30];
lv_chart_series_t* chartSeriesEQ;

X32_BTN lastButton = X32_BTN_NONE;
bool lastButtonPressed;

static const char* displayEncoderButtonMap[] = {
    displayEncoderText[0],
    displayEncoderText[1],
    displayEncoderText[2],
    displayEncoderText[3],
    displayEncoderText[4],
    displayEncoderText[5],
    NULL};



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

  // TODO Implement correctly
  throw;
  //mixer->ShowPage((X32_PAGE)-1);   // shows the welcome page
  //mixer->SetChangeFlags(X32_MIXER_CHANGED_ALL); // trigger first sync to gui/surface

  // install event-handler for buttonmatrix (for mouse-control)
  lv_obj_add_event_cb(objects.display_encoders, guiDisplayEncoderEventHandler, LV_EVENT_ALL, NULL);

  // setup EQ-graph
  lv_chart_set_type(objects.current_channel_eq, LV_CHART_TYPE_LINE);
  lv_obj_set_style_size(objects.current_channel_eq, 0, 0, LV_PART_INDICATOR);
  lv_obj_set_style_line_width(objects.current_channel_eq, 5, LV_PART_ITEMS);
  // TODO Implement correctly
  throw;
  //mixer->chartSeriesEQ = lv_chart_add_series(objects.current_channel_eq, lv_palette_main(LV_PALETTE_AMBER), LV_CHART_AXIS_PRIMARY_Y);
  //lv_chart_set_div_line_count(objects.current_channel_eq, 5, 7);
  lv_chart_set_range(objects.current_channel_eq, LV_CHART_AXIS_PRIMARY_Y, -15000, 15000);
  lv_chart_set_point_count(objects.current_channel_eq, 200);
  // TODO Implement correctly
  throw;
  //lv_chart_set_series_color(objects.current_channel_eq, mixer->chartSeriesEQ, lv_color_hex(0xef7900));
  // chart-shadow: 0x7e4000

  // start endless loop
  driver_backends_run_loop();
}

void guiSetEncoderText(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6) {
    sprintf(&displayEncoderText[0][0], "%s", enc1.c_str());
    sprintf(&displayEncoderText[1][0], "%s", enc2.c_str());
    sprintf(&displayEncoderText[2][0], "%s", enc3.c_str());
    sprintf(&displayEncoderText[3][0], "%s", enc4.c_str());
    sprintf(&displayEncoderText[4][0], "%s", enc5.c_str());
    sprintf(&displayEncoderText[5][0], "%s", enc6.c_str());
    lv_btnmatrix_set_map(objects.display_encoders, displayEncoderButtonMap);
}

static void guiDisplayEncoderEventHandler(lv_event_t* e) {
/*
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);

    if (code == LV_EVENT_CLICKED) {
        const char* txt = lv_btnmatrix_get_btn_text(obj, id);
        x32debug("Button %s was clicked", txt);
    }

    if (code == LV_EVENT_PRESSED) {
        
    }

    if (code == LV_EVENT_DRAW_POST_END) {
        lv_draw_label_dsc_t labelDsc;
        lv_draw_label_dsc_init(&labelDsc);

        labelDsc.color = lv_color_hex(0xFFFFFF);
        switch (id) {
            case 0:
                labelDsc.text = "ENC1";
                break;
            case 1:
                labelDsc.text = "ENC2";
                break;
            case 2:
                labelDsc.text = "ENC3";
                break;
            case 3:
                labelDsc.text = "ENC4";
                break;
            case 4:
                labelDsc.text = "ENC5";
                break;
            case 5:
                labelDsc.text = "ENC6";
                break;
        }

        lv_area_t a = {10, 10, 200, 50}; // Draw label here
        lv_draw_label(lv_event_get_layer(e), &labelDsc, &a);
    }
*/
}

void guiDrawEq(uint8_t selectedChannelIndex) {
    if (selectedChannelIndex >= 40) {
        return;
    }

    // calculate the filter-response between 20 Hz and 20 kHz for all 4 PEQs
    sPEQ* peq;
    float eqValue[200];
    float freq;

    memset(&eqValue[0], 0, sizeof(eqValue));

    for (uint16_t pixel = 0; pixel < 200; pixel++) {
        freq = 20.0f * powf(1000.0f, ((float)pixel/199.0f));

        // LowCut
        // TODO Implement correctly
        throw;
        //eqValue[pixel] += fxCalcFrequencyResponse_LC(freq, mixer->dsp.dspChannel[selectedChannelIndex].lowCutFrequency, mixer->dsp.samplerate);

        // PEQ
        for (uint8_t i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++) {
            // TODO Implement correctly
            throw;
          //peq = &mixer->dsp.dspChannel[mixer->GetSelectedvChannelIndex()].peq[i_peq];
          //  eqValue[pixel] += fxCalcFrequencyResponse_PEQ(peq->a[0], peq->a[1], peq->a[2], peq->b[1], peq->b[2], freq, mixer->dsp.samplerate);
        }
    }

    // TODO Implement correctly
    throw;
    //int32_t* chartSeriesEqPoints = lv_chart_get_series_y_array(objects.current_channel_eq, mixer->chartSeriesEQ);
    //for (uint16_t i = 0; i < 200; i++) {
    //    chartSeriesEqPoints[i] = eqValue[i] * 1000.0f; // draw diagonal line
    //}
    //lv_chart_refresh(objects.current_channel_eq);
}