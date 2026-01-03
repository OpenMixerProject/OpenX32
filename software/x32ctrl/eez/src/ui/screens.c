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
            // channel_box
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.channel_box = obj;
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
            // openx32logo_1
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.openx32logo_1 = obj;
            lv_obj_set_pos(obj, 196, 13);
            lv_obj_set_size(obj, 395, 52);
            lv_image_set_src(obj, &img_openx32logo);
            lv_image_set_scale(obj, 100);
        }
        {
            // maintab
            lv_obj_t *obj = lv_tabview_create(parent_obj);
            objects.maintab = obj;
            lv_obj_set_pos(obj, LV_PCT(0), 78);
            lv_obj_set_size(obj, LV_PCT(100), 402);
            lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
            lv_tabview_set_tab_bar_size(obj, 0);
            add_style_tabview_nopad(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Home");
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    add_style_tab_nopad(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hometab
                            lv_obj_t *obj = lv_tabview_create(parent_obj);
                            objects.hometab = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
                            lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
                            lv_tabview_set_tab_bar_size(obj, 20);
                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                            add_style_tabview_nopad(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Home");
                                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                    add_style_tab_nopad(obj);
                                }
                                {
                                    // configtab
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Config");
                                    objects.configtab = obj;
                                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // source_panel
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.source_panel = obj;
                                            lv_obj_set_pos(obj, 12, 35);
                                            lv_obj_set_size(obj, 499, 190);
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
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // current_channel_gate
                                            lv_obj_t *obj = lv_chart_create(parent_obj);
                                            objects.current_channel_gate = obj;
                                            lv_obj_set_pos(obj, 25, 25);
                                            lv_obj_set_size(obj, 200, 200);
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Dyn");
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // current_channel_comp
                                            lv_obj_t *obj = lv_chart_create(parent_obj);
                                            objects.current_channel_comp = obj;
                                            lv_obj_set_pos(obj, 25, 25);
                                            lv_obj_set_size(obj, 200, 200);
                                        }
                                    }
                                }
                                {
                                    // eqtab
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "EQ");
                                    objects.eqtab = obj;
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // current_channel_eq
                                            lv_obj_t *obj = lv_chart_create(parent_obj);
                                            objects.current_channel_eq = obj;
                                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                                            lv_obj_set_size(obj, LV_PCT(100), 322);
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Sends");
                                    add_style_tab_nopad(obj);
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Main");
                                    add_style_tab_nopad(obj);
                                }
                            }
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Meters");
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    add_style_tab_nopad(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // metertab
                            lv_obj_t *obj = lv_tabview_create(parent_obj);
                            objects.metertab = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
                            lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
                            lv_tabview_set_tab_bar_size(obj, 20);
                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                            add_style_tabview_nopad(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Meters");
                                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // slider01
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider01 = obj;
                                            lv_obj_set_pos(obj, 15, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider02
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider02 = obj;
                                            lv_obj_set_pos(obj, 38, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider03
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider03 = obj;
                                            lv_obj_set_pos(obj, 61, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider04
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider04 = obj;
                                            lv_obj_set_pos(obj, 84, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider05
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider05 = obj;
                                            lv_obj_set_pos(obj, 107, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider06
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider06 = obj;
                                            lv_obj_set_pos(obj, 130, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider07
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider07 = obj;
                                            lv_obj_set_pos(obj, 153, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider08
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider08 = obj;
                                            lv_obj_set_pos(obj, 176, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider09
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider09 = obj;
                                            lv_obj_set_pos(obj, 206, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider10
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider10 = obj;
                                            lv_obj_set_pos(obj, 229, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider11
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider11 = obj;
                                            lv_obj_set_pos(obj, 252, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider12
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider12 = obj;
                                            lv_obj_set_pos(obj, 275, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider13
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider13 = obj;
                                            lv_obj_set_pos(obj, 298, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider14
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider14 = obj;
                                            lv_obj_set_pos(obj, 321, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider15
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider15 = obj;
                                            lv_obj_set_pos(obj, 344, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider16
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider16 = obj;
                                            lv_obj_set_pos(obj, 367, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider17
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider17 = obj;
                                            lv_obj_set_pos(obj, 418, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider18
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider18 = obj;
                                            lv_obj_set_pos(obj, 441, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider19
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider19 = obj;
                                            lv_obj_set_pos(obj, 464, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider20
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider20 = obj;
                                            lv_obj_set_pos(obj, 487, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider21
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider21 = obj;
                                            lv_obj_set_pos(obj, 510, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider22
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider22 = obj;
                                            lv_obj_set_pos(obj, 533, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider23
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider23 = obj;
                                            lv_obj_set_pos(obj, 556, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider24
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider24 = obj;
                                            lv_obj_set_pos(obj, 579, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider25
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider25 = obj;
                                            lv_obj_set_pos(obj, 609, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider26
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider26 = obj;
                                            lv_obj_set_pos(obj, 632, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider27
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider27 = obj;
                                            lv_obj_set_pos(obj, 655, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider28
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider28 = obj;
                                            lv_obj_set_pos(obj, 678, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider29
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider29 = obj;
                                            lv_obj_set_pos(obj, 701, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider30
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider30 = obj;
                                            lv_obj_set_pos(obj, 724, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider31
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider31 = obj;
                                            lv_obj_set_pos(obj, 747, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            // slider32
                                            lv_obj_t *obj = lv_slider_create(parent_obj);
                                            objects.slider32 = obj;
                                            lv_obj_set_pos(obj, 770, 27);
                                            lv_obj_set_size(obj, 11, 111);
                                            lv_slider_set_range(obj, -60, 0);
                                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 18, 150);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "1    2    3   4    5   6    7    8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 206, 150);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "9   10   11   12  13  14   15  16");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 606, 150);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "25  26 27  28  29 30  31  32");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 418, 150);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "17  18  19  20  21  22  23  24");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 386, 2);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "dB\n0\n-8\n-16\n-25\n-34\n-42\n-51\n-60");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 168, 169);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "Channel");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 571, 169);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "Channel");
                                        }
                                        {
                                            // meter_aux
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meter_aux = obj;
                                            lv_obj_set_pos(obj, 10, 204);
                                            lv_obj_set_size(obj, 182, 132);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_8meters(obj, 54);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 17, 337);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "1    2    3   4    5   6    7    8");
                                        }
                                        {
                                            // meter_fxret
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meter_fxret = obj;
                                            lv_obj_set_pos(obj, 201, 204);
                                            lv_obj_set_size(obj, 182, 132);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_8meters(obj, 63);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 209, 336);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "1    2    3   4    5   6    7    8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 386, 189);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "dB\n0\n-8\n-16\n-25\n-34\n-42\n-51\n-60");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 83, 357);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "AUX");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 250, 357);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "FX Return");
                                        }
                                        {
                                            // meter_bus18
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meter_bus18 = obj;
                                            lv_obj_set_pos(obj, 413, 204);
                                            lv_obj_set_size(obj, 182, 132);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_8meters(obj, 72);
                                        }
                                        {
                                            // meter_bux916
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meter_bux916 = obj;
                                            lv_obj_set_pos(obj, 604, 204);
                                            lv_obj_set_size(obj, 182, 132);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_8meters(obj, 81);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 479, 357);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "Bus 1-8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 666, 357);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "Bus 9-16");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 421, 337);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "1    2    3   4    5   6    7    8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 611, 337);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "9   10   11  12   13  14   15  16");
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Meter Prototypes");
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // led_xlr01_phantom
                                            lv_obj_t *obj = lv_led_create(parent_obj);
                                            objects.led_xlr01_phantom = obj;
                                            lv_obj_set_pos(obj, 59, 46);
                                            lv_obj_set_size(obj, 5, 5);
                                            lv_led_set_color(obj, lv_color_hex(0xffffbc00));
                                            lv_led_set_brightness(obj, 255);
                                        }
                                        {
                                            // led_xlr01_invert
                                            lv_obj_t *obj = lv_led_create(parent_obj);
                                            objects.led_xlr01_invert = obj;
                                            lv_obj_set_pos(obj, 122, 46);
                                            lv_obj_set_size(obj, 5, 5);
                                            lv_led_set_color(obj, lv_color_hex(0xffffbc00));
                                            lv_led_set_brightness(obj, 255);
                                        }
                                        {
                                            // led_xlr01_input_level
                                            lv_obj_t *obj = lv_led_create(parent_obj);
                                            objects.led_xlr01_input_level = obj;
                                            lv_obj_set_pos(obj, 191, 46);
                                            lv_obj_set_size(obj, 5, 5);
                                            lv_led_set_color(obj, lv_color_hex(0xff00ff14));
                                            lv_led_set_brightness(obj, 255);
                                        }
                                        {
                                            // led_ch01_input
                                            lv_obj_t *obj = lv_led_create(parent_obj);
                                            objects.led_ch01_input = obj;
                                            lv_obj_set_pos(obj, 361, 45);
                                            lv_obj_set_size(obj, 5, 5);
                                            lv_led_set_color(obj, lv_color_hex(0xff00ff14));
                                            lv_led_set_brightness(obj, 255);
                                        }
                                        {
                                            // led_ch01_output
                                            lv_obj_t *obj = lv_led_create(parent_obj);
                                            objects.led_ch01_output = obj;
                                            lv_obj_set_pos(obj, 417, 45);
                                            lv_obj_set_size(obj, 5, 5);
                                            lv_led_set_color(obj, lv_color_hex(0xff00ff14));
                                            lv_led_set_brightness(obj, 255);
                                        }
                                        {
                                            // led_main_l
                                            lv_obj_t *obj = lv_led_create(parent_obj);
                                            objects.led_main_l = obj;
                                            lv_obj_set_pos(obj, 497, 46);
                                            lv_obj_set_size(obj, 5, 5);
                                            lv_led_set_color(obj, lv_color_hex(0xff00ff14));
                                            lv_led_set_brightness(obj, 255);
                                        }
                                        {
                                            // led_main_r
                                            lv_obj_t *obj = lv_led_create(parent_obj);
                                            objects.led_main_r = obj;
                                            lv_obj_set_pos(obj, 520, 46);
                                            lv_obj_set_size(obj, 5, 5);
                                            lv_led_set_color(obj, lv_color_hex(0xff00ff14));
                                            lv_led_set_brightness(obj, 255);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 12, 40);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "01");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 30, 14);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "Phantom, Invert, Input Level");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 341, 6);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Channel\nInput | Output");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 323, 40);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "01");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 494, 9);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Main\nL | R");
                                        }
                                        {
                                            // meterstrip_ch01
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meterstrip_ch01 = obj;
                                            lv_obj_set_pos(obj, 683, 9);
                                            lv_obj_set_size(obj, 20, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_meter_strip(obj, 97);
                                        }
                                        {
                                            // meterstrip_ch03
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meterstrip_ch03 = obj;
                                            lv_obj_set_pos(obj, 723, 9);
                                            lv_obj_set_size(obj, 20, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_meter_strip(obj, 105);
                                        }
                                        {
                                            // meterstrip_ch02
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meterstrip_ch02 = obj;
                                            lv_obj_set_pos(obj, 703, 9);
                                            lv_obj_set_size(obj, 20, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_meter_strip(obj, 113);
                                        }
                                        {
                                            // meterstrip_ch09
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meterstrip_ch09 = obj;
                                            lv_obj_set_pos(obj, 683, 129);
                                            lv_obj_set_size(obj, 20, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_meter_strip(obj, 121);
                                        }
                                        {
                                            // meterstrip_ch10
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meterstrip_ch10 = obj;
                                            lv_obj_set_pos(obj, 703, 129);
                                            lv_obj_set_size(obj, 20, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_meter_strip(obj, 129);
                                        }
                                        {
                                            // meterstrip_ch011
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meterstrip_ch011 = obj;
                                            lv_obj_set_pos(obj, 723, 129);
                                            lv_obj_set_size(obj, 20, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_meter_strip(obj, 137);
                                        }
                                        {
                                            // meterstrip_aux1
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meterstrip_aux1 = obj;
                                            lv_obj_set_pos(obj, 683, 250);
                                            lv_obj_set_size(obj, 20, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_meter_strip(obj, 145);
                                        }
                                        {
                                            // meterstrip_aux2
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meterstrip_aux2 = obj;
                                            lv_obj_set_pos(obj, 703, 250);
                                            lv_obj_set_size(obj, 20, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_meter_strip(obj, 153);
                                        }
                                        {
                                            // meterstrip_aux3
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meterstrip_aux3 = obj;
                                            lv_obj_set_pos(obj, 723, 250);
                                            lv_obj_set_size(obj, 20, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_meter_strip(obj, 161);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 606, 92);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "Ch1, 2, 3");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 606, 325);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "AUX1, 2, 3");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 606, 210);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "Ch9, 10, 11");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Routing");
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    add_style_tab_nopad(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // routingtab
                            lv_obj_t *obj = lv_tabview_create(parent_obj);
                            objects.routingtab = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
                            lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
                            lv_tabview_set_tab_bar_size(obj, 20);
                            add_style_tabview_nopad(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Overview");
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // hw_inputs
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.hw_inputs = obj;
                                            lv_obj_set_pos(obj, 12, 28);
                                            lv_obj_set_size(obj, 163, 108);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "Hardware Inputs\n- XLR\n- AUX / TB\n- CARD\n- AES50");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffd4ea6b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // hw_outputs
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.hw_outputs = obj;
                                            lv_obj_set_pos(obj, 12, 156);
                                            lv_obj_set_size(obj, 163, 123);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "Hardware Outputs\n- XLR\n- AUX / Mon\n- CARD\n- Ultranet\n- AES50");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffde59), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // fpga
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.fpga = obj;
                                            lv_obj_set_pos(obj, 175, 28);
                                            lv_obj_set_size(obj, 168, 251);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "Input/Output (FPGA)\n\nRoutes the audio from any input to any output");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffbcbcbc), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // mixer
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.mixer = obj;
                                            lv_obj_set_pos(obj, 437, 28);
                                            lv_obj_set_size(obj, 118, 251);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "Mixer (DSP1)\n\n\nChannels\nBuses\nMain LR\nMonitoring");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff94beff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // routing_input
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.routing_input = obj;
                                            lv_obj_set_pos(obj, 343, 114);
                                            lv_obj_set_size(obj, 94, 56);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "40 Ch\n<---------->");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // routing_input_1
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.routing_input_1 = obj;
                                            lv_obj_set_pos(obj, 555, 115);
                                            lv_obj_set_size(obj, 94, 56);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "24 Ch\n<---------->");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // dsp
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.dsp = obj;
                                            lv_obj_set_pos(obj, 649, 28);
                                            lv_obj_set_size(obj, 136, 251);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "FX (DSP2)\n\n\nFX Processing\nUSB\nAES/EBU");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffd48d), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Signal-Routing (FPGA)");
                                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // table_routing_fpga
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.table_routing_fpga = obj;
                                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                                            lv_obj_set_size(obj, LV_PCT(100), 322);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_set_style_pad_top(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Input-Routing (DSP)");
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // table_routing_dsp_input
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.table_routing_dsp_input = obj;
                                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                                            lv_obj_set_size(obj, LV_PCT(100), 322);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_set_style_pad_top(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Output-Routing (DSP)");
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // table_routing_dsp_output
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.table_routing_dsp_output = obj;
                                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                                            lv_obj_set_size(obj, LV_PCT(100), 322);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_set_style_pad_top(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Setup");
                    add_style_tab_nopad(obj);
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
                    add_style_tab_nopad(obj);
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
                    add_style_tab_nopad(obj);
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
                    add_style_tab_nopad(obj);
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
                    add_style_tab_nopad(obj);
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
        {
            // display_encoders
            lv_obj_t *obj = lv_buttonmatrix_create(parent_obj);
            objects.display_encoders = obj;
            lv_obj_set_pos(obj, 0, 420);
            lv_obj_set_size(obj, 800, 60);
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
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
    tick_user_widget_8meters(54);
    tick_user_widget_8meters(63);
    tick_user_widget_8meters(72);
    tick_user_widget_8meters(81);
    tick_user_widget_meter_strip(97);
    tick_user_widget_meter_strip(105);
    tick_user_widget_meter_strip(113);
    tick_user_widget_meter_strip(121);
    tick_user_widget_meter_strip(129);
    tick_user_widget_meter_strip(137);
    tick_user_widget_meter_strip(145);
    tick_user_widget_meter_strip(153);
    tick_user_widget_meter_strip(161);
}

void create_user_widget_8meters(lv_obj_t *parent_obj, int startWidgetIndex) {
    (void)startWidgetIndex;
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            // slider01
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
            lv_obj_set_pos(obj, 5, 10);
            lv_obj_set_size(obj, 11, 111);
            lv_slider_set_range(obj, -60, 0);
            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
        }
        {
            // slider02
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
            lv_obj_set_pos(obj, 28, 10);
            lv_obj_set_size(obj, 11, 111);
            lv_slider_set_range(obj, -60, 0);
            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
        }
        {
            // slider03
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 2] = obj;
            lv_obj_set_pos(obj, 51, 10);
            lv_obj_set_size(obj, 11, 111);
            lv_slider_set_range(obj, -60, 0);
            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
        }
        {
            // slider04
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 3] = obj;
            lv_obj_set_pos(obj, 74, 10);
            lv_obj_set_size(obj, 11, 111);
            lv_slider_set_range(obj, -60, 0);
            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
        }
        {
            // slider05
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 4] = obj;
            lv_obj_set_pos(obj, 97, 10);
            lv_obj_set_size(obj, 11, 111);
            lv_slider_set_range(obj, -60, 0);
            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
        }
        {
            // slider06
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 5] = obj;
            lv_obj_set_pos(obj, 120, 10);
            lv_obj_set_size(obj, 11, 111);
            lv_slider_set_range(obj, -60, 0);
            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
        }
        {
            // slider07
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 6] = obj;
            lv_obj_set_pos(obj, 143, 10);
            lv_obj_set_size(obj, 11, 111);
            lv_slider_set_range(obj, -60, 0);
            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
        }
        {
            // slider08
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 7] = obj;
            lv_obj_set_pos(obj, 166, 10);
            lv_obj_set_size(obj, 11, 111);
            lv_slider_set_range(obj, -60, 0);
            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
        }
    }
}

void tick_user_widget_8meters(int startWidgetIndex) {
    (void)startWidgetIndex;
}

void create_user_widget_meter_strip(lv_obj_t *parent_obj, int startWidgetIndex) {
    (void)startWidgetIndex;
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            // led_clip
            lv_obj_t *obj = lv_led_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
            lv_obj_set_pos(obj, 5, 4);
            lv_obj_set_size(obj, 10, 1);
            lv_led_set_color(obj, lv_color_hex(0xffff0000));
            lv_led_set_brightness(obj, 255);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_clip_corner(obj, false, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // led_5
            lv_obj_t *obj = lv_led_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
            lv_obj_set_pos(obj, 5, 11);
            lv_obj_set_size(obj, 10, 1);
            lv_led_set_color(obj, lv_color_hex(0xffffde00));
            lv_led_set_brightness(obj, 255);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // led_4
            lv_obj_t *obj = lv_led_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 2] = obj;
            lv_obj_set_pos(obj, 5, 19);
            lv_obj_set_size(obj, 10, 1);
            lv_led_set_color(obj, lv_color_hex(0xffffde00));
            lv_led_set_brightness(obj, 255);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // led_3
            lv_obj_t *obj = lv_led_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 3] = obj;
            lv_obj_set_pos(obj, 5, 26);
            lv_obj_set_size(obj, 10, 1);
            lv_led_set_color(obj, lv_color_hex(0xffffde00));
            lv_led_set_brightness(obj, 255);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // led_2
            lv_obj_t *obj = lv_led_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 4] = obj;
            lv_obj_set_pos(obj, 5, 34);
            lv_obj_set_size(obj, 10, 1);
            lv_led_set_color(obj, lv_color_hex(0xff53ff00));
            lv_led_set_brightness(obj, 255);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // led_1
            lv_obj_t *obj = lv_led_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 5] = obj;
            lv_obj_set_pos(obj, 5, 41);
            lv_obj_set_size(obj, 10, 1);
            lv_led_set_color(obj, lv_color_hex(0xff53ff00));
            lv_led_set_brightness(obj, 255);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // fader
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 6] = obj;
            lv_obj_set_pos(obj, 8, 54);
            lv_obj_set_size(obj, 4, 58);
            lv_slider_set_range(obj, -60, 10);
            lv_slider_set_value(obj, -20, LV_ANIM_OFF);
        }
    }
}

void tick_user_widget_meter_strip(int startWidgetIndex) {
    (void)startWidgetIndex;
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
