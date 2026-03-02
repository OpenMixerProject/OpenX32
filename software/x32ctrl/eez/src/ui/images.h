#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_openx32logo;
extern const lv_img_dsc_t img_meterbridge;
extern const lv_img_dsc_t img_knob;
extern const lv_img_dsc_t img_knob0;
extern const lv_img_dsc_t img_knob1;
extern const lv_img_dsc_t img_knob2;
extern const lv_img_dsc_t img_knob3;
extern const lv_img_dsc_t img_knob4;
extern const lv_img_dsc_t img_knob5;
extern const lv_img_dsc_t img_knob6;
extern const lv_img_dsc_t img_knob7;
extern const lv_img_dsc_t img_logo;
extern const lv_img_dsc_t img_tux;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[13];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/