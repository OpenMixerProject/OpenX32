#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: tabview_nopad
lv_style_t *get_style_tabview_nopad_MAIN_DEFAULT();
void add_style_tabview_nopad(lv_obj_t *obj);
void remove_style_tabview_nopad(lv_obj_t *obj);

// Style: tab_nopad
lv_style_t *get_style_tab_nopad_MAIN_DEFAULT();
void add_style_tab_nopad(lv_obj_t *obj);
void remove_style_tab_nopad(lv_obj_t *obj);

// Style: slider_fader
lv_style_t *get_style_slider_fader_KNOB_DEFAULT();
lv_style_t *get_style_slider_fader_INDICATOR_DEFAULT();
lv_style_t *get_style_slider_fader_MAIN_DEFAULT();
void add_style_slider_fader(lv_obj_t *obj);
void remove_style_slider_fader(lv_obj_t *obj);

// Style: slider_fader_solo
lv_style_t *get_style_slider_fader_solo_MAIN_DEFAULT();
lv_style_t *get_style_slider_fader_solo_INDICATOR_DEFAULT();
lv_style_t *get_style_slider_fader_solo_KNOB_DEFAULT();
void add_style_slider_fader_solo(lv_obj_t *obj);
void remove_style_slider_fader_solo(lv_obj_t *obj);

// Style: slider_fader_mute
lv_style_t *get_style_slider_fader_mute_KNOB_DEFAULT();
lv_style_t *get_style_slider_fader_mute_INDICATOR_DEFAULT();
lv_style_t *get_style_slider_fader_mute_MAIN_DEFAULT();
void add_style_slider_fader_mute(lv_obj_t *obj);
void remove_style_slider_fader_mute(lv_obj_t *obj);

// Style: slider_config
lv_style_t *get_style_slider_config_KNOB_DEFAULT();
lv_style_t *get_style_slider_config_INDICATOR_DEFAULT();
lv_style_t *get_style_slider_config_MAIN_DEFAULT();
void add_style_slider_config(lv_obj_t *obj);
void remove_style_slider_config(lv_obj_t *obj);

// Style: panel_flat_black
lv_style_t *get_style_panel_flat_black_MAIN_DEFAULT();
void add_style_panel_flat_black(lv_obj_t *obj);
void remove_style_panel_flat_black(lv_obj_t *obj);

// Style: label_bg_yellow
lv_style_t *get_style_label_bg_yellow_MAIN_DEFAULT();
void add_style_label_bg_yellow(lv_obj_t *obj);
void remove_style_label_bg_yellow(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/