#pragma once
#include "page.h"
using namespace std;

class PageUtility: public Page {
    public:
        PageUtility(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 7;
            led = X32_BTN_UTILITY;
        }

        void OnShow() override
        {
            BindEncoder(DISPLAY_ENCODER_1, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_2, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_3, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_4, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_5, PAGE_CUSTOM_ENCODER);
            BindEncoder(DISPLAY_ENCODER_6, PAGE_CUSTOM_ENCODER);

            custom_encoder[DISPLAY_ENCODER_1].label = "Reload DSPs";
            custom_encoder[DISPLAY_ENCODER_2].label = "FX: Install Effects";
            custom_encoder[DISPLAY_ENCODER_3].label = "FX: Send Parameters";
            custom_encoder[DISPLAY_ENCODER_4].label = "XLR1 -> DSP2, DSP2 -> Ch9..24";
        }

        void OnChange(bool force_update) override
        {
            // TODO only update if values are changed
            custom_encoder[DISPLAY_ENCODER_5].label = String("D2: ") + String(state->debugvalue2);
            custom_encoder[DISPLAY_ENCODER_6].label = String("D1: ") + String(state->debugvalue);
        }

        bool OnDisplayButton(X32_BTN button, bool pressed) override
        {
            bool handled = false;

        	if (pressed)
            {
                handled = true;

				switch (button)
                {
                    // DEBUG
                    // case X32_BTN_UP:
                    // 	surface->SetLed(state->debugvalue2, state->debugvalue, true);
                    // 	break;
                    // case X32_BTN_DOWN:
                    // 	surface->SetLed(state->debugvalue2, state->debugvalue, false);
                    // 	break;
                    case X32_BTN_ENCODER1:
                        // Reload DSP1
                        mixer->dsp->spi->CloseConnectionDsps();
                        mixer->dsp->spi->UploadBitstreamDsps(false); // use UI to show progress
                        mixer->dsp->spi->OpenConnectionDsps();
                        usleep(50000); // wait 50ms
                        mixer->dsp->SendAll();
                        mixer->dsp->SendAll(); // currently we need to send the data twice. Maybe a bug in the SPI-connection or a timing-issue?
                        break;
                    case X32_BTN_ENCODER2:
                        // install effects
                        mixer->dsp->DSP2_SetFx(0, FX_TYPE::REVERB, 2); // Reverb
                        mixer->dsp->DSP2_SetFx(1, FX_TYPE::CHORUS, 2); // Chorus
                        mixer->dsp->DSP2_SetFx(2, FX_TYPE::TRANSIENTSHAPER, 2); // TransientShaper
                        mixer->dsp->DSP2_SetFx(3, FX_TYPE::DELAY, 2); // Delay
                        //mixer->dsp->DSP2_SetFx(4, FX_TYPE::OVERDRIVE, 2); // Overdrive
                        //mixer->dsp->DSP2_SetFx(5, FX_TYPE::MULTIBANDCOMPRESOR, 2); // MultibandCompressor
                        //mixer->dsp->DSP2_SetFx(6, FX_TYPE::DYNAMICEQ, 2); // DynamicEQ
                        //mixer->dsp->DSP2_SetFx(7, FX_TYPE::NONE, 2); // no effect
                        break;
                    case X32_BTN_ENCODER3:
                        mixer->dsp->DSP2_SendFxParameter(0); // send parameters for fxSlot 0
                        mixer->dsp->DSP2_SendFxParameter(1); // send parameters for fxSlot 1
                        mixer->dsp->DSP2_SendFxParameter(2); // send parameters for fxSlot 2
                        mixer->dsp->DSP2_SendFxParameter(3); // send parameters for fxSlot 3
                        mixer->dsp->DSP2_SendFxParameter(4); // send parameters for fxSlot 4
                        mixer->dsp->DSP2_SendFxParameter(5); // send parameters for fxSlot 5
                        mixer->dsp->DSP2_SendFxParameter(6); // send parameters for fxSlot 6
                        mixer->dsp->DSP2_SendFxParameter(7); // send parameters for fxSlot 7
                        break;
                    case X32_BTN_ENCODER4:
                        {
                            // DEBUG DATA FOR CHRIS

                            // set gain for XLR-Input 1 and 2
                            config->Set(MP_ID::CHANNEL_GAIN, 47.0f, 0);
                            config->Set(MP_ID::CHANNEL_GAIN, 47.0f, 1);

                            // route FPGA-Channel 1 to DSP-Channel 1
                            config->Set(ROUTING_DSP_INPUT, DSP_BUF_IDX_DSPCHANNEL, 0);
                            config->Set(ROUTING_DSP_INPUT_TAPPOINT, to_underlying(DSP_TAP::INPUT), 0);
                            mixer->dsp->SetInputRouting(0);

                            // route DSP-Input Channel 1 to DSP2-Send 1-8
                            for (int i = 0; i < 7; i++) {
                                config->Set(ROUTING_DSP_OUTPUT, DSP_BUF_IDX_DSPCHANNEL, 40 + i);
                                config->Set(ROUTING_DSP_OUTPUT_TAPPOINT, to_underlying(DSP_TAP::INPUT), 0);
                                mixer->dsp->SetOutputRouting(i);
                            }
                            // route MainL/R to DSP-Outputs 39 and 40
                            config->Set(ROUTING_DSP_OUTPUT, DSP_BUF_IDX_MAINLEFT, 38);
                            config->Set(ROUTING_DSP_OUTPUT_TAPPOINT, to_underlying(DSP_TAP::INPUT), 0);
                            config->Set(ROUTING_DSP_OUTPUT, DSP_BUF_IDX_MAINRIGHT, 39);
                            config->Set(ROUTING_DSP_OUTPUT_TAPPOINT, to_underlying(DSP_TAP::INPUT), 0);
                            mixer->dsp->SetOutputRouting(38);
                            mixer->dsp->SetOutputRouting(39);
                            
                            /*
                            config->Set(MP_ID::CHANNEL_MUTE, 1.0f, 0); // mute channel 1
                            config->Set(MP_ID::CHANNEL_MUTE, 1.0f, 10); // mute channel 11 (Chorus)
                            config->Set(MP_ID::CHANNEL_MUTE, 1.0f, 11); // mute channel 12 (Chorus)
                            config->Set(MP_ID::CHANNEL_MUTE, 1.0f, 12); // mute channel 13 (TransientShaper)
                            config->Set(MP_ID::CHANNEL_MUTE, 1.0f, 13); // mute channel 14 (TransientShaper)
                            config->Set(MP_ID::CHANNEL_MUTE, 1.0f, 14); // mute channel 15 (overdrive)
                            config->Set(MP_ID::CHANNEL_MUTE, 1.0f, 15); // mute channel 16 (overdrive)
                            config->Set(MP_ID::CHANNEL_MUTE, 1.0f, DSP_BUF_IDX_AUX + 5); // mute channel A7
                            config->Set(MP_ID::CHANNEL_MUTE, 1.0f, DSP_BUF_IDX_AUX + 6); // mute channel A8
                            */

                            config->Set(MP_ID::CHANNEL_VOLUME, 0.0f, 80); // Main 0dBfs
                        }
                        break;
                    default:
                        handled = false;
                        // dummy
				}
			}

            return handled;
        }
};