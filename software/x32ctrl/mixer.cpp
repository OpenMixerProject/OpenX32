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

Mixer::Mixer(X32BaseParameter* basepar): X32Base(basepar) {
    fpga = new Fpga(basepar);
    dsp = new DSP1(basepar);
    adda = new Adda(basepar);
    card = new Card(basepar, adda);

    for(int v=0;v<MAX_VCHANNELS;v++) {
        vchannel[v] = new VChannel(basepar);
    }    
}

void Mixer::Init() {

    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "dsp->Init()");
    dsp->Init();

    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "dsp->SendAll()");
    dsp->SendAll();

    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "fpga->Init()");
    fpga->Init();

    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "adda->Init()");
    adda->Init();

    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "card->Init()");
    card->Init();

    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "LoadVChannelLayout()");
    LoadVChannelLayout();

    if(helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE)) {
		DebugPrintvChannels();
	}
}





void Mixer::LoadVChannelLayout() {
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
        chan->dspChannel =  &dsp->Channel[i];
        chan->name = String("Kanal ") + String(i+1);
        chan->nameIntern = String("CH") + String(i+1);
        chan->color = SURFACE_COLOR_YELLOW;
        chan->vChannelType = X32_VCHANNELTYPE::NORMAL;
    }

    // AUX 1-6 / USB
    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up AUX");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = (uint)X32_VCHANNEL_BLOCK::AUX + i;

        VChannel* chan = vchannel[index];
        chan->dspChannel = &dsp->Channel[index];

        if(i <=5){
            chan->name = String("AUX") + String(i+1);
            chan->nameIntern = chan->name;
            chan->color = SURFACE_COLOR_GREEN;
        } else {
            chan->name = String("USB");
            chan->nameIntern = chan->name;
            chan->color = SURFACE_COLOR_YELLOW;
        }
        chan->vChannelType = X32_VCHANNELTYPE::AUX;
    }

    // FX Returns 1-8
    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up FX Returns");
    for (uint i=0; i<=7;i++){
        uint8_t index = (uint)X32_VCHANNEL_BLOCK::FXRET + i;
        VChannel* chan = vchannel[index];
        chan->name = String("FX RET") + String(i+1);
        chan->nameIntern = chan->name;
        chan->color = SURFACE_COLOR_BLUE;
        chan->vChannelType = X32_VCHANNELTYPE::FXRET;
    }

    // Bus 1-16
    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up Busses");
    for (uint8_t i=0; i<=15;i++){
        uint8_t index = (uint)X32_VCHANNEL_BLOCK::BUS + i;
        VChannel* chan = vchannel[index];
        chan->name = String("BUS") + String(i+1);
        chan->nameIntern = chan->name;
        chan->color = SURFACE_COLOR_CYAN;
        chan->vChannelType = X32_VCHANNELTYPE::BUS;
    }

    // Matrix 1-6 / Special / SUB
    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up Matrix / SPECIAL / SUB");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = (uint)X32_VCHANNEL_BLOCK::MATRIX + i;
        VChannel* chan = vchannel[index];
        if(i <=5){
            chan->name = String("MATRIX") + String(i+1);
            chan->nameIntern = chan->name;
            chan->color =  SURFACE_COLOR_PINK;
            chan->vChannelType = X32_VCHANNELTYPE::MATRIX;
        } else if (i == 6){
            chan->name = String("SPECIAL");
            chan->nameIntern = chan->name;
            chan->color =  SURFACE_COLOR_RED;
            chan->vChannelType = X32_VCHANNELTYPE::SPECIAL;
        } else if (i == 7){
            chan->name = String("M/C");
            chan->nameIntern = chan->name;
            chan->color =  SURFACE_COLOR_WHITE;
            chan->vChannelType = X32_VCHANNELTYPE::MAINSUB;
        };
    }

    // DCA 1-8
    helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up DCA");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = (uint)X32_VCHANNEL_BLOCK::DCA + i;
        VChannel* chan = vchannel[index];
        chan->name = String("DCA") + String(i+1);
        chan->nameIntern = chan->name;
        chan->color = SURFACE_COLOR_PINK;
        chan->vChannelType = X32_VCHANNELTYPE::DCA;;
    }

    // Main Channel
    {
        VChannel* chan = vchannel[(uint)X32_VCHANNEL_BLOCK::MAIN];
        chan->name = String("MAIN");
        chan->nameIntern = chan->name;
        chan->color = SURFACE_COLOR_WHITE;
        chan->vChannelType = X32_VCHANNELTYPE::MAIN;
    }
}


// ####################################################################
// #
// #
// #        Routing + Hardware channel assignment
// #
// #
// ###################################################################


void Mixer::ChangeHardwareInput(uint8_t outputIndex, int8_t amount) {
    // get current routingIndex
    uint8_t currentRouting = fpga->GetOutputByIndex(outputIndex + 1);
    int16_t newValue = currentRouting + amount;

    if (newValue > NUM_INPUT_CHANNEL) {
        newValue -= (NUM_INPUT_CHANNEL + 1);
    }
    if (newValue < 0) {
        newValue += (NUM_INPUT_CHANNEL + 1); // we allow 208 input channels plus a single "OFF" channel
    }

    helper->DEBUG_MIXER(DEBUGLEVEL_NORMAL, "Change! %d -> %d", currentRouting, newValue);

    fpga->ConnectByIndex(newValue, outputIndex + 1);
    fpga->SendRoutingToFpga(outputIndex);
    config->Refresh(ROUTING_FPGA);

    // old
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspInput(uint8_t vChannelIndex, int8_t amount) {
    int16_t newValue = dsp->Channel[vChannelIndex].input + amount;

    if (newValue >= DSP_MAX_INTERNAL_CHANNELS) {
        newValue -= DSP_MAX_INTERNAL_CHANNELS;
    }
    if (newValue < 0) {
        newValue += DSP_MAX_INTERNAL_CHANNELS;
    }

    dsp->Channel[vChannelIndex].input = newValue;
    dsp->SetInputRouting(vChannelIndex);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspInputTapPoint(uint8_t vChannelIndex, int8_t amount) {
    int16_t newValue = dsp->Channel[vChannelIndex].inputTapPoint + amount;

    if (newValue > 4) {
        newValue = 0;
    }
    if (newValue < 0) {
        newValue = 4;
    }

    dsp->Channel[vChannelIndex].inputTapPoint = newValue;
    dsp->SetInputRouting(vChannelIndex);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspOutput(uint8_t channel, int8_t amount) {
    int16_t newValue = dsp->Dsp1toFpga[channel].input + amount;

    if (newValue >= DSP_MAX_INTERNAL_CHANNELS) {
        newValue -= DSP_MAX_INTERNAL_CHANNELS;
    }
    if (newValue < 0) {
        newValue += DSP_MAX_INTERNAL_CHANNELS;
    }

    dsp->Dsp1toFpga[channel].input = newValue;
    dsp->SetOutputRouting(channel);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspOutputTapPoint(uint8_t channel, int8_t amount) {
    int16_t newValue = dsp->Dsp1toFpga[channel].tapPoint + amount;

    if (newValue > 4) {
        newValue = 0;
    }
    if (newValue < 0) {
        newValue = 4;
    }

    dsp->Dsp1toFpga[channel].tapPoint = newValue;
    dsp->SetOutputRouting(channel);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspFxOutput(uint8_t channel, int8_t amount) {
    int16_t newValue = dsp->Dsp1toDsp2Routing[channel].input + amount;

    if (newValue >= DSP_MAX_INTERNAL_CHANNELS) {
        newValue -= DSP_MAX_INTERNAL_CHANNELS;
    }
    if (newValue < 0) {
        newValue += DSP_MAX_INTERNAL_CHANNELS;
    }

    dsp->Dsp1toDsp2Routing[channel].input = newValue;
    dsp->SetFxOutputRouting(channel);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::ChangeDspFxOutputTapPoint(uint8_t channel, int8_t amount) {
    int16_t newValue = dsp->Dsp1toDsp2Routing[channel].tapPoint + amount;

    if (newValue > 4) {
        newValue = 0;
    }
    if (newValue < 0) {
        newValue = 4;
    }

    dsp->Dsp1toDsp2Routing[channel].tapPoint = newValue;
    dsp->SetFxOutputRouting(channel);
    state->SetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void Mixer::SetVChannelChangeFlagsFromIndex(uint8_t p_chanIndex, uint16_t p_flag){
    vchannel[p_chanIndex]->SetChanged(p_flag);
    state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void Mixer::ClearSolo(void){
    if (IsSoloActivated()){
        for (int i=0; i<MAX_VCHANNELS; i++){
            config->Set(MP_ID::CHANNEL_SOLO, 0.0f, i);
        }
    }
}


// oldstyle

void Mixer::SetBusSend(uint8_t vChannelIndex, uint8_t index, float value) {
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    
    float newValue = value;
    if (value > 10) {
        newValue = 10;
    }else if (value < -100) {
        newValue = -100;
    }

    VChannel* chan = GetVChannel(vChannelIndex);

    switch(chan->vChannelType){
        case X32_VCHANNELTYPE::NORMAL:
        case X32_VCHANNELTYPE::AUX: {
            dsp->Channel[vChannelIndex].sendMixbus[index] = newValue;
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE::BUS: {
            // we have only 6 matrices -> check it
            if (index < 6) {
                dsp->Bus[vChannelIndex].sendMatrix[index] = newValue;
            }
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE::MAINSUB: {
            // we have only 6 matrices -> check it
            if (index < 6) {
                dsp->MainChannelSub.sendMatrix[index] = newValue;
            }
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
        case X32_VCHANNELTYPE::MAIN: {
            // we have only 6 matrices -> check it
            if (index < 6) {
                dsp->MainChannelLR.sendMatrix[index] = newValue;
            }
            chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
            break;
        }
    }
}

void Mixer::ChangeBusSend(uint8_t vChannelIndex, uint8_t encoderIndex, int8_t p_amount, uint8_t activeBusSend){
    if (vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    
    float newValue = GetBusSend(vChannelIndex, encoderIndex)  + pow((float)p_amount, 3.0f);

    if (newValue > 10) {
        newValue = 10;
    }else if (newValue < -100) {
        newValue = -100;
    }
    SetBusSend(vChannelIndex, encoderIndex, newValue);
}

void Mixer::SetPeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, float value){
    if (pChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    if (eqIndex >= MAX_CHAN_EQS) {
        return;
    }

    VChannel* chan = GetVChannel(pChannelIndex);

    sPEQ* peq = nullptr;

    if (pChannelIndex < 40) {
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
        dsp->fxmath->RecalcFilterCoefficients_PEQ(peq);

        chan->SetChanged(X32_VCHANNEL_CHANGED_EQ);
    }
}

void Mixer::ChangePeq(uint8_t pChannelIndex, uint8_t eqIndex, char option, int8_t p_amount){
    if (pChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    if (eqIndex >= MAX_CHAN_EQS) {
        return;
    }

    sPEQ* peq = nullptr;

    if (pChannelIndex < 40) {
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

bool Mixer::IsSoloActivated(void){
    for (uint8_t i=0; i < MAX_VCHANNELS; i++)
    {
        if (config->GetBool(MP_ID::CHANNEL_SOLO, i)){
            return true;
        }
    }
    return false;
}


String Mixer::GetCardModelString(){
    return adda->GetExpansion();
}


//#########################################
//#
//#  ######  ##    ## ##    ##  ######  
//# ##    ##  ##  ##  ###   ## ##    ## 
//# ##         ####   ####  ## ##       
//#  ######     ##    ## ## ## ##       
//#       ##    ##    ##  #### ##       
//# ##    ##    ##    ##   ### ##    ## 
//#  ######     ##    ##    ##  ######  
//#
//#########################################

void Mixer::Sync(void){

    vector<MP_ID> filter = { CHANNEL_VOLUME, CHANNEL_MUTE, CHANNEL_PANORAMA };
    if (config->HasParametersChanged(filter))
    {        
        for (auto const& changedIndex : config->GetChangedParameterIndexes(filter))
        {
            dsp->SendChannelVolume(changedIndex);
        }
    }


    if (config->HasParametersChanged(MP_CAT::CHANNEL_GATE))
    {        
        for (auto const& changedIndex : config->GetChangedParameterIndexes(MP_CAT::CHANNEL_GATE))
        {
            dsp->SendGate(changedIndex);
        }
    }

    if (config->HasParameterChanged(CHANNEL_PHANTOM))
    {
        for (auto const& changedIndex : config->GetChangedParameterIndexes({CHANNEL_PHANTOM}))
        {
            halSendPhantomPower(changedIndex);
        }
    }


    // loop trough all vchannels and sync to hardware

    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        VChannel* chan = vchannel[i];

        if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK::NORMAL) ||
            helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK::AUX)) {

            // one of the 40 DSP-channels
            if (chan->HasChanged(X32_VCHANNEL_CHANGED_INPUT)) {
                dsp->SetInputRouting(i);
            }

            if (chan->HasChanged(X32_VCHANNEL_CHANGED_GAIN)){
                halSendGain(i);
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
        } else {
            // one of the other channels like Mixbus, DCA, Main, etc.
            uint8_t group = ' ';
            if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK::FXRET)) {
                // FX Returns 1-8
                group = 'f';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK::BUS)) {
                // Busmaster 1-16
                group = 'b';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK::MATRIX)) {
                // Matrix 1-6
                group = 'x';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK::SPECIAL)) {
                // "VERY SPECIAL CHANNEL"
                group = 'v';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK::MAINSUB)) {
                // Mono/Sub
                group = 's';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK::DCA)) {
                // DCA 1-8
                group = 'd';
            }else if (helper->IsInChannelBlock(i, X32_VCHANNEL_BLOCK::MAIN)) {
                // main-LR
                group = 'm';
            }

            if ((
                chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) ||
                chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)
            )){
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
}

// set the gain of the local XLR head-amp-control
void Mixer::halSendGain(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].input;

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input

            // send value to adda-board
            uint8_t boardId = adda->GetXlrInBoardId(dspInputSource);
            if (boardId >= 0 && boardId < 4) {
                uint8_t addaChannel = dspInputSource;
                while (addaChannel > 8) {
                    addaChannel -= 8;
                }
                adda->SetGain(boardId, addaChannel, preamps.gainXlr[dspInputSource - 1], preamps.phantomPowerXlr[dspInputSource - 1]);
            }
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
        }
    }
}


// enable or disable phatom-power of local XLR-inputs
void Mixer::halSendPhantomPower(uint8_t chanIndex) {
    uint8_t channelInputSource = config->GetUint(CHANNEL_SOURCE, chanIndex);

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((channelInputSource >= 1) && (channelInputSource <= MAX_FPGA_TO_DSP1_CHANNELS)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t dspInputSource = fpga->fpgaRouting.dsp[channelInputSource - 1];
        if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
            // XLR-input

            // send value to adda-board
            uint8_t boardId = adda->GetXlrInBoardId(dspInputSource);
            if (boardId >= 0 && boardId < 4) {
                uint8_t addaChannel = dspInputSource;
                while (addaChannel > 8) {
                    addaChannel -= 8;
                }
                adda->SetGain(boardId, addaChannel, preamps.gainXlr[dspInputSource - 1], preamps.phantomPowerXlr[dspInputSource - 1]);
            }
        }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
            // AES50A input
        }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
            // AES50B input
        }
    }
}

uint8_t Mixer::halGetDspInputSource(uint8_t dspChannel) {
    uint8_t channelInputSource = dsp->Channel[dspChannel].input;

    // check if we are using one of the FPGA-routed channels
    if ((channelInputSource >= 1) && (channelInputSource < 40)) {
        return fpga->fpgaRouting.dsp[channelInputSource - 1];
    }else{
        // DSP is not using one of the FPGA-routed channels
        return 0;
    }
}

uint16_t Mixer::GetSelectedVChannelIndex(void) {
    return config->GetUint(MP_ID::SELECTED_CHANNEL);
}

VChannel* Mixer::GetVChannel(uint8_t vCHannelIndex){
    return vchannel[vCHannelIndex];
}

float Mixer::GetBusSend(uint8_t dspChannel, uint8_t index) {
    if (dspChannel < 40) {
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

void Mixer::DebugPrintvChannels(void){
	for (int i = 0; i < MAX_VCHANNELS; i++)
	{
	   printf(GetVChannel(i)->ToString().c_str());
	}
}

// ######## ##      ## 
// ##        ##    ##  
// ##         ##  ##   
// ######      ####    
// ##         ##  ##   
// ##        ##    ##  
// ##       ##      ## 

void Mixer::ResetFxParameter(uint fxIdx, uint parIdx) {
    FxSlot* f = dsp->fx_slot[fxIdx];
    if (f->HasFx())
    {
        if (f->fx->ResetParameter(parIdx)) {
            dsp->DSP2_SendFxParameter(fxIdx);
        }
    }
}


void Mixer::ChangeFxParameter(uint fxIdx, uint parIdx, int8_t amount) {
    FxSlot* slot = dsp->fx_slot[fxIdx];
    if (slot->HasFx())
    {
        if (slot->fx->ChangeParameter(parIdx, amount)) {
            dsp->DSP2_SendFxParameter(fxIdx);
        }
    }
}




