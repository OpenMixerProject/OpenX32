#pragma once

#include "page.h"

using namespace std;

class PageGate: public Page {
    public:
        PageGate(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::CONFIG;
            nextPage = X32_PAGE::COMPRESSOR;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 2;
            led = X32_BTN_VIEW_GATE;
            noLedOnRack = true;
        }

        void OnInit() override {
            // setup Gate-graph
            lv_chart_set_type(objects.current_channel_gate, LV_CHART_TYPE_LINE);
            lv_obj_set_style_size(objects.current_channel_gate, 0, 0, LV_PART_INDICATOR);
            lv_obj_set_style_line_width(objects.current_channel_gate, 5, LV_PART_ITEMS);
            chartSeriesGate = lv_chart_add_series(objects.current_channel_gate, lv_palette_main(LV_PALETTE_AMBER), LV_CHART_AXIS_PRIMARY_Y);
            chartSeriesGateAudio = lv_chart_add_series(objects.current_channel_gate, lv_palette_main(LV_PALETTE_BLUE_GREY), LV_CHART_AXIS_PRIMARY_Y);
            lv_chart_set_div_line_count(objects.current_channel_gate, 5, 7);
            lv_chart_set_range(objects.current_channel_gate, LV_CHART_AXIS_PRIMARY_Y, -6000, 0);
            lv_chart_set_point_count(objects.current_channel_gate, 200); // with incoming audio every 10ms we see 2 seconds of audio-history
            lv_chart_set_series_color(objects.current_channel_gate, chartSeriesGate, lv_color_hex(0xef7900));

            // register draw event callback to change color of audio level depending on gate threshold
            lv_obj_add_event_cb(objects.current_channel_gate, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
            lv_obj_add_flag(objects.current_channel_gate, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

            // store config pointer in user data for use in draw callback
            lv_obj_set_user_data(objects.current_channel_gate, config);
            //chart-shadow: 0x7e4000
        }

        void UpdateEncoderBinding(uint chanindex) 
        {
            using enum MP_ID;

            BindEncoder(DISPLAY_ENCODER_1, CHANNEL_GATE_TRESHOLD, chanindex);
            BindEncoder(DISPLAY_ENCODER_2, CHANNEL_GATE_RANGE, chanindex);
            BindEncoder(DISPLAY_ENCODER_3, CHANNEL_GATE_ATTACK, chanindex);
            BindEncoder(DISPLAY_ENCODER_4, CHANNEL_GATE_HOLD, chanindex);
            BindEncoder(DISPLAY_ENCODER_5, CHANNEL_GATE_RELEASE, chanindex);

            SyncEncoderWidgets(true);
        }

        void OnChange(bool force_update) override {
            using enum MP_ID;

            uint chanIndex = config->GetUint(SELECTED_CHANNEL);
            VChannel* chan = mixer->GetVChannel(chanIndex);

            if (config->HasParametersChanged({CHANNEL_GATE_ATTACK, CHANNEL_GATE_HOLD, CHANNEL_GATE_RANGE, CHANNEL_GATE_RELEASE, CHANNEL_GATE_TRESHOLD}, chanIndex) ||
                config->HasParameterChanged(SELECTED_CHANNEL) ||
                force_update)
            {
                DrawGate(chanIndex);
                UpdateEncoderBinding(chanIndex);
            }
        }

        void OnUpdateMeters() override {
            int32_t gateValueAudioDbfs = -120;

            uint8_t selectedChannelIndex = config->GetUint(MP_ID::SELECTED_CHANNEL);
            if (selectedChannelIndex < 40) {
                gateValueAudioDbfs = helper->sample2Dbfs(mixer->dsp->rChannel[selectedChannelIndex].meterDecay) * 100.0f;
            }
            
            // add new value to chart
            lv_chart_set_next_value(objects.current_channel_gate, chartSeriesGateAudio, gateValueAudioDbfs);

            //lv_chart_refresh(objects.current_channel_gate);
        }

        // bool OnDisplayButton(X32_BTN button, bool pressed) override {
        //     if (pressed){
		// 		switch (button){
		// 			case X32_BTN_ENCODER1:
		// 				mixer->SetGate(config->GetUint(SELECTED_CHANNEL), 'T', GATE_THRESHOLD_MIN);
		// 				break;
		// 			case X32_BTN_ENCODER2:
		// 				mixer->SetGate(config->GetUint(SELECTED_CHANNEL), 'R', GATE_RANGE_MAX);
		// 				break;
		// 			case X32_BTN_ENCODER3:
		// 				mixer->SetGate(config->GetUint(SELECTED_CHANNEL), 'A', 10.0f);
		// 				break;
		// 			case X32_BTN_ENCODER4:
		// 				mixer->SetGate(config->GetUint(SELECTED_CHANNEL), 'H', 50.0f);
		// 				break;
		// 			case X32_BTN_ENCODER5:
		// 				mixer->SetGate(config->GetUint(SELECTED_CHANNEL), 'r', 250.0f);
		// 				break;
		// 			case X32_BTN_ENCODER6:
		// 				break;
		// 			default:
		// 				break;
		// 		}
		// 	}

        //     return true;
        // }

        // bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override {
        //     switch (encoder){
		// 		case X32_ENC_ENCODER1:
		// 			mixer->ChangeGate(config->GetUint(SELECTED_CHANNEL), 'T', amount);
		// 			break;
		// 		case X32_ENC_ENCODER2:
		// 			mixer->ChangeGate(config->GetUint(SELECTED_CHANNEL), 'R', amount);
		// 			break;
		// 		case X32_ENC_ENCODER3:
		// 			mixer->ChangeGate(config->GetUint(SELECTED_CHANNEL), 'A', amount);
		// 			break;
		// 		case X32_ENC_ENCODER4:
		// 			mixer->ChangeGate(config->GetUint(SELECTED_CHANNEL), 'H', amount);
		// 			break;
		// 		case X32_ENC_ENCODER5:
		// 			mixer->ChangeGate(config->GetUint(SELECTED_CHANNEL), 'r', amount);
		// 			break;
		// 		case X32_ENC_ENCODER6:
		// 			break;
		// 		default:
		// 			break;
		// 	}

        //     return false;
        // }

    private:
        lv_chart_series_t* chartSeriesGate;
        lv_chart_series_t* chartSeriesGateAudio;

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

                Config* config = (Config*)lv_obj_get_user_data(obj);
                if (value >= config->GetFloat(MP_ID::CHANNEL_GATE_TRESHOLD, config->GetUint(MP_ID::SELECTED_CHANNEL)) * 100.0f) {
                    line_dsc->color = lv_palette_main(LV_PALETTE_GREEN);
                } else {
                    line_dsc->color = lv_palette_main(LV_PALETTE_BLUE_GREY);
                }
            }
        }

        void DrawGate(uint8_t selectedChannelIndex) {
            if (selectedChannelIndex >= 40) {
                return;
            }

            // calculate the gate curve
            float gateValue[200];
            float inputLevel;
            float outputLevel;

            memset(&gateValue[0], 0, sizeof(gateValue));

            for (uint16_t pixel = 0; pixel < 200; pixel++) {
                inputLevel = (60.0f * ((float)pixel/199.0f)) - 60.0f; // from -60 dB to 0 dB

                if (inputLevel >= config->GetFloat(MP_ID::CHANNEL_GATE_TRESHOLD)) {
                    // above threshold
                    outputLevel = inputLevel;
                }else{
                    // below threshold -> apply reduction
                    outputLevel = inputLevel - config->GetFloat(MP_ID::CHANNEL_GATE_RANGE);
                }

                // scale outputLevel to -6000 .. 0
                gateValue[pixel] = outputLevel * 100.0f;
            }

            int32_t* chartSeriesGatePoints = lv_chart_get_series_y_array(objects.current_channel_gate, chartSeriesGate);
            for (uint16_t i = 0; i < 200; i++) {
                chartSeriesGatePoints[i] = gateValue[i];
            }
            lv_chart_refresh(objects.current_channel_gate);
        }
};