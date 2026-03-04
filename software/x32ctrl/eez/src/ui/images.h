#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_openx32logo;
extern const lv_img_dsc_t img_meterbridge;
extern const lv_img_dsc_t img_knob;
extern const lv_img_dsc_t img_logo;
extern const lv_img_dsc_t img_tux;
extern const lv_img_dsc_t img_buttononoff;
extern const lv_img_dsc_t img_checkbox;
extern const lv_img_dsc_t img_pause;
extern const lv_img_dsc_t img_play;
extern const lv_img_dsc_t img_playpause;
extern const lv_img_dsc_t img_record;
extern const lv_img_dsc_t img_sdcard;
extern const lv_img_dsc_t img_stop;
extern const lv_img_dsc_t img_select;
extern const lv_img_dsc_t img_sdusb;
extern const lv_img_dsc_t img_vumeter_colored;
extern const lv_img_dsc_t img_openx32_knob;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[17];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/