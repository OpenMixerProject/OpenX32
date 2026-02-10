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

        void OnShow() override {
            SetEncoder(DISPLAY_ENCODER_2, "FX-Rack", "Install Effects");
            SetEncoder(DISPLAY_ENCODER_3, "FX-Rack", "Send Parameters");
            SetEncoder(DISPLAY_ENCODER_4, "XLR1 -> DSP2", "DSP2 -> Ch9..24");
        }

        void OnChange(bool force_update) override {
            // TODO only update if values are changed
            encoderSliders[0].label = "Reload DSPs";
            encoderSliders[4].label = String("D2: ") + String(state->debugvalue2);
            encoderSliders[5].label = String("D1: ") + String(state->debugvalue);
        }

        bool OnDisplayButton(X32_BTN button, bool pressed) override {
        	if (pressed){
				switch (button){
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
                    mixer->dsp->DSP2_SetFx(3, FX_TYPE::OVERDRIVE, 2); // Overdrive
                    mixer->dsp->DSP2_SetFx(4, FX_TYPE::DELAY, 2); // Delay
                    //mixer->dsp->DSP2_SetFx(5, FX_TYPE::MULTIBANDCOMPRESOR, 2); // MultibandCompressor
                    mixer->dsp->DSP2_SetFx(6, FX_TYPE::DYNAMICEQ, 2); // DynamicEQ
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
                        mixer->SetGain(0, 47); // XLR In 1
                        mixer->SetGain(1, 47); // XLR In 2

                        // route FX-Return-Channels 1-16 to DSP1-Input-Channels 9 to 24, set volume to 0dBfs and Panning to hard L/R
                        for (int i = 8; i < 24; i++) {
                            mixer->dsp->SetInputRouting(i);
                            mixer->SetVolume(i, 0); // 0dBfs

                            mixer->dsp->Channel[i].input = DSP_BUF_IDX_DSP2_FX + (i - 8);
                            mixer->dsp->Channel[i].inputTapPoint = DSP_TAP_INPUT;
                        }
                        for (int i = 8; i < 24; i+=2) {
                            mixer->SetBalance(i, -100); // left
                            mixer->SetBalance(i + 1, 100); // right
                        }

                        // route XLR1 to DSP2-Send 1/2
                        mixer->dsp->Dsp1toDsp2Routing[0].input = DSP_BUF_IDX_DSPCHANNEL; // Channel 1 from FPGA
                        mixer->dsp->Dsp1toDsp2Routing[0].tapPoint = DSP_TAP_INPUT;
                        mixer->dsp->Dsp1toDsp2Routing[1].input = DSP_BUF_IDX_DSPCHANNEL; // Channel 1 from FPGA
                        mixer->dsp->Dsp1toDsp2Routing[1].tapPoint = DSP_TAP_INPUT;
                        mixer->dsp->SetFxOutputRouting(0);
                        mixer->dsp->SetFxOutputRouting(1);

                        mixer->SetMute(0, true); // mute channel 1
                        mixer->SetMute(10, true); // mute channel 11 (Chorus)
                        mixer->SetMute(11, true); // mute channel 12 (Chorus)
                        mixer->SetMute(12, true); // mute channel 13 (TransientShaper)
                        mixer->SetMute(13, true); // mute channel 14 (TransientShaper)
                        mixer->SetMute(14, true); // mute channel 15 (overdrive)
                        mixer->SetMute(15, true); // mute channel 16 (overdrive)
                        mixer->SetMute(DSP_BUF_IDX_AUX + 5, true); // mute channel A7
                        mixer->SetMute(DSP_BUF_IDX_AUX + 6, true); // mute channel A8

                        mixer->SetVolume(80, 0); // set main-volume to 0dBfs
                    }
                    break;
                case X32_BTN_ENCODER5:
                    state->debugvalue2 = 0;
                    state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
                    break;
                case X32_BTN_ENCODER6:
                    state->debugvalue = 0;
                    state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
                    break;
                default:
                    break;
				}
			}

            return true;
        }

        bool OnDisplayEncoderTurned(X32_ENC encoder, int amount) override {
            if ((X32_PAGE)config->GetUint(MP_ID::ACTIVE_PAGE) == X32_PAGE::UTILITY) {
                switch (encoder){
                    case X32_ENC_ENCODER1:
                        break;
                    case X32_ENC_ENCODER2:
                        break;
                    case X32_ENC_ENCODER3:
                        break;
                    case X32_ENC_ENCODER4:
                        break;
                    case X32_ENC_ENCODER5:
                        state->debugvalue2 += amount;
                        state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
                        break;
                    case X32_ENC_ENCODER6:
                        state->debugvalue += amount;
                        state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
                        break;
                    default:  
                        // just here to avoid compiler warnings                  
                        break;
                }
		    }

            return true;
        }
};