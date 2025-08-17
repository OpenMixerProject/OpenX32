#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *maintab;
    lv_obj_t *openx32logo;
    lv_obj_t *slider01;
    lv_obj_t *slider02;
    lv_obj_t *slider03;
    lv_obj_t *slider04;
    lv_obj_t *slider05;
    lv_obj_t *slider06;
    lv_obj_t *slider07;
    lv_obj_t *slider08;
    lv_obj_t *phantomindicators;
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