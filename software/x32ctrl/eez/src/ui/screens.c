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
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff5188db), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 3, 3);
            lv_obj_set_size(obj, 94, 72);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // current_channel_number
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.current_channel_number = obj;
                    lv_obj_set_pos(obj, -19, 15);
                    lv_obj_set_size(obj, 80, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "CH32");
                }
                {
                    // current_channel_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.current_channel_name = obj;
                    lv_obj_set_pos(obj, -19, 33);
                    lv_obj_set_size(obj, 80, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Kanal 32");
                }
                {
                    // current_channel_color
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.current_channel_color = obj;
                    lv_obj_set_pos(obj, 61, -22);
                    lv_obj_set_size(obj, 11, 72);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffafafa), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // display_encoders
            lv_obj_t *obj = lv_buttonmatrix_create(parent_obj);
            objects.display_encoders = obj;
            lv_obj_set_pos(obj, 2, 443);
            lv_obj_set_size(obj, 796, 35);
            static const char *map[7] = {
                "-",
                "-",
                "-",
                "-",
                "-",
                "-",
                NULL,
            };
            lv_buttonmatrix_set_map(obj, map);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // maintab
            lv_obj_t *obj = lv_tabview_create(parent_obj);
            objects.maintab = obj;
            lv_obj_set_pos(obj, 2, 78);
            lv_obj_set_size(obj, 796, 363);
            lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
            lv_tabview_set_tab_bar_size(obj, 0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Welcome");
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
                            lv_obj_set_size(obj, 691, 217);
                            lv_textarea_set_max_length(obj, 255);
                            lv_textarea_set_text(obj, "Currently (rudimentary) implemented pages:\n- HOME\n- METERS\n- SETUP\n- UTILITY (for Development)\n\n\n\n-> Press the EFFECTS button to get a nice demo <-");
                            lv_textarea_set_one_line(obj, false);
                            lv_textarea_set_password_mode(obj, false);
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Home");
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hometab
                            lv_obj_t *obj = lv_tabview_create(parent_obj);
                            objects.hometab = obj;
                            lv_obj_set_pos(obj, -21, -20);
                            lv_obj_set_size(obj, 796, 399);
                            lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
                            lv_tabview_set_tab_bar_size(obj, 20);
                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Home");
                                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                }
                                {
                                    // configtab
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Config");
                                    objects.configtab = obj;
                                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // source_panel
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.source_panel = obj;
                                            lv_obj_set_pos(obj, -16, 142);
                                            lv_obj_set_size(obj, 499, 181);
                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, -11, -9);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "DSP-Channel Configuration");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, -11, 25);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "Source: ");
                                                }
                                                {
                                                    // current_channel_source
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.current_channel_source = obj;
                                                    lv_obj_set_pos(obj, 59, 25);
                                                    lv_obj_set_size(obj, 100, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "x32");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 192, 26);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "Gain:");
                                                }
                                                {
                                                    // current_channel_gain
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.current_channel_gain = obj;
                                                    lv_obj_set_pos(obj, 273, 25);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "0 dBfs");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 192, 51);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "48V:");
                                                }
                                                {
                                                    // current_channel_phantom
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.current_channel_phantom = obj;
                                                    lv_obj_set_pos(obj, 273, 50);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "0");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 192, 76);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "Invert:");
                                                }
                                                {
                                                    // current_channel_invert
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.current_channel_invert = obj;
                                                    lv_obj_set_pos(obj, 273, 75);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "0");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 192, 101);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "PAN / BAL:");
                                                }
                                                {
                                                    // current_channel_pan_bal
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.current_channel_pan_bal = obj;
                                                    lv_obj_set_pos(obj, 273, 101);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "0");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 192, 124);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "Volume:");
                                                }
                                                {
                                                    // current_channel_volume
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.current_channel_volume = obj;
                                                    lv_obj_set_pos(obj, 273, 124);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "0");
                                                }
                                            }
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Gate");
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Dyn");
                                }
                                {
                                    // eqtab
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "EQ");
                                    objects.eqtab = obj;
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // current_channel_eq
                                            lv_obj_t *obj = lv_chart_create(parent_obj);
                                            objects.current_channel_eq = obj;
                                            lv_obj_set_pos(obj, -18, -20);
                                            lv_obj_set_size(obj, 794, 343);
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Sends");
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Main");
                                }
                            }
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Meters");
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_tabview_create(parent_obj);
                            lv_obj_set_pos(obj, -21, -20);
                            lv_obj_set_size(obj, 796, 399);
                            lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
                            lv_tabview_set_tab_bar_size(obj, 20);
                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Channel");
                                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // slider01
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider01 = obj;
                                            lv_obj_set_pos(obj, -5, 150);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider02
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider02 = obj;
                                            lv_obj_set_pos(obj, 32, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider03
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider03 = obj;
                                            lv_obj_set_pos(obj, 72, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider04
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider04 = obj;
                                            lv_obj_set_pos(obj, 112, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider05
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider05 = obj;
                                            lv_obj_set_pos(obj, 153, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider06
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider06 = obj;
                                            lv_obj_set_pos(obj, 192, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider07
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider07 = obj;
                                            lv_obj_set_pos(obj, 233, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider08
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider08 = obj;
                                            lv_obj_set_pos(obj, 274, 150);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider09
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider09 = obj;
                                            lv_obj_set_pos(obj, 316, 150);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider10
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider10 = obj;
                                            lv_obj_set_pos(obj, 356, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider11
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider11 = obj;
                                            lv_obj_set_pos(obj, 397, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider12
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider12 = obj;
                                            lv_obj_set_pos(obj, 438, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider13
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider13 = obj;
                                            lv_obj_set_pos(obj, 478, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider14
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider14 = obj;
                                            lv_obj_set_pos(obj, 519, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider15
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider15 = obj;
                                            lv_obj_set_pos(obj, 565, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider16
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider16 = obj;
                                            lv_obj_set_pos(obj, 606, 151);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, 0, 4095);
                                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                        }
                                        {
                                            // phantomindicators
                                            lv_obj_t *obj = lv_buttonmatrix_create(parent_obj);
                                            objects.phantomindicators = obj;
                                            lv_obj_set_pos(obj, -15, 20);
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
                                            lv_obj_set_pos(obj, -6, 288);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "1        2        3        4        5        6        7        8 ");
                                        }
                                        {
                                            // volumes
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.volumes = obj;
                                            lv_obj_set_pos(obj, -12, 115);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "Text");
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Mix Bus");
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "AUF/FX");
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "IN/OUT");
                                }
                            }
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Routing");
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 349, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Routing");
                        }
                        {
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            lv_obj_set_pos(obj, -7, 217);
                            lv_obj_set_size(obj, 261, 107);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 138, 19);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_text(obj, "Source:");
                                }
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 16, 19);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_text(obj, "Output:");
                                }
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, -6, -13);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_text(obj, "Hardware Channel-Routing");
                                }
                                {
                                    // hardware_channel_output
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.hardware_channel_output = obj;
                                    lv_obj_set_pos(obj, -6, 45);
                                    lv_obj_set_size(obj, 100, LV_SIZE_CONTENT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "X32");
                                }
                                {
                                    // hardware_channel_source
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.hardware_channel_source = obj;
                                    lv_obj_set_pos(obj, 115, 45);
                                    lv_obj_set_size(obj, 100, LV_SIZE_CONTENT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "X32");
                                }
                            }
                        }
                        {
                            lv_obj_t *obj = lv_table_create(parent_obj);
                            lv_obj_set_pos(obj, 377, 217);
                            lv_obj_set_size(obj, 381, 107);
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Setup");
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 15, 20);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Setup");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Library");
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 15, 20);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Library");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Effects");
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 15, 20);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Effects");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Mute Group");
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 15, 20);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Mute Groups");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Utility");
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 15, 20);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Utility");
                        }
                        {
                            // testbar
                            lv_obj_t *obj = lv_bar_create(parent_obj);
                            objects.testbar = obj;
                            lv_obj_set_pos(obj, 3, 61);
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
                            lv_obj_set_pos(obj, 3, 128);
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
