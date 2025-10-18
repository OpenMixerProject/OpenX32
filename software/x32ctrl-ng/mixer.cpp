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

#include "mixer.h"

Mixer::Mixer(Config* config, State* state): X32Base(config, state) { 
    dsp = new DSP1(config, state);
    fpga = new Fpga(config, state);
    adda = new Adda(config, state);
    
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        vchannel[i] = new VChannel(config, state);
    }

    //##################################################################################
    //#
    //#   create default vchannels (what the user is refering to as "mixer channel")
    //#
    //#   0   -  31  Main DSP-Channels
    //#   32  -  39  AUX 1-6 / USB
    //#   40  -  47  FX Returns
    //#   48  -  63  Bus 1-16
    //#
    //#   64  -  71  Matrix 1-6 / Special / Main Center
    //#   72  -  79  DCA
    //#   80         Main Left/Right
    //#
    //##################################################################################
    for (int i=0; i<=31; i++) {
        VChannel* chan = vchannel[i];
        chan->name = String("Kanal ") + String(i);
        chan->color = SURFACE_COLOR_YELLOW;
    }

    // AUX 1-6 / USB
    helper->Debug("Setting up AUX\n");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = 32 + i;

        VChannel* chan = vchannel[index];

        chan->dspChannel.inputSource = index + 1;

        if(i <=5){
            chan->name = String("AUX") + String(i+1);
            chan->color = SURFACE_COLOR_GREEN;
        } else {
            chan->name = String("USB");
            chan->color = SURFACE_COLOR_YELLOW;
        }
    }

    // FX Returns 1-8
    helper->Debug("Setting up FX Returns\n");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = 40 + i;
        VChannel* chan = vchannel[index];
        chan->name = String("FX Ret") + String(i+1);
        chan->color = SURFACE_COLOR_BLUE;
    }

    // Bus 1-16
    helper->Debug("Setting up Busses\n");
    for (uint8_t i=0; i<=15;i++){
        uint8_t index = 48 + i;
        VChannel* chan = vchannel[index];
        chan->name = String("BUS") + String(i+1);
        chan->color = SURFACE_COLOR_CYAN;
    }

    // Matrix 1-6 / Special / SUB
    helper->Debug("Setting up Matrix / SPECIAL / SUB\n");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = 64 + i;
        VChannel* chan = vchannel[index];
        if(i <=5){
            chan->name = String("MATRIX") + String(i+1);
            chan->color =  SURFACE_COLOR_PINK;
        } else if (i == 6){
            chan->name = String("SPECIAL");
            chan->color =  SURFACE_COLOR_RED;
        } else if (i == 7){
            chan->name = String("M/C");
            chan->color =  SURFACE_COLOR_WHITE;
        }
    }

    // DCA 1-8
    helper->Debug("Setting up DCA\n");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = 72 + i;
        VChannel* chan = vchannel[index];
        chan->name = String("DCA") + String(i+1);
        chan->color = SURFACE_COLOR_PINK;
    }

    // Main Channel
    {
        VChannel* chan = vchannel[80];
        chan->name = String("Main");
        chan->color = SURFACE_COLOR_WHITE;
        chan->vChannelType = 1; // main channel
    }
}


// ####################################################################
// #
// #
// #        Routing + Hardware channel assignment
// #
// #
// ###################################################################



void Mixer::ChangeHardwareOutput(int8_t amount) {
    // output-taps
    // 1-16 = XLR-outputs
    // 17-32 = UltraNet/P16-outputs
    // 33-64 = Card-outputs
    // 65-72 = AUX-outputs
    // 73-112 = DSP-inputs
    // 113-160 = AES50A-outputs
    // 161-208 = AES50B-outputs

    int16_t newValue = (int16_t)selectedOutputChannelIndex + amount;

    if (newValue > NUM_OUTPUT_CHANNEL) {
        newValue = 1;
    }
    if (newValue < 1) {
        newValue = NUM_OUTPUT_CHANNEL;
    }
    selectedOutputChannelIndex = newValue;
    // no sending to FPGA as we are not changing the hardware-routing here
    state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
}

void Mixer::ChangeHardwareInput(int8_t amount) {
    // get current routingIndex
    int16_t newValue = fpga->RoutingGetOutputSourceByIndex(selectedOutputChannelIndex) + amount;

    if (newValue > NUM_INPUT_CHANNEL) {
        newValue = 0;
    }
    if (newValue < 0) {
        newValue = NUM_INPUT_CHANNEL;
    }
    fpga->RoutingSetOutputSourceByIndex(selectedOutputChannelIndex, newValue);
    fpga->RoutingSendConfigToFpga();
    state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
}



void Mixer::SetVChannelChangeFlagsFromIndex(uint8_t p_chanIndex, uint16_t p_flag){
    vchannel[p_chanIndex]->SetChanged(p_flag);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}



void Mixer::SetSolo(uint8_t p_vChannelIndex, bool solo){
    vchannel[p_vChannelIndex]->solo = solo;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_SOLO);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);

    //TODOs
    // - switch monitor source
}

void Mixer::ToggleSolo(uint8_t vChannelIndex){
    SetSolo(vChannelIndex, !vchannel[vChannelIndex]->solo);
}

void Mixer::ClearSolo(void){
    if (IsSoloActivated()){
        for (int i=0; i<MAX_VCHANNELS; i++){
            SetSolo(i, false);
        }
    }
}

void Mixer::SetPhantom(uint8_t p_vChannelIndex, bool p_phantom){
    // TODO vChannel[p_vChannelIndex].inputSource.phantomPower = p_phantom;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_PHANTOM);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::TogglePhantom(uint8_t p_vChannelIndex){
    // TODO SetPhantom(selectedVChannel, !vChannel[p_vChannelIndex].inputSource.phantomPower);
}

void Mixer::SetPhaseInvert(uint8_t p_vChannelIndex, bool p_phaseInvert){
    // TODO vChannel[p_vChannelIndex].inputSource.phaseInvert = p_phaseInvert;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_PHASE_INVERT);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::TogglePhaseInvert(uint8_t vChannelIndex){
    // TODO SetPhaseInvert(selectedVChannel, !vChannel[vChannelIndex].inputSource.phaseInvert);
}

void Mixer::SetMute(uint8_t p_vChannelIndex, bool mute){
    vchannel[p_vChannelIndex]->mute = mute;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_MUTE);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::ToggleMute(uint8_t vChannelIndex){
    SetMute(vChannelIndex, !vchannel[vChannelIndex]->mute);
}



// volume in dBfs
void Mixer::ChangeGain(uint8_t p_vChannelIndex, int8_t p_amount){
    float newValue;
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    // TODO newValue = vChannel[p_vChannelIndex].inputSource.gain + (2.5f * p_amount);
    if (newValue > 60) {
        newValue = 60;
    }else if (newValue < -12) {
        newValue = -12;
    }
    // TODO vChannel[p_vChannelIndex].inputSource.gain = newValue;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_GAIN);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

// volume in dBfs
void Mixer::ChangeVolume(uint8_t p_vChannelIndex, int8_t p_amount){
    float newValue;
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    newValue = vchannel[p_vChannelIndex]->volumeLR + ((float)p_amount * abs((float)p_amount));
    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }
    vchannel[p_vChannelIndex]->volumeLR = newValue;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_VOLUME);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

// volume in dBfs
void Mixer::SetVolume(uint8_t p_vChannelIndex, float p_volume){
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    float newValue = p_volume;
    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }
    vchannel[p_vChannelIndex]->volumeLR = newValue;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_VOLUME);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::ChangePan(uint8_t p_vChannelIndex, int8_t p_amount){
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    float newValue = vchannel[p_vChannelIndex]->balance + pow((float)p_amount, 3.0f);
    if (newValue > 100) {
        newValue = 100;
    }else if (newValue < -100) {
        newValue = -100;
    }
    vchannel[p_vChannelIndex]->balance = newValue;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_VOLUME);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::ChangeBusSend(uint8_t p_vChannelIndex, uint8_t encoderIndex, int8_t p_amount, uint8_t activeBusSend){
    float newValue;
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    //halSetBusSend(pChannelIndex, (mixer.activeBusSend * 4) + encoderIndex, halGetBusSend(pChannelIndex, (mixer.activeBusSend * 4) + encoderIndex) + ((float)p_amount * abs((float)p_amount)));
    newValue = vchannel[p_vChannelIndex]->sends[(activeBusSend * 4) + encoderIndex] + pow((float)p_amount, 3.0f);
    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }
    vchannel[p_vChannelIndex]->sends[(activeBusSend * 4) + encoderIndex] = newValue;
    SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_SENDS);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}
void Mixer::ChangeGate(uint8_t p_vChannelIndex, int8_t p_amount){
    float newValue;
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    
    if ((p_vChannelIndex >= 0) && (p_vChannelIndex < 40)) {
        newValue = dsp->Channel[p_vChannelIndex].gate.threshold + ((float)p_amount * abs((float)p_amount)) * 0.4f;
        if (newValue > 0) {
            newValue = 0;
        }else if (newValue < -80) {
            newValue = -80;
        }
        
        //mixer.dsp->dspChannel[pChannelIndex].gate.threshold = newValue;
        // TODO dsp->dspChannel[vChannel[p_vChannelIndex].inputSource.dspChannel-1].gate.threshold = newValue;

        SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_GATE);
        state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
    }else{
        // no support for gate at mixbus
    }
}
void Mixer::ChangeLowcut(uint8_t p_vChannelIndex, int8_t p_amount){
    float newValue;
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }

    if ((p_vChannelIndex >= 0) && (p_vChannelIndex < 40)) {
        newValue = dsp->Channel[p_vChannelIndex].lowCutFrequency * (1 + (float)p_amount/20.0f);
        if (newValue > 400) {
            newValue = 400;
        }else if (newValue < 20) {
            newValue = 20;
        }
        //mixer.dsp->dspChannel[pChannelIndex].lowCutFrequency = newValue;
        // TODO dsp->dspChannel[vChannel[p_vChannelIndex].inputSource.dspChannel-1].lowCutFrequency = newValue;
        SetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_EQ);
        state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
    }else{
        // no support for lowcut
    }
}

void Mixer::ChangeDynamics(uint8_t pChannelIndex, int8_t p_amount){
    float newValue;
    if (pChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    if ((pChannelIndex >= 0) && (pChannelIndex < 40)) {
        newValue = dsp->Channel[pChannelIndex].compressor.threshold + ((float)p_amount * abs((float)p_amount)) * 0.4f;
        if (newValue > 0) {
            newValue = 0;
        }else if (newValue < -60) {
            newValue = -60;
        }
        dsp->Channel[pChannelIndex].compressor.threshold = newValue;
        SetVChannelChangeFlagsFromIndex(pChannelIndex, X32_VCHANNEL_CHANGED_DYNAMIC);
        state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
    }else{
        // no support for lowcut
    }
}

void Mixer::SetPeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, float value){
    if (pChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    if (eqIndex >= MAX_CHAN_EQS) {
        return;
    }

    sPEQ* peq;

    if ((pChannelIndex >= 0) && (pChannelIndex < 40)) {
        peq = &dsp->Channel[pChannelIndex].peq[eqIndex];
    }else if ((pChannelIndex >= 48) && (pChannelIndex <= 63)) {
        peq = &dsp->Bus[pChannelIndex - 48].peq[eqIndex];
    }else if ((pChannelIndex >= 64) && (pChannelIndex <= 69)) {
        peq = &dsp->Channel[pChannelIndex - 64].peq[eqIndex];
    }

    if (peq != nullptr) {
        switch (option) {
            case 'Q':
                if (value < 0.3) {
                    peq->Q = 0.3;
                }else if (value > 10) {
                    peq->Q = 10;
                }else{
                    peq->Q = value;
                }
                break;
            case 'F':
                if (value < 20) {
                    peq->fc = 20;
                }else if (value > 20000) {
                    peq->fc = 20000;
                }else{
                    peq->fc = value;
                }
                break;
            case 'G':
                if (value < -15) {
                    peq->gain = -15;
                }else if (value > 15) {
                    peq->gain = 15;
                }else{
                    peq->gain = value;
                }
                break;
            case 'T':
                if (value > 7) {
                    peq->type = 7;
                }else if (value < 0){
                    peq->type = 0;
                }else{
                    peq->type = value;
                }
                break;
        }
        dsp->fx->RecalcFilterCoefficients_PEQ(peq);
        SetVChannelChangeFlagsFromIndex(pChannelIndex, X32_VCHANNEL_CHANGED_EQ);
        state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
    }
}

void Mixer::ChangePeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, int8_t p_amount){
    if (pChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    if (eqIndex >= MAX_CHAN_EQS) {
        return;
    }

    sPEQ* peq;

    if ((pChannelIndex >= 0) && (pChannelIndex < 40)) {
        peq = &dsp->Channel[pChannelIndex].peq[eqIndex];
    }else if ((pChannelIndex >= 48) && (pChannelIndex <= 63)) {
        peq = &dsp->Bus[pChannelIndex - 48].peq[eqIndex];
    }else if ((pChannelIndex >= 64) && (pChannelIndex <= 69)) {
        peq = &dsp->Channel[pChannelIndex - 64].peq[eqIndex];
    }

    if (peq != nullptr) {
        switch (option) {
            case 'Q':
                SetPeq(pChannelIndex, eqIndex, option, peq->Q + ((float)p_amount * abs((float)p_amount))/10.0f);
                break;
            case 'F':
                SetPeq(pChannelIndex, eqIndex, option, pow(10.0f, log10(peq->fc) + (float)p_amount * 0.01f));
                break;
            case 'G':
                SetPeq(pChannelIndex, eqIndex, option, peq->gain + ((float)p_amount * abs((float)p_amount)) * 0.1f);
                break;
            case 'T':
                SetPeq(pChannelIndex, eqIndex, option, peq->type + p_amount);
                break;
        }
    }
}



// ####################################################################
// #
// #
// #        Current state
// #
// #
// ###################################################################




bool Mixer::IsSoloActivated(void){
    //for (int i=0; i<40; i++) {
    for (int i=0; i<MAX_VCHANNELS; i++)
    {
        if (vchannel[i]->solo){
            return true;
        }
    } 
    for (int i=0; i<16; i++) {
        if (dsp->Bus[i].solo){ return true; }
    } 
    for (int i=0; i<8; i++) {
        if (dsp->Matrix[i].solo){ return true; }
    }
    return false;
}

// ####################################################################
// #
// #
// #        Hardware Abstraction
// #
// #
// ####################################################################

void Mixer::halSyncChannelConfigFromMixer(void){
    // loop trough all channels
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        VChannel* chan = vchannel[i];

        if (i < 40) {
            // one of the 40 DSP-channels
            if (chan->HasChanged(X32_VCHANNEL_CHANGED_INPUT)) {
                dsp->SetInputRouting(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_GAIN)){
                halSendGain(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_PHANTOM)){
                halSendPhantomPower(i);
            }

            if ((chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || (chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)))){
                dsp->SendChannelVolume(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_GATE)){
                dsp->SendGate(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_EQ)){
                dsp->SendEQ(i);
                dsp->SendLowcut(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_DYNAMIC)){
                dsp->SendCompressor(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_SENDS)) {
                dsp->SendChannelSend(i);
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

            if ((chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || (chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)))){
                switch (group) {
                    case 'b':
                        dsp->SendMixbusVolume(i - 48);
                        break;
                    case 'x':
                        dsp->SendMatrixVolume(i - 64);
                        break;
                    case 'm':
                        dsp->SendMainVolume();
                        break;
                    case 's':
                        dsp->SendMainVolume();
                        break;
                }
            }
        }
    }

    helper->Debug("Mixer gain to hardware synced\n");
}

/*
The surface of the X32 has following order:
===========================================
mixer->index
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
dsp->dspChannel[idx].inputSource
     0  OFF
 1..32  Input 1-32
33..40  AUX 1-8
41..56  Mixbus 1-16
-------- allow above this line as channel input -------
57..62  Matrix 1-6
63..65  Main L/R/S
66..68  Monitor L/R and Talkback
*/

void Mixer::halSetVolume(uint8_t dspChannel, float volume) {
    if ((dspChannel >= 0) && (dspChannel < 40)) {
        dsp->Channel[dspChannel].volumeLR = volume;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX return
        dsp->volumeFxReturn[dspChannel - 40] = volume;
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        // Mixbus
        dsp->Bus[dspChannel - 48].volumeLR = volume;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        // Matrix
        dsp->Matrix[dspChannel - 64].volume = volume;
    }else if (dspChannel == 70) {
        // Special
        dsp->volumeSpecial = volume;
    }else if (dspChannel == 71) {
        // Main Sub
        dsp->MainChannelSub.volume = volume;
    }else if ((dspChannel >= 72) && (dspChannel < 80)) {
        // DCA 1-8
        dsp->volumeDca[dspChannel - 72] = volume;
    }else if (dspChannel == 80) {
        dsp->MainChannelLR.volume = volume;
    }
}

void Mixer::halSetMute(uint8_t dspChannel, bool mute) {
    if ((dspChannel >= 0) && (dspChannel <= 39)) {
        dsp->Channel[dspChannel].muted = mute;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX-Return
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        dsp->Bus[dspChannel - 48].muted = mute;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        dsp->Matrix[dspChannel - 64].muted = mute;
    }else if (dspChannel == 70) {
        // special
    }else if (dspChannel == 71) {
        dsp->MainChannelSub.muted = mute;
    }else if ((dspChannel >= 72) && (dspChannel <= 79)) {
        // DCA
    }else if (dspChannel == 80) {
        dsp->MainChannelLR.muted = mute;
    }
}

void Mixer::halSetSolo(uint8_t dspChannel, bool solo) {
    if ((dspChannel >= 0) && (dspChannel <= 39)) {
        dsp->Channel[dspChannel].solo = solo;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX-Return
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        dsp->Bus[dspChannel - 48].solo = solo;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        dsp->Matrix[dspChannel - 64].solo = solo;
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

void Mixer::halSetBalance(uint8_t dspChannel, float balance) {
    if ((dspChannel >= 0) && (dspChannel < 40)) {
        dsp->Channel[dspChannel].balance = balance;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX return -> no support for balance
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        // Mixbus
        dsp->Bus[dspChannel - 48].balance = balance;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        // Matrix -> no support for balance
    }else if (dspChannel == 70) {
        // Special -> no support for balance
    }else if (dspChannel == 71) {
        dsp->MainChannelSub.balance = balance; // TODO: check if we want to support balance here
    }else if ((dspChannel >= 72) && (dspChannel < 80)) {
        // DCA 1-8 -> no support for balance
    }else if (dspChannel == 80) {
        dsp->MainChannelLR.balance = balance;
    }
}

void Mixer::halSetGain(uint8_t dspChannel, float gain) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input
           preamps.gainXlr[dspInputSource - 1] = gain;
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
            preamps.gainAes50a[dspInputSource - 1] = gain;
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            preamps.gainAes50b[dspInputSource - 1] = gain;
            // AES50B input
        }
    }
}

void Mixer::halSetPhantomPower(uint8_t dspChannel, bool phantomPower) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input
           preamps.phantomPowerXlr[dspInputSource - 1] = phantomPower;
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            preamps.phantomPowerAes50a[dspInputSource - 1] = phantomPower;
            // AES50A input
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            preamps.phantomPowerAes50b[dspInputSource - 1] = phantomPower;
            // AES50B input
        }
    }
}

void Mixer::halSetPhaseInversion(uint8_t dspChannel, bool phaseInverted) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input
           preamps.phaseInvertXlr[dspInputSource - 1] = phaseInverted;
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
            preamps.phaseAes50a[dspInputSource - 1] = phaseInverted;
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
            preamps.phaseAes50b[dspInputSource - 1] = phaseInverted;
        }
    }
}

void Mixer::halSetBusSend(uint8_t dspChannel, uint8_t index, float value) {
    float newValue;
    if (value > 10) {
        newValue = 10;
    }else if (value < -100) {
        newValue = -100;
    }

    if ((dspChannel >= 0) && (dspChannel < 40)) {
        dsp->Channel[dspChannel].sendMixbus[index] = newValue;
    }else if ((dspChannel >= 48) && (dspChannel < 63)) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            dsp->Bus[dspChannel].sendMatrix[index] = newValue;
        }
    }else if (dspChannel == 71) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            dsp->MainChannelSub.sendMatrix[index] = newValue;
        }
    }else if (dspChannel == 80) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            dsp->MainChannelLR.sendMatrix[index] = newValue;
        }
    }
}

// set the gain of the local XLR head-amp-control
void Mixer::halSendGain(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input

            // send value to adda-board
            uint8_t boardId = adda->GetBoardId(dspInputSource);
            uint8_t addaChannel = dspInputSource;
            while (addaChannel > 8) {
               addaChannel -= 8;
            }
            adda->SetGain(boardId, addaChannel, preamps.gainXlr[dspInputSource - 1], preamps.phantomPowerXlr[dspInputSource - 1]);
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
        }
    }
}


// enable or disable phatom-power of local XLR-inputs
void Mixer::halSendPhantomPower(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input

            // send value to adda-board
            uint8_t boardId = adda->GetBoardId(dspInputSource);
            uint8_t addaChannel = dspInputSource;
            while (addaChannel > 8) {
               addaChannel -= 8;
            }
            adda->SetGain(boardId, addaChannel, preamps.gainXlr[dspInputSource - 1], preamps.phantomPowerXlr[dspInputSource - 1]);
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
        }
    }
}

uint8_t Mixer::halGetDspInputSource(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].inputSource;

    // check if we are using one of the FPGA-routed channels
    if ((channelInputSource >= 1) && (channelInputSource < 40)) {
        return fpga->fpgaRouting.dsp[channelInputSource - 1];
    }else{
        // DSP is not using one of the FPGA-routed channels
        return 0;
    }
}

float Mixer::halGetVolume(uint8_t dspChannel) {
    if ((dspChannel >= 0) && (dspChannel <= 39)) {
        uint8_t channelInputSource = dsp->Channel[dspChannel].inputSource;

        // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
        if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
            // we are connected to one of the DSP-inputs
            return dsp->Channel[channelInputSource - 1].volumeLR;
        }else if ((channelInputSource >= 41) && (channelInputSource <= 56)) {
            // Mixbus
            return dsp->Bus[channelInputSource - 41].volumeLR;
        }else if ((channelInputSource >= 57) && (channelInputSource <= 62)) {
            // Matrix
            return dsp->Matrix[channelInputSource - 57].volume;
        }else if ((channelInputSource == 63) || (channelInputSource == 64)) {
            // Main LR
            return dsp->MainChannelLR.volume;
        }else if (channelInputSource == 65) {
            // Main Sub
            return dsp->MainChannelSub.volume;
        }else{
            // we are connected to an internal DSP-signal
            return -100;
        }
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX return
        return dsp->volumeFxReturn[dspChannel - 40];
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        // Mixbus
        return dsp->Bus[dspChannel - 48].volumeLR;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        // Matrix
        return dsp->Matrix[dspChannel - 64].volume;
    }else if (dspChannel == 70) {
        // Special
        return dsp->volumeSpecial;
    }else if (dspChannel == 71) {
        // Main Sub
        return dsp->MainChannelSub.volume;
    }else if ((dspChannel >= 72) && (dspChannel < 80)) {
        // DCA 1-8
        return dsp->volumeDca[dspChannel - 72];
    }else if (dspChannel == 80) {
        return dsp->MainChannelLR.volume;
    }else{
        return -100;
    }
}

bool Mixer::halGetMute(uint8_t dspChannel) {
    if ((dspChannel >= 0) && (dspChannel <= 39)) {
        return dsp->Channel[dspChannel].muted;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX-Return
        return false;
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        return dsp->Bus[dspChannel - 48].muted;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        return dsp->Matrix[dspChannel - 64].muted;
    }else if (dspChannel == 70) {
        // special
        return false;
    }else if (dspChannel == 71) {
        return dsp->MainChannelSub.muted;
    }else if ((dspChannel >= 72) && (dspChannel <= 79)) {
        // DCA
        return false;
    }else if (dspChannel == 80) {
        return dsp->MainChannelLR.muted;
    }

    return false;
}

bool Mixer::halGetSolo(uint8_t dspChannel) {
    if ((dspChannel >= 0) && (dspChannel <= 39)) {
        return dsp->Channel[dspChannel].solo;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX-Return
        return false;
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        return dsp->Bus[dspChannel - 48].solo;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        return dsp->Matrix[dspChannel - 64].solo;
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

    return false;
}

float Mixer::halGetBalance(uint8_t dspChannel) {
    if ((dspChannel >= 0) && (dspChannel < 40)) {
        return dsp->Channel[dspChannel].balance;
    }else if ((dspChannel >= 40) && (dspChannel <= 47)) {
        // FX return -> no support for balance
        return 0;
    }else if ((dspChannel >= 48) && (dspChannel <= 63)) {
        // Mixbus
        return dsp->Bus[dspChannel - 48].balance;
    }else if ((dspChannel >= 64) && (dspChannel <= 69)) {
        // Matrix -> no support for balance
        return 0;
    }else if (dspChannel == 70) {
        // Special -> no support for balance
        return 0;
    }else if (dspChannel == 71) {
        return dsp->MainChannelSub.balance; // TODO: check if we want to support balance here
    }else if ((dspChannel >= 72) && (dspChannel < 80)) {
        // DCA 1-8 -> no support for balance
        return 0;
    }else if (dspChannel == 80) {
        return dsp->MainChannelLR.balance;
    }
    
    return 0;
}

float Mixer::halGetGain(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input

            return preamps.gainXlr[dspInputSource - 1];
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
            return preamps.gainAes50a[dspInputSource - 113];
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
            return preamps.gainAes50b[dspInputSource - 161];
        }
    }
    
    // we are connected to an internal DSP-signal OR not connected at all
    return 0;
}

bool Mixer::halGetPhantomPower(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input
            return preamps.phantomPowerXlr[dspInputSource - 1];
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
            return preamps.phantomPowerAes50a[dspInputSource - 113];
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
            return preamps.phantomPowerAes50b[dspInputSource - 161];
        }
    }

    // we are connected to an internal DSP-signal OR not connected at all
    return 0;
}

bool Mixer::halGetPhaseInvert(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].inputSource;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input
            return preamps.phaseInvertXlr[dspInputSource - 1];
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
            return preamps.phaseAes50a[dspInputSource - 113];
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
            return preamps.phaseAes50b[dspInputSource - 161];
        }
    }

    // we are connected to an internal DSP-signal OR not connected at all
    return 0;
}

float Mixer::halGetBusSend(uint8_t dspChannel, uint8_t index) {
    if ((dspChannel >= 0) && (dspChannel < 40)) {
        return dsp->Channel[dspChannel].sendMixbus[index];
    }else if ((dspChannel >= 48) && (dspChannel < 63)) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            return dsp->Bus[dspChannel].sendMatrix[index];
        }
    }else if (dspChannel == 71) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            return dsp->MainChannelSub.sendMatrix[index];
        }
    }else if (dspChannel == 80) {
        // we have only 6 matrices -> check it
        if (index < 6) {
            return dsp->MainChannelLR.sendMatrix[index];
        }
    }

    return 0;
}