#pragma once

#include "page.h"

using namespace std;

class PageEq: public Page {
    public:
        PageEq(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE_COMPRESSOR;
            nextPage = X32_PAGE_NONE;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 4;
            led = X32_BTN_VIEW_EQ;
            noLedOnRack = true;
        }

        void OnInit() override {
            // setup EQ-graph
            lv_chart_set_type(objects.current_channel_eq, LV_CHART_TYPE_LINE);
            lv_obj_set_style_size(objects.current_channel_eq, 0, 0, LV_PART_INDICATOR);
            lv_obj_set_style_line_width(objects.current_channel_eq, 5, LV_PART_ITEMS);
            chartSeriesEQ = lv_chart_add_series(objects.current_channel_eq, lv_palette_main(LV_PALETTE_AMBER), LV_CHART_AXIS_PRIMARY_Y);
            chartSeriesEQPhase = lv_chart_add_series(objects.current_channel_eq, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
            lv_chart_set_div_line_count(objects.current_channel_eq, 5, 7);
            lv_chart_set_range(objects.current_channel_eq, LV_CHART_AXIS_PRIMARY_Y, -15000, 15000);
            lv_chart_set_point_count(objects.current_channel_eq, 200);
            lv_chart_set_series_color(objects.current_channel_eq, chartSeriesEQ, lv_color_hex(0xef7900));
            lv_chart_set_series_color(objects.current_channel_eq, chartSeriesEQPhase, lv_color_hex(0x008000));
            //chart-shadow: 0x7e4000
        }

        void OnShow() override {
            DrawEq(config->selectedVChannel);
            EncoderText(config->selectedVChannel);
        }

        void OnChange() override {
            uint8_t chanIndex = config->selectedVChannel;
            VChannel* chan = mixer->GetVChannel(chanIndex);

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_EQ) || state->HasChanged(X32_MIXER_CHANGED_SELECT)) {
                DrawEq(chanIndex);
                EncoderText(chanIndex);
            }
        }

        void OnDisplayButton(X32_BTN button, bool pressed) override {
            if (pressed){
				switch (button){
					case X32_BTN_ENCODER1:
						mixer->SetLowcut(config->selectedVChannel, 20); // set to 20 Hz
						break;
					case X32_BTN_ENCODER2:
						mixer->SetPeq(config->selectedVChannel, state->activeEQ, 'F', 3000);
						break;
					case X32_BTN_ENCODER3:
						mixer->SetPeq(config->selectedVChannel, state->activeEQ, 'G', 0);
						break;
					case X32_BTN_ENCODER4:
						mixer->SetPeq(config->selectedVChannel, state->activeEQ, 'Q', 2);
						break;
					case X32_BTN_ENCODER5:
						mixer->SetPeq(config->selectedVChannel, state->activeEQ, 'T', 1);
						break;
					case X32_BTN_ENCODER6:
						mixer->dsp->ResetEq(config->selectedVChannel);
						break;
					default:
						break;
				}
			}
        }

        void OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) {
            switch (encoder) {
				case X32_ENC_ENCODER1:
					mixer->ChangeLowcut(config->selectedVChannel, amount);
					break;
				case X32_ENC_ENCODER2:
					mixer->ChangePeq(config->selectedVChannel, state->activeEQ, 'F', amount);
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangePeq(config->selectedVChannel, state->activeEQ, 'G', amount);
					break;
				case X32_ENC_ENCODER4:
					mixer->ChangePeq(config->selectedVChannel, state->activeEQ, 'Q', amount);
					break;
				case X32_ENC_ENCODER5:
					mixer->ChangePeq(config->selectedVChannel, state->activeEQ, 'T', amount);
					break;
				case X32_ENC_ENCODER6:
                    {
                        int8_t newValue = state->activeEQ + amount;
                        if (newValue < 0) {
                            state->activeEQ = 0;
                        }else if(newValue >= MAX_CHAN_EQS) {
                            state->activeEQ = MAX_CHAN_EQS - 1;
                        }else{
                            state->activeEQ = newValue;
                        }
                        mixer->ChangePeq(config->selectedVChannel, state->activeEQ, 'T', 0);
                    }
					break;
				default:
					break;
			}
        }

    private:
        lv_chart_series_t* chartSeriesEQ;
        lv_chart_series_t* chartSeriesEQPhase;

        void EncoderText(uint8_t chanIndex) {
            if (chanIndex < 40) {
                // support EQ-channel
                SetEncoderText("LC: " + helper->freq2String(mixer->dsp->Channel[chanIndex].lowCutFrequency),
                    "F: " + helper->freq2String(mixer->dsp->Channel[chanIndex].peq[state->activeEQ].fc),
                    "G: " + String(mixer->dsp->Channel[chanIndex].peq[state->activeEQ].gain, 1) + " dB",
                    "Q: " + String(mixer->dsp->Channel[chanIndex].peq[state->activeEQ].Q, 1),
                    "M: " + helper->eqType2String(mixer->dsp->Channel[chanIndex].peq[state->activeEQ].type),
                    "PEQ: " + String(state->activeEQ + 1)
                );
            } else {
                SetEncoderText("-", "-", "-", "-", "-", "-");
            }
        }

        void DrawEq(uint8_t selectedChannelIndex) {
            if (selectedChannelIndex >= 40) {
                return;
            }

            // calculate the filter-response between 20 Hz and 20 kHz for all 4 PEQs
            sPEQ* peq;
            float eqValue[200];
            float freq;

            memset(&eqValue[0], 0, sizeof(eqValue));

            // draw the amplitude response over frequency
            int32_t* chartSeriesEqPoints = lv_chart_get_series_y_array(objects.current_channel_eq, chartSeriesEQ);
            for (uint16_t pixel = 0; pixel < 200; pixel++) {
                freq = 20.0f * powf(1000.0f, ((float)pixel/199.0f));

                // LowCut
                eqValue[pixel] += mixer->dsp->fx->CalcFrequencyResponse_LC(freq, mixer->dsp->Channel[selectedChannelIndex].lowCutFrequency, config->GetSamplerate());

                // PEQ
                for (uint8_t i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++) {
                    peq = &mixer->dsp->Channel[config->selectedVChannel].peq[i_peq];
                    eqValue[pixel] += mixer->dsp->fx->CalcFrequencyResponse_PEQ(peq->a[0], peq->a[1], peq->a[2], peq->b[1], peq->b[2], freq, config->GetSamplerate());
                }

                // draw point
                chartSeriesEqPoints[pixel] = eqValue[pixel] * 1000.0f; // convert to primary Y-axis range (+/-15 -> +/-15,000)
            }

            // draw the phase response over frequency
            int32_t* chartSeriesEqPhasePoints = lv_chart_get_series_y_array(objects.current_channel_eq, chartSeriesEQPhase);
            for (uint16_t pixel = 0; pixel < 200; pixel++) {
                freq = 20.0f * powf(1000.0f, ((float)pixel/199.0f));
                float phase = 0.0f;
                // LowCut
                phase += mixer->dsp->fx->CalcPhaseResponse_LC(freq, mixer->dsp->Channel[selectedChannelIndex].lowCutFrequency);

                // PEQ  
                for (uint8_t i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++) {
                    peq = &mixer->dsp->Channel[config->selectedVChannel].peq[i_peq];
                    phase += mixer->dsp->fx->CalcPhaseResponse_PEQ(peq->a[0], peq->a[1], peq->a[2], 1.0f, peq->b[1], peq->b[2], freq, config->GetSamplerate());
                }

                // limit phase to +/- PI
                while (phase > PI)  phase -= 2.0f * PI;
                while (phase < -PI) phase += 2.0f * PI;

                // draw point
                chartSeriesEqPhasePoints[pixel] = phase * (-15000.0f / PI); // convert to primary Y-axis range (+/-PI -> +/-15,000)
            }

            lv_chart_refresh(objects.current_channel_eq);
        }
};