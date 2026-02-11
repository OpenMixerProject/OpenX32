#pragma once

#include "page.h"

using namespace std;

class PageEq: public Page {

    using enum MP_ID;

    public:
        PageEq(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::COMPRESSOR;
            nextPage = X32_PAGE::NONE;
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
            chartSeriesEQPhase = lv_chart_add_series(objects.current_channel_eq, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_SECONDARY_Y);
            lv_chart_set_div_line_count(objects.current_channel_eq, 5, 7); // hdiv_num, vdiv_num
            lv_chart_set_range(objects.current_channel_eq, LV_CHART_AXIS_PRIMARY_Y, -15000, 15000);
            lv_chart_set_range(objects.current_channel_eq, LV_CHART_AXIS_SECONDARY_Y, -15000, 15000);
            lv_chart_set_point_count(objects.current_channel_eq, 200);
            lv_chart_set_series_color(objects.current_channel_eq, chartSeriesEQ, lv_color_hex(0xef7900));
            lv_chart_set_series_color(objects.current_channel_eq, chartSeriesEQPhase, lv_color_hex(0x008000));
            //chart-shadow: 0x7e4000
        }

        void OnShow() override {
            DrawEq();
            EncoderText();
        }

        void OnChange(bool force_update) override {
            using enum MP_ID;

            if (config->HasParameterChanged(SELECTED_CHANNEL))
            {
                BindEncoder(DISPLAY_ENCODER_1, CHANNEL_LOWCUT, config->GetUint(SELECTED_CHANNEL));
            }

            if (config->HasParameterChanged(CHANNEL_EQ_TODO) /* TODO: implement HasParameterChanges with array argument to check multiple Mixerparameters at once */ ||
                config->HasParameterChanged(SELECTED_CHANNEL))
            {
                DrawEq();
                EncoderText();
            }
        }

        bool OnDisplayButton(X32_BTN button, bool pressed) override
        {
            using enum MP_ID;

            bool message_handled = false;

            if (pressed){
                message_handled = true;

				switch (button){
					case X32_BTN_ENCODER2:
						mixer->SetPeq(config->GetUint(SELECTED_CHANNEL), state->activeEQ, 'F', 3000);
						break;
					case X32_BTN_ENCODER3:
						mixer->SetPeq(config->GetUint(SELECTED_CHANNEL), state->activeEQ, 'G', 0);
						break;
					case X32_BTN_ENCODER4:
						mixer->SetPeq(config->GetUint(SELECTED_CHANNEL), state->activeEQ, 'Q', 2);
						break;
					case X32_BTN_ENCODER5:
						mixer->SetPeq(config->GetUint(SELECTED_CHANNEL), state->activeEQ, 'T', 1);
						break;
					case X32_BTN_ENCODER6:
						mixer->dsp->ResetEq(config->GetUint(SELECTED_CHANNEL));
						break;
					default:
                        message_handled = false;
						break;
				}
			}

            return message_handled;
        }

        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override
        {
            using enum MP_ID;

            switch (encoder) {
				case X32_ENC_ENCODER2:
					mixer->ChangePeq(config->GetUint(SELECTED_CHANNEL), state->activeEQ, 'F', amount);
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangePeq(config->GetUint(SELECTED_CHANNEL), state->activeEQ, 'G', amount);
					break;
				case X32_ENC_ENCODER4:
					mixer->ChangePeq(config->GetUint(SELECTED_CHANNEL), state->activeEQ, 'Q', amount);
					break;
				case X32_ENC_ENCODER5:
					mixer->ChangePeq(config->GetUint(SELECTED_CHANNEL), state->activeEQ, 'T', amount);
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
                        mixer->ChangePeq(config->GetUint(SELECTED_CHANNEL), state->activeEQ, 'T', 0);
                    }
					break;
				default:
                    return false;
					break;
			}

            return true;
        }

    private:
        lv_chart_series_t* chartSeriesEQ;
        lv_chart_series_t* chartSeriesEQPhase;

        void EncoderText() {
            
            uint chanIndex = config->GetUint(SELECTED_CHANNEL);

            // TODO: move to encoderbinding

            if (chanIndex < 40) {
                // support EQ-channel
                custom_encoder[0].label = "LC: " + helper->freq2String(config->GetFloat(CHANNEL_LOWCUT, chanIndex));
                custom_encoder[1].label = "F: " + helper->freq2String(mixer->dsp->Channel[chanIndex].peq[state->activeEQ].fc);
                custom_encoder[2].label = "G: " + String(mixer->dsp->Channel[chanIndex].peq[state->activeEQ].gain, 1) + " dB";
                custom_encoder[3].label = "Q: " + String(mixer->dsp->Channel[chanIndex].peq[state->activeEQ].Q, 1);
                custom_encoder[4].label = "M: " + helper->eqType2String(mixer->dsp->Channel[chanIndex].peq[state->activeEQ].type);
                custom_encoder[5].label = "PEQ: " + String(state->activeEQ + 1);
            } 
        }

        void DrawEq() {

            uint selectedChannelIndex = config->GetUint(SELECTED_CHANNEL);

            // Draw EQ only for normal and aux channels
            if (selectedChannelIndex >= to_underlying(X32_VCHANNEL_BLOCK::FXRET)) {
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
                eqValue[pixel] += mixer->dsp->fxmath->CalcFrequencyResponse_LC(freq, config->GetFloat(CHANNEL_LOWCUT, selectedChannelIndex), config->GetUint(SAMPLERATE));

                // PEQ
                for (uint8_t i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++) {
                    peq = &mixer->dsp->Channel[config->GetUint(SELECTED_CHANNEL)].peq[i_peq];
                    eqValue[pixel] += mixer->dsp->fxmath->CalcFrequencyResponse_PEQ(peq->a[0], peq->a[1], peq->a[2], peq->b[1], peq->b[2], freq, config->GetUint(SAMPLERATE));
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
                phase += mixer->dsp->fxmath->CalcPhaseResponse_LC(freq, config->GetFloat(CHANNEL_LOWCUT, selectedChannelIndex));

                // PEQ  
                for (uint8_t i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++) {
                    peq = &mixer->dsp->Channel[config->GetUint(SELECTED_CHANNEL)].peq[i_peq];
                    phase += mixer->dsp->fxmath->CalcPhaseResponse_PEQ(peq->a[0], peq->a[1], peq->a[2], 1.0f, peq->b[1], peq->b[2], freq, config->GetUint(SAMPLERATE));
                }

                // limit phase to +/- PI
                while (phase > PI)  phase -= 2.0f * PI;
                while (phase < -PI) phase += 2.0f * PI;

                // draw point
                chartSeriesEqPhasePoints[pixel] = phase * (-15000.0f / PI); // convert to secondary Y-axis range (+/-PI -> +/-15,000)
            }

            lv_chart_refresh(objects.current_channel_eq);
        }
};