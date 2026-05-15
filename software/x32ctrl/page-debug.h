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
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_1, "Reload DSPs"); // just the title
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_1);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_2, "ArtNET"); // just the title
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_2, MixerparameterAction::TOGGLE, DMX_ARTNET_ENABLE);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_3, MixerparameterAction::CHANGE, DMX_ARTNET_OFFSET);
            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_BUTTON_3, MixerparameterAction::TOGGLE, DMX_ARTNET_ON_FADERS);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_4, "XLR1 -> DSP2, DSP2 -> Ch9..24");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_4);
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_5, "Debug Header on/off");
            config->SurfaceBindCustom(SurfaceElementId::DISPLAY_ENCODER_BUTTON_5);

            config->SurfaceBind(SurfaceElementId::DISPLAY_ENCODER_6, MixerparameterAction::CHANGE, DEBUG_VALUE);
        }

        void OnChange(bool force) override
        {
            if (config->HasParameterChanged(DEBUG_VALUE))
            {
                surface->SetLedRaw(1, config->GetUint(DEBUG_VALUE), true);
            }
        }

        void OnChangeCustomButton(SurfaceElementId surface_element_id) override
        {
            switch (surface_element_id)
            {
                case SurfaceElementId::DISPLAY_ENCODER_BUTTON_1:
                    {
                        // Reload DSP1
                        mixer->dsp->spi->CloseConnectionDsps();
                        mixer->dsp->spi->UploadBitstreamDsps(false); // use UI to show progress
                        mixer->dsp->spi->OpenConnectionDsps();
                        usleep(50000); // wait 50ms

                        // the gate, the dynamics and the EQ-settings are not loaded correctly on first load, so load it again after a short time
                        mixer->LoadConfig(0);

                        // in the following lines the default configuration is set so that the users of the beta-version
                        // can start with a working system

                        // route channel 1-4 to effects using post-fader tapping
                        for (uint8_t i = 0; i < 8; i++)
                        {
                            config->Set(ROUTING_DSP_OUTPUT, DSP_BUF_IDX_DSPCHANNEL + (i / 2), 40 + i);
                            config->Set(ROUTING_DSP_OUTPUT_TAPPOINT, to_underlying(DSP_TAP::POST_FADER), 40 + i);
                        }

                        // set volume of FX-return to 0dBfs
                        for (int i = 0; i < 8; i++)
                        {
                            config->Set(CHANNEL_VOLUME, 0, 40 + i);
                        }

                        // set default FXes in FX slots
                        mixer->dsp->DSP2_SetFx(0, FX_TYPE::REVERB, 2); // on first load this effect has a bug, so we have to disable it a bit later
                        mixer->dsp->DSP2_SetFx(1, FX_TYPE::CHORUS, 2);
                        mixer->dsp->DSP2_SetFx(2, FX_TYPE::DELAY, 2);
                        mixer->dsp->DSP2_SetFx(3, FX_TYPE::NONE, 2);
                        mixer->dsp->DSP2_SetFx(4, FX_TYPE::NONE, 2);
                        mixer->dsp->DSP2_SetFx(5, FX_TYPE::NONE, 2);
                        mixer->dsp->DSP2_SetFx(6, FX_TYPE::NONE, 2);
                        mixer->dsp->DSP2_SetFx(7, FX_TYPE::NONE, 2);

                        // set FX-settings to wet on slot 1-4
                        config->Set(FX_REVERB_DRY, 0, 0); // fx-slot 1
                        config->Set(FX_REVERB_WET, 1, 0); // fx-slot 1
                        config->Set(FX_CHORUS_MIX, 1, 1); // fx-slot 2
                    }
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_BUTTON_4:
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
                    break;
                case SurfaceElementId::DISPLAY_ENCODER_BUTTON_5:
                    {
                        //mixer->debugText = ""; 
                        //lv_label_set_text(objects.setup_debug_label, mixer->debugText.c_str());
                        config->Toggle(DEBUG_HEADER);
			        }            
                    break;
                default:
                    break;
            }
        }
};