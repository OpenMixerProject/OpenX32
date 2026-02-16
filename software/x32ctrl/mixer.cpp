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
}





void Mixer::LoadVChannelLayout() {
    // //##################################################################################
    // //#
    // //#   create default vchannels (what the user is refering to as "mixer channel")
    // //#
    // //#   0   -  31  Main DSP-Channels
    // //#   32  -  39  AUX 1-6 / USB
    // //#   40  -  47  FX Returns
    // //#   48  -  63  Bus 1-16
    // //#
    // //#   64  -  71  Matrix 1-6 / Special / Main Center
    // //#   72  -  79  DCA
    // //#   80         Main Left/Right
    // //#
    // //##################################################################################
    
    for (int i=0; i<=31; i++)
    {
        config->Set(CHANNEL_COLOR, SURFACE_COLOR_YELLOW, i);
    }

    // AUX 1-6 / USB
    for (uint8_t i=0; i<=7;i++)
    {
        uint8_t index = (uint)X32_VCHANNEL_BLOCK::AUX + i;

        if(i <=5){
            config->Set(CHANNEL_NAME, String("AUX") + String(i+1), index);
            config->Set(CHANNEL_COLOR, SURFACE_COLOR_GREEN, index);
        } else {
            config->Set(CHANNEL_NAME, String("USB"), index);
            config->Set(CHANNEL_COLOR, SURFACE_COLOR_YELLOW, index);
        }
    }

    // // FX Returns 1-8
    // helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up FX Returns");
    // for (uint i=0; i<=7;i++){
    //     uint8_t index = (uint)X32_VCHANNEL_BLOCK::FXRET + i;
    //     VChannel* chan = vchannel[index];
    //     chan->name = String("FX RET") + String(i+1);
    //     chan->nameIntern = chan->name;
    //     chan->color = SURFACE_COLOR_BLUE;
    //     chan->vChannelType = X32_VCHANNELTYPE::FXRET;
    // }

    // // Bus 1-16
    // helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up Busses");
    // for (uint8_t i=0; i<=15;i++){
    //     uint8_t index = (uint)X32_VCHANNEL_BLOCK::BUS + i;
    //     VChannel* chan = vchannel[index];
    //     chan->name = String("BUS") + String(i+1);
    //     chan->nameIntern = chan->name;
    //     chan->color = SURFACE_COLOR_CYAN;
    //     chan->vChannelType = X32_VCHANNELTYPE::BUS;
    // }

    // // Matrix 1-6 / Special / SUB
    // helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up Matrix / SPECIAL / SUB");
    // for (uint8_t i=0; i<=7;i++){
    //     uint8_t index = (uint)X32_VCHANNEL_BLOCK::MATRIX + i;
    //     VChannel* chan = vchannel[index];
    //     if(i <=5){
    //         chan->name = String("MATRIX") + String(i+1);
    //         chan->nameIntern = chan->name;
    //         chan->color =  SURFACE_COLOR_PINK;
    //         chan->vChannelType = X32_VCHANNELTYPE::MATRIX;
    //     } else if (i == 6){
    //         chan->name = String("SPECIAL");
    //         chan->nameIntern = chan->name;
    //         chan->color =  SURFACE_COLOR_RED;
    //         chan->vChannelType = X32_VCHANNELTYPE::SPECIAL;
    //     } else if (i == 7){
    //         chan->name = String("M/C");
    //         chan->nameIntern = chan->name;
    //         chan->color =  SURFACE_COLOR_WHITE;
    //         chan->vChannelType = X32_VCHANNELTYPE::MAINSUB;
    //     };
    // }

    // // DCA 1-8
    // helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE, "Setting up DCA");
    // for (uint8_t i=0; i<=7;i++){
    //     uint8_t index = (uint)X32_VCHANNEL_BLOCK::DCA + i;
    //     VChannel* chan = vchannel[index];
    //     chan->name = String("DCA") + String(i+1);
    //     chan->nameIntern = chan->name;
    //     chan->color = SURFACE_COLOR_PINK;
    //     chan->vChannelType = X32_VCHANNELTYPE::DCA;;
    // }

    // Main Channel
    {
        config->Set(CHANNEL_NAME, String("Main"),(uint)X32_VCHANNEL_BLOCK::MAIN);
        config->Set(CHANNEL_COLOR, SURFACE_COLOR_WHITE, (uint)X32_VCHANNEL_BLOCK::MAIN);
    }
}


// ####################################################################
// #
// #
// #        Routing + Hardware channel assignment
// #
// #
// ###################################################################



void Mixer::ClearSolo(void){
    if (IsSoloActivated()){
        for (int i=0; i<MAX_VCHANNELS; i++){
            config->Set(MP_ID::CHANNEL_SOLO, 0.0f, i);
        }
    }
}


// oldstyle

void Mixer::SetBusSend(uint8_t vChannelIndex, uint8_t index, float value) {
    // if (vChannelIndex == VCHANNEL_NOT_SET) {
    //     return;
    // }
    
    // float newValue = value;
    // if (value > 10) {
    //     newValue = 10;
    // }else if (value < -100) {
    //     newValue = -100;
    // }


    // switch(chan->vChannelType){
    //     case X32_VCHANNELTYPE::NORMAL:
    //     case X32_VCHANNELTYPE::AUX: {
    //         dsp->Channel[vChannelIndex].sendMixbus[index] = newValue;
    //         //chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
    //         break;
    //     }
    //     case X32_VCHANNELTYPE::BUS: {
    //         // we have only 6 matrices -> check it
    //         if (index < 6) {
    //             dsp->Bus[vChannelIndex].sendMatrix[index] = newValue;
    //         }
    //         //chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
    //         break;
    //     }
    //     case X32_VCHANNELTYPE::MAINSUB: {
    //         // we have only 6 matrices -> check it
    //         if (index < 6) {
    //             dsp->MainChannelSub.sendMatrix[index] = newValue;
    //         }
    //         //chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
    //         break;
    //     }
    //     case X32_VCHANNELTYPE::MAIN: {
    //         // we have only 6 matrices -> check it
    //         if (index < 6) {
    //             dsp->MainChannelLR.sendMatrix[index] = newValue;
    //         }
    //         //chan->SetChanged(X32_VCHANNEL_CHANGED_VOLUME);
    //         break;
    //     }
    // }
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
            if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::NORMAL) ||
                helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::AUX))
            {
                dsp->SendChannelVolume(changedIndex);
            }
            else if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::BUS))
            {
                dsp->SendMixbusVolume(changedIndex - 48);
            }
            else if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::MATRIX))
            {
                dsp->SendMatrixVolume(changedIndex - 64);
            }
            else if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::MAINSUB)) {
                dsp->SendMainVolume();
            }
            else if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::MAIN)) {
                dsp->SendMainVolume();
            }                                  
        }
    }

    if (config->HasParameterChanged(ROUTING_FPGA))
    { 
        for (auto const& changedIndex : config->GetChangedParameterIndexes({ROUTING_FPGA}))
        {
            fpga->SendRoutingToFpga(changedIndex);
        }
    }

    if (config->HasParametersChanged({ROUTING_DSP_OUTPUT, ROUTING_DSP_OUTPUT_TAPPOINT}))
    { 
        for (auto const& changedIndex : config->GetChangedParameterIndexes({ROUTING_DSP_OUTPUT, ROUTING_DSP_OUTPUT_TAPPOINT}))
        {
            dsp->SetOutputRouting(changedIndex);
        }
    }

    if (config->HasParametersChanged({ROUTING_DSP_INPUT, ROUTING_DSP_INPUT_TAPPOINT}))
    { 
        for (auto const& changedIndex : config->GetChangedParameterIndexes({ROUTING_DSP_INPUT, ROUTING_DSP_INPUT_TAPPOINT}))
        {
            dsp->SetInputRouting(changedIndex);
        }
    }

    if (config->HasParametersChanged(MP_CAT::CHANNEL_SENDS))
    { 
        for (auto const& changedIndex : config->GetChangedParameterIndexes(MP_CAT::CHANNEL_SENDS))
        {
            dsp->SendChannelSend(changedIndex);

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

    if (config->HasParameterChanged(CHANNEL_GAIN))
    {
        for (auto const& changedIndex : config->GetChangedParameterIndexes({CHANNEL_GAIN}))
        {
            halSendGain(changedIndex);
        }
    }

    if (config->HasParametersChanged(MP_CAT::CHANNEL_EQ))
    {
        for (auto const& changedIndex : config->GetChangedParameterIndexes(MP_CAT::CHANNEL_EQ))
        {
            // copy values fromm Mixerparameter
            for (uint8_t peq = 0; peq < MAX_CHAN_EQS; peq++)
            {
                dsp->Channel[changedIndex].peq[peq].type = config->GetUint((MP_ID)(to_underlying(CHANNEL_EQ_TYPE1) + peq));
                dsp->Channel[changedIndex].peq[peq].fc = config->GetFloat((MP_ID)(to_underlying(CHANNEL_EQ_FREQ1) + peq));
                dsp->Channel[changedIndex].peq[peq].Q = config->GetFloat((MP_ID)(to_underlying(CHANNEL_EQ_Q1) + peq));
                dsp->Channel[changedIndex].peq[peq].gain = config->GetFloat((MP_ID)(to_underlying(CHANNEL_EQ_GAIN1) + peq));
            }
    
            dsp->SendEQ(changedIndex);
            dsp->SendLowcut(changedIndex);
        }
    }

    if (config->HasParametersChanged(MP_CAT::CHANNEL_DYNAMICS))
    {
        for (auto const& changedIndex : config->GetChangedParameterIndexes(MP_CAT::CHANNEL_DYNAMICS))
        {
            dsp->SendCompressor(changedIndex);
        }
    }
}

// set the gain of the local XLR head-amp-control
void Mixer::halSendGain(uint8_t dspChannel) {
    // get the channel-number of the internal DSP-routing
    uint8_t internalDspSourceIndex = config->GetUint(ROUTING_DSP_INPUT, dspChannel);

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((internalDspSourceIndex >= 1) && (internalDspSourceIndex <= 40)) {
        // we are connected to one of the DSP-inputs from FPGA

        // check if we are connected to a channel with gain
        uint8_t externalDspSourceIndex = config->GetUint(ROUTING_FPGA, (FPGA_OUTPUT_IDX_DSP - 1) + (internalDspSourceIndex - 1));
        if ((externalDspSourceIndex >= FPGA_OUTPUT_IDX_XLR) && (externalDspSourceIndex < (FPGA_OUTPUT_IDX_XLR + 32))) {
            // XLR-input

            // send value to adda-board
            uint8_t boardId = adda->GetXlrInBoardId(externalDspSourceIndex);
            if (boardId >= 0 && boardId < 4) {
                uint8_t addaChannel = externalDspSourceIndex;
                while (addaChannel > 8) {
                    addaChannel -= 8;
                }
                adda->SetGain(boardId, addaChannel, config->GetFloat(CHANNEL_GAIN,  dspChannel), config->GetFloat(CHANNEL_PHANTOM,  dspChannel));
            }
        }else if ((externalDspSourceIndex >= FPGA_OUTPUT_IDX_AES50A) && (externalDspSourceIndex < (FPGA_OUTPUT_IDX_AES50A + 48))) {
            // AES50A input
        }else if ((externalDspSourceIndex >= FPGA_OUTPUT_IDX_AES50B) && (externalDspSourceIndex < (FPGA_OUTPUT_IDX_AES50B + 48))) {
            // AES50B input
        }
    }
}


// enable or disable phatom-power of local XLR-inputs
void Mixer::halSendPhantomPower(uint8_t dspChannel) {
    // get the channel-number of the internal DSP-routing
    uint8_t internalDspSourceIndex = config->GetUint(ROUTING_DSP_INPUT, dspChannel);

    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if ((internalDspSourceIndex >= 1) && (internalDspSourceIndex <= MAX_FPGA_TO_DSP1_CHANNELS)) {
        // we are connected to one of the DSP-inputs

        // check if we are connected to a channel with gain
        uint8_t externalDspSourceIndex = config->GetUint(ROUTING_FPGA, (FPGA_OUTPUT_IDX_DSP - 1) + (internalDspSourceIndex - 1));

        if ((externalDspSourceIndex >= FPGA_OUTPUT_IDX_XLR) && (externalDspSourceIndex < (FPGA_OUTPUT_IDX_XLR + 32))) {
            // XLR-input

            // send value to adda-board
            uint8_t boardId = adda->GetXlrInBoardId(externalDspSourceIndex);

            if (boardId >= 0 && boardId < 4) {
                uint8_t addaChannel = externalDspSourceIndex;
                while (addaChannel > 8) {
                    addaChannel -= 8;
                }
                adda->SetGain(boardId, addaChannel, config->GetFloat(CHANNEL_GAIN,  dspChannel), config->GetFloat(CHANNEL_PHANTOM,  dspChannel));
            }
        }else if ((externalDspSourceIndex >= FPGA_OUTPUT_IDX_AES50A) && (externalDspSourceIndex < (FPGA_OUTPUT_IDX_AES50A + 48))) {
            // AES50A input
        }else if ((externalDspSourceIndex >= FPGA_OUTPUT_IDX_AES50B) && (externalDspSourceIndex < (FPGA_OUTPUT_IDX_AES50B + 48))) {
            // AES50B input
        }
    }
}

// uint8_t Mixer::halGetDspInputSource(uint8_t dspChannel) {
//     uint8_t channelInputSource = config->GetUint(ROUTING_DSP_INPUT, dspChannel);

//     // check if we are using one of the FPGA-routed channels
//     if ((channelInputSource >= 1) && (channelInputSource < 40)) {
//         return fpga->fpgaRouting.dsp[channelInputSource - 1];
//     }else{
//         // DSP is not using one of the FPGA-routed channels
//         return 0;
//     }
// }

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




