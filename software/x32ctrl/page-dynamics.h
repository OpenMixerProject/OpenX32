#pragma once

#include "page.h"

using namespace std;

class PageDynamics: public Page {
    public:
        PageDynamics(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE_GATE;
            nextPage = X32_PAGE_EQ;
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
            //chart-shadow: 0x7e4000
        }

        void OnShow() override {
            DrawDynamics(config->selectedVChannel);
            EncoderText(config->selectedVChannel);
        }

        void OnChange() override {
            uint8_t chanIndex = config->selectedVChannel;
            VChannel* chan = mixer->GetVChannel(chanIndex);

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_DYNAMIC) || state->HasChanged(X32_MIXER_CHANGED_SELECT)){
                DrawDynamics(chanIndex);
                EncoderText(chanIndex);
            }
        }

        void OnTime10ms() override {
            int32_t compValueAudioDbfs = -120;

            uint8_t selectedChannelIndex = config->selectedVChannel;
            if (selectedChannelIndex < 40) {
                compValueAudioDbfs = helper->sample2Dbfs(mixer->dsp->rChannel[selectedChannelIndex].meterDecay) * 100.0f;
            }

            // add new value to chart
            lv_chart_set_next_value(objects.current_channel_comp, chartSeriesCompressorAudio, compValueAudioDbfs);
            //lv_chart_refresh(objects.current_channel_comp);
        }

        void OnDisplayButton(X32_BTN button, bool pressed) override {
            if (pressed){
				switch (button){
					case X32_BTN_ENCODER1:
						mixer->SetDynamics(config->selectedVChannel, 'T', 0.0f);
						break;
					case X32_BTN_ENCODER2:
						mixer->SetDynamics(config->selectedVChannel, 'R', 60.0f);
						break;
					case X32_BTN_ENCODER3:
						mixer->SetDynamics(config->selectedVChannel, 'M', 0.0f);
						break;
					case X32_BTN_ENCODER4:
						mixer->SetDynamics(config->selectedVChannel, 'A', 10.0f);
						break;
					case X32_BTN_ENCODER5:
						mixer->SetDynamics(config->selectedVChannel, 'H', 10.0f);
						break;
					case X32_BTN_ENCODER6:
						mixer->SetDynamics(config->selectedVChannel, 'r', 150.0f);
						break;
					default:
						break;
				}
            }
        }

        void OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) {
            switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeDynamics(config->selectedVChannel, 'T', amount);
					break;
				case X32_ENC_ENCODER2:
					mixer->ChangeDynamics(config->selectedVChannel, 'R', amount);
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangeDynamics(config->selectedVChannel, 'M', amount);
					break;
				case X32_ENC_ENCODER4:
					mixer->ChangeDynamics(config->selectedVChannel, 'A', amount);
					break;
				case X32_ENC_ENCODER5:
					mixer->ChangeDynamics(config->selectedVChannel, 'H', amount);
					break;
				case X32_ENC_ENCODER6:
					mixer->ChangeDynamics(config->selectedVChannel, 'r', amount);
					break;
				default:
					break;
			}
        }

    private:
        lv_chart_series_t* chartSeriesCompressor;
        lv_chart_series_t* chartSeriesCompressorAudio;

        void EncoderText(uint8_t chanIndex) {
            if (chanIndex < 40) {
                SetEncoderText("Thresh: " + String(mixer->dsp->Channel[chanIndex].compressor.threshold, 1) + " dB",
                    "Ratio: " + String(mixer->dsp->Channel[chanIndex].compressor.ratio, 1) + ":1",
                    "Makeup: " + String(mixer->dsp->Channel[chanIndex].compressor.makeup, 1) + " dB",
                    "Attack: " + String(mixer->dsp->Channel[chanIndex].compressor.attackTime_ms, 0) + " ms",
                    "Hold: " + String(mixer->dsp->Channel[chanIndex].compressor.holdTime_ms, 0) + " ms",
                    "Release: " + String(mixer->dsp->Channel[chanIndex].compressor.releaseTime_ms, 0) + " ms"
                );
            } else {
                SetEncoderText("-", "-", "-", "-", "-", "-");
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

            sCompressor* comp = &mixer->dsp->Channel[config->selectedVChannel].compressor;

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