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

//
// Screens
//

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
                                            // warning
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.warning = obj;
                                            lv_obj_set_pos(obj, 303, 21);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Alpha Firmware");
                                        }
                                        {
                                            // openx32logo_1
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            objects.openx32logo_1 = obj;
                                            lv_obj_set_pos(obj, 303, 54);
                                            lv_obj_set_size(obj, 200, 117);
                                            lv_image_set_src(obj, &img_logo);
                                            lv_image_set_scale(obj, 100);
                                        }
                                        {
                                            // warning_1
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.warning_1 = obj;
                                            lv_obj_set_pos(obj, 115, 171);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "https://github.com/OpenMixerProject/OpenX32");
                                        }
                                        {
                                            // warning_2
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.warning_2 = obj;
                                            lv_obj_set_pos(obj, 197, 210);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Possible 48V Phantom Power on XLR Inputs!\nPossible Loud Noises on Outputs!\nProtect your Gear!");
                                        }
                                        {
                                            // home_ch_1
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.home_ch_1 = obj;
                                            lv_obj_set_pos(obj, 0, 299);
                                            lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Text");
                                        }
                                        {
                                            // home_ch_2
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.home_ch_2 = obj;
                                            lv_obj_set_pos(obj, 134, 299);
                                            lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Text");
                                        }
                                        {
                                            // home_ch_3
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.home_ch_3 = obj;
                                            lv_obj_set_pos(obj, 268, 299);
                                            lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Text");
                                        }
                                        {
                                            // home_ch_4
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.home_ch_4 = obj;
                                            lv_obj_set_pos(obj, 403, 299);
                                            lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Text");
                                        }
                                        {
                                            // home_ch_5
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.home_ch_5 = obj;
                                            lv_obj_set_pos(obj, 537, 299);
                                            lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Text");
                                        }
                                        {
                                            // home_ch_6
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.home_ch_6 = obj;
                                            lv_obj_set_pos(obj, 670, 299);
                                            lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Text");
                                        }
                                        {
                                            // bodyless_instructions
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.bodyless_instructions = obj;
                                            lv_obj_set_pos(obj, 134, 3);
                                            lv_obj_set_size(obj, 536, LV_SIZE_CONTENT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff595151), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Bodyless Mode - Keymapping:\n\n1 -> HOME\n2 -> METERS\n3 -> ROUTING\n4 -> LIBRARY\n5 -> EFFECT\n6 -> SETUP\n7 -> MONITOR\n8 -> SCENES\n9 -> MUTE GRP\n0 -> UTILITY\n\narrow keys -> up, down, left, right\nQ..Z -> Encoder++\nA..H -> Encoder--\nY..N -> Encoder Buttons");
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
                                            // current_channel_source_1
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.current_channel_source_1 = obj;
                                            lv_obj_set_pos(obj, 584, 26);
                                            lv_obj_set_size(obj, 189, LV_SIZE_CONTENT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff264056), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "OUT");
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
                                            lv_obj_set_pos(obj, 125, 174);
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
                                            lv_obj_set_pos(obj, 698, 174);
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
                                            lv_obj_set_pos(obj, 584, 174);
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
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff264056), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "IN");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 149, 63);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "48V");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 149, 92);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "INVERT");
                                        }
                                        {
                                            // current_channel_pan_bal
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.current_channel_pan_bal = obj;
                                            lv_obj_set_pos(obj, 698, 154);
                                            lv_obj_set_size(obj, 75, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "0");
                                        }
                                        {
                                            // current_channel_volume
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.current_channel_volume = obj;
                                            lv_obj_set_pos(obj, 584, 154);
                                            lv_obj_set_size(obj, 75, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "0.0dBfs");
                                        }
                                        {
                                            // config_phantom_checkbox
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            objects.config_phantom_checkbox = obj;
                                            lv_obj_set_pos(obj, 114, 57);
                                            lv_obj_set_size(obj, 32, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_checkbox);
                                            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                        }
                                        {
                                            // config_phase_checkbox
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            objects.config_phase_checkbox = obj;
                                            lv_obj_set_pos(obj, 114, 86);
                                            lv_obj_set_size(obj, 32, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_checkbox);
                                            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                        }
                                        {
                                            // config_mute_checkbox
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            objects.config_mute_checkbox = obj;
                                            lv_obj_set_pos(obj, 114, 115);
                                            lv_obj_set_size(obj, 32, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_checkbox);
                                            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 148, 121);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "MUTE");
                                        }
                                        {
                                            // current_channel_gain
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.current_channel_gain = obj;
                                            lv_obj_set_pos(obj, 130, 154);
                                            lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "0.0dB");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 114, 252);
                                            lv_obj_set_size(obj, 97, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "GAIN");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 574, 252);
                                            lv_obj_set_size(obj, 97, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "VOLUME");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 688, 252);
                                            lv_obj_set_size(obj, 97, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "PAN");
                                        }
                                        {
                                            // openx32_demo_knob
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            objects.openx32_demo_knob = obj;
                                            lv_obj_set_pos(obj, 426, 148);
                                            lv_obj_set_size(obj, 111, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_openx32_knob);
                                            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 434, 253);
                                            lv_obj_set_size(obj, 97, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "OpenX32 Knob Design");
                                        }
                                        {
                                            lv_obj_t *obj = lv_scale_create(parent_obj);
                                            objects.obj0 = obj;
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
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_line_color(obj, lv_color_hex(0xff555555), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_line_color(obj, lv_color_hex(0xff555555), LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_line_color(obj, lv_color_hex(0xff555555), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // config_phantom_button
                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                            objects.config_phantom_button = obj;
                                            lv_obj_set_pos(obj, 223, 39);
                                            lv_obj_set_size(obj, 80, LV_SIZE_CONTENT);
                                            lv_image_set_src(obj, &img_buttononoff);
                                            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TOP_LEFT);
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
                                            create_user_widget_ms_bar8(obj, 40);
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
                                            create_user_widget_ms_bar8(obj, 113);
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
                                            create_user_widget_ms_bar8(obj, 186);
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
                                            create_user_widget_ms_bar8(obj, 259);
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
                                            create_user_widget_ms_bar8(obj, 332);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 6, 133);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "1     2     3    4    5    6     7    8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 167, 133);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "9   10    11   12   13   14   15   16");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 324, 133);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "17   18   19  20  21   22  23  24");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 483, 133);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "25  26  27  28  29  30  31  32");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 647, 133);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "1     2     3    4    5    6    7     8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 295, 150);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Channel");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 706, 150);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "AUX");
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
                                            create_user_widget_ms_bar8(obj, 405);
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
                                            create_user_widget_ms_bar8(obj, 478);
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
                                            create_user_widget_ms_bar8(obj, 551);
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
                                            create_user_widget_ms_bar8(obj, 624);
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
                                            create_user_widget_ms_bar(obj, 697);
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 26, 306);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "1     2     3    4    5    6     7    8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 222, 306);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "1     2     3    4    5    6     7    8");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 382, 306);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "9   10    11   12   13   14   15   16");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 582, 306);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "1     2     3    4    5    6    S  M/C");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 61, 323);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "DSP2 Return");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 355, 322);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Mixbus");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 617, 323);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Matrix");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 746, 306);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "L/R\nMain");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 490, 363);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "DEBUG: Press Encoder 6 -> CH1-16 48V + Gain 47dB");
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
                                            objects.obj1 = obj;
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
                                            // table_routing_fpga_header
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.table_routing_fpga_header = obj;
                                            lv_obj_set_pos(obj, 148, 12);
                                            lv_obj_set_size(obj, 648, 30);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_set_style_pad_top(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // roller_routing_fpga_block
                                            lv_obj_t *obj = lv_roller_create(parent_obj);
                                            objects.roller_routing_fpga_block = obj;
                                            lv_obj_set_pos(obj, 4, 12);
                                            lv_obj_set_size(obj, 140, 304);
                                            lv_roller_set_options(obj, "Mixer Channels\nXLR Outputs\nAUX Outputs\nUltranet\nCARD Recording\nAES50 A Outputs\nAES50 B Outputs", LV_ROLLER_MODE_NORMAL);
                                        }
                                        {
                                            // table_routing_fpga
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.table_routing_fpga = obj;
                                            lv_obj_set_pos(obj, 148, 42);
                                            lv_obj_set_size(obj, 648, 274);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_set_style_pad_column(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Channel Routing (DSP)");
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
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Output-Routing (DSP)");
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
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff264056), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "BOARD VERSION INFO");
                                        }
                                        {
                                            // setup_versioninfotext
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.setup_versioninfotext = obj;
                                            lv_obj_set_pos(obj, 32, 47);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "...");
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
                                            lv_label_set_text(obj, "...");
                                        }
                                        {
                                            // setup_card_currentposition
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.setup_card_currentposition = obj;
                                            lv_obj_set_pos(obj, 31, 231);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "...");
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
                                            lv_label_set_text(obj, "X-Card-Source:");
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // setup_card_sourcemode
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.setup_card_sourcemode = obj;
                                                    lv_obj_set_pos(obj, 126, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "Text");
                                                }
                                            }
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 32, 98);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "X-Card-String:");
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // setup_card_detected
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.setup_card_detected = obj;
                                                    lv_obj_set_pos(obj, 126, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "-");
                                                }
                                            }
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 32, 47);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "Channelmode:");
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // setup_card_channelmode
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.setup_card_channelmode = obj;
                                                    lv_obj_set_pos(obj, 126, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "Text");
                                                }
                                            }
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 32, 154);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "SD-Card 1:");
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // setup_card_sd1info
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.setup_card_sd1info = obj;
                                                    lv_obj_set_pos(obj, 128, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "Text");
                                                }
                                            }
                                        }
                                        {
                                            // current_channel_source_2
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.current_channel_source_2 = obj;
                                            lv_obj_set_pos(obj, 31, 122);
                                            lv_obj_set_size(obj, 312, LV_SIZE_CONTENT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff264056), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "SD CARD INFORMATION (X-LIVE)");
                                        }
                                        {
                                            // current_channel_source_3
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.current_channel_source_3 = obj;
                                            lv_obj_set_pos(obj, 32, 9);
                                            lv_obj_set_size(obj, 312, LV_SIZE_CONTENT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff264056), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "EXPANSION CARD INFORMATION");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 31, 174);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "SD-Card 2:");
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // setup_card_sd2info
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.setup_card_sd2info = obj;
                                                    lv_obj_set_pos(obj, 129, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_text(obj, "Text");
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
                                            lv_obj_set_pos(obj, 285, 180);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "AES50-IP-Core: Markus Noll");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 268, 150);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "FPGA and DSPs: Chris Noeding");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 261, 120);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "User-Interface: Alexander Schulz");
                                        }
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 188, 63);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_text(obj, "https://www.github.com/OpenMixerProject/OpenX32");
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
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Prototype GUI");
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // roller_routing_block
                                            lv_obj_t *obj = lv_roller_create(parent_obj);
                                            objects.roller_routing_block = obj;
                                            lv_obj_set_pos(obj, -20, -12);
                                            lv_obj_set_size(obj, 134, 306);
                                            lv_roller_set_options(obj, "Mixer Channels\nXLR Outputs\nAUX Outputs\nUltranet\nCARD Recording\nAES50 A Outputs\nAES50 B Outputs", LV_ROLLER_MODE_NORMAL);
                                            lv_roller_set_selected(obj, 6, LV_ANIM_OFF);
                                        }
                                        {
                                            // table_routing_fpga_debug
                                            lv_obj_t *obj = lv_table_create(parent_obj);
                                            objects.table_routing_fpga_debug = obj;
                                            lv_obj_set_pos(obj, 124, -14);
                                            lv_obj_set_size(obj, 351, 308);
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
                            lv_label_set_text(obj, "Mute Groups");
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
                    create_user_widget_config_slider_display_encoder(obj, 746);
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
                    create_user_widget_config_slider_display_encoder(obj, 750);
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
                    create_user_widget_config_slider_display_encoder(obj, 754);
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
                    create_user_widget_config_slider_display_encoder(obj, 758);
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
                    create_user_widget_config_slider_display_encoder(obj, 762);
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
                    create_user_widget_config_slider_display_encoder(obj, 766);
                    add_style_display_encoder_panels(obj);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // debugtext_dsp2
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.debugtext_dsp2 = obj;
            lv_obj_set_pos(obj, 125, 32);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "DSP2 Debugtext");
        }
        {
            // debugtext_x32ctrl
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.debugtext_x32ctrl = obj;
            lv_obj_set_pos(obj, 125, 51);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "X32CTRL Debugtext");
        }
        {
            // debugtext_dsp1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.debugtext_dsp1 = obj;
            lv_obj_set_pos(obj, 125, 13);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "DSP1 Debugtext");
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
    tick_user_widget_ms_bar8(40);
    tick_user_widget_ms_bar8(113);
    tick_user_widget_ms_bar8(186);
    tick_user_widget_ms_bar8(259);
    tick_user_widget_ms_bar8(332);
    tick_user_widget_ms_bar8(405);
    tick_user_widget_ms_bar8(478);
    tick_user_widget_ms_bar8(551);
    tick_user_widget_ms_bar8(624);
    tick_user_widget_ms_bar(697);
    tick_user_widget_config_slider_display_encoder(746);
    tick_user_widget_config_slider_display_encoder(750);
    tick_user_widget_config_slider_display_encoder(754);
    tick_user_widget_config_slider_display_encoder(758);
    tick_user_widget_config_slider_display_encoder(762);
    tick_user_widget_config_slider_display_encoder(766);
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
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_panel_flat_black(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // panel_4
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
            lv_obj_set_pos(obj, 3, 12);
            lv_obj_set_size(obj, 14, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_panel_flat_black(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffab00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // panel_3
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 2] = obj;
            lv_obj_set_pos(obj, 3, 18);
            lv_obj_set_size(obj, 14, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_panel_flat_black(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffab00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // panel_2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 3] = obj;
            lv_obj_set_pos(obj, 3, 24);
            lv_obj_set_size(obj, 14, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_panel_flat_black(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffab00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // panel_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 4] = obj;
            lv_obj_set_pos(obj, 3, 30);
            lv_obj_set_size(obj, 14, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_panel_flat_black(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // panel_0
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 5] = obj;
            lv_obj_set_pos(obj, 3, 36);
            lv_obj_set_size(obj, 14, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_panel_flat_black(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff565353), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_label_set_text(obj, "Label");
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
            lv_label_set_text(obj, "Center");
        }
    }
}

void tick_user_widget_config_slider_display_encoder(int startWidgetIndex) {
    (void)startWidgetIndex;
}

void create_user_widget_config_slider(lv_obj_t *parent_obj, int startWidgetIndex) {
    (void)startWidgetIndex;
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            // label_1
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_PCT(50), LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "");
        }
        {
            // value_1
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
            lv_obj_set_pos(obj, 65, 0);
            lv_obj_set_size(obj, LV_PCT(50), LV_SIZE_CONTENT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "");
        }
        {
            // slider_1
            lv_obj_t *obj = lv_slider_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 2] = obj;
            lv_obj_set_pos(obj, 5, 22);
            lv_obj_set_size(obj, 120, 4);
            add_style_slider_config(obj);
        }
    }
}

void tick_user_widget_config_slider(int startWidgetIndex) {
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
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff167ada), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_label_set_text(obj, "Empty");
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
//
//

void create_screens() {

// Set default LVGL theme
    lv_display_t *dispp = lv_display_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_display_set_theme(dispp, theme);
    
    // Initialize screens
    // Create screens
    create_screen_main();
}