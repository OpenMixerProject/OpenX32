#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *channel_box;
    lv_obj_t *current_channel_number;
    lv_obj_t *current_channel_name;
    lv_obj_t *current_channel_color;
    lv_obj_t *openx32logo_1;
    lv_obj_t *display_encoders;
    lv_obj_t *maintab;
    lv_obj_t *hometab;
    lv_obj_t *configtab;
    lv_obj_t *routingtab;
    lv_obj_t *source_panel;
    lv_obj_t *current_channel_source;
    lv_obj_t *current_channel_gain;
    lv_obj_t *current_channel_phantom;
    lv_obj_t *current_channel_invert;
    lv_obj_t *current_channel_pan_bal;
    lv_obj_t *current_channel_volume;
    lv_obj_t *eqtab;
    lv_obj_t *current_channel_eq;
    lv_obj_t *slider01;
    lv_obj_t *slider02;
    lv_obj_t *slider03;
    lv_obj_t *slider04;
    lv_obj_t *slider05;
    lv_obj_t *slider06;
    lv_obj_t *slider07;
    lv_obj_t *slider08;
    lv_obj_t *slider09;
    lv_obj_t *slider10;
    lv_obj_t *slider11;
    lv_obj_t *slider12;
    lv_obj_t *slider13;
    lv_obj_t *slider14;
    lv_obj_t *slider15;
    lv_obj_t *slider16;
    lv_obj_t *phantomindicators;
    lv_obj_t *volumes;
    lv_obj_t *hw_inputs;
    lv_obj_t *hw_outputs;
    lv_obj_t *fpga;
    lv_obj_t *mixer;
    lv_obj_t *routing_input;
    lv_obj_t *routing_output;
    lv_obj_t *routing_hw_output;
    lv_obj_t *dsp;
    lv_obj_t *routing_dsp;
    lv_obj_t *routing_dsp_return;
    lv_obj_t *hardware_channel_output;
    lv_obj_t *hardware_channel_source;
    lv_obj_t *testbar;
    lv_obj_t *testbartext;
    lv_obj_t *debugtext;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/
