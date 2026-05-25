#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: tabview_nopad
//

void init_style_tabview_nopad_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
    lv_style_set_pad_top(style, 0);
};

lv_style_t *get_style_tabview_nopad_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_tabview_nopad_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_tabview_nopad(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_tabview_nopad_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_tabview_nopad(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_tabview_nopad_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: tab_nopad
//

void init_style_tab_nopad_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
    lv_style_set_pad_top(style, 0);
};

lv_style_t *get_style_tab_nopad_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_tab_nopad_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_tab_nopad(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_tab_nopad_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_tab_nopad(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_tab_nopad_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: slider_fader
//

void init_style_slider_fader_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_radius(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(0x838383));
};

lv_style_t *get_style_slider_fader_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_KNOB_DEFAULT(style);
    }
    return style;
};

void init_style_slider_fader_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffff));
    lv_style_set_bg_opa(style, 51);
};

lv_style_t *get_style_slider_fader_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_slider_fader_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffff));
};

lv_style_t *get_style_slider_fader_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_slider_fader(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slider_fader_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_fader_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_fader_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_slider_fader(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slider_fader_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_fader_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_fader_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: slider_fader_solo
//

void init_style_slider_fader_solo_MAIN_DEFAULT(lv_style_t *style) {
    init_style_slider_fader_MAIN_DEFAULT(style);
    
    lv_style_set_bg_color(style, lv_color_hex(0xffff00));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_slider_fader_solo_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_solo_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_slider_fader_solo_INDICATOR_DEFAULT(lv_style_t *style) {
    init_style_slider_fader_INDICATOR_DEFAULT(style);
    
    lv_style_set_bg_color(style, lv_color_hex(0xffff00));
};

lv_style_t *get_style_slider_fader_solo_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_solo_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_slider_fader_solo_KNOB_DEFAULT(lv_style_t *style) {
    init_style_slider_fader_KNOB_DEFAULT(style);
    
};

lv_style_t *get_style_slider_fader_solo_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_solo_KNOB_DEFAULT(style);
    }
    return style;
};

void add_style_slider_fader_solo(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slider_fader_solo_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_fader_solo_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_fader_solo_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

void remove_style_slider_fader_solo(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slider_fader_solo_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_fader_solo_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_fader_solo_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

//
// Style: slider_fader_mute
//

void init_style_slider_fader_mute_KNOB_DEFAULT(lv_style_t *style) {
    init_style_slider_fader_KNOB_DEFAULT(style);
    
    lv_style_set_bg_color(style, lv_color_hex(0xff0000));
};

lv_style_t *get_style_slider_fader_mute_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_mute_KNOB_DEFAULT(style);
    }
    return style;
};

void init_style_slider_fader_mute_INDICATOR_DEFAULT(lv_style_t *style) {
    init_style_slider_fader_INDICATOR_DEFAULT(style);
    
};

lv_style_t *get_style_slider_fader_mute_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_mute_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_slider_fader_mute_MAIN_DEFAULT(lv_style_t *style) {
    init_style_slider_fader_MAIN_DEFAULT(style);
    
};

lv_style_t *get_style_slider_fader_mute_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_mute_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_slider_fader_mute(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slider_fader_mute_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_fader_mute_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_fader_mute_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_slider_fader_mute(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slider_fader_mute_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_fader_mute_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_fader_mute_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: slider_config
//

void init_style_slider_config_KNOB_DEFAULT(lv_style_t *style) {
    init_style_slider_fader_KNOB_DEFAULT(style);
    
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_slider_config_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_config_KNOB_DEFAULT(style);
    }
    return style;
};

void init_style_slider_config_INDICATOR_DEFAULT(lv_style_t *style) {
    init_style_slider_fader_INDICATOR_DEFAULT(style);
    
    lv_style_set_bg_opa(style, 255);
    lv_style_set_bg_color(style, lv_color_hex(0xffff00));
    lv_style_set_radius(style, 0);
};

lv_style_t *get_style_slider_config_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_config_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_slider_config_MAIN_DEFAULT(lv_style_t *style) {
    init_style_slider_fader_MAIN_DEFAULT(style);
    
    lv_style_set_bg_color(style, lv_color_hex(0xffffff));
    lv_style_set_bg_opa(style, 100);
    lv_style_set_radius(style, 0);
};

lv_style_t *get_style_slider_config_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_config_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_slider_config(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slider_config_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_config_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_config_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_slider_config(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slider_config_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_config_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_config_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: panel_flat_dark_grey
//

void init_style_panel_flat_dark_grey_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(0x292929));
};

lv_style_t *get_style_panel_flat_dark_grey_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_flat_dark_grey_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_flat_dark_grey(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_flat_dark_grey_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_flat_dark_grey(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_flat_dark_grey_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: panel_flat_black
//

void init_style_panel_flat_black_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(0x15171a));
};

lv_style_t *get_style_panel_flat_black_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_flat_black_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_flat_black(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_flat_black_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_flat_black(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_flat_black_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: panel_flat_bg
//

void init_style_panel_flat_bg_MAIN_DEFAULT(lv_style_t *style) {
    init_style_panel_flat_black_MAIN_DEFAULT(style);
    
    lv_style_set_bg_color(style, lv_color_hex(0x282b30));
};

lv_style_t *get_style_panel_flat_bg_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_flat_bg_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_flat_bg(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_flat_bg_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_flat_bg(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_flat_bg_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: label_bg_yellow
//

void init_style_label_bg_yellow_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffff00));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(0x000000));
};

lv_style_t *get_style_label_bg_yellow_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_bg_yellow_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_bg_yellow(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_bg_yellow_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_bg_yellow(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_bg_yellow_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: display_encoder_panels
//

void init_style_display_encoder_panels_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_radius(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(0x282b30));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_display_encoder_panels_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_display_encoder_panels_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_display_encoder_panels(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_display_encoder_panels_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_display_encoder_panels(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_display_encoder_panels_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: table_standard
//

void init_style_table_standard_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_top(style, 5);
    lv_style_set_pad_bottom(style, 2);
    lv_style_set_pad_left(style, 5);
    lv_style_set_pad_right(style, 5);
};

lv_style_t *get_style_table_standard_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_table_standard_ITEMS_DEFAULT(style);
    }
    return style;
};

void add_style_table_standard(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_table_standard_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
};

void remove_style_table_standard(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_table_standard_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
};

//
// Style: roller_standard
//

void init_style_roller_standard_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 1);
    lv_style_set_pad_column(style, 1);
    lv_style_set_radius(style, 0);
    lv_style_set_text_line_space(style, 10);
};

lv_style_t *get_style_roller_standard_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_roller_standard_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_roller_standard_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_text_opa(style, 0);
};

lv_style_t *get_style_roller_standard_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_roller_standard_MAIN_DISABLED(style);
    }
    return style;
};

void init_style_roller_standard_SELECTED_DISABLED(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
};

lv_style_t *get_style_roller_standard_SELECTED_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_roller_standard_SELECTED_DISABLED(style);
    }
    return style;
};

void add_style_roller_standard(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_roller_standard_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_roller_standard_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_add_style(obj, get_style_roller_standard_SELECTED_DISABLED(), LV_PART_SELECTED | LV_STATE_DISABLED);
};

void remove_style_roller_standard(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_roller_standard_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_roller_standard_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_remove_style(obj, get_style_roller_standard_SELECTED_DISABLED(), LV_PART_SELECTED | LV_STATE_DISABLED);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_tabview_nopad,
        add_style_tab_nopad,
        add_style_slider_fader,
        add_style_slider_fader_solo,
        add_style_slider_fader_mute,
        add_style_slider_config,
        add_style_panel_flat_dark_grey,
        add_style_panel_flat_black,
        add_style_panel_flat_bg,
        add_style_label_bg_yellow,
        add_style_display_encoder_panels,
        add_style_table_standard,
        add_style_roller_standard,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_tabview_nopad,
        remove_style_tab_nopad,
        remove_style_slider_fader,
        remove_style_slider_fader_solo,
        remove_style_slider_fader_mute,
        remove_style_slider_config,
        remove_style_panel_flat_dark_grey,
        remove_style_panel_flat_black,
        remove_style_panel_flat_bg,
        remove_style_label_bg_yellow,
        remove_style_display_encoder_panels,
        remove_style_table_standard,
        remove_style_roller_standard,
    };
    remove_style_funcs[styleIndex](obj);
}