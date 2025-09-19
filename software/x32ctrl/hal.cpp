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
#include "routing.h"
#include "dsp.h"

void halSyncVChannelConfigFromMixer(void){
    // loop trough all vChannels
    uint8_t group = 0;
    uint8_t channel = 0;
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        if (i >= 40) {
            s_vChannel* chan = &mixer.vChannel[i];
            uint8_t group;
            if ((i >= 40) && (i <= 47)) {
                // FX Returns 1-8
                group = 'f';
            }else if ((i >= 48) && (i <= 63)) {
                // Busmaster 1-16
                group = 'b';
            }else if ((i >= 64) && (i <= 69)) {
                // Matrix 1-6
                group = 'x';
            }else if (i == 70) {
                // "VERY SPECIAL CHANNEL"
                group = 'v';
            }else if (i == 71) {
                // Mono/Sub
                group = 's';
            }else if ((i >= 72) && (i <= 79)) {
                // DCA 1-8
                group = 'd';
            }else if (i == 80) {
                // main-LR
                group = 'm';
            }

            if ((mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_VOLUME) || (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_MUTE)))){
                switch (group) {
                    case 'b':
                        if (!chan->mute) {
                            dspSendMixbusVolume(i - 48 + 1, chan->volumeLR, dsp.mainSubVolume, chan->balance);
                        }else{
                            dspSendMixbusVolume(i - 48 + 1, -100, dsp.mainSubVolume, chan->balance);
                        }
                        break;
                    case 'x':
                        if (!chan->mute) {
                            dspSendMatrixVolume(i - 64 + 1, chan->volumeLR);
                        }else{
                            dspSendMatrixVolume(i - 64 + 1, -100);
                        }
                        break;
                    case 'm':
                        if (!chan->mute) {
                            dspSendMainVolume(chan->volumeLR, dsp.mainSubVolume, chan->balance);
                        }else{
                            dspSendMainVolume(-100, dsp.mainSubVolume, chan->balance);
                        }
                        break;
                    case 's':
                        if (!chan->mute) {
                            dspSendMainVolume(dsp.mainLRVolume, chan->volumeLR, dsp.mainBalance);
                        }else{
                            dspSendMainVolume(dsp.mainLRVolume, -100, dsp.mainBalance);
                        }
                        break;
                }
            }
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
                        dspSendChannelVolume(chan->inputSource.dspChannel, chan->volumeLR, chan->volumeSub, chan->balance);
                    }else{
                        dspSendChannelVolume(chan->inputSource.dspChannel, -100, -100, chan->balance); // dBfs
                    }
                }

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_GATE)){
                    dspSendGate(chan->inputSource.dspChannel);
                }

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_EQ)){
                    dspSendEQ(chan->inputSource.dspChannel);
                }

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_DYNAMIC)){
                    dspSendCompressor(chan->inputSource.dspChannel);
                }

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_SENDS)) {
                    // TODO
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
