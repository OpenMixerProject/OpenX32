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
    lv_obj_t *maintab;
    lv_obj_t *hometab;
    lv_obj_t *configtab;
    lv_obj_t *source_panel;
    lv_obj_t *current_channel_source;
    lv_obj_t *current_channel_gain;
    lv_obj_t *current_channel_phantom;
    lv_obj_t *current_channel_invert;
    lv_obj_t *current_channel_pan_bal;
    lv_obj_t *current_channel_volume;
    lv_obj_t *current_channel_gate;
    lv_obj_t *current_channel_comp;
    lv_obj_t *eqtab;
    lv_obj_t *current_channel_eq;
    lv_obj_t *metertab;
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
    lv_obj_t *slider17;
    lv_obj_t *slider18;
    lv_obj_t *slider19;
    lv_obj_t *slider20;
    lv_obj_t *slider21;
    lv_obj_t *slider22;
    lv_obj_t *slider23;
    lv_obj_t *slider24;
    lv_obj_t *slider25;
    lv_obj_t *slider26;
    lv_obj_t *slider27;
    lv_obj_t *slider28;
    lv_obj_t *slider29;
    lv_obj_t *slider30;
    lv_obj_t *slider31;
    lv_obj_t *slider32;
    lv_obj_t *meter_aux;
    lv_obj_t *meter_aux__slider01;
    lv_obj_t *meter_aux__slider02;
    lv_obj_t *meter_aux__slider03;
    lv_obj_t *meter_aux__slider04;
    lv_obj_t *meter_aux__slider05;
    lv_obj_t *meter_aux__slider06;
    lv_obj_t *meter_aux__slider07;
    lv_obj_t *meter_aux__slider08;
    lv_obj_t *meter_fxret;
    lv_obj_t *meter_fxret__slider01;
    lv_obj_t *meter_fxret__slider02;
    lv_obj_t *meter_fxret__slider03;
    lv_obj_t *meter_fxret__slider04;
    lv_obj_t *meter_fxret__slider05;
    lv_obj_t *meter_fxret__slider06;
    lv_obj_t *meter_fxret__slider07;
    lv_obj_t *meter_fxret__slider08;
    lv_obj_t *meter_bus18;
    lv_obj_t *meter_bus18__slider01;
    lv_obj_t *meter_bus18__slider02;
    lv_obj_t *meter_bus18__slider03;
    lv_obj_t *meter_bus18__slider04;
    lv_obj_t *meter_bus18__slider05;
    lv_obj_t *meter_bus18__slider06;
    lv_obj_t *meter_bus18__slider07;
    lv_obj_t *meter_bus18__slider08;
    lv_obj_t *meter_bux916;
    lv_obj_t *meter_bux916__slider01;
    lv_obj_t *meter_bux916__slider02;
    lv_obj_t *meter_bux916__slider03;
    lv_obj_t *meter_bux916__slider04;
    lv_obj_t *meter_bux916__slider05;
    lv_obj_t *meter_bux916__slider06;
    lv_obj_t *meter_bux916__slider07;
    lv_obj_t *meter_bux916__slider08;
    lv_obj_t *led_xlr01_phantom;
    lv_obj_t *led_xlr01_invert;
    lv_obj_t *led_xlr01_input_level;
    lv_obj_t *led_ch01_input;
    lv_obj_t *led_ch01_output;
    lv_obj_t *led_main_l;
    lv_obj_t *led_main_r;
    lv_obj_t *routingtab;
    lv_obj_t *hw_inputs;
    lv_obj_t *hw_outputs;
    lv_obj_t *fpga;
    lv_obj_t *mixer;
    lv_obj_t *routing_input;
    lv_obj_t *routing_input_1;
    lv_obj_t *dsp;
    lv_obj_t *table_routing_fpga;
    lv_obj_t *table_routing_dsp_input;
    lv_obj_t *table_routing_dsp_output;
    lv_obj_t *testbar;
    lv_obj_t *testbartext;
    lv_obj_t *debugtext;
    lv_obj_t *display_encoders;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void create_user_widget_8meters(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_8meters(int startWidgetIndex);

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/