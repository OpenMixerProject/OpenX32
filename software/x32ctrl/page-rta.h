#pragma once
#include "page.h"

using namespace std;
using enum MP_ID;

class PageRta : public Page 
{
     private:
        lv_chart_series_t* chartSeriesRta;
        uint8_t updateRateReducer = 0;

        static void draw_event_cb(lv_event_t * e)
        {
            lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
            lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);

            if(base_dsc->part != LV_PART_ITEMS) {
                return;
            }

            lv_draw_fill_dsc_t * fill_dsc = lv_draw_task_get_fill_dsc(draw_task);
            if(fill_dsc) {
                lv_obj_t * chart = lv_event_get_target_obj(e);
                int32_t * y_array = lv_chart_get_series_y_array(chart, lv_chart_get_series_next(chart, NULL));
                int32_t v = y_array[base_dsc->id2];

                uint8_t ratio = (uint8_t)(v * 255 / -1200);
                fill_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_GREEN), lv_palette_main(LV_PALETTE_RED), ratio);
            }
        }

    public:
        PageRta(PageBaseParameter* pagebasepar) : Page(pagebasepar)
        {
            prevPage = X32_PAGE::METERS;
            tabLayer0 = objects.maintab;
            tabIndex0 = 1;
            tabLayer1 = objects.meterstab;
            tabIndex1 = 1;
        }

        void OnInit() override
        {
             // setup bar-graph for RTA-analyzer
            lv_chart_set_type(objects.rta_chart, LV_CHART_TYPE_BAR);
            lv_chart_set_point_count(objects.rta_chart, 64);
            lv_chart_set_range(objects.rta_chart, LV_CHART_AXIS_PRIMARY_Y, -1200, 0);
            lv_obj_set_style_pad_column(objects.rta_chart, -1, LV_PART_MAIN);
            lv_obj_set_style_pad_column(objects.rta_chart, -1, LV_PART_ITEMS);
            lv_obj_set_size(objects.rta_chart, 640, 275);

            chartSeriesRta = lv_chart_add_series(objects.rta_chart, lv_color_hex(0xff0000), LV_CHART_AXIS_PRIMARY_Y);
            lv_obj_add_event_cb(objects.rta_chart, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
            lv_obj_add_flag(objects.rta_chart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

            uint dsp2_rta_input = MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS - 1;
            BindEncoder(DISPLAY_ENCODER_1, ROUTING_DSP_OUTPUT, dsp2_rta_input);
        }

        void OnUpdateMeters() override
        {
            //if (updateRateReducer < 1) {
            //    updateRateReducer++;
            //    return;
            //}
            updateRateReducer = 0;

            for (uint8_t i = 0; i < 64; i++) {
                // convert linear value to logarithmic value
                float currentValue = helper->sample2Dbfs(mixer->dsp->rtaData[i]); // value between -120 ... 0dBfs
                currentValue *= 10.0f;
                lv_chart_set_next_value(objects.rta_chart, chartSeriesRta, (int32_t)currentValue);
            }
        }      
};