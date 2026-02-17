#pragma once

#include "page.h"

using namespace std;

class PageEq: public Page {

    using enum MP_ID;

    public:
        PageEq(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::COMPRESSOR;
            nextPage = X32_PAGE::SENDS;
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

            BindEncoder(DISPLAY_ENCODER_6, BANKING_EQ);
        }

        void OnShow() override {
            DrawEq();
        }

        void OnChange(bool force_update) override {
            using enum MP_ID;

            if (config->HasParametersChanged({BANKING_EQ, SELECTED_CHANNEL}) || force_update)
            {
                uint chanIndex = config->GetUint(SELECTED_CHANNEL);

                BindEncoder(DISPLAY_ENCODER_1, CHANNEL_LOWCUT_FREQ, chanIndex);
                BindEncoder(DISPLAY_ENCODER_2, (MP_ID)((uint)CHANNEL_EQ_FREQ1 + config->GetUint(BANKING_EQ)), chanIndex);
                BindEncoder(DISPLAY_ENCODER_3, (MP_ID)((uint)CHANNEL_EQ_GAIN1 + config->GetUint(BANKING_EQ)), chanIndex);
                BindEncoder(DISPLAY_ENCODER_4, (MP_ID)((uint)CHANNEL_EQ_Q1 + config->GetUint(BANKING_EQ)), chanIndex);
                BindEncoder(DISPLAY_ENCODER_5, (MP_ID)((uint)CHANNEL_EQ_TYPE1 + config->GetUint(BANKING_EQ)), chanIndex);
            }

            if (config->HasParameterChanged(CHANNEL_LOWCUT_FREQ) ||
                config->HasParametersChanged({MP_CAT::CHANNEL_EQ}) ||
                config->HasParametersChanged({BANKING_EQ, SELECTED_CHANNEL}) ||
                force_update
            )
            {
                DrawEq();
                SyncEncoderWidgets(true);
            }
        }

    private:
        lv_chart_series_t* chartSeriesEQ;
        lv_chart_series_t* chartSeriesEQPhase;

        void DrawEq() {

            uint selectedChannelIndex = config->GetUint(SELECTED_CHANNEL);

            // Draw EQ only for normal and aux channels
            if (selectedChannelIndex >= to_underlying(X32_VCHANNEL_BLOCK::FXRET)) {
                return;
            }

            // calculate the filter-response between 20 Hz and 20 kHz for all 4 PEQs
            float eqValue[200];
            float freq;

            memset(&eqValue[0], 0, sizeof(eqValue));

            // draw the amplitude response over frequency
            int32_t* chartSeriesEqPoints = lv_chart_get_series_y_array(objects.current_channel_eq, chartSeriesEQ);
            for (uint16_t pixel = 0; pixel < 200; pixel++) {
                freq = 20.0f * powf(1000.0f, ((float)pixel/199.0f));

                // LowCut
                eqValue[pixel] += mixer->dsp->fxmath->CalcFrequencyResponse_LC(freq, config->GetFloat(CHANNEL_LOWCUT_FREQ, selectedChannelIndex), config->GetUint(SAMPLERATE));

                // PEQ
                for (uint8_t i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++)
                {
                    mixer->dsp->fxmath->RecalcFilterCoefficients_PEQ(&mixer->dsp->Channel[selectedChannelIndex].peq[i_peq]);
                    eqValue[pixel] += mixer->dsp->fxmath->CalcFrequencyResponse_PEQ(
                        mixer->dsp->Channel[selectedChannelIndex].peq[i_peq].a[0],
                        mixer->dsp->Channel[selectedChannelIndex].peq[i_peq].a[1],
                        mixer->dsp->Channel[selectedChannelIndex].peq[i_peq].a[2],
                        mixer->dsp->Channel[selectedChannelIndex].peq[i_peq].b[1],
                        mixer->dsp->Channel[selectedChannelIndex].peq[i_peq].b[2],
                        freq,
                        config->GetUint(SAMPLERATE)
                    );
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
                phase += mixer->dsp->fxmath->CalcPhaseResponse_LC(freq, config->GetFloat(CHANNEL_LOWCUT_FREQ, selectedChannelIndex));

                // PEQ  
                for (uint8_t i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++)
                {
                    phase += mixer->dsp->fxmath->CalcPhaseResponse_PEQ(
                        mixer->dsp->Channel[selectedChannelIndex].peq[i_peq].a[0],
                        mixer->dsp->Channel[selectedChannelIndex].peq[i_peq].a[1],
                        mixer->dsp->Channel[selectedChannelIndex].peq[i_peq].a[2],
                        1.0f,
                        mixer->dsp->Channel[selectedChannelIndex].peq[i_peq].b[1],
                        mixer->dsp->Channel[selectedChannelIndex].peq[i_peq].b[2],
                        freq,
                        config->GetUint(SAMPLERATE));
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