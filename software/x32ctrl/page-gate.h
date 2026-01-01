#pragma once

#include "page.h"

using namespace std;

class PageGate: public Page {
    public:
        PageGate(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE_CONFIG;
            nextPage = X32_PAGE_COMPRESSOR;
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
            //chart-shadow: 0x7e4000
        }

        void OnShow() override {
            DrawGate(config->selectedVChannel);
            EncoderText(config->selectedVChannel);
        }

        void OnChange() override {
            uint8_t chanIndex = config->selectedVChannel;
            VChannel* chan = mixer->GetVChannel(chanIndex);

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_GATE) || state->HasChanged(X32_MIXER_CHANGED_SELECT)){
                DrawGate(chanIndex);
                EncoderText(chanIndex);
            }
        }

        void OnTime10ms() override {
            int32_t gateValueAudioDbfs = -120;

            uint8_t selectedChannelIndex = config->selectedVChannel;
            if (selectedChannelIndex < 40) {
                gateValueAudioDbfs = helper->sample2Dbfs(mixer->dsp->rChannel[selectedChannelIndex].meter6Decay) * 100.0f;
            }

            // add new value to chart
            lv_chart_set_next_value(objects.current_channel_gate, chartSeriesGateAudio, gateValueAudioDbfs);
            //lv_chart_refresh(objects.current_channel_gate);
        }

        void OnDisplayButton(X32_BTN button, bool pressed) override {
            if (pressed){
				switch (button){
					case X32_BTN_ENCODER1:
						mixer->SetGate(config->selectedVChannel, 'T', -80.0f);
						break;
					case X32_BTN_ENCODER2:
						mixer->SetGate(config->selectedVChannel, 'R', 60.0f);
						break;
					case X32_BTN_ENCODER3:
						mixer->SetGate(config->selectedVChannel, 'A', 10.0f);
						break;
					case X32_BTN_ENCODER4:
						mixer->SetGate(config->selectedVChannel, 'H', 50.0f);
						break;
					case X32_BTN_ENCODER5:
						mixer->SetGate(config->selectedVChannel, 'r', 250.0f);
						break;
					case X32_BTN_ENCODER6:
						break;
					default:
						break;
				}
			}
        }

        void OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) {
            switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeGate(config->selectedVChannel, 'T', amount);
					break;
				case X32_ENC_ENCODER2:
					mixer->ChangeGate(config->selectedVChannel, 'R', amount);
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangeGate(config->selectedVChannel, 'A', amount);
					break;
				case X32_ENC_ENCODER4:
					mixer->ChangeGate(config->selectedVChannel, 'H', amount);
					break;
				case X32_ENC_ENCODER5:
					mixer->ChangeGate(config->selectedVChannel, 'r', amount);
					break;
				case X32_ENC_ENCODER6:
					break;
				default:
					break;
			}
        }

    private:

        lv_chart_series_t* chartSeriesGate;
        lv_chart_series_t* chartSeriesGateAudio;

        void EncoderText(uint8_t chanIndex) {
            if (chanIndex < 40) {
                SetEncoderText(
                    "Thresh: " + String(mixer->dsp->Channel[chanIndex].gate.threshold, 1) + " dB",
                    "Range: " + String(mixer->dsp->Channel[chanIndex].gate.range, 1) + " dB",
                    "Attack: " + String(mixer->dsp->Channel[chanIndex].gate.attackTime_ms, 0) + " ms",
                    "Hold: " + String(mixer->dsp->Channel[chanIndex].gate.holdTime_ms, 0) + " ms",
                    "Release: " + String(mixer->dsp->Channel[chanIndex].gate.releaseTime_ms, 0) + " ms",
                    "-"
                );
            }  else {
                SetEncoderText("-", "-", "-", "-", "-", "-");
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

            sGate* gate = &mixer->dsp->Channel[config->selectedVChannel].gate;

            for (uint16_t pixel = 0; pixel < 200; pixel++) {
                inputLevel = (60.0f * ((float)pixel/199.0f)) - 60.0f; // from -60 dB to 0 dB

                if (inputLevel >= gate->threshold) {
                    // above threshold
                    outputLevel = inputLevel;
                }else{
                    // below threshold -> apply reduction
                    outputLevel = inputLevel - gate->range;
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