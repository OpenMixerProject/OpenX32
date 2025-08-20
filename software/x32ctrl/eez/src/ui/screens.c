#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    {
        lv_obj_t *parent_obj = obj;
        {
            // maintab
            lv_obj_t *obj = lv_tabview_create(parent_obj);
            objects.maintab = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
            lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
            lv_tabview_set_tab_bar_size(obj, 32);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Home");
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // openx32logo
                            lv_obj_t *obj = lv_image_create(parent_obj);
                            objects.openx32logo = obj;
                            lv_obj_set_pos(obj, -118, -5);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_image_set_src(obj, &img_openx32logo);
                            lv_image_set_scale(obj, 177);
                        }
                        {
                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                            lv_obj_set_pos(obj, 35, 127);
                            lv_obj_set_size(obj, 691, 254);
                            lv_textarea_set_max_length(obj, 255);
                            lv_textarea_set_text(obj, "Currently (rudimentary) implemented pages:\n- HOME\n- METERS\n- UTILITY (for Development)\n\n\n\n-> Press the EFFECTS button to get a nice demo <-");
                            lv_textarea_set_one_line(obj, false);
                            lv_textarea_set_password_mode(obj, false);
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Meters");
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // slider01
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.slider01 = obj;
                            lv_obj_set_pos(obj, 14, 279);
                            lv_obj_set_size(obj, 11, 111);
                            lv_slider_set_range(obj, 0, 4095);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                        }
                        {
                            // slider02
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.slider02 = obj;
                            lv_obj_set_pos(obj, 51, 280);
                            lv_obj_set_size(obj, 11, 111);
                            lv_slider_set_range(obj, 0, 4095);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                        }
                        {
                            // slider03
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.slider03 = obj;
                            lv_obj_set_pos(obj, 91, 280);
                            lv_obj_set_size(obj, 11, 111);
                            lv_slider_set_range(obj, 0, 4095);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                        }
                        {
                            // slider04
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.slider04 = obj;
                            lv_obj_set_pos(obj, 131, 280);
                            lv_obj_set_size(obj, 11, 111);
                            lv_slider_set_range(obj, 0, 4095);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                        }
                        {
                            // slider05
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.slider05 = obj;
                            lv_obj_set_pos(obj, 172, 280);
                            lv_obj_set_size(obj, 11, 111);
                            lv_slider_set_range(obj, 0, 4095);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                        }
                        {
                            // slider06
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.slider06 = obj;
                            lv_obj_set_pos(obj, 211, 280);
                            lv_obj_set_size(obj, 11, 111);
                            lv_slider_set_range(obj, 0, 4095);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                        }
                        {
                            // slider07
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.slider07 = obj;
                            lv_obj_set_pos(obj, 252, 280);
                            lv_obj_set_size(obj, 11, 111);
                            lv_slider_set_range(obj, 0, 4095);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                        }
                        {
                            // slider08
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.slider08 = obj;
                            lv_obj_set_pos(obj, 293, 279);
                            lv_obj_set_size(obj, 11, 111);
                            lv_slider_set_range(obj, 0, 4095);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                        }
                        {
                            // phantomindicators
                            lv_obj_t *obj = lv_buttonmatrix_create(parent_obj);
                            objects.phantomindicators = obj;
                            lv_obj_set_pos(obj, -15, 98);
                            lv_obj_set_size(obj, 350, 82);
                            static const char *map[8] = {
                                "48V",
                                "48V",
                                "48V",
                                "48V",
                                "48V",
                                "48V",
                                "48V",
                                NULL,
                            };
                            static lv_buttonmatrix_ctrl_t ctrl_map[7] = {
                                1 | LV_BUTTONMATRIX_CTRL_CHECKABLE,
                                1 | LV_BUTTONMATRIX_CTRL_CHECKABLE,
                                1 | LV_BUTTONMATRIX_CTRL_CHECKABLE,
                                1 | LV_BUTTONMATRIX_CTRL_CHECKABLE,
                                1 | LV_BUTTONMATRIX_CTRL_CHECKABLE,
                                1 | LV_BUTTONMATRIX_CTRL_CHECKABLE,
                                1 | LV_BUTTONMATRIX_CTRL_CHECKABLE,
                            };
                            lv_buttonmatrix_set_map(obj, map);
                            lv_buttonmatrix_set_ctrl_map(obj, ctrl_map);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 14, 403);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "1        2        3        4        5        6        7        8 ");
                        }
                        {
                            // volumes
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.volumes = obj;
                            lv_obj_set_pos(obj, -9, 216);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Text");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Utility");
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // testbar
                            lv_obj_t *obj = lv_bar_create(parent_obj);
                            objects.testbar = obj;
                            lv_obj_set_pos(obj, 35, 188);
                            lv_obj_set_size(obj, 691, 33);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // testbartext
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.testbartext = obj;
                                    lv_obj_set_pos(obj, 329, 4);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "0%");
                                }
                            }
                        }
                        {
                            // debugtext
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.debugtext = obj;
                            lv_obj_set_pos(obj, 55, 297);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "...");
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}
