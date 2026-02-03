#pragma once
#include "page.h"
using namespace std;

class PageEffects: public Page {
    public:
        PageEffects(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 5;
            led = X32_BTN_EFFECTS;
        }

        void OnInit() override {
            SetEncoder(DISPLAY_ENCODER_1, "FX", "");
            SetEncoder(DISPLAY_ENCODER_3, "FX-Rack", "Install Effects");
            SetEncoder(DISPLAY_ENCODER_4, "FX-Rack", "Send Parameters");
            SetEncoder(DISPLAY_ENCODER_5, "XLR1 -> DSP2", "DSP2 -> Ch9..24");
        }

        void OnDisplayButton(X32_BTN button, bool pressed) override {
            if (pressed){
                switch (button){
                    case X32_BTN_ENCODER1:
                        break;
                    case X32_BTN_ENCODER2:
                        break;
                    case X32_BTN_ENCODER3:
                        // install effects
                        mixer->dsp->DSP2_SetFx(0, 0, 2); // Reverb
                        mixer->dsp->DSP2_SetFx(1, 1, 2); // Chorus
                        mixer->dsp->DSP2_SetFx(2, 2, 2); // TransientShaper
                        mixer->dsp->DSP2_SetFx(3, 3, 2); // Overdrive
                        mixer->dsp->DSP2_SetFx(4, 4, 2); // Delay
                        //mixer->dsp->DSP2_SetFx(5, 5, 2); // MultibandCompressor
                        mixer->dsp->DSP2_SetFx(6, 6, 2); // DynamicEQ
                        //mixer->dsp->DSP2_SetFx(7, -1, 2); // no effect
                        break;
                    case X32_BTN_ENCODER4:
                        mixer->dsp->DSP2_SendFxParameter(0); // send parameters for fxSlot 0
                        mixer->dsp->DSP2_SendFxParameter(1); // send parameters for fxSlot 1
                        mixer->dsp->DSP2_SendFxParameter(2); // send parameters for fxSlot 2
                        mixer->dsp->DSP2_SendFxParameter(3); // send parameters for fxSlot 3
                        mixer->dsp->DSP2_SendFxParameter(4); // send parameters for fxSlot 4
                        mixer->dsp->DSP2_SendFxParameter(5); // send parameters for fxSlot 5
                        mixer->dsp->DSP2_SendFxParameter(6); // send parameters for fxSlot 6
                        mixer->dsp->DSP2_SendFxParameter(7); // send parameters for fxSlot 7
                        break;
                    case X32_BTN_ENCODER5:
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
                    case X32_BTN_ENCODER6:
                        {
							for (int i = 0; i < 16; i++) {
								mixer->SetPhantom(i, true);
								mixer->SetGain(i, 47);
							}
    					}
                        break;
                    default:
                        // just here to avoid compiler warnings
                        break;
                }
            }
        }
};