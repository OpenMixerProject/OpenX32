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



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/