#pragma once
#include "page.h"

using namespace std;

class PageDebug: public Page
{
    public:
        PageDebug(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::ABOUT;
            nextPage = X32_PAGE::PROTOTYPEGUI;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 3;
            led = X32_BTN_SETUP;
        }

        void OnShow() override 
        {
            config->GetParameter(DISPLAY_ENCODER_1_BUTTON)->SetName("Reload DSPs");
            config->GetParameter(DISPLAY_ENCODER_2_BUTTON)->SetName("FX: Install Effects");
            config->GetParameter(DISPLAY_ENCODER_3_BUTTON)->SetName("FX: Send Parameters");
            config->GetParameter(DISPLAY_ENCODER_4_BUTTON)->SetName("XLR1 -> DSP2, DSP2 -> Ch9..24");
            config->GetParameter(DISPLAY_ENCODER_5_BUTTON)->SetName("Clear DebugLabelText");

            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_6, MixerparameterAction::SET, DEBUG);
        }

        void OnChange(bool force) override
        {
            if (config->HasParameterChanged(DISPLAY_ENCODER_1_BUTTON))
            {
                // Reload DSP1
                mixer->dsp->spi->CloseConnectionDsps();
                mixer->dsp->spi->UploadBitstreamDsps(false); // use UI to show progress
                mixer->dsp->spi->OpenConnectionDsps();
                usleep(50000); // wait 50ms
                //mixer->dsp->SendAll();
                //mixer->dsp->SendAll(); // currently we need to send the data twice. Maybe a bug in the SPI-connection or a timing-issue?
            }

            if (config->HasParameterChanged(DISPLAY_ENCODER_2_BUTTON))
            {
                // install effects
                mixer->dsp->DSP2_SetFx(0, FX_TYPE::REVERB, 2); // Reverb
                mixer->dsp->DSP2_SetFx(1, FX_TYPE::CHORUS, 2); // Chorus
                mixer->dsp->DSP2_SetFx(2, FX_TYPE::DELAY, 2); // Delay
                mixer->dsp->DSP2_SetFx(3, FX_TYPE::OVERDRIVE, 2); // Overdrive
                //mixer->dsp->DSP2_SetFx(4, FX_TYPE::TRANSIENTSHAPER, 2); // TransientShaper
                //mixer->dsp->DSP2_SetFx(5, FX_TYPE::MULTIBANDCOMPRESOR, 2); // MultibandCompressor
                //mixer->dsp->DSP2_SetFx(6, FX_TYPE::DYNAMICEQ, 2); // DynamicEQ
                //mixer->dsp->DSP2_SetFx(7, FX_TYPE::NONE, 2); // no effect
            }

            if (config->HasParameterChanged(DISPLAY_ENCODER_3_BUTTON))
            {
                mixer->dsp->DSP2_SendFxParameter(0); // send parameters for fxSlot 0
                mixer->dsp->DSP2_SendFxParameter(1); // send parameters for fxSlot 1
                mixer->dsp->DSP2_SendFxParameter(2); // send parameters for fxSlot 2
                mixer->dsp->DSP2_SendFxParameter(3); // send parameters for fxSlot 3
                mixer->dsp->DSP2_SendFxParameter(4); // send parameters for fxSlot 4
                mixer->dsp->DSP2_SendFxParameter(5); // send parameters for fxSlot 5
                mixer->dsp->DSP2_SendFxParameter(6); // send parameters for fxSlot 6
                mixer->dsp->DSP2_SendFxParameter(7); // send parameters for fxSlot 7
            }
            
            if (config->HasParameterChanged(DISPLAY_ENCODER_4_BUTTON))
            {
                // DEBUG DATA FOR CHRIS

                // set gain for XLR-Input 1 and 2
                config->Set(MP_ID::CHANNEL_GAIN, 45.0f, 0);
                config->Set(MP_ID::CHANNEL_GAIN, 45.0f, 1);
                config->Set(MP_ID::CHANNEL_VOLUME, 0.0f, 0);
                config->Set(MP_ID::CHANNEL_VOLUME, 0.0f, 1);

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
                
                config->Set(MP_ID::CHANNEL_VOLUME, 0.0f, 80); // Main 0dBfs
            }

            if (config->HasParameterChanged(DISPLAY_ENCODER_5_BUTTON))
            {
                mixer->debugText = "";
                lv_label_set_text(objects.setup_debug_label, mixer->debugText.c_str());
			}            
        }
};