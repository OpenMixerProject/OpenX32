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
    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "fpga->Init()");
    fpga->Init();

    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "dsp->Init()");
    dsp->Init();

    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "adda->Init()");
    adda->Init();

    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "card->Init()");
    card->Init();

    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "Load Default Channel Layout");
    LoadVChannelLayout();

    helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "Load Default Routing");
    LoadRoutingDefault();
}

// #################################################################################################################################
//
// ########  ######## ########    ###    ##     ## ##       ######## 
// ##     ## ##       ##         ## ##   ##     ## ##          ##    
// ##     ## ##       ##        ##   ##  ##     ## ##          ##    
// ##     ## ######   ######   ##     ## ##     ## ##          ##    
// ##     ## ##       ##       ######### ##     ## ##          ##    
// ##     ## ##       ##       ##     ## ##     ## ##          ##    
// ########  ######## ##       ##     ##  #######  ########    ##    
//
//
//  ######  ##     ##    ###    ##    ## ##    ## ######## ##       ##          ###    ##    ##  #######  ##     ## ######## 
// ##    ## ##     ##   ## ##   ###   ## ###   ## ##       ##       ##         ## ##    ##  ##  ##     ## ##     ##    ##    
// ##       ##     ##  ##   ##  ####  ## ####  ## ##       ##       ##        ##   ##    ####   ##     ## ##     ##    ##    
// ##       ######### ##     ## ## ## ## ## ## ## ######   ##       ##       ##     ##    ##    ##     ## ##     ##    ##    
// ##       ##     ## ######### ##  #### ##  #### ##       ##       ##       #########    ##    ##     ## ##     ##    ##    
// ##    ## ##     ## ##     ## ##   ### ##   ### ##       ##       ##       ##     ##    ##    ##     ## ##     ##    ##    
//  ######  ##     ## ##     ## ##    ## ##    ## ######## ######## ######## ##     ##    ##     #######   #######     ##    
//
// #################################################################################################################################


void Mixer::LoadVChannelLayout()
{
    // Channel 1-32
    for (int i=0; i<=31; i++)
    {
        config->Set(CHANNEL_COLOR, SURFACE_COLOR_YELLOW, i);
    }

    // AUX 1-6 / USB
    for (uint8_t i=0; i<=7;i++)
    {
        uint8_t index = (uint)X32_VCHANNEL_BLOCK::AUX + i;

        config->Set(CHANNEL_NAME, String("AUX") + String(i+1), index);
        config->Set(CHANNEL_COLOR, SURFACE_COLOR_GREEN, index);
    }

    // FX Returns 1-8
    for (uint i=0; i<=7;i++)
    {
        uint8_t index = (uint)X32_VCHANNEL_BLOCK::FXRET + i;
        
        config->Set(CHANNEL_NAME, String("FX RET") + String(i+1), index);
        config->Set(CHANNEL_COLOR, SURFACE_COLOR_BLUE, index);
    }

    // Bus 1-16
    for (uint8_t i=0; i<=15;i++)
    {
        uint8_t index = (uint)X32_VCHANNEL_BLOCK::BUS + i;
    
        config->Set(CHANNEL_NAME, String("BUS") + String(i+1), index);
        config->Set(CHANNEL_COLOR, SURFACE_COLOR_CYAN, index);
    }

    // Matrix 1-6 / Special / SUB
    for (uint8_t i=0; i<=7;i++)
    {
        uint8_t index = (uint)X32_VCHANNEL_BLOCK::MATRIX + i;
        
        if(i <=5)
        {
            config->Set(CHANNEL_NAME, String("MATRIX") + String(i+1), index);
            config->Set(CHANNEL_COLOR, SURFACE_COLOR_PINK, index);
        }
        else if (i == 6)
        {
            config->Set(CHANNEL_NAME, String("SPECIAL"), index);
            config->Set(CHANNEL_COLOR, SURFACE_COLOR_RED, index);
        }
        else if (i == 7)
        {
            config->Set(CHANNEL_NAME, String("M/C"), index);
            config->Set(CHANNEL_COLOR, SURFACE_COLOR_RED, index);
        };
    }

    // DCA 1-8
    for (uint8_t i=0; i<=7;i++)
    {
        uint8_t index = (uint)X32_VCHANNEL_BLOCK::DCA + i;

        config->Set(CHANNEL_NAME, String("DCA") + String(i+1), index);
        config->Set(CHANNEL_COLOR, SURFACE_COLOR_PINK, index);
    }

    // Main Channel
    {
        config->Set(CHANNEL_NAME, String("Main"),(uint)X32_VCHANNEL_BLOCK::MAIN);
        config->Set(CHANNEL_COLOR, SURFACE_COLOR_WHITE, (uint)X32_VCHANNEL_BLOCK::MAIN);
    }
}

// #################################################################################################################################
//
// ########  ######## ########    ###    ##     ## ##       ######## 
// ##     ## ##       ##         ## ##   ##     ## ##          ##    
// ##     ## ##       ##        ##   ##  ##     ## ##          ##    
// ##     ## ######   ######   ##     ## ##     ## ##          ##    
// ##     ## ##       ##       ######### ##     ## ##          ##    
// ##     ## ##       ##       ##     ## ##     ## ##          ##    
// ########  ######## ##       ##     ##  #######  ########    ##    
//
//
// ########   #######  ##     ## ######## #### ##    ##  ######   
// ##     ## ##     ## ##     ##    ##     ##  ###   ## ##    ##  
// ##     ## ##     ## ##     ##    ##     ##  ####  ## ##        
// ########  ##     ## ##     ##    ##     ##  ## ## ## ##   #### 
// ##   ##   ##     ## ##     ##    ##     ##  ##  #### ##    ##  
// ##    ##  ##     ## ##     ##    ##     ##  ##   ### ##    ##  
// ##     ##  #######   #######     ##    #### ##    ##  ######   
//
// #################################################################################################################################

void Mixer::LoadRoutingDefault()
{
    // FPGA	

	// XLR-inputs 1-32 -> DSP-inputs 1-32
	for (uint8_t ch = 0; ch < 32; ch++) {
		config->Set(ROUTING_FPGA, ch + 1, ch + 72);
	}
	// AUX-inputs 1-8 -> DSP-inputs 33-40
	for (uint8_t ch = 0; ch < 8; ch++) {
		config->Set(ROUTING_FPGA, ch + 65, ch + 104);
	}

	// ---------------------------------------------

	// DSP Return 1-16 -> XLR-outputs 1-16
	for (uint8_t ch = 0; ch < 16; ch++) {
		config->Set(ROUTING_FPGA, ch + 72 + 1, ch);
	}
	// DSP Return 17-32 -> P16-output 1-16
	for (uint8_t ch = 0; ch < 16; ch++) {
		config->Set(ROUTING_FPGA, ch + 72 + 16 + 1, ch + 16);
	}
	// DSP Return 33-40 -> AUX-outputs 1-6 + ControlRoom L/R
	for (uint8_t ch = 0; ch < 8 ; ch++) {
		config->Set(ROUTING_FPGA, ch + 72 + 32 + 1, ch + 64);
	}

	// ---------------------------------------------

	// XLR-inputs 1-32 -> Card 1-32
	for (uint8_t ch = 0; ch < 32; ch++) {
		config->Set(ROUTING_FPGA, ch + 1, ch + 32);
	}

    // DSP1

    for (uint8_t i = 0; i < 40; i++)
    {
        // DSP1 Routing Sources
        //
        // 0:       OFF
        // 1..32:   Input 1-32 (from FPGA)
        // 33..38:  AuxIn 1-6 (from FPGA)
        // 39:      Talkback intern (from FPGA)
        // 40:      Talkback extern (from FPGA)
        // 41..56:  MixBus 1-16
        // 57..62:  Matrix 1-6
        // 63:      MainL
        // 64:      MainR
        // 65:      MainSub
        // 66:      Monitor L
        // 67:      Monitor R
        // 68:      TalkBack
        // 69..92:  DSP2 Return 1-24 (from DSP2)

        // connect FPGA2DSP-Source 1-40 to all 40 Mixing Channels (1-32 + AUX 1-8)
        config->Set(ROUTING_DSP_INPUT, DSP_BUF_IDX_DSPCHANNEL + i, i);
        config->Set(ROUTING_DSP_INPUT_TAPPOINT, to_underlying(DSP_TAP::INPUT), i);
    }

    // connect MainLeft on even and MainRight on odd channels as PostFader
    for (uint8_t i = 0; i < 40; i++)
    {
        // connect MainLeft on even and MainRight on odd channels as PostFader
        config->Set(ROUTING_DSP_OUTPUT, DSP_BUF_IDX_MAINLEFT + (i % 2), i);
        config->Set(ROUTING_DSP_OUTPUT_TAPPOINT, to_underlying(DSP_TAP::POST_FADER), i);
    }
	// connect mixbus-channels 1-8 to DSP2-FX-Channels 1-8 as PostFader
    for (uint8_t i = 0; i < 8; i++)
    {
        config->Set(ROUTING_DSP_OUTPUT, DSP_BUF_IDX_MIXBUS + i, 40 + i);
        config->Set(ROUTING_DSP_OUTPUT_TAPPOINT, to_underlying(DSP_TAP::POST_FADER), 40 + i);
    }

    // connect MainLeft to RTA
    uint indexRTA = (MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS) - 1;
    config->Set(ROUTING_DSP_OUTPUT, DSP_BUF_IDX_MAINLEFT, indexRTA) ;
    config->Set(ROUTING_DSP_OUTPUT_TAPPOINT, to_underlying(DSP_TAP::POST_FADER), indexRTA);
}

void Mixer::ClearSolo(void)
{
    if (IsSoloActivated())
    {
        for (int i=0; i<MAX_VCHANNELS; i++)
        {
            config->Set(MP_ID::CHANNEL_SOLO, 0.0f, i);
        }
    }
}

bool Mixer::IsSoloActivated(void)
{
    for (uint8_t i=0; i < MAX_VCHANNELS; i++)
    {
        if (config->GetBool(MP_ID::CHANNEL_SOLO, i))
        {
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

void Mixer::Sync(void)
{
    if (helper->DEBUG_MIXER(DEBUGLEVEL_NORMAL))
    {
        uint changedParameterCount = 0;
        for (auto const& [parameter_id, indexSet] : *(config->GetChangedParameterList()))
        {
            changedParameterCount += indexSet.size();
        }
        helper->DEBUG_MIXER(DEBUGLEVEL_NORMAL, "syncing %d changed Mixerparameters to hardware", changedParameterCount);
    }

    vector<MP_ID> filter;

    // Mute Groups - check if mute group has changed
    for (uint muteGroupIdx = 0; muteGroupIdx < MUTE_GROUPS; muteGroupIdx++)
    {
        filter.push_back(config->MpCalcId(MUTE_GROUP_1_MUTE, muteGroupIdx));
    }
    if (config->HasParametersChanged(filter))
    {
        // loop through all channels
        for (uint chanIndex = 0; chanIndex < MAX_VCHANNELS; chanIndex++)
        {
            // loop through all mute groups
            bool muteChannel = false;
            for (uint i = 0; i < MUTE_GROUPS; i++)
            {
                MP_ID muteGroupSwitchId = config->MpCalcId(MUTE_GROUP_1_MUTE, i);
                MP_ID muteGroupId = config->MpCalcId(MUTE_GROUP_1, i);
        
                // if we are part of the mute group
                if (config->GetBool(muteGroupId, chanIndex))
                {
                    // add to the desired channel mute state according to mute group switch
                    // -> if at least one group is muted, mute the channel
                    muteChannel |= config->GetBool(muteGroupSwitchId);
                }
            }

            config->Set(CHANNEL_MUTE, muteChannel, chanIndex);
        }
    }

    // DCA Groups - check if DCA group has changed (channel added or removed to DCA group)
    filter.clear();
    for (uint dcaGroupIdx = 0; dcaGroupIdx < DCA_GROUPS; dcaGroupIdx++)
    {
        filter.push_back(config->MpCalcId(DCA_GROUP_1, dcaGroupIdx));
    }
    if (config->HasParametersChanged(filter))
    {
        // loop through all channels
        for (uint chanIndex = 0; chanIndex < MAX_VCHANNELS; chanIndex++)
        {
            bool dcaBindOnChannel = false;

            // loop through all DCA groups
            for (uint i = 0; i < DCA_GROUPS; i++)
            {
                MP_ID dcaGroupId = config->MpCalcId(DCA_GROUP_1, i);
        
                // check if we are part of the DCA group
                if (config->GetBool(dcaGroupId, chanIndex))
                {
                    dcaBindOnChannel = true;
                }
            }

            if (dcaBindOnChannel)
            {
                // this channel is part of at least one DCA group -> resend channel-volume for this channel to DSP to update DCA group values
                if (helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::NORMAL) ||
                helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::AUX) ||
                helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::FXRET) ||
                helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::BUS) )
                {
                    dsp->SendChannelVolume(chanIndex);
                }
                else if (helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::MATRIX))
                {
                    dsp->SendMatrixVolume(chanIndex);
                }
                else if (helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::MAINSUB))
                {
                    dsp->SendMainVolume(chanIndex);
                }
                else if (helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::MAIN))
                {
                    dsp->SendMainVolume(chanIndex);
                }
            }
        }
    }

    filter = {CHANNEL_VOLUME, CHANNEL_VOLUME_SUB, CHANNEL_MUTE, CHANNEL_PANORAMA, CHANNEL_SEND_LR, CHANNEL_SEND_SUB};
    if (config->HasParametersChanged(filter))
    {
        vector<uint> changedIndexes = config->GetChangedParameterIndexes(filter);
        for (auto const& changedIndex : changedIndexes)
        {
            if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::NORMAL) ||
                helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::AUX) ||
                helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::FXRET) ||
                helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::BUS) )
            {
                dsp->SendChannelVolume(changedIndex);
            }
            else if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::MATRIX))
            {
                dsp->SendMatrixVolume(changedIndex);
            }
            else if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::DCA))
            {
                // DCA channel has changed -> resend volume for all channels that are part of this DCA group to update DCA group values
                for (uint chanIndex = 0; chanIndex < MAX_VCHANNELS; chanIndex++)
                {
                    MP_ID dcaGroupId = config->MpCalcId(DCA_GROUP_1, changedIndex - (uint)X32_VCHANNEL_BLOCK::DCA); 
                    if (config->GetBool(dcaGroupId, chanIndex))
                    {
                        if (helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::NORMAL) ||
                            helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::AUX) ||
                            helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::FXRET) ||
                            helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::BUS) )
                        {
                            dsp->SendChannelVolume(chanIndex);
                        }
                        else if (helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::MATRIX))
                        {
                            dsp->SendMatrixVolume(chanIndex);
                        }
                        else if (helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::MAINSUB))
                        {
                            dsp->SendMainVolume(chanIndex);
                        }
                        else if (helper->IsInChannelBlock(chanIndex, X32_VCHANNEL_BLOCK::MAIN))
                        {
                            dsp->SendMainVolume(chanIndex);
                        }  
                    }
                }
            }
            else if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::MAINSUB))
            {
                dsp->SendMainVolume(changedIndex);
            }
            else if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::MAIN))
            {
                dsp->SendMainVolume(changedIndex);
            }  
        }
    }

    filter = {CHANNEL_SOLO};
    if (config->HasParametersChanged(filter))
    {
        vector<uint> changedIndexes = config->GetChangedParameterIndexes(filter);
        for (auto const& changedIndex : changedIndexes)
        {
            if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::NORMAL) ||
                helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::AUX) ||
                helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::FXRET) ||
                helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::BUS) )
            {
                dsp->SendChannelSolo(changedIndex, IsSoloActivated());
            }
            else if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::MATRIX))
            {
                dsp->SendMatrixSolo(changedIndex, IsSoloActivated());
            }
            else if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::MAINSUB)) {
                dsp->SendMainSolo(IsSoloActivated());
            }
            else if (helper->IsInChannelBlock(changedIndex, X32_VCHANNEL_BLOCK::MAIN)) {
                dsp->SendMainSolo(IsSoloActivated());
            }  
        }
    }

    if (config->HasParametersChanged(MP_CAT::CHANNEL_SENDS))
    { 
        vector<uint> changedIndexes = config->GetChangedParameterIndexes(MP_CAT::CHANNEL_SENDS);
        for (auto const& changedIndex : changedIndexes)
        {
            dsp->SendChannelSend(changedIndex);
            dsp->ChannelSendTapPoints(changedIndex);
        }
    }

    if (config->HasParameterChanged(ROUTING_FPGA))
    { 
        vector<uint> changedIndexes = config->GetChangedParameterIndexes({ROUTING_FPGA});
        for (auto const& changedIndex : changedIndexes)
        {
            fpga->SendRoutingToFpga(changedIndex);
        }
    }

    if (config->HasParametersChanged({DELAY_DSP_OUTPUT}))
    { 
        vector<uint> changedIndexes = config->GetChangedParameterIndexes({DELAY_DSP_OUTPUT});
        for (auto const& changedIndex : changedIndexes)
        {
            dsp->SetOutputDelay(changedIndex);
        }
    }

    if (config->HasParametersChanged({DELAY_DSP_INPUT}))
    { 
        vector<uint> changedIndexes = config->GetChangedParameterIndexes({DELAY_DSP_INPUT});
        for (auto const& changedIndex : changedIndexes)
        {
            dsp->SetInputDelay(changedIndex);
        }
    }

    if (config->HasParametersChanged({ROUTING_DSP_OUTPUT, ROUTING_DSP_OUTPUT_TAPPOINT}))
    { 
        vector<uint> changedIndexes = config->GetChangedParameterIndexes({ROUTING_DSP_OUTPUT, ROUTING_DSP_OUTPUT_TAPPOINT});
        for (auto const& changedIndex : changedIndexes)
        {
            dsp->SetOutputRouting(changedIndex);
        }
    }

    if (config->HasParametersChanged({ROUTING_DSP_INPUT, ROUTING_DSP_INPUT_TAPPOINT}))
    { 
        vector<uint> changedIndexes = config->GetChangedParameterIndexes({ROUTING_DSP_INPUT, ROUTING_DSP_INPUT_TAPPOINT});
        for (auto const& changedIndex : changedIndexes)
        {
            dsp->SetInputRouting(changedIndex);
        }
    }

    if (config->HasParametersChanged(MP_CAT::CHANNEL_SENDS))
    { 
        vector<uint> changedIndexes = config->GetChangedParameterIndexes(MP_CAT::CHANNEL_SENDS);
        for (auto const& changedIndex : changedIndexes)
        {
            dsp->SendChannelSend(changedIndex);
        }
    }

    if (config->HasParametersChanged(MP_CAT::CHANNEL_GATE))
    {        
        vector<uint> changedIndexes = config->GetChangedParameterIndexes(MP_CAT::CHANNEL_GATE);
        for (auto const& changedIndex : changedIndexes)
        {
            dsp->SendGate(changedIndex);
        }
    }

    if (config->HasParameterChanged(CHANNEL_PHANTOM))
    {
        vector<uint> changedIndexes = config->GetChangedParameterIndexes({CHANNEL_PHANTOM});
        for (auto const& changedIndex : changedIndexes)
        {
            halSendPhantomPower(changedIndex);
        }
    }

    if (config->HasParameterChanged(CHANNEL_GAIN))
    {
        vector<uint> changedIndexes = config->GetChangedParameterIndexes({CHANNEL_GAIN});
        for (auto const& changedIndex : changedIndexes)
        {
            halSendGain(changedIndex);
        }
    }

    if (config->HasParametersChanged(MP_CAT::CHANNEL_EQ))
    {
        vector<uint> changedIndexes = config->GetChangedParameterIndexes(MP_CAT::CHANNEL_EQ);
        for (auto const& changedIndex : changedIndexes)
        {
            // dsp->Channel holds only 40 Elements
            if (changedIndex > 39)
            {
                continue;
            }

            // TODO: Implement EQ without old Channel struct!
            // copy values fromm Mixerparameter
            for (uint8_t peq = 0; peq < MAX_CHAN_EQS; peq++)
            {
                dsp->Channel[changedIndex].peq[peq].type = config->GetUint(config->MpCalcId(CHANNEL_EQ_TYPE1, peq), changedIndex);
                dsp->Channel[changedIndex].peq[peq].fc = config->GetFloat(config->MpCalcId(CHANNEL_EQ_FREQ1, peq), changedIndex);
                dsp->Channel[changedIndex].peq[peq].Q = config->GetFloat(config->MpCalcId(CHANNEL_EQ_Q1, peq), changedIndex);
                dsp->Channel[changedIndex].peq[peq].gain = config->GetFloat(config->MpCalcId(CHANNEL_EQ_GAIN1, peq), changedIndex);
            }
    
            dsp->SendEQ(changedIndex);
            dsp->SendLowcut(changedIndex);
        }
    }

    if (config->HasParametersChanged(MP_CAT::CHANNEL_DYNAMICS))
    {
        vector<uint> changedIndexes = config->GetChangedParameterIndexes(MP_CAT::CHANNEL_DYNAMICS);
        for (auto const& changedIndex : changedIndexes)
        {
            dsp->SendCompressor(changedIndex);
        }
    }

    if (config->HasParametersChanged(MP_CAT::FX))
    {
        vector<uint> changedIndexes = config->GetChangedParameterIndexes(MP_CAT::FX);
        for (auto const& fxSlot : changedIndexes)
        {
            dsp->DSP2_SendFxParameter(fxSlot);
        }
    }

    helper->DEBUG_MIXER(DEBUGLEVEL_NORMAL, "sync done");
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
            if (boardId < 4) {
                uint8_t addaChannel = externalDspSourceIndex;
                while (addaChannel > 8) {
                    addaChannel -= 8;
                }
                adda->SetGain(boardId, addaChannel, config->GetFloat(CHANNEL_GAIN,  dspChannel), config->GetFloat(CHANNEL_PHANTOM,  dspChannel));

                // update channel-volume for virtual gain-resolution-increment
                // as hardware is switched in 2.5dB steps, we are using channel-volume to increase the resolution
                dsp->SendChannelVolume(dspChannel);
            }
        }else if ((externalDspSourceIndex >= FPGA_OUTPUT_IDX_AES50A) && (externalDspSourceIndex < (FPGA_OUTPUT_IDX_AES50A + 48))) {
            // AES50A input (never thought that we get this far in the reverse-engineering process, but here we are... :) )
            fpga->AES50SetHeadampGain(0, externalDspSourceIndex - FPGA_OUTPUT_IDX_AES50A + 1, config->GetFloat(CHANNEL_GAIN,  dspChannel));

            // update channel-volume for virtual gain-resolution-increment
            // as hardware is switched in 2.5dB steps, we are using channel-volume to increase the resolution
            dsp->SendChannelVolume(dspChannel);

        }else if ((externalDspSourceIndex >= FPGA_OUTPUT_IDX_AES50B) && (externalDspSourceIndex < (FPGA_OUTPUT_IDX_AES50B + 48))) {
            // AES50B input (we need more optimizations in the FPGA to get the second AES50-port working, so this is not implemented yet)
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

            if (boardId < 4) {
                uint8_t addaChannel = externalDspSourceIndex;
                while (addaChannel > 8) {
                    addaChannel -= 8;
                }
                adda->SetGain(boardId, addaChannel, config->GetFloat(CHANNEL_GAIN,  dspChannel), config->GetFloat(CHANNEL_PHANTOM,  dspChannel));
            }
        }else if ((externalDspSourceIndex >= FPGA_OUTPUT_IDX_AES50A) && (externalDspSourceIndex < (FPGA_OUTPUT_IDX_AES50A + 48))) {
            // AES50A input
            fpga->AES50SetPhantomPowerState(0, externalDspSourceIndex - FPGA_OUTPUT_IDX_AES50A + 1, config->GetFloat(CHANNEL_PHANTOM,  dspChannel));

        }else if ((externalDspSourceIndex >= FPGA_OUTPUT_IDX_AES50B) && (externalDspSourceIndex < (FPGA_OUTPUT_IDX_AES50B + 48))) {
            // AES50B input (we need more optimizations in the FPGA to get the second AES50-port working, so this is not implemented yet)
        }
    }
}

