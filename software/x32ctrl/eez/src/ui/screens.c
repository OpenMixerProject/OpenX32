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

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

static void event_handler_cb_main_main(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_SCREEN_LOAD_START) {
        // group: grp_KEY
        lv_group_remove_all_objs(groups.grp_KEY);
        lv_group_add_obj(groups.grp_KEY, objects.channel_box);
    }
}

//
// Screens
//

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_add_event_cb(obj, event_handler_cb_main_main, LV_EVENT_ALL, 0);
    {
        lv_obj_t *parent_obj = obj;
        {
            // channel_box
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.channel_box = obj;
            lv_obj_set_pos(obj, 3, 3);
            lv_obj_set_size(obj, 94, 72);
            lv_obj_add_event_cb(obj, action_action_key, LV_EVENT_KEY, (void *)0);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
                    lv_label_set_text_static(obj, "CH32");
                }
                {
                    // current_channel_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.current_channel_name = obj;
                    lv_obj_set_pos(obj, -19, 33);
                    lv_obj_set_size(obj, 80, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Kanal 32");
                }
                {
                    // current_channel_color
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.current_channel_color = obj;
                    lv_obj_set_pos(obj, 61, -22);
                    lv_obj_set_size(obj, 11, 72);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfafafa), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // header_ip
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.header_ip = obj;
            lv_obj_set_pos(obj, 403, 3);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text_static(obj, "IP:");
        }
        {
            // header_debug
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.header_debug = obj;
            lv_obj_set_pos(obj, 101, 32);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text_static(obj, "DEBUG:");
        }
        {
            // header_aes50_info
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.header_aes50_info = obj;
            lv_obj_set_pos(obj, 102, 3);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text_static(obj, "AES50: ");
        }
        {
            // header_statustext
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.header_statustext = obj;
            lv_obj_set_pos(obj, 101, 58);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x7d7d7d), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Status");
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
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // home_channelstrip_1
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.home_channelstrip_1 = obj;
                                            lv_obj_set_pos(obj, 0, 12);
                                            lv_obj_set_size(obj, 130, 306);
                                            add_style_panel_flat_bg(obj);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // home_ch_1
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.home_ch_1 = obj;
                                                    lv_obj_set_pos(obj, 0, 39);
                                                    lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "Text");
                                                }
                                                {
                                                    // home_vumeter_1
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.home_vumeter_1 = obj;
                                                    lv_obj_set_pos(obj, 24, 82);
                                                    lv_obj_set_size(obj, 40, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_vumeter_colored);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_scale_create(parent_obj);
                                                    objects.obj0 = obj;
                                                    lv_obj_set_pos(obj, 70, 91);
                                                    lv_obj_set_size(obj, 36, 191);
                                                    lv_scale_set_mode(obj, LV_SCALE_MODE_VERTICAL_RIGHT);
                                                    lv_scale_set_range(obj, 10, 40);
                                                    lv_scale_set_angle_range(obj, 270);
                                                    lv_scale_set_rotation(obj, 135);
                                                    lv_scale_set_total_tick_count(obj, 31);
                                                    lv_scale_set_major_tick_every(obj, 5);
                                                    lv_scale_set_label_show(obj, true);
                                                    static const char *label_texts[8] = {
                                                        "SIG",
                                                        "-30",
                                                        "-18",
                                                        "-12",
                                                        "-6",
                                                        "-3",
                                                        "CLIP",
                                                        NULL
                                                    };
                                                    lv_scale_set_text_src(obj, label_texts);
                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_ITEMS | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                                }
                                            }
                                        }
                                        {
                                            // home_channelstrip_2
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.home_channelstrip_2 = obj;
                                            lv_obj_set_pos(obj, 134, 12);
                                            lv_obj_set_size(obj, 130, 306);
                                            add_style_panel_flat_bg(obj);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // home_ch_2
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.home_ch_2 = obj;
                                                    lv_obj_set_pos(obj, 0, 39);
                                                    lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "Text");
                                                }
                                                {
                                                    // home_vumeter_2
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.home_vumeter_2 = obj;
                                                    lv_obj_set_pos(obj, 24, 82);
                                                    lv_obj_set_size(obj, 40, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_vumeter_colored);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_scale_create(parent_obj);
                                                    objects.obj1 = obj;
                                                    lv_obj_set_pos(obj, 70, 91);
                                                    lv_obj_set_size(obj, 36, 191);
                                                    lv_scale_set_mode(obj, LV_SCALE_MODE_VERTICAL_RIGHT);
                                                    lv_scale_set_range(obj, 10, 40);
                                                    lv_scale_set_angle_range(obj, 270);
                                                    lv_scale_set_rotation(obj, 135);
                                                    lv_scale_set_total_tick_count(obj, 31);
                                                    lv_scale_set_major_tick_every(obj, 5);
                                                    lv_scale_set_label_show(obj, true);
                                                    static const char *label_texts[8] = {
                                                        "SIG",
                                                        "-30",
                                                        "-18",
                                                        "-12",
                                                        "-6",
                                                        "-3",
                                                        "CLIP",
                                                        NULL
                                                    };
                                                    lv_scale_set_text_src(obj, label_texts);
                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_ITEMS | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                                }
                                            }
                                        }
                                        {
                                            // home_channelstrip_3
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.home_channelstrip_3 = obj;
                                            lv_obj_set_pos(obj, 268, 12);
                                            lv_obj_set_size(obj, 130, 306);
                                            add_style_panel_flat_bg(obj);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // home_ch_3
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.home_ch_3 = obj;
                                                    lv_obj_set_pos(obj, 0, 39);
                                                    lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "Text");
                                                }
                                                {
                                                    // home_vumeter_3
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.home_vumeter_3 = obj;
                                                    lv_obj_set_pos(obj, 24, 82);
                                                    lv_obj_set_size(obj, 40, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_vumeter_colored);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_scale_create(parent_obj);
                                                    objects.obj2 = obj;
                                                    lv_obj_set_pos(obj, 70, 91);
                                                    lv_obj_set_size(obj, 36, 191);
                                                    lv_scale_set_mode(obj, LV_SCALE_MODE_VERTICAL_RIGHT);
                                                    lv_scale_set_range(obj, 10, 40);
                                                    lv_scale_set_angle_range(obj, 270);
                                                    lv_scale_set_rotation(obj, 135);
                                                    lv_scale_set_total_tick_count(obj, 31);
                                                    lv_scale_set_major_tick_every(obj, 5);
                                                    lv_scale_set_label_show(obj, true);
                                                    static const char *label_texts[8] = {
                                                        "SIG",
                                                        "-30",
                                                        "-18",
                                                        "-12",
                                                        "-6",
                                                        "-3",
                                                        "CLIP",
                                                        NULL
                                                    };
                                                    lv_scale_set_text_src(obj, label_texts);
                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_ITEMS | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                                }
                                            }
                                        }
                                        {
                                            // home_channelstrip_4
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.home_channelstrip_4 = obj;
                                            lv_obj_set_pos(obj, 402, 12);
                                            lv_obj_set_size(obj, 130, 306);
                                            add_style_panel_flat_bg(obj);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // home_ch_4
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.home_ch_4 = obj;
                                                    lv_obj_set_pos(obj, 0, 39);
                                                    lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "Text");
                                                }
                                                {
                                                    // home_vumeter_4
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.home_vumeter_4 = obj;
                                                    lv_obj_set_pos(obj, 24, 82);
                                                    lv_obj_set_size(obj, 40, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_vumeter_colored);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_scale_create(parent_obj);
                                                    objects.obj3 = obj;
                                                    lv_obj_set_pos(obj, 70, 91);
                                                    lv_obj_set_size(obj, 36, 191);
                                                    lv_scale_set_mode(obj, LV_SCALE_MODE_VERTICAL_RIGHT);
                                                    lv_scale_set_range(obj, 10, 40);
                                                    lv_scale_set_angle_range(obj, 270);
                                                    lv_scale_set_rotation(obj, 135);
                                                    lv_scale_set_total_tick_count(obj, 31);
                                                    lv_scale_set_major_tick_every(obj, 5);
                                                    lv_scale_set_label_show(obj, true);
                                                    static const char *label_texts[8] = {
                                                        "SIG",
                                                        "-30",
                                                        "-18",
                                                        "-12",
                                                        "-6",
                                                        "-3",
                                                        "CLIP",
                                                        NULL
                                                    };
                                                    lv_scale_set_text_src(obj, label_texts);
                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_ITEMS | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                                }
                                            }
                                        }
                                        {
                                            // home_channelstrip_5
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.home_channelstrip_5 = obj;
                                            lv_obj_set_pos(obj, 536, 12);
                                            lv_obj_set_size(obj, 130, 306);
                                            add_style_panel_flat_bg(obj);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // home_ch_5
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.home_ch_5 = obj;
                                                    lv_obj_set_pos(obj, 0, 39);
                                                    lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "Text");
                                                }
                                                {
                                                    // home_vumeter_5
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.home_vumeter_5 = obj;
                                                    lv_obj_set_pos(obj, 24, 82);
                                                    lv_obj_set_size(obj, 40, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_vumeter_colored);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_scale_create(parent_obj);
                                                    objects.obj4 = obj;
                                                    lv_obj_set_pos(obj, 70, 91);
                                                    lv_obj_set_size(obj, 36, 191);
                                                    lv_scale_set_mode(obj, LV_SCALE_MODE_VERTICAL_RIGHT);
                                                    lv_scale_set_range(obj, 10, 40);
                                                    lv_scale_set_angle_range(obj, 270);
                                                    lv_scale_set_rotation(obj, 135);
                                                    lv_scale_set_total_tick_count(obj, 31);
                                                    lv_scale_set_major_tick_every(obj, 5);
                                                    lv_scale_set_label_show(obj, true);
                                                    static const char *label_texts[8] = {
                                                        "SIG",
                                                        "-30",
                                                        "-18",
                                                        "-12",
                                                        "-6",
                                                        "-3",
                                                        "CLIP",
                                                        NULL
                                                    };
                                                    lv_scale_set_text_src(obj, label_texts);
                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_ITEMS | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                                }
                                            }
                                        }
                                        {
                                            // home_channelstrip_6
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.home_channelstrip_6 = obj;
                                            lv_obj_set_pos(obj, 670, 12);
                                            lv_obj_set_size(obj, 130, 306);
                                            add_style_panel_flat_bg(obj);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // home_ch_6
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.home_ch_6 = obj;
                                                    lv_obj_set_pos(obj, 0, 39);
                                                    lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "Text");
                                                }
                                                {
                                                    // home_vumeter_6
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.home_vumeter_6 = obj;
                                                    lv_obj_set_pos(obj, 24, 82);
                                                    lv_obj_set_size(obj, 40, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_vumeter_colored);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_scale_create(parent_obj);
                                                    objects.obj5 = obj;
                                                    lv_obj_set_pos(obj, 70, 91);
                                                    lv_obj_set_size(obj, 36, 191);
                                                    lv_scale_set_mode(obj, LV_SCALE_MODE_VERTICAL_RIGHT);
                                                    lv_scale_set_range(obj, 10, 40);
                                                    lv_scale_set_angle_range(obj, 270);
                                                    lv_scale_set_rotation(obj, 135);
                                                    lv_scale_set_total_tick_count(obj, 31);
                                                    lv_scale_set_major_tick_every(obj, 5);
                                                    lv_scale_set_label_show(obj, true);
                                                    static const char *label_texts[8] = {
                                                        "SIG",
                                                        "-30",
                                                        "-18",
                                                        "-12",
                                                        "-6",
                                                        "-3",
                                                        "CLIP",
                                                        NULL
                                                    };
                                                    lv_scale_set_text_src(obj, label_texts);
                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_ITEMS | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                                }
                                            }
                                        }
                                    }
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
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 800, 322);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // current_channel_source_1
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.current_channel_source_1 = obj;
                                                    lv_obj_set_pos(obj, 563, 27);
                                                    lv_obj_set_size(obj, 209, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x264056), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "OUT");
                                                }
                                                {
                                                    // config_vumeter
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.config_vumeter = obj;
                                                    lv_obj_set_pos(obj, 18, 17);
                                                    lv_obj_set_size(obj, 40, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_vumeter_colored);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                                }
                                                {
                                                    // config_gain_knob
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.config_gain_knob = obj;
                                                    lv_obj_set_pos(obj, 162, 182);
                                                    lv_obj_set_size(obj, 75, 75);
                                                    lv_image_set_src(obj, &img_knob);
                                                    lv_image_set_scale(obj, 192);
                                                    lv_obj_set_style_transform_pivot_x(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_transform_pivot_y(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_transform_rotation(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    // config_pan_knob
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.config_pan_knob = obj;
                                                    lv_obj_set_pos(obj, 697, 184);
                                                    lv_obj_set_size(obj, 75, 75);
                                                    lv_image_set_src(obj, &img_knob);
                                                    lv_image_set_scale(obj, 192);
                                                    lv_obj_set_style_transform_pivot_x(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_transform_pivot_y(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_transform_rotation(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    // config_volume_knob
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.config_volume_knob = obj;
                                                    lv_obj_set_pos(obj, 563, 183);
                                                    lv_obj_set_size(obj, 75, 75);
                                                    lv_image_set_src(obj, &img_knob);
                                                    lv_image_set_scale(obj, 192);
                                                    lv_obj_set_style_transform_pivot_x(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_transform_pivot_y(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_transform_rotation(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    // current_channel_source
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.current_channel_source = obj;
                                                    lv_obj_set_pos(obj, 114, 26);
                                                    lv_obj_set_size(obj, 189, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x264056), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "IN");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 149, 71);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "48V");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 149, 100);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "INVERT");
                                                }
                                                {
                                                    // current_channel_pan_bal
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.current_channel_pan_bal = obj;
                                                    lv_obj_set_pos(obj, 697, 163);
                                                    lv_obj_set_size(obj, 75, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "0");
                                                }
                                                {
                                                    // current_channel_volume
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.current_channel_volume = obj;
                                                    lv_obj_set_pos(obj, 563, 163);
                                                    lv_obj_set_size(obj, 75, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "0.0dBfs");
                                                }
                                                {
                                                    // config_phantom_checkbox
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.config_phantom_checkbox = obj;
                                                    lv_obj_set_pos(obj, 114, 65);
                                                    lv_obj_set_size(obj, 32, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_checkbox);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                                }
                                                {
                                                    // config_phase_checkbox
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.config_phase_checkbox = obj;
                                                    lv_obj_set_pos(obj, 114, 94);
                                                    lv_obj_set_size(obj, 32, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_checkbox);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                                }
                                                {
                                                    // config_mute_checkbox
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.config_mute_checkbox = obj;
                                                    lv_obj_set_pos(obj, 114, 123);
                                                    lv_obj_set_size(obj, 32, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_checkbox);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 148, 129);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "MUTE");
                                                }
                                                {
                                                    // current_channel_gain
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.current_channel_gain = obj;
                                                    lv_obj_set_pos(obj, 167, 163);
                                                    lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "0.0dB");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 151, 260);
                                                    lv_obj_set_size(obj, 97, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "GAIN");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 284, 127);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "Mute Groups");
                                                }
                                                {
                                                    // list_mute_groups
                                                    lv_obj_t *obj = lv_list_create(parent_obj);
                                                    objects.list_mute_groups = obj;
                                                    lv_obj_set_pos(obj, 290, 151);
                                                    lv_obj_set_size(obj, 83, 148);
                                                    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            // config_mute_group_1
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_mute_group_1 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "1");
                                                        }
                                                        {
                                                            // config_mute_group_2
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_mute_group_2 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "2");
                                                        }
                                                        {
                                                            // config_mute_group_3
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_mute_group_3 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "3");
                                                        }
                                                        {
                                                            // config_mute_group_4
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_mute_group_4 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "4");
                                                        }
                                                        {
                                                            // config_mute_group_5
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_mute_group_5 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "5");
                                                        }
                                                        {
                                                            // config_mute_group_6
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_mute_group_6 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "6");
                                                        }
                                                    }
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 553, 260);
                                                    lv_obj_set_size(obj, 97, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "VOLUME");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 687, 260);
                                                    lv_obj_set_size(obj, 97, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "PAN");
                                                }
                                                {
                                                    // openx32_demo_knob
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.openx32_demo_knob = obj;
                                                    lv_obj_set_pos(obj, 611, 56);
                                                    lv_obj_set_size(obj, 111, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_openx32_knob);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_scale_create(parent_obj);
                                                    objects.obj6 = obj;
                                                    lv_obj_set_pos(obj, 61, 27);
                                                    lv_obj_set_size(obj, 36, 191);
                                                    lv_scale_set_mode(obj, LV_SCALE_MODE_VERTICAL_RIGHT);
                                                    lv_scale_set_range(obj, 10, 40);
                                                    lv_scale_set_angle_range(obj, 270);
                                                    lv_scale_set_rotation(obj, 135);
                                                    lv_scale_set_total_tick_count(obj, 31);
                                                    lv_scale_set_major_tick_every(obj, 5);
                                                    lv_scale_set_label_show(obj, true);
                                                    static const char *label_texts[8] = {
                                                        "SIG",
                                                        "-30",
                                                        "-18",
                                                        "-12",
                                                        "-6",
                                                        "-3",
                                                        "CLIP",
                                                        NULL
                                                    };
                                                    lv_scale_set_text_src(obj, label_texts);
                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_ITEMS | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_line_color(obj, lv_color_hex(0x555555), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 424, 92);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "DCA Groups");
                                                }
                                                {
                                                    // list_dca_groups
                                                    lv_obj_t *obj = lv_list_create(parent_obj);
                                                    objects.list_dca_groups = obj;
                                                    lv_obj_set_pos(obj, 427, 118);
                                                    lv_obj_set_size(obj, 83, 184);
                                                    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            // config_dca_group_1
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_dca_group_1 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "1");
                                                        }
                                                        {
                                                            // config_dca_group_2
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_dca_group_2 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "2");
                                                        }
                                                        {
                                                            // config_dca_group_3
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_dca_group_3 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "3");
                                                        }
                                                        {
                                                            // config_dca_group_4
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_dca_group_4 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "4");
                                                        }
                                                        {
                                                            // config_dca_group_5
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_dca_group_5 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "5");
                                                        }
                                                        {
                                                            // config_dca_group_6
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_dca_group_6 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "6");
                                                        }
                                                        {
                                                            // config_dca_group_7
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_dca_group_7 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "7");
                                                        }
                                                        {
                                                            // config_dca_group_8
                                                            lv_obj_t *obj = lv_checkbox_create(parent_obj);
                                                            objects.config_dca_group_8 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_checkbox_set_text_static(obj, "8");
                                                        }
                                                    }
                                                }
                                                {
                                                    // config_phantom_button
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    objects.config_phantom_button = obj;
                                                    lv_obj_set_pos(obj, 223, 47);
                                                    lv_obj_set_size(obj, 80, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_buttononoff);
                                                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
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
                                            lv_obj_set_pos(obj, LV_PCT(0), 4);
                                            lv_obj_set_size(obj, LV_PCT(100), 314);
                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            // meterstab
                            lv_obj_t *obj = lv_tabview_create(parent_obj);
                            objects.meterstab = obj;
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
                                            // meters_1_8
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meters_1_8 = obj;
                                            lv_obj_set_pos(obj, 0, 12);
                                            lv_obj_set_size(obj, 160, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_ms_bar8(obj, 73);
                                        }
                                        {
                                            // meters_9_16
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meters_9_16 = obj;
                                            lv_obj_set_pos(obj, 160, 12);
                                            lv_obj_set_size(obj, 160, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_ms_bar8(obj, 146);
                                        }
                                        {
                                            // meters_17_24
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meters_17_24 = obj;
                                            lv_obj_set_pos(obj, 320, 12);
                                            lv_obj_set_size(obj, 160, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_ms_bar8(obj, 219);
                                        }
                                        {
                                            // meters_25_32
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meters_25_32 = obj;
                                            lv_obj_set_pos(obj, 480, 12);
                                            lv_obj_set_size(obj, 160, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_ms_bar8(obj, 292);
                                        }
                                        {
                                            // meters_aux
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meters_aux = obj;
                                            lv_obj_set_pos(obj, 640, 12);
                                            lv_obj_set_size(obj, 160, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_ms_bar8(obj, 365);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 6, 133);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "1     2     3    4    5    6     7    8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 167, 133);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "9   10    11   12   13   14   15   16");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 324, 133);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "17   18   19  20  21   22  23  24");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 483, 133);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "25  26  27  28  29  30  31  32");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 647, 133);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "1     2     3    4    5    6    7     8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 295, 150);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Channel");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 706, 150);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "AUX");
                                        }
                                        {
                                            // meters_fxret
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meters_fxret = obj;
                                            lv_obj_set_pos(obj, 20, 184);
                                            lv_obj_set_size(obj, 160, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_ms_bar8(obj, 438);
                                        }
                                        {
                                            // meters_bus_1_8
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meters_bus_1_8 = obj;
                                            lv_obj_set_pos(obj, 216, 184);
                                            lv_obj_set_size(obj, 160, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_ms_bar8(obj, 511);
                                        }
                                        {
                                            // meters_bus_9_16
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meters_bus_9_16 = obj;
                                            lv_obj_set_pos(obj, 376, 184);
                                            lv_obj_set_size(obj, 160, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_ms_bar8(obj, 584);
                                        }
                                        {
                                            // meters_matrix
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.meters_matrix = obj;
                                            lv_obj_set_pos(obj, 575, 184);
                                            lv_obj_set_size(obj, 160, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_ms_bar8(obj, 657);
                                        }
                                        {
                                            // ms_main_lr
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.ms_main_lr = obj;
                                            lv_obj_set_pos(obj, 749, 184);
                                            lv_obj_set_size(obj, 20, 120);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            create_user_widget_ms_bar(obj, 730);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 26, 306);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "1     2     3    4    5    6     7    8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 222, 306);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "1     2     3    4    5    6     7    8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 382, 306);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "9   10    11   12   13   14   15   16");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 582, 306);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "1     2     3    4    5    6    S  M/C");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 61, 323);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "DSP2 Return");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 355, 322);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Mixbus");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 617, 323);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Matrix");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 746, 306);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "L/R\nMain");
                                        }
                                    }
                                }
                                {
                                    // configtab_1
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "RTA");
                                    objects.configtab_1 = obj;
                                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            lv_obj_t *obj = lv_scale_create(parent_obj);
                                            objects.obj7 = obj;
                                            lv_obj_set_pos(obj, 78, 295);
                                            lv_obj_set_size(obj, 640, 26);
                                            lv_scale_set_mode(obj, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
                                            lv_scale_set_range(obj, 10, 40);
                                            lv_scale_set_angle_range(obj, 270);
                                            lv_scale_set_rotation(obj, 135);
                                            lv_scale_set_total_tick_count(obj, 25);
                                            lv_scale_set_major_tick_every(obj, 6);
                                            lv_scale_set_label_show(obj, true);
                                            static const char *label_texts[6] = {
                                                "0 Hz",
                                                " 600 Hz",
                                                " 2.5 kHz",
                                                " 8 kHz",
                                                " 20 kHz",
                                                NULL
                                            };
                                            lv_scale_set_text_src(obj, label_texts);
                                        }
                                        {
                                            // rta_chart
                                            lv_obj_t *obj = lv_chart_create(parent_obj);
                                            objects.rta_chart = obj;
                                            lv_obj_set_pos(obj, 78, 12);
                                            lv_obj_set_size(obj, 640, 275);
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
                                            lv_obj_set_pos(obj, 20, 33);
                                            lv_obj_set_size(obj, 113, 108);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "Inputs\n- XLR\n- AUX / TB\n- CARD\n- AES50");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xd4ea6b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // hw_outputs
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.hw_outputs = obj;
                                            lv_obj_set_pos(obj, 20, 161);
                                            lv_obj_set_size(obj, 113, 123);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "Outputs\n- XLR\n- AUX / Mon\n- CARD\n- Ultranet\n- AES50");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffde59), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // fpga
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.fpga = obj;
                                            lv_obj_set_pos(obj, 131, 33);
                                            lv_obj_set_size(obj, 133, 251);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "In-/Output\nRouting\n(FPGA)\n\nRoutes the audio from any input to any output");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xbcbcbc), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // mixer
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.mixer = obj;
                                            lv_obj_set_pos(obj, 354, 33);
                                            lv_obj_set_size(obj, 118, 251);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "Mixer (DSP1)\n\n\nChannels\nBuses\nMain LR\nMonitoring");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x94beff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // routing_input
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.routing_input = obj;
                                            lv_obj_set_pos(obj, 262, 33);
                                            lv_obj_set_size(obj, 94, 56);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "40 Ch\n<---------->");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // routing_input_1
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.routing_input_1 = obj;
                                            lv_obj_set_pos(obj, 470, 33);
                                            lv_obj_set_size(obj, 94, 56);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "24 Ch\n<---------->");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // dsp
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.dsp = obj;
                                            lv_obj_set_pos(obj, 562, 33);
                                            lv_obj_set_size(obj, 136, 90);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "FX (DSP2)\n\n\nFX Processing");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffd48d), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // routing_input_2
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.routing_input_2 = obj;
                                            lv_obj_set_pos(obj, 643, 121);
                                            lv_obj_set_size(obj, 75, 40);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "2 Ch");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // routing_input_3
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.routing_input_3 = obj;
                                            lv_obj_set_pos(obj, 544, 121);
                                            lv_obj_set_size(obj, 75, 40);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "2 Ch");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // main_cpu
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.main_cpu = obj;
                                            lv_obj_set_pos(obj, 643, 159);
                                            lv_obj_set_size(obj, 128, 107);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "Main CPU\n\n2 Channel\nSoundcard\naka \"USB\"");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xb2fcdb), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // aes_ebu
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.aes_ebu = obj;
                                            lv_obj_set_pos(obj, 497, 159);
                                            lv_obj_set_size(obj, 122, 57);
                                            lv_textarea_set_max_length(obj, 128);
                                            lv_textarea_set_text(obj, "AES/EBU\nPort");
                                            lv_textarea_set_one_line(obj, false);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xb2fcdb), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                                            // routing_fpga_target
                                            lv_obj_t *obj = lv_roller_create(parent_obj);
                                            objects.routing_fpga_target = obj;
                                            lv_obj_set_pos(obj, 0, 15);
                                            lv_obj_set_size(obj, 92, 303);
                                            lv_roller_set_options(obj, "Channels\nAUX Out\nXLR Out\nA50 Out\nCARD Out\nUltranet", LV_ROLLER_MODE_NORMAL);
                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_color(obj, lv_color_hex(0x2f3237), LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // routing_matrix
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.routing_matrix = obj;
                                            lv_obj_set_pos(obj, 94, 15);
                                            lv_obj_set_size(obj, 579, 303);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj8 = obj;
                                                    lv_obj_set_pos(obj, 0, 0);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 756);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj9 = obj;
                                                    lv_obj_set_pos(obj, 72, 0);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 760);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj10 = obj;
                                                    lv_obj_set_pos(obj, 144, 0);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 764);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj11 = obj;
                                                    lv_obj_set_pos(obj, 216, 0);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 768);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj12 = obj;
                                                    lv_obj_set_pos(obj, 288, 0);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 772);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj13 = obj;
                                                    lv_obj_set_pos(obj, 360, 0);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 776);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj14 = obj;
                                                    lv_obj_set_pos(obj, 432, 0);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 780);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj15 = obj;
                                                    lv_obj_set_pos(obj, 504, 0);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 784);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj16 = obj;
                                                    lv_obj_set_pos(obj, 0, 50);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 788);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj17 = obj;
                                                    lv_obj_set_pos(obj, 72, 50);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 792);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj18 = obj;
                                                    lv_obj_set_pos(obj, 144, 50);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 796);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj19 = obj;
                                                    lv_obj_set_pos(obj, 216, 50);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 800);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj20 = obj;
                                                    lv_obj_set_pos(obj, 288, 50);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 804);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj21 = obj;
                                                    lv_obj_set_pos(obj, 360, 50);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 808);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj22 = obj;
                                                    lv_obj_set_pos(obj, 432, 50);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 812);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj23 = obj;
                                                    lv_obj_set_pos(obj, 504, 50);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 816);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj24 = obj;
                                                    lv_obj_set_pos(obj, 0, 100);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 820);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj25 = obj;
                                                    lv_obj_set_pos(obj, 72, 100);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 824);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj26 = obj;
                                                    lv_obj_set_pos(obj, 144, 100);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 828);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj27 = obj;
                                                    lv_obj_set_pos(obj, 216, 100);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 832);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj28 = obj;
                                                    lv_obj_set_pos(obj, 288, 100);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 836);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj29 = obj;
                                                    lv_obj_set_pos(obj, 360, 100);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 840);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj30 = obj;
                                                    lv_obj_set_pos(obj, 432, 100);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 844);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj31 = obj;
                                                    lv_obj_set_pos(obj, 504, 100);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 848);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj32 = obj;
                                                    lv_obj_set_pos(obj, 0, 150);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 852);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj33 = obj;
                                                    lv_obj_set_pos(obj, 72, 150);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 856);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj34 = obj;
                                                    lv_obj_set_pos(obj, 144, 150);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 860);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj35 = obj;
                                                    lv_obj_set_pos(obj, 216, 150);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 864);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj36 = obj;
                                                    lv_obj_set_pos(obj, 288, 150);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 868);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj37 = obj;
                                                    lv_obj_set_pos(obj, 360, 150);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 872);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj38 = obj;
                                                    lv_obj_set_pos(obj, 432, 150);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 876);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj39 = obj;
                                                    lv_obj_set_pos(obj, 504, 150);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 880);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj40 = obj;
                                                    lv_obj_set_pos(obj, 0, 200);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 884);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj41 = obj;
                                                    lv_obj_set_pos(obj, 72, 200);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 888);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj42 = obj;
                                                    lv_obj_set_pos(obj, 144, 200);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 892);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj43 = obj;
                                                    lv_obj_set_pos(obj, 216, 200);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 896);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj44 = obj;
                                                    lv_obj_set_pos(obj, 288, 200);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 900);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj45 = obj;
                                                    lv_obj_set_pos(obj, 360, 200);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 904);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj46 = obj;
                                                    lv_obj_set_pos(obj, 432, 200);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 908);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj47 = obj;
                                                    lv_obj_set_pos(obj, 504, 200);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 912);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj48 = obj;
                                                    lv_obj_set_pos(obj, 0, 250);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 916);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj49 = obj;
                                                    lv_obj_set_pos(obj, 72, 250);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 920);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj50 = obj;
                                                    lv_obj_set_pos(obj, 144, 250);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 924);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj51 = obj;
                                                    lv_obj_set_pos(obj, 216, 250);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 928);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj52 = obj;
                                                    lv_obj_set_pos(obj, 288, 250);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 932);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj53 = obj;
                                                    lv_obj_set_pos(obj, 360, 250);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 936);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj54 = obj;
                                                    lv_obj_set_pos(obj, 432, 250);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 940);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj55 = obj;
                                                    lv_obj_set_pos(obj, 504, 250);
                                                    lv_obj_set_size(obj, 71, 49);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    create_user_widget_routing_element(obj, 944);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                            }
                                        }
                                        {
                                            // routing_fpga_source
                                            lv_obj_t *obj = lv_roller_create(parent_obj);
                                            objects.routing_fpga_source = obj;
                                            lv_obj_set_pos(obj, 675, 15);
                                            lv_obj_set_size(obj, 125, 303);
                                            lv_roller_set_options(obj, "XLR\nCARD\nAUX\nDSP\nAES50 A", LV_ROLLER_MODE_NORMAL);
                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Inputs");
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // table_routing_dsp_input_header
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.table_routing_dsp_input_header = obj;
                                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                                            lv_obj_set_size(obj, LV_PCT(100), 30);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_set_style_pad_top(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // table_routing_dsp_input
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.table_routing_dsp_input = obj;
                                            lv_obj_set_pos(obj, LV_PCT(0), 0);
                                            lv_obj_set_size(obj, LV_PCT(100), 320);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_set_style_pad_top(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Outputs");
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // table_routing_dsp_output_header
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.table_routing_dsp_output_header = obj;
                                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                                            lv_obj_set_size(obj, LV_PCT(100), 30);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_set_style_pad_top(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // table_routing_dsp_output
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.table_routing_dsp_output = obj;
                                            lv_obj_set_pos(obj, LV_PCT(0), 30);
                                            lv_obj_set_size(obj, LV_PCT(100), 290);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_set_style_pad_top(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
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
                            // setuptab
                            lv_obj_t *obj = lv_tabview_create(parent_obj);
                            objects.setuptab = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
                            lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
                            lv_tabview_set_tab_bar_size(obj, 20);
                            add_style_tabview_nopad(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Setup");
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // current_channel_source_4
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.current_channel_source_4 = obj;
                                            lv_obj_set_pos(obj, 32, 9);
                                            lv_obj_set_size(obj, 218, LV_SIZE_CONTENT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x264056), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "BOARD VERSION INFO");
                                        }
                                        {
                                            // current_channel_source_5
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.current_channel_source_5 = obj;
                                            lv_obj_set_pos(obj, 277, 9);
                                            lv_obj_set_size(obj, 218, LV_SIZE_CONTENT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x264056), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "IP ADDRESS");
                                        }
                                        {
                                            // setup_ipaddresstext
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.setup_ipaddresstext = obj;
                                            lv_obj_set_pos(obj, 277, 48);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text_static(obj, "...");
                                        }
                                        {
                                            // setup_versioninfotext
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.setup_versioninfotext = obj;
                                            lv_obj_set_pos(obj, 32, 47);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text_static(obj, "...");
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Card");
                                    add_style_tab_nopad(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            lv_obj_set_pos(obj, 448, 265);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_play);
                                        }
                                        {
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            lv_obj_set_pos(obj, 312, 265);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_stop);
                                        }
                                        {
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            lv_obj_set_pos(obj, 579, 264);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_record);
                                        }
                                        {
                                            // setup_card_sdcard
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            objects.setup_card_sdcard = obj;
                                            lv_obj_set_pos(obj, 167, 254);
                                            lv_obj_set_size(obj, 64, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_sdcard);
                                            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                        }
                                        {
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            lv_obj_set_pos(obj, 705, 261);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_select);
                                        }
                                        {
                                            // setup_card_sdusb
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            objects.setup_card_sdusb = obj;
                                            lv_obj_set_pos(obj, 40, 256);
                                            lv_obj_set_size(obj, 50, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_sdusb);
                                            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                        }
                                        {
                                            // setup_card_toc_header
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.setup_card_toc_header = obj;
                                            lv_obj_set_pos(obj, 354, 8);
                                            lv_obj_set_size(obj, 400, 34);
                                            lv_obj_set_style_pad_top(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // setup_card_totaltime
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.setup_card_totaltime = obj;
                                            lv_obj_set_pos(obj, 568, 230);
                                            lv_obj_set_size(obj, 201, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "...");
                                        }
                                        {
                                            // setup_card_currentposition
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.setup_card_currentposition = obj;
                                            lv_obj_set_pos(obj, 31, 231);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text_static(obj, "...");
                                        }
                                        {
                                            // setup_card_toc
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.setup_card_toc = obj;
                                            lv_obj_set_pos(obj, 354, 44);
                                            lv_obj_set_size(obj, 400, 147);
                                            lv_obj_set_style_pad_top(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // setup_card_progress
                                            lv_obj_t *obj = lv_bar_create(parent_obj);
                                            objects.setup_card_progress = obj;
                                            lv_obj_set_pos(obj, 31, 203);
                                            lv_obj_set_size(obj, 738, 22);
                                            lv_bar_set_value(obj, 50, LV_ANIM_OFF);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 32, 73);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text_static(obj, "X-Card-Source:");
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // setup_card_sourcemode
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.setup_card_sourcemode = obj;
                                                    lv_obj_set_pos(obj, 126, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "Text");
                                                }
                                            }
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 32, 98);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text_static(obj, "X-Card-String:");
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // setup_card_detected
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.setup_card_detected = obj;
                                                    lv_obj_set_pos(obj, 126, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "-");
                                                }
                                            }
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 32, 47);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text_static(obj, "Channelmode:");
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // setup_card_channelmode
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.setup_card_channelmode = obj;
                                                    lv_obj_set_pos(obj, 126, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "Text");
                                                }
                                            }
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 32, 154);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text_static(obj, "SD-Card 1:");
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // setup_card_sd1info
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.setup_card_sd1info = obj;
                                                    lv_obj_set_pos(obj, 128, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "Text");
                                                }
                                            }
                                        }
                                        {
                                            // current_channel_source_2
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.current_channel_source_2 = obj;
                                            lv_obj_set_pos(obj, 31, 122);
                                            lv_obj_set_size(obj, 312, LV_SIZE_CONTENT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x264056), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "SD CARD INFORMATION (X-LIVE)");
                                        }
                                        {
                                            // current_channel_source_3
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.current_channel_source_3 = obj;
                                            lv_obj_set_pos(obj, 32, 9);
                                            lv_obj_set_size(obj, 312, LV_SIZE_CONTENT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x264056), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "EXPANSION CARD INFORMATION");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 31, 174);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text_static(obj, "SD-Card 2:");
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // setup_card_sd2info
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.setup_card_sd2info = obj;
                                                    lv_obj_set_pos(obj, 129, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "Text");
                                                }
                                            }
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Surface");
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            lv_obj_set_pos(obj, -20, -20);
                                            lv_obj_set_size(obj, 800, 323);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 43, 17);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "Bank");
                                                }
                                                {
                                                    // setup_surface_roller_bank
                                                    lv_obj_t *obj = lv_roller_create(parent_obj);
                                                    objects.setup_surface_roller_bank = obj;
                                                    lv_obj_set_pos(obj, 0, 42);
                                                    lv_obj_set_size(obj, 130, 276);
                                                    lv_roller_set_options(obj, "Assign A\nAssign B\nAssign C", LV_ROLLER_MODE_NORMAL);
                                                    add_style_roller_standard(obj);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 172, 17);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "Control");
                                                }
                                                {
                                                    // setup_surface_roller_control
                                                    lv_obj_t *obj = lv_roller_create(parent_obj);
                                                    objects.setup_surface_roller_control = obj;
                                                    lv_obj_set_pos(obj, 134, 42);
                                                    lv_obj_set_size(obj, 130, 276);
                                                    lv_roller_set_options(obj, "ASSIGN Encoder 1\nASSIGN Encoder 2\nASSIGN Encoder 3\nASSIGN Encoder 4\nASSIGN Button 1\nASSIGN Button 2\nASSIGN Button 3\nASSIGN Button 4\nASSIGN Button 5\nASSIGN Button 6\nASSIGN Button 7\nASSIGN Button 8", LV_ROLLER_MODE_NORMAL);
                                                    lv_roller_set_selected(obj, 3, LV_ANIM_OFF);
                                                    add_style_roller_standard(obj);
                                                }
                                                {
                                                    // setup_surface_roller_action
                                                    lv_obj_t *obj = lv_roller_create(parent_obj);
                                                    objects.setup_surface_roller_action = obj;
                                                    lv_obj_set_pos(obj, 536, 42);
                                                    lv_obj_set_size(obj, 130, 276);
                                                    lv_roller_set_options(obj, "Set\nSet to Index\nChange\nToggle\nReset\nRefresh\nClear Solo", LV_ROLLER_MODE_NORMAL);
                                                    lv_roller_set_selected(obj, 2, LV_ANIM_OFF);
                                                    add_style_roller_standard(obj);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 409, 17);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "Mixerparameter");
                                                }
                                                {
                                                    // setup_surface_roller_mixerparmeter
                                                    lv_obj_t *obj = lv_roller_create(parent_obj);
                                                    objects.setup_surface_roller_mixerparmeter = obj;
                                                    lv_obj_set_pos(obj, 402, 42);
                                                    lv_obj_set_size(obj, 130, 276);
                                                    lv_roller_set_options(obj, "Channel\nDMX", LV_ROLLER_MODE_NORMAL);
                                                    add_style_roller_standard(obj);
                                                }
                                                {
                                                    // setup_surface_roller_index
                                                    lv_obj_t *obj = lv_roller_create(parent_obj);
                                                    objects.setup_surface_roller_index = obj;
                                                    lv_obj_set_pos(obj, 670, 42);
                                                    lv_obj_set_size(obj, 130, 276);
                                                    lv_roller_set_options(obj, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n", LV_ROLLER_MODE_NORMAL);
                                                    lv_roller_set_selected(obj, 2, LV_ANIM_OFF);
                                                    add_style_roller_standard(obj);
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 576, 17);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "Action");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 310, 17);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "Group");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    lv_obj_set_pos(obj, 682, 17);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text_static(obj, "Channel/Index");
                                                }
                                                {
                                                    // setup_surface_roller_group
                                                    lv_obj_t *obj = lv_roller_create(parent_obj);
                                                    objects.setup_surface_roller_group = obj;
                                                    lv_obj_set_pos(obj, 268, 42);
                                                    lv_obj_set_size(obj, 130, 276);
                                                    lv_roller_set_options(obj, "Settings\nRouting\nState\nGlobal\nChannel\nChannel Gate\nChannel EQ\nChannel Dyn\nChannel Sends\nFX\nDMX", LV_ROLLER_MODE_NORMAL);
                                                    lv_roller_set_selected(obj, 3, LV_ANIM_OFF);
                                                    add_style_roller_standard(obj);
                                                }
                                            }
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "About");
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            lv_obj_set_pos(obj, -4, 194);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_tux);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 83, 159);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "AES50-IP-Core:\nMarkus Noll (YetAnotherElectronicsChannel)");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 433, 109);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "FPGA, DSPs and Effects:\nChris Noeding (xn--nding-jua)");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 83, 109);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Main-Control, UI and Backend:\nAlexander Schulz (schulz-alexander)");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 188, 53);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "https://www.openx32.com\nhttps://www.github.com/OpenMixerProject/OpenX32");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 433, 159);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Optimizations for Linux:\nSebastian Gottschall (BrainSlayer)");
                                        }
                                        {
                                            // openx32logo_2
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            objects.openx32logo_2 = obj;
                                            lv_obj_set_pos(obj, 177, -4);
                                            lv_obj_set_size(obj, 402, 59);
                                            lv_image_set_src(obj, &img_openx32logo);
                                            lv_image_set_scale(obj, 100);
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "DEBUG");
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // setup_debug_label
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.setup_debug_label = obj;
                                            lv_obj_set_pos(obj, 1, -2);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text_static(obj, "Text");
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Prototype GUI");
                                }
                            }
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
                            lv_label_set_text_static(obj, "Library");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Effects");
                    add_style_tab_nopad(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // fxtable
                            lv_obj_t *obj = lv_table_create(parent_obj);
                            objects.fxtable = obj;
                            lv_obj_set_pos(obj, 0, 1);
                            lv_obj_set_size(obj, 800, 337);
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
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Mute Group");
                    add_style_tab_nopad(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 15, 20);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text_static(obj, "Mute Groups");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Scenes");
                }
            }
        }
        {
            // display_encoder_sliders
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.display_encoder_sliders = obj;
            lv_obj_set_pos(obj, 0, 420);
            lv_obj_set_size(obj, LV_PCT(100), 60);
            add_style_panel_flat_black(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // widget1
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.widget1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 130, 60);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_config_slider_display_encoder(obj, 982);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ONE|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    add_style_display_encoder_panels(obj);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // widget2
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.widget2 = obj;
                    lv_obj_set_pos(obj, 134, 0);
                    lv_obj_set_size(obj, 130, 60);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_config_slider_display_encoder(obj, 986);
                    add_style_display_encoder_panels(obj);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // widget3
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.widget3 = obj;
                    lv_obj_set_pos(obj, 268, 0);
                    lv_obj_set_size(obj, 130, 60);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_config_slider_display_encoder(obj, 990);
                    add_style_display_encoder_panels(obj);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // widget4
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.widget4 = obj;
                    lv_obj_set_pos(obj, 402, 0);
                    lv_obj_set_size(obj, 130, 60);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_config_slider_display_encoder(obj, 994);
                    add_style_display_encoder_panels(obj);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // widget5
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.widget5 = obj;
                    lv_obj_set_pos(obj, 536, 0);
                    lv_obj_set_size(obj, 130, 60);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_config_slider_display_encoder(obj, 998);
                    add_style_display_encoder_panels(obj);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // widget6
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.widget6 = obj;
                    lv_obj_set_pos(obj, 670, 0);
                    lv_obj_set_size(obj, 130, 60);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_config_slider_display_encoder(obj, 1002);
                    add_style_display_encoder_panels(obj);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
    tick_user_widget_ms_bar8(73);
    tick_user_widget_ms_bar8(146);
    tick_user_widget_ms_bar8(219);
    tick_user_widget_ms_bar8(292);
    tick_user_widget_ms_bar8(365);
    tick_user_widget_ms_bar8(438);
    tick_user_widget_ms_bar8(511);
    tick_user_widget_ms_bar8(584);
    tick_user_widget_ms_bar8(657);
    tick_user_widget_ms_bar(730);
    tick_user_widget_routing_element(756);
    tick_user_widget_routing_element(760);
    tick_user_widget_routing_element(764);
    tick_user_widget_routing_element(768);
    tick_user_widget_routing_element(772);
    tick_user_widget_routing_element(776);
    tick_user_widget_routing_element(780);
    tick_user_widget_routing_element(784);
    tick_user_widget_routing_element(788);
    tick_user_widget_routing_element(792);
    tick_user_widget_routing_element(796);
    tick_user_widget_routing_element(800);
    tick_user_widget_routing_element(804);
    tick_user_widget_routing_element(808);
    tick_user_widget_routing_element(812);
    tick_user_widget_routing_element(816);
    tick_user_widget_routing_element(820);
    tick_user_widget_routing_element(824);
    tick_user_widget_routing_element(828);
    tick_user_widget_routing_element(832);
    tick_user_widget_routing_element(836);
    tick_user_widget_routing_element(840);
    tick_user_widget_routing_element(844);
    tick_user_widget_routing_element(848);
    tick_user_widget_routing_element(852);
    tick_user_widget_routing_element(856);
    tick_user_widget_routing_element(860);
    tick_user_widget_routing_element(864);
    tick_user_widget_routing_element(868);
    tick_user_widget_routing_element(872);
    tick_user_widget_routing_element(876);
    tick_user_widget_routing_element(880);
    tick_user_widget_routing_element(884);
    tick_user_widget_routing_element(888);
    tick_user_widget_routing_element(892);
    tick_user_widget_routing_element(896);
    tick_user_widget_routing_element(900);
    tick_user_widget_routing_element(904);
    tick_user_widget_routing_element(908);
    tick_user_widget_routing_element(912);
    tick_user_widget_routing_element(916);
    tick_user_widget_routing_element(920);
    tick_user_widget_routing_element(924);
    tick_user_widget_routing_element(928);
    tick_user_widget_routing_element(932);
    tick_user_widget_routing_element(936);
    tick_user_widget_routing_element(940);
    tick_user_widget_routing_element(944);
    tick_user_widget_config_slider_display_encoder(982);
    tick_user_widget_config_slider_display_encoder(986);
    tick_user_widget_config_slider_display_encoder(990);
    tick_user_widget_config_slider_display_encoder(994);
    tick_user_widget_config_slider_display_encoder(998);
    tick_user_widget_config_slider_display_encoder(1002);
}

void create_user_widget_ms_bar(lv_obj_t *parent_obj, int startWidgetIndex) {
    (void)startWidgetIndex;
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_clip
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
            lv_obj_set_pos(obj, 3, 5);
            lv_obj_set_size(obj, 14, 6);
            add_style_panel_flat_dark_grey(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_CHECKED);
        }
        {
            // panel_4
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
            lv_obj_set_pos(obj, 3, 12);
            lv_obj_set_size(obj, 14, 5);
            add_style_panel_flat_dark_grey(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffab00), LV_PART_MAIN | LV_STATE_CHECKED);
        }
        {
            // panel_3
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 2] = obj;
            lv_obj_set_pos(obj, 3, 18);
            lv_obj_set_size(obj, 14, 5);
            add_style_panel_flat_dark_grey(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffab00), LV_PART_MAIN | LV_STATE_CHECKED);
        }
        {
            // panel_2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 3] = obj;
            lv_obj_set_pos(obj, 3, 24);
            lv_obj_set_size(obj, 14, 5);
            add_style_panel_flat_dark_grey(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffab00), LV_PART_MAIN | LV_STATE_CHECKED);
        }
        {
            // panel_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 4] = obj;
            lv_obj_set_pos(obj, 3, 30);
            lv_obj_set_size(obj, 14, 5);
            add_style_panel_flat_dark_grey(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x00ff00), LV_PART_MAIN | LV_STATE_CHECKED);
        }
        {
            // panel_0
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 5] = obj;
            lv_obj_set_pos(obj, 3, 36);
            lv_obj_set_size(obj, 14, 5);
            add_style_panel_flat_dark_grey(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x00ff00), LV_PART_MAIN | LV_STATE_CHECKED);
        }
        {
            // fader
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 6] = obj;
            lv_obj_set_pos(obj, 8, 54);
            lv_obj_set_size(obj, 4, 58);
            lv_slider_set_range(obj, -60, 10);
            lv_slider_set_value(obj, 10, LV_ANIM_OFF);
            add_style_slider_fader(obj);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 7] = obj;
            lv_obj_set_pos(obj, 2, 62);
            lv_obj_set_size(obj, 16, 2);
            add_style_panel_flat_black(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x565353), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void tick_user_widget_ms_bar(int startWidgetIndex) {
    (void)startWidgetIndex;
}

void create_user_widget_ms_bar8(lv_obj_t *parent_obj, int startWidgetIndex) {
    (void)startWidgetIndex;
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 20, 120);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_ms_bar(obj, startWidgetIndex + 1);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 9] = obj;
            lv_obj_set_pos(obj, 20, 0);
            lv_obj_set_size(obj, 20, 120);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_ms_bar(obj, startWidgetIndex + 10);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 18] = obj;
            lv_obj_set_pos(obj, 40, 0);
            lv_obj_set_size(obj, 20, 120);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_ms_bar(obj, startWidgetIndex + 19);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 27] = obj;
            lv_obj_set_pos(obj, 60, 0);
            lv_obj_set_size(obj, 20, 120);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_ms_bar(obj, startWidgetIndex + 28);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 36] = obj;
            lv_obj_set_pos(obj, 80, 0);
            lv_obj_set_size(obj, 20, 120);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_ms_bar(obj, startWidgetIndex + 37);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 45] = obj;
            lv_obj_set_pos(obj, 100, 0);
            lv_obj_set_size(obj, 20, 120);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_ms_bar(obj, startWidgetIndex + 46);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 54] = obj;
            lv_obj_set_pos(obj, 120, 0);
            lv_obj_set_size(obj, 20, 120);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_ms_bar(obj, startWidgetIndex + 55);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 63] = obj;
            lv_obj_set_pos(obj, 140, 0);
            lv_obj_set_size(obj, 20, 120);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_ms_bar(obj, startWidgetIndex + 64);
        }
    }
}

void tick_user_widget_ms_bar8(int startWidgetIndex) {
    (void)startWidgetIndex;
    tick_user_widget_ms_bar(startWidgetIndex + 1);
    tick_user_widget_ms_bar(startWidgetIndex + 10);
    tick_user_widget_ms_bar(startWidgetIndex + 19);
    tick_user_widget_ms_bar(startWidgetIndex + 28);
    tick_user_widget_ms_bar(startWidgetIndex + 37);
    tick_user_widget_ms_bar(startWidgetIndex + 46);
    tick_user_widget_ms_bar(startWidgetIndex + 55);
    tick_user_widget_ms_bar(startWidgetIndex + 64);
}

void create_user_widget_config_slider_display_encoder(lv_obj_t *parent_obj, int startWidgetIndex) {
    (void)startWidgetIndex;
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            // label
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
            lv_obj_set_pos(obj, 0, 6);
            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Label");
        }
        {
            // slider
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
            lv_obj_set_pos(obj, 6, 28);
            lv_obj_set_size(obj, 119, 4);
            lv_slider_set_range(obj, -2, 101);
            lv_slider_set_value(obj, 100, LV_ANIM_OFF);
            add_style_slider_config(obj);
        }
        {
            // label_buttonpress
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 2] = obj;
            lv_obj_set_pos(obj, 6, 39);
            lv_obj_set_size(obj, 119, LV_SIZE_CONTENT);
            lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Center");
        }
    }
}

void tick_user_widget_config_slider_display_encoder(int startWidgetIndex) {
    (void)startWidgetIndex;
}

void create_user_widget_widget_fx(lv_obj_t *parent_obj, int startWidgetIndex) {
    (void)startWidgetIndex;
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            // label
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_PCT(100), 20);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x167ada), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_label_set_text_static(obj, "Empty");
        }
        {
            // table
            lv_obj_t *obj = lv_table_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
            lv_obj_set_pos(obj, 0, 20);
            lv_obj_set_size(obj, 100, 320);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
        }
    }
}

void tick_user_widget_widget_fx(int startWidgetIndex) {
    (void)startWidgetIndex;
}

void create_user_widget_routing_element(lv_obj_t *parent_obj, int startWidgetIndex) {
    (void)startWidgetIndex;
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            // lbl_header
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
            lv_obj_set_pos(obj, 2, 4);
            lv_obj_set_size(obj, 67, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xb7b6b6), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x2196f3), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 1255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_label_set_text_static(obj, "CH 01");
        }
        {
            // lbl
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
            lv_obj_set_pos(obj, 2, 17);
            lv_obj_set_size(obj, 67, LV_SIZE_CONTENT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "XLR 1");
        }
        {
            // lbl_footer
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 2] = obj;
            lv_obj_set_pos(obj, 2, 34);
            lv_obj_set_size(obj, 67, LV_SIZE_CONTENT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xb7b6b6), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Kanal 1");
        }
    }
}

void tick_user_widget_routing_element(int startWidgetIndex) {
    (void)startWidgetIndex;
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 1) {
        tick_screen_funcs[screen_index]();
    }
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen(screenId - 1);
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
// Color themes
//

uint32_t active_theme_index = 0;

//
// Groups
//

groups_t groups;
static bool groups_created = false;
void ui_create_groups() {
    if (!groups_created) {
        groups.grp_KEY = lv_group_create();
        groups_created = true;
    }
}

//
//
//

void create_screens() {
    
    // Initialize groups
    ui_create_groups();
    
    // Set default LVGL theme
    lv_display_t *dispp = lv_display_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_display_set_theme(dispp, theme);
    
    // Initialize screens
    // Create screens
    create_screen_main();
}