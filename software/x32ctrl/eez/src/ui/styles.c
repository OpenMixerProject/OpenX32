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
        style = lv_malloc(sizeof(lv_style_t));
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
        style = lv_malloc(sizeof(lv_style_t));
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
    lv_style_set_bg_color(style, lv_color_hex(0xff838383));
};

lv_style_t *get_style_slider_fader_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_KNOB_DEFAULT(style);
    }
    return style;
};

void init_style_slider_fader_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff313131));
};

lv_style_t *get_style_slider_fader_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_slider_fader(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slider_fader_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_fader_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_slider_fader(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slider_fader_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_fader_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: slider_fader_solo
//

void init_style_slider_fader_solo_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffff00));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_slider_fader_solo_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_solo_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_slider_fader_solo_INDICATOR_DEFAULT(lv_style_t *style) {
    init_style_slider_fader_INDICATOR_DEFAULT(style);
    
    lv_style_set_bg_color(style, lv_color_hex(0xffffff00));
};

lv_style_t *get_style_slider_fader_solo_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
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
        style = lv_malloc(sizeof(lv_style_t));
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
    
    lv_style_set_bg_color(style, lv_color_hex(0xffff0000));
};

lv_style_t *get_style_slider_fader_mute_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
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
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_fader_mute_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_slider_fader_mute(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slider_fader_mute_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_fader_mute_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_slider_fader_mute(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slider_fader_mute_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_fader_mute_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
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
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_panel_flat_black_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
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
        add_style_panel_flat_black,
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
        remove_style_panel_flat_black,
    };
    remove_style_funcs[styleIndex](obj);
}

