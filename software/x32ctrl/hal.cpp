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
#include "dsp.h"

/*
The surface of the X32 has following order:
===========================================
mixer.index
 0..31  Channel Input
32..39  AUX-Input
40..47  FX Returns
48..63  Mixbus 1-16
64..69  Matrix 1-6
    70  Special
    71  Mono/Sub
72..79  DCA 1-8
    80  MainLR

The DSP is using the following order in the internal buffer:
============================================================
mixer.dsp.dspChannel[idx].inputSource
     0  OFF
 1..32  Input 1-32
33..40  AUX 1-8
41..56  Mixbus 1-16
-------- allow above this line as channel input -------
57..62  Matrix 1-6
63..65  Main L/R/S
66..68  Monitor L/R and Talkback
*/

void halSyncChannelConfigFromMixer(void){
    // loop trough all channels
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        sChannel* chan = &mixer.channel[i];

        if (i < 40) {
            // one of the 40 DSP-channels
            if (mixerHasChannelChanged(chan, X32_CHANNEL_CHANGED_INPUT)) {
                //dspSetInputRouting(chan->index);
            }

            if (mixerHasChannelChanged(chan, X32_CHANNEL_CHANGED_GAIN)){
                halSendGain(chan->index);
            }

            if (mixerHasChannelChanged(chan, X32_CHANNEL_CHANGED_PHANTOM)){
                halSendPhantomPower(chan->index);
            }

            if ((mixerHasChannelChanged(chan, X32_CHANNEL_CHANGED_VOLUME) || (mixerHasChannelChanged(chan, X32_CHANNEL_CHANGED_MUTE)))){
                dspSendChannelVolume(chan->index);
            }

            if (mixerHasChannelChanged(chan, X32_CHANNEL_CHANGED_GATE)){
                dspSendGate(chan->index);
            }

            if (mixerHasChannelChanged(chan, X32_CHANNEL_CHANGED_EQ)){
                dspSendEQ(chan->index);
                dspSendLowcut(chan->index);
            }

            if (mixerHasChannelChanged(chan, X32_CHANNEL_CHANGED_DYNAMIC)){
                dspSendCompressor(chan->index);
            }

            if (mixerHasChannelChanged(chan, X32_CHANNEL_CHANGED_SENDS)) {
                dspSendChannelSend(chan->index);
            }
        }else{
            // one of the other channels like Mixbus, DCA, Main, etc.
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

            if ((mixerHasChannelChanged(chan, X32_CHANNEL_CHANGED_VOLUME) || (mixerHasChannelChanged(chan, X32_CHANNEL_CHANGED_MUTE)))){
                switch (group) {
                    case 'b':
                        dspSendMixbusVolume(i - 48);
                        break;
                    case 'x':
                        dspSendMatrixVolume(i - 64);
                        break;
                    case 'm':
                        dspSendMainVolume();
                        break;
                    case 's':
                        dspSendMainVolume();
                        break;
                }
            }
        }
    }

    x32debug("Mixer gain to hardware synced\n");
}

void halSetVolume(uint8_t dspChannel, float volume) {
    if ((dspChannel >= 0) && (dspChannel < 40)) {
        mixer.dsp.dspChannel[dspChannel].volumeLR = volume;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX return
        mixer.dsp.volumeFxReturn[dspChannel - 40] = volume;
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        // Mixbus
        mixer.dsp.mixbusChannel[dspChannel - 48].volumeLR = volume;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        // Matrix
        mixer.dsp.matrixChannel[dspChannel - 64].volume = volume;
    }else if (dspChannel == 70) {
        // Special
        mixer.dsp.volumeSpecial = volume;
    }else if (dspChannel == 71) {
        // Main Sub
        mixer.dsp.mainChannelSub.volume = volume;
    }else if ((dspChannel >= 72) && (dspChannel < 80)) {
        // DCA 1-8
        mixer.dsp.volumeDca[dspChannel - 72] = volume;
    }else if (dspChannel == 80) {
        mixer.dsp.mainChannelLR.volume = volume;
    }
}

void halSetMute(uint8_t dspChannel, bool mute) {
    if ((dspChannel >= 0) && (dspChannel <= 39)) {
        mixer.dsp.dspChannel[dspChannel].muted = mute;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX-Return
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        mixer.dsp.mixbusChannel[dspChannel - 48].muted = mute;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        mixer.dsp.matrixChannel[dspChannel - 64].muted = mute;
    }else if (dspChannel == 70) {
        // special
    }else if (dspChannel == 71) {
        mixer.dsp.mainChannelSub.muted = mute;
    }else if ((dspChannel >= 72) && (dspChannel <= 79)) {
        // DCA
    }else if (dspChannel == 80) {
        mixer.dsp.mainChannelLR.muted = mute;
    }
}

void halSetSolo(uint8_t dspChannel, bool solo) {
    if ((dspChannel >= 0) && (dspChannel <= 39)) {
        mixer.dsp.dspChannel[dspChannel].solo = solo;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX-Return
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        mixer.dsp.mixbusChannel[dspChannel - 48].solo = solo;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        mixer.dsp.matrixChannel[dspChannel - 64].solo = solo;
    }else if (dspChannel == 70) {
        // special
    }else if (dspChannel == 71) {
        // MainSub
    }else if ((dspChannel >= 72) && (dspChannel <= 79)) {
        // DCA
    }else if (dspChannel == 80) {
        // MainLR
    }
}

void halSetBalance(uint8_t dspChannel, float balance) {
    if ((dspChannel >= 0) && (dspChannel < 40)) {
        mixer.dsp.dspChannel[dspChannel].balance = balance;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX return -> no support for balance
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        // Mixbus
        mixer.dsp.mixbusChannel[dspChannel - 48].balance = balance;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        // Matrix -> no support for balance
    }else if (dspChannel == 70) {
        // Special -> no support for balance
    }else if (dspChannel == 71) {
        mixer.dsp.mainChannelSub.balance = balance; // TODO: check if we want to support balance here
    }else if ((dspChannel >= 72) && (dspChannel < 80)) {
        // DCA 1-8 -> no support for balance
    }else if (dspChannel == 80) {
        mixer.dsp.mainChannelLR.balance = balance;
    }
}

void halSetGain(uint8_t dspChannel, float gain) {
    uint8_t channelInputSource = mixer.dsp.dspChannel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = mixer.fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input
           mixer.preamps.gainXlr[dspInputSource - 1] = gain;
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
            mixer.preamps.gainAes50a[dspInputSource - 1] = gain;
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            mixer.preamps.gainAes50b[dspInputSource - 1] = gain;
            // AES50B input
        }
    }else{
        // we are connected to an internal DSP-signal
        return;
    }
}

void halSetPhantomPower(uint8_t dspChannel, bool phantomPower) {
    uint8_t channelInputSource = mixer.dsp.dspChannel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = mixer.fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input
           mixer.preamps.phantomPowerXlr[dspInputSource - 1] = phantomPower;
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            mixer.preamps.phantomPowerAes50a[dspInputSource - 1] = phantomPower;
            // AES50A input
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            mixer.preamps.phantomPowerAes50b[dspInputSource - 1] = phantomPower;
            // AES50B input
        }
    }else{
        // we are connected to an internal DSP-signal
        return;
    }
}

void halSetPhaseInversion(uint8_t dspChannel, bool phaseInverted) {
    uint8_t channelInputSource = mixer.dsp.dspChannel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = mixer.fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input
           mixer.preamps.phaseInvertXlr[dspInputSource - 1] = phaseInverted;
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
            mixer.preamps.phaseAes50a[dspInputSource - 1] = phaseInverted;
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
            mixer.preamps.phaseAes50b[dspInputSource - 1] = phaseInverted;
        }
    }else{
        // we are connected to an internal DSP-signal
        return;
    }
}

void halSetBusSend(uint8_t dspChannel, uint8_t index, float value) {
    float newValue;
    if (value > 10) {
        newValue = 10;
    }else if (value < -100) {
        newValue = -100;
    }

    if ((dspChannel >= 0) && (dspChannel < 40)) {
        mixer.dsp.dspChannel[dspChannel].sendMixbus[index] = newValue;
    }else if ((dspChannel >= 48) && (dspChannel < 63)) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            mixer.dsp.mixbusChannel[dspChannel].sendMatrix[index] = newValue;
        }
    }else if (dspChannel == 71) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            mixer.dsp.mainChannelSub.sendMatrix[index] = newValue;
        }
    }else if (dspChannel == 80) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            mixer.dsp.mainChannelLR.sendMatrix[index] = newValue;
        }
    }
}

// set the gain of the local XLR head-amp-control
void halSendGain(uint8_t dspChannel) {
    uint8_t channelInputSource = mixer.dsp.dspChannel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = mixer.fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input

            // send value to adda-board
            uint8_t boardId = addaGetBoardId(dspInputSource);
            uint8_t addaChannel = dspInputSource;
            while (addaChannel > 8) {
               addaChannel -= 8;
            }
            addaSetGain(boardId, addaChannel, mixer.preamps.gainXlr[dspInputSource - 1], mixer.preamps.phantomPowerXlr[dspInputSource - 1]);
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
        }
    }else{
        // we are connected to an internal DSP-signal
        return;
    }
}


// enable or disable phatom-power of local XLR-inputs
void halSendPhantomPower(uint8_t dspChannel) {
    uint8_t channelInputSource = mixer.dsp.dspChannel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = mixer.fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input

            // send value to adda-board
            uint8_t boardId = addaGetBoardId(dspInputSource);
            uint8_t addaChannel = dspInputSource;
            while (addaChannel > 8) {
               addaChannel -= 8;
            }
            addaSetGain(boardId, addaChannel, mixer.preamps.gainXlr[dspInputSource - 1], mixer.preamps.phantomPowerXlr[dspInputSource - 1]);
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
        }
    }else{
        // we are connected to an internal DSP-signal
        return;
    }
}

uint8_t halGetDspInputSource(uint8_t dspChannel) {
    uint8_t channelInputSource = mixer.dsp.dspChannel[dspChannel].inputSource;

    // check if we are using one of the FPGA-routed channels
    if ((channelInputSource >= 1) && (channelInputSource < 40)) {
        return mixer.fpgaRouting.dsp[channelInputSource - 1];
    }else{
        // DSP is not using one of the FPGA-routed channels
        return 0;
    }
}

float halGetVolume(uint8_t dspChannel) {
    if ((dspChannel >= 0) && (dspChannel <= 39)) {
        uint8_t channelInputSource = mixer.dsp.dspChannel[dspChannel].inputSource;

        // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
        if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
            // we are connected to one of the DSP-inputs
            return mixer.dsp.dspChannel[channelInputSource - 1].volumeLR;
        }else if ((channelInputSource >= 41) && (channelInputSource <= 56)) {
            // Mixbus
            return mixer.dsp.mixbusChannel[channelInputSource - 41].volumeLR;
        }else if ((channelInputSource >= 57) && (channelInputSource <= 62)) {
            // Matrix
            return mixer.dsp.matrixChannel[channelInputSource - 57].volume;
        }else if ((channelInputSource == 63) || (channelInputSource == 64)) {
            // Main LR
            return mixer.dsp.mainChannelLR.volume;
        }else if (channelInputSource == 65) {
            // Main Sub
            return mixer.dsp.mainChannelSub.volume;
        }else{
            // we are connected to an internal DSP-signal
            return -100;
        }
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX return
        return mixer.dsp.volumeFxReturn[dspChannel - 40];
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        // Mixbus
        return mixer.dsp.mixbusChannel[dspChannel - 48].volumeLR;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        // Matrix
        return mixer.dsp.matrixChannel[dspChannel - 64].volume;
    }else if (dspChannel == 70) {
        // Special
        return mixer.dsp.volumeSpecial;
    }else if (dspChannel == 71) {
        // Main Sub
        return mixer.dsp.mainChannelSub.volume;
    }else if ((dspChannel >= 72) && (dspChannel < 80)) {
        // DCA 1-8
        return mixer.dsp.volumeDca[dspChannel - 72];
    }else if (dspChannel == 80) {
        return mixer.dsp.mainChannelLR.volume;
    }else{
        return -100;
    }
}

bool halGetMute(uint8_t dspChannel) {
    if ((dspChannel >= 0) && (dspChannel <= 39)) {
        return mixer.dsp.dspChannel[dspChannel].muted;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX-Return
        return false;
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        return mixer.dsp.mixbusChannel[dspChannel - 48].muted;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        return mixer.dsp.matrixChannel[dspChannel - 64].muted;
    }else if (dspChannel == 70) {
        // special
        return false;
    }else if (dspChannel == 71) {
        return mixer.dsp.mainChannelSub.muted;
    }else if ((dspChannel >= 72) && (dspChannel <= 79)) {
        // DCA
        return false;
    }else if (dspChannel == 80) {
        return mixer.dsp.mainChannelLR.muted;
    }
}

bool halGetSolo(uint8_t dspChannel) {
    if ((dspChannel >= 0) && (dspChannel <= 39)) {
        return mixer.dsp.dspChannel[dspChannel].solo;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX-Return
        return false;
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        return mixer.dsp.mixbusChannel[dspChannel - 48].solo;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        return mixer.dsp.matrixChannel[dspChannel - 64].solo;
    }else if (dspChannel == 70) {
        // special
        return false;
    }else if (dspChannel == 71) {
        return false;
    }else if ((dspChannel >= 72) && (dspChannel <= 79)) {
        // DCA
        return false;
    }else if (dspChannel == 80) {
        return false;
    }   
}

float halGetBalance(uint8_t dspChannel) {
    if ((dspChannel >= 0) && (dspChannel < 40)) {
        return mixer.dsp.dspChannel[dspChannel].balance;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX return -> no support for balance
        return 0;
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        // Mixbus
        return mixer.dsp.mixbusChannel[dspChannel - 48].balance;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        // Matrix -> no support for balance
        return 0;
    }else if (dspChannel == 70) {
        // Special -> no support for balance
        return 0;
    }else if (dspChannel == 71) {
        return mixer.dsp.mainChannelSub.balance; // TODO: check if we want to support balance here
    }else if ((dspChannel >= 72) && (dspChannel < 80)) {
        // DCA 1-8 -> no support for balance
        return 0;
    }else if (dspChannel == 80) {
        return mixer.dsp.mainChannelLR.balance;
    }else{
        return 0;
    }
}

float halGetGain(uint8_t dspChannel) {
    uint8_t channelInputSource = mixer.dsp.dspChannel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = mixer.fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input

            return mixer.preamps.gainXlr[dspInputSource - 1];
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
            return mixer.preamps.gainAes50a[dspInputSource - 113];
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
            return mixer.preamps.gainAes50b[dspInputSource - 161];
        }
    }else{
        // we are connected to an internal DSP-signal
        return 0;
    }
}

bool halGetPhantomPower(uint8_t dspChannel) {
    uint8_t channelInputSource = mixer.dsp.dspChannel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = mixer.fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input
            return mixer.preamps.phantomPowerXlr[dspInputSource - 1];
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
            return mixer.preamps.phantomPowerAes50a[dspInputSource - 113];
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
            return mixer.preamps.phantomPowerAes50b[dspInputSource - 161];
        }
    }else{
        // we are connected to an internal DSP-signal
        return 0;
    }
}

bool halGetPhaseInvert(uint8_t dspChannel) {
    uint8_t channelInputSource = mixer.dsp.dspChannel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = mixer.fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input
            return mixer.preamps.phaseInvertXlr[dspInputSource - 1];
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
            return mixer.preamps.phaseAes50a[dspInputSource - 113];
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
            return mixer.preamps.phaseAes50b[dspInputSource - 161];
        }
    }else{
        // we are connected to an internal DSP-signal
        return 0;
    }
}

float halGetBusSend(uint8_t dspChannel, uint8_t index) {
    if ((dspChannel >= 0) && (dspChannel < 40)) {
        return mixer.dsp.dspChannel[dspChannel].sendMixbus[index];
    }else if ((dspChannel >= 48) && (dspChannel < 63)) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            return mixer.dsp.mixbusChannel[dspChannel].sendMatrix[index];
        }
    }else if (dspChannel == 71) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            return mixer.dsp.mainChannelSub.sendMatrix[index];
        }
    }else if (dspChannel == 80) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            return mixer.dsp.mainChannelLR.sendMatrix[index];
        }
    }

    return 0;
}