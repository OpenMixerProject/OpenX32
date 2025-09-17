/*
    ____                  __   ______ ___  
   / __ \                 \ \ / /___ \__ \ 
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
  | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |                               
         |_|                               
  
  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "hal.h"
#include "uart.h"
#include "adda.h"
#include "mixer.h"
#include "spi.h"
#include "fx.h"
#include "routing.h"

void halSyncVChannelConfigFromMixer(void){
    // loop trough all vChannels
    uint8_t group = 0;
    uint8_t channel = 0;
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        if (mixer.vChannel[i].inputSource.dspChannel == 0){
            continue;
        } else {
            s_vChannel* chan = &mixer.vChannel[i];

            // only the first 40 channels are able to control gain and phantomPower. DSP-Channels 41..64 do not support this
            if ((mixer.vChannel[i].inputSource.dspChannel >= 1) && (mixer.vChannel[i].inputSource.dspChannel <= 40)) {
                routingGetSourceGroupAndChannelByDspChannel(mixer.vChannel[i].inputSource.dspChannel, &group, &channel);

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_GAIN)){
                    halSendGain(group, channel, chan->inputSource.gain);
                }

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_PHANTOM)){
                    halSendPhantomPower(group, channel, chan->inputSource.phantomPower);
                }

                if ((mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_VOLUME) || (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_MUTE)))){
                    if (!chan->mute) {
                        halSendVolume(chan->inputSource.dspChannel, chan->volumeLR, chan->volumeSub, chan->balance);
                    }else{
                        halSendVolume(chan->inputSource.dspChannel, -100, -100, chan->balance); // dBfs
                    }
                }

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_GATE)){
                    halSendGate(chan->inputSource.dspChannel);
                }

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_EQ)){
                    halSendEQ(chan->inputSource.dspChannel);
                }

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_DYNAMIC)){
                    halSendCompressor(chan->inputSource.dspChannel);
                }
            }
        }
    }

    x32debug("Mixer gain to hardware synced\n");
}

// set the gain of the local XLR head-amp-control
void halSendGain(uint8_t group, uint8_t channel, float gain) {
    uint8_t boardId;
    uint8_t addaChannel;

    switch (group) {
        case 'x': // local XLR-inputs
            // set value to internal struct
            dsp.preamps.gainXlr[channel - 1] = gain;

            // now send value to adda-board
            boardId = addaGetBoardId(channel);
            addaChannel = channel;
            while (addaChannel > 8) {
                addaChannel -= 8;
            }
            addaSetGain(boardId, addaChannel, dsp.preamps.gainXlr[channel - 1], dsp.preamps.phantomPowerXlr[channel - 1]);
            break;
        case 'A': // AES50A
            dsp.preamps.gainAes50a[channel - 1] = gain;
            break;
        case 'B': // AES50B
            dsp.preamps.gainAes50b[channel - 1] = gain;
            break;
    }
}


// enable or disable phatom-power of local XLR-inputs
void halSendPhantomPower(uint8_t group, uint8_t channel, bool active) {
    uint8_t boardId = 0;
    uint8_t addaChannel = 0;

    switch (group) {
        case 'x': // local XLR-inputs
            // set value to internal struct
            dsp.preamps.phantomPowerXlr[channel - 1] = active;

            // now send value to adda-board
            boardId = addaGetBoardId(channel);
            addaChannel = channel;
            while (addaChannel > 8) {
               addaChannel -= 8;
            }
            addaSetGain(boardId, addaChannel, dsp.preamps.gainXlr[channel - 1], dsp.preamps.phantomPowerXlr[channel - 1]);
            break;
        case 'A': // AES50A
            dsp.preamps.phantomPowerAes50a[channel - 1] = active;
            break;
        case 'B': // AES50B
            dsp.preamps.phantomPowerAes50b[channel - 1] = active;
            break;
    }
}

// set the general volume of one of the 40 DSP-channels
void halSendVolume(uint8_t dspChannel, float volumeLR, float volumeSub, float balance) {
    // set value to interal struct
    dsp.dspChannel[dspChannel-1].volumeLR = volumeLR;
    dsp.dspChannel[dspChannel-1].volumeSub = volumeSub;
    dsp.dspChannel[dspChannel-1].balance = balance;

    float balanceLeft = saturate(100.0f - balance, 0.0f, 100.0f) / 100.0f;
    float balanceRight = saturate(balance + 100.0f, 0.0f, 100.0f) / 100.0f;

    // send volume to DSP via SPI
    float values[4];
    values[0] = pow(10.0f, volumeLR/20.0f); // volume of this specific channel
    values[1] = balanceLeft; // 100 .. 100 ..  0
    values[2] = balanceRight; // 0  .. 100 .. 100
    values[3] = pow(10.0f, volumeSub/20.0f); // subwoofer

    spiSendDspParameterArray(0, 'v', dspChannel-1, 0, 4, &values[0]);
}

void halSendGate(uint8_t dspChannel) {
    fxRecalcGate(&dsp.dspChannel[dspChannel-1].gate);

    float values[5];
    values[0] = dsp.dspChannel[dspChannel-1].gate.value_threshold;
    values[1] = dsp.dspChannel[dspChannel-1].gate.value_gainmin;
    values[2] = dsp.dspChannel[dspChannel-1].gate.value_coeff_attack;
    values[3] = dsp.dspChannel[dspChannel-1].gate.value_hold_ticks;
    values[4] = dsp.dspChannel[dspChannel-1].gate.value_coeff_release;

    spiSendDspParameterArray(0, 'g', dspChannel-1, 0, 5, &values[0]);
}

void halSendEQ(uint8_t dspChannel) {
    // biquad_trans() needs the coeffs in the following order
    // a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 0/1)
    // a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 2/3)
    // a0 a1 a2 b1 b2 (section 4)

    float values[MAX_CHAN_EQS * 5];

    for (int peq = 0; peq < MAX_CHAN_EQS; peq++) {
        if (((MAX_CHAN_EQS % 2) == 0) || (peq < (MAX_CHAN_EQS - 1))) {
            // we have even number of PEQ-sections
            // or we have odd number but we are still below the last section
            // store data with interleaving
            int sectionIndex = ((peq / 2) * 2) * 5;
            if ((peq % 2) != 0) {
                // odd section index
                sectionIndex += 1;
            }
            values[sectionIndex + 0] = dsp.dspChannel[dspChannel-1].peq[peq].a[0]; // a0 (zeros)
            values[sectionIndex + 2] = dsp.dspChannel[dspChannel-1].peq[peq].a[1]; // a1 (zeros)
            values[sectionIndex + 4] = dsp.dspChannel[dspChannel-1].peq[peq].a[2]; // a2 (zeros)
            values[sectionIndex + 6] = -dsp.dspChannel[dspChannel-1].peq[peq].b[1]; // -b1 (poles)
            values[sectionIndex + 8] = -dsp.dspChannel[dspChannel-1].peq[peq].b[2]; // -b2 (poles)
        }else{
            // last section: store without interleaving
            int sectionIndex = (MAX_CHAN_EQS - 1) * 5;
            values[sectionIndex + 0] = dsp.dspChannel[dspChannel-1].peq[peq].a[0]; // a0 (zeros)
            values[sectionIndex + 1] = dsp.dspChannel[dspChannel-1].peq[peq].a[1]; // a1 (zeros)
            values[sectionIndex + 2] = dsp.dspChannel[dspChannel-1].peq[peq].a[2]; // a2 (zeros)
            values[sectionIndex + 3] = -dsp.dspChannel[dspChannel-1].peq[peq].b[1]; // -b1 (poles)
            values[sectionIndex + 4] = -dsp.dspChannel[dspChannel-1].peq[peq].b[2]; // -b2 (poles)
        }
    }

    spiSendDspParameterArray(0, 'e', dspChannel-1, 0, MAX_CHAN_EQS * 5, &values[0]);
}

void halSendCompressor(uint8_t dspChannel) {
    fxRecalcCompressor(&dsp.dspChannel[dspChannel-1].compressor);

    float values[6];
    values[0] = dsp.dspChannel[dspChannel-1].compressor.value_threshold;
    values[1] = dsp.dspChannel[dspChannel-1].compressor.value_ratio;
    values[2] = dsp.dspChannel[dspChannel-1].compressor.value_makeup;
    values[3] = dsp.dspChannel[dspChannel-1].compressor.value_coeff_attack;
    values[4] = dsp.dspChannel[dspChannel-1].compressor.value_hold_ticks;
    values[5] = dsp.dspChannel[dspChannel-1].compressor.value_coeff_release;

    spiSendDspParameterArray(0, 'c', dspChannel-1, 0, 6, &values[0]);
}
