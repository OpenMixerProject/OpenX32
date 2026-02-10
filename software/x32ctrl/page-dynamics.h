#pragma once

#include "page.h"

using namespace std;

class PageDynamics: public Page {
    public:
        PageDynamics(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::GATE;
            nextPage = X32_PAGE::EQ;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 3;
            led = X32_BTN_VIEW_COMPRESSOR;
            noLedOnRack = true;
        }

        void OnInit() override {
            // setup Dynamics-graph
            lv_chart_set_type(objects.current_channel_comp, LV_CHART_TYPE_LINE);
            lv_obj_set_style_size(objects.current_channel_comp, 0, 0, LV_PART_INDICATOR);
            lv_obj_set_style_line_width(objects.current_channel_comp, 5, LV_PART_ITEMS);
            chartSeriesCompressor = lv_chart_add_series(objects.current_channel_comp, lv_palette_main(LV_PALETTE_AMBER), LV_CHART_AXIS_PRIMARY_Y);
            chartSeriesCompressorAudio = lv_chart_add_series(objects.current_channel_comp, lv_palette_main(LV_PALETTE_BLUE_GREY), LV_CHART_AXIS_PRIMARY_Y);
            lv_chart_set_div_line_count(objects.current_channel_comp, 5, 7);
            lv_chart_set_range(objects.current_channel_comp, LV_CHART_AXIS_PRIMARY_Y, -6000, 0);
            lv_chart_set_point_count(objects.current_channel_comp, 200); // with incoming audio every 10ms we see 2 seconds of audio-history
            lv_chart_set_series_color(objects.current_channel_comp, chartSeriesCompressor, lv_color_hex(0xef7900));

            // register draw event callback to change color of audio level depending on gate threshold
            lv_obj_add_event_cb(objects.current_channel_comp, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
            lv_obj_add_flag(objects.current_channel_comp, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

            // store mixer pointer in user data for use in draw callback
            lv_obj_set_user_data(objects.current_channel_comp, mixer);
            //chart-shadow: 0x7e4000

            UpdateEncoderBinding();
        }

        void UpdateEncoderBinding()
        {
            using enum MP_ID;

            uint targetindex = config->GetUint(MP_ID::SELECTED_CHANNEL);

            BindEncoder(DISPLAY_ENCODER_1, MP_ID::CHANNEL_DYNAMICS_TRESHOLD, targetindex);
            BindEncoder(DISPLAY_ENCODER_2, MP_ID::CHANNEL_DYNAMICS_RATIO, targetindex);
            BindEncoder(DISPLAY_ENCODER_3, MP_ID::CHANNEL_DYNAMICS_MAKEUP, targetindex);
            BindEncoder(DISPLAY_ENCODER_4, MP_ID::CHANNEL_DYNAMICS_ATTACK, targetindex);
            BindEncoder(DISPLAY_ENCODER_5, MP_ID::CHANNEL_DYNAMICS_HOLD, targetindex);
            BindEncoder(DISPLAY_ENCODER_6, MP_ID::CHANNEL_DYNAMICS_RELEASE, targetindex);

            SyncEncoderWidgets(true);
        }

        void OnChange(bool force) override 
        {
            if (config->HasParameterChanged(MP_ID::SELECTED_CHANNEL))
            {
                UpdateEncoderBinding();
            }
        }

        void OnUpdateMeters() override {
            int32_t compValueAudioDbfs = -120;

            uint8_t selectedChannelIndex = config->GetUint(MP_ID::SELECTED_CHANNEL);
            if (selectedChannelIndex < 40) {
                compValueAudioDbfs = helper->sample2Dbfs(mixer->dsp->rChannel[selectedChannelIndex].meterDecay) * 100.0f;
            }

            // add new value to chart
            lv_chart_set_next_value(objects.current_channel_comp, chartSeriesCompressorAudio, compValueAudioDbfs);
            //lv_chart_refresh(objects.current_channel_comp);
        }

        bool OnDisplayButton(X32_BTN button, bool pressed) override {
            bool message_handled = false;

            if (pressed){
                message_handled = true;

				switch (button){
					case X32_BTN_ENCODER1:
						mixer->SetDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'T', 0.0f);
						break;
					case X32_BTN_ENCODER2:
						mixer->SetDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'R', 60.0f);
						break;
					case X32_BTN_ENCODER3:
						mixer->SetDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'M', 0.0f);
						break;
					case X32_BTN_ENCODER4:
						mixer->SetDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'A', 10.0f);
						break;
					case X32_BTN_ENCODER5:
						mixer->SetDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'H', 10.0f);
						break;
					case X32_BTN_ENCODER6:
						mixer->SetDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'r', 150.0f);
						break;
					default:
                        message_handled = false;
						break;
				}
            }

            return message_handled;
        }

        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override {
            bool message_handled = true;

            switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'T', amount);
					break;
				case X32_ENC_ENCODER2:
					mixer->ChangeDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'R', amount);
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangeDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'M', amount);
					break;
				case X32_ENC_ENCODER4:
					mixer->ChangeDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'A', amount);
					break;
				case X32_ENC_ENCODER5:
					mixer->ChangeDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'H', amount);
					break;
				case X32_ENC_ENCODER6:
					mixer->ChangeDynamics(config->GetUint(MP_ID::SELECTED_CHANNEL), 'r', amount);
					break;
				default:
                    message_handled = false;
					break;
			}

            return message_handled;
        }

    private:
        lv_chart_series_t* chartSeriesCompressor;
        lv_chart_series_t* chartSeriesCompressorAudio;

        static void draw_event_cb(lv_event_t * e) {
            lv_draw_task_t* draw_task = lv_event_get_draw_task(e);
            lv_draw_dsc_base_t* base_dsc = (lv_draw_dsc_base_t*)lv_draw_task_get_draw_dsc(draw_task);

            if (base_dsc->part != LV_PART_ITEMS) {
                return;
            }

            if (lv_draw_task_get_type(draw_task) != LV_DRAW_TASK_TYPE_LINE) {
                return;
            }

            lv_obj_t* chart = (lv_obj_t*)lv_event_get_target(e);
            lv_obj_t* obj = (lv_obj_t*)lv_event_get_target_obj(e);

            uint32_t series_id = base_dsc->id1;
            //uint32_t point_id = base_dsc->id2; // this does not work for moving charts

            // only change audio-level line
            if (series_id != 1) {
                return; // series_id 1 = audio level
            }

            // find desired series
            lv_chart_series_t* series = lv_chart_get_series_next(chart, NULL);
            for (uint32_t i = 0; i < series_id; i++) {
                series = lv_chart_get_series_next(chart, series);
            }

            lv_draw_line_dsc_t* line_dsc = lv_draw_task_get_line_dsc(draw_task);
            if (line_dsc) {
                // change color depending on gate threshold

                // find the first point (left) of chart and add 199 to get the last point (right) being drawn
                uint32_t point_id = lv_chart_get_x_start_point(chart, series) + lv_chart_get_point_count(chart) - 1;
                if (point_id >= lv_chart_get_point_count(chart)) {
                    point_id -= lv_chart_get_point_count(chart);
                }

                int32_t* y_array = lv_chart_get_series_y_array(chart, series); //lv_chart_get_y_array
                int32_t value = y_array[point_id];

                Mixer* mixer = (Mixer*)lv_obj_get_user_data(obj);
                if (value >= mixer->dsp->Channel[mixer->GetSelectedVChannelIndex()].compressor.threshold * 100.0f) {
                    line_dsc->color = lv_palette_main(LV_PALETTE_ORANGE);
                } else {
                    line_dsc->color = lv_palette_main(LV_PALETTE_BLUE_GREY);
                }
            }
        }

        void DrawDynamics(uint8_t selectedChannelIndex) {
            if (selectedChannelIndex >= 40) {
                return;
            }

            // calculate the compressor curve
            float compValue[200];
            float inputLevel;
            float outputLevel;

            memset(&compValue[0], 0, sizeof(compValue));

            sCompressor* comp = &mixer->dsp->Channel[config->GetUint(MP_ID::SELECTED_CHANNEL)].compressor;

            for (uint16_t pixel = 0; pixel < 200; pixel++) {
                inputLevel = (60.0f * ((float)pixel/199.0f)) - 60.0f; // from -60 dB to 0 dB

                if (inputLevel < comp->threshold) {
                    // below threshold
                    outputLevel = inputLevel;
                }else{
                    // above threshold -> calculate overshoot (inputLevel - comp->threshold) and take ratio into account
                    outputLevel = comp->threshold + ((inputLevel - comp->threshold) / comp->ratio);
                }

                // scale outputLevel to -6000 .. 0
                compValue[pixel] = outputLevel * 100.0f;
            }

            int32_t* chartSeriesCompPoints = lv_chart_get_series_y_array(objects.current_channel_comp, chartSeriesCompressor);
            for (uint16_t i = 0; i < 200; i++) {
                chartSeriesCompPoints[i] = compValue[i];
            }
            lv_chart_refresh(objects.current_channel_comp);
        }
};