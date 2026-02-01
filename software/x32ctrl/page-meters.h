#pragma once
#include "page.h"
using namespace std;

class PageMeters : public Page {
    public:
        PageMeters(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 1;
            tabLayer1 = objects.metertab;
            tabIndex1 = 0;
            led = X32_BTN_METERS;
            hideEncoders = false;            
        }

        void OnInit() override {
            meterBlocks[0] = objects.meters_1_8;
            meterBlocks[1] = objects.meters_9_16;
            meterBlocks[2] = objects.meters_17_24;
            meterBlocks[3] = objects.meters_25_32;
            meterBlocks[4] = objects.meters_aux;
            meterBlocks[5] = objects.meters_fxret;
            meterBlocks[6] = objects.meters_bus_1_8;
            meterBlocks[7] = objects.meters_bus_9_16;
            meterBlocks[8] = objects.meters_matrix;

            SetEncoder(DISPLAY_ENCODER_5, "XLR1 -> DSP2", "DSP2 -> Ch9..24");
            SetEncoder(DISPLAY_ENCODER_6, "DEBUG: 1-16", "Phanton + 47dB");
        }

        void OnUpdateMeters() override {

            for(int m = 0; m < 9; m++) {
                lv_obj_t* parent = meterBlocks[m];
                for(int i = 0; i < lv_obj_get_child_count(parent); i++) {

                    uint8_t index = i+(m*8);

                    lv_obj_t * strip = lv_obj_get_child(parent, i);
                    lv_obj_t * meter = lv_obj_get_child(strip, 0); 

                    // CH1-32 + AUX
                    if (m < 5) { 
                        lv_bar_set_value(meter, helper->sample2Dbfs(mixer->dsp->rChannel[index].meterDecay), LV_ANIM_OFF);
                    } else 

                    // FX Ret
                    if (m == 5) {
                        // TODO Meter
                    } else 
                    
                    // Bus 1-16
                    if ((m == 6) || (m == 7)) {
                        // TODO Meter
                    } else
                    
                    // Matrix
                    if (m == 8) {
                        if (i < 6) {
                            //lv_bar_set_value(meter, helper->sample2Dbfs(mixer->dsp->Matrix[i]. MainChannelSub.meterDecay[0]), LV_ANIM_OFF);
                        } else if (i == 7) {
                            lv_bar_set_value(meter, helper->sample2Dbfs(mixer->dsp->MainChannelSub.meterDecay[0]), LV_ANIM_OFF);
                        }
                    }
                }
            }

            // Main L/R
            // TODO use both channels!
            lv_bar_set_value(objects.ms_main_lr__meter, helper->sample2Dbfs(mixer->dsp->MainChannelLR.meterDecay[0]), LV_ANIM_OFF);
        }

        void OnChange(bool force_update) override {

            if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL) || force_update)
            {
                bool changed_vchannel = state->HasChanged(X32_MIXER_CHANGED_VCHANNEL) || force_update;

                for(int m = 0; m < 9; m++) {
                    lv_obj_t* parent = meterBlocks[m];
                    for(int i = 0; i < lv_obj_get_child_count(parent); i++) {

                        uint8_t index = i+(m*8);
                        lv_obj_t * strip = lv_obj_get_child(parent, i);
                        lv_obj_t * fader = lv_obj_get_child(strip, 2);   

                        // CH1-32 + AUX
                        if (m < 5) { 
                            if (changed_vchannel) {
                                VChannel* chan = mixer->GetVChannel(index);

                                if (chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO) || force_update) {
                                    mixer->dsp->Channel[index].solo ? add_style_slider_fader_solo(fader) : remove_style_slider_fader_solo(fader);
                                }
                                if (chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE) || force_update) {
                                    mixer->dsp->Channel[index].muted ? add_style_slider_fader_mute(fader) : remove_style_slider_fader_mute(fader);
                                } 
                                if (chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || force_update) {
                                    lv_slider_set_value(fader, mixer->dsp->Channel[index].volumeLR, LV_ANIM_OFF);
                                } 
                            }
                        } else 

                        // FX Ret
                        if (m == 5) {
                            if (changed_vchannel) {
                                VChannel* chan = mixer->GetVChannel(index);

                                // if (chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO)) {
                                //     mixer->dsp->Channel[index].solo ? add_style_slider_fader_solo(fader) : remove_style_slider_fader_solo(fader);
                                // }
                                // if (chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)) {
                                //     mixer->dsp->Channel[index].muted ? add_style_slider_fader_mute(fader) : remove_style_slider_fader_mute(fader);
                                // } 
                                if (chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || force_update) {
                                    lv_slider_set_value(fader, mixer->dsp->volumeFxReturn[i], LV_ANIM_OFF);
                                } 
                            }
                        } else 
                        
                        // Bus 1-16
                        if ((m == 6) || (m == 7)) {
                            if (changed_vchannel) {
                                VChannel* chan = mixer->GetVChannel(index);
                                int8_t channelindex = (m == 6) ? i : i + 8;

                                if (chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || force_update) {
                                    lv_slider_set_value(fader, mixer->dsp->Bus[channelindex].volumeLR, LV_ANIM_OFF);
                                }                          
                            }
                        } else
                        
                        // Matrix
                        if (m == 8) {
                            if (changed_vchannel) {
                                VChannel* chan = mixer->GetVChannel(index);
                                if (chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO) || force_update) {
                                    if (i < 6) {
                                        mixer->dsp->Matrix[i].solo ? add_style_slider_fader_solo(fader) : remove_style_slider_fader_solo(fader);
                                    } else if (i == 7) {
                                        mixer->dsp->MainChannelSub.solo ? add_style_slider_fader_solo(fader) : remove_style_slider_fader_solo(fader);
                                    } 
                                }
                                if (chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE) || force_update) {
                                    if (i < 6) {
                                        mixer->dsp->Matrix[i].muted ? add_style_slider_fader_mute(fader) : remove_style_slider_fader_mute(fader);
                                    } else if (i == 7) {
                                        mixer->dsp->MainChannelSub.muted ? add_style_slider_fader_mute(fader) : remove_style_slider_fader_mute(fader);
                                    } 
                                }
                                if (chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || force_update) {
                                    if (i < 6) {
                                        lv_slider_set_value(fader, mixer->dsp->Matrix[i].volume, LV_ANIM_OFF);
                                    } else if (i == 7) {
                                        lv_slider_set_value(fader, mixer->dsp->MainChannelSub.volume, LV_ANIM_OFF);
                                    } 
                                }
                            }
                        }
                    }
                }  
                

                // Main L/R
                if (changed_vchannel) {
                    VChannel* main = mixer->GetVChannel(X32_VCHANNEL_BLOCK_MAIN);
                                             
                    if (main->HasChanged(X32_VCHANNEL_CHANGED_SOLO) || force_update) {
                        mixer->dsp->MainChannelLR.solo ? add_style_slider_fader_solo(objects.ms_main_lr__fader) : remove_style_slider_fader_solo(objects.ms_main_lr__fader);
                    }
                    
                    if (main->HasChanged(X32_VCHANNEL_CHANGED_MUTE) || force_update) {
                        mixer->dsp->MainChannelLR.muted ? add_style_slider_fader_mute(objects.ms_main_lr__fader) : remove_style_slider_fader_mute(objects.ms_main_lr__fader);
                    }
                    if (main->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || force_update) {
                        lv_slider_set_value(objects.ms_main_lr__fader, mixer->dsp->MainChannelLR.volume, LV_ANIM_OFF);
                    }
                }
            }
        }

        void OnDisplayButton(X32_BTN button, bool pressed) override {
            if (pressed){
                switch (button){
                    case X32_BTN_ENCODER1:
                        break;
                    case X32_BTN_ENCODER2:
                        break;
                    case X32_BTN_ENCODER3:
                        break;
                    case X32_BTN_ENCODER4:
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

        private:
            lv_obj_t* meterBlocks[9];

};