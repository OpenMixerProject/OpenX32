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

#include "dsp1.h"

DSP1::DSP1(X32BaseParameter* basepar) : X32Base(basepar) {
    spi = new SPI(basepar);
    spi->UploadBitstreamDsps(true); // use CLI to show progress
    spi->OpenConnectionDsps();

    fx = new FX(basepar);
};

void DSP1::Init(void) {
    MainChannelLR.volume = VOLUME_MIN; // dB
    MainChannelLR.balance = 0; // -100 .. 0 .. +100
    MainChannelSub.volume = VOLUME_MIN; // dB
    MainChannelSub.balance = 0; // -100 .. 0 .. +100

    for (uint8_t i = 0; i < 40; i++) {

        Channel[i].muted = false;
        Channel[i].solo = false;

        Channel[i].volumeLR = 0; // dbFS
        Channel[i].volumeSub = VOLUME_MIN;
        Channel[i].balance = 0; // Center


        Channel[i].lowCutFrequency = 100.0f; // Hz

        Channel[i].gate.threshold = -80; // dB -> no gate
        Channel[i].gate.range = 60; // dB -> full range
        Channel[i].gate.attackTime_ms = 10;
        Channel[i].gate.holdTime_ms = 50;
        Channel[i].gate.releaseTime_ms = 250;

        Channel[i].compressor.threshold = 0; // dB -> no compression
        Channel[i].compressor.ratio = 3.0f; // 1:3
        Channel[i].compressor.makeup = 0; // dB -> no makeup
        Channel[i].compressor.attackTime_ms = 10;
        Channel[i].compressor.holdTime_ms = 10;
        Channel[i].compressor.releaseTime_ms = 150;

        for (uint8_t peq = 0; peq < MAX_CHAN_EQS; peq++) {
            Channel[i].peq[peq].type = 1; // PEQ
            Channel[i].peq[peq].fc = 3000; // Hz
            Channel[i].peq[peq].Q = 2.0;
            Channel[i].peq[peq].gain = 0; // dB
        }

        for (uint8_t i_mixbus = 0; i_mixbus < 16; i_mixbus++) {
            Channel[i].sendMixbus[i_mixbus] = VOLUME_MIN;
            Channel[i].sendMixbusTapPoint[i_mixbus] = DSP_TAP_PRE_FADER;
        }

        monitorVolume = 0; // dBfs
        monitorTapPoint = DSP_TAP_INPUT;

    }

    LoadRouting_X32Default();
}

void DSP1::LoadRouting_X32Default()
{
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
        Channel[i].input = DSP_BUF_IDX_DSPCHANNEL + i; // 0=OFF, 1..32=DSP-Channel, 33..40=Aux, 41..56=Mixbus, 57..62=Matrix, 63=MainL, 64=MainR, 65=MainSub, 66..68=MonL,MonR,Talkback
        Channel[i].inputTapPoint = DSP_TAP_INPUT;

        // Volumes, Balance and Mute/Solo is setup in mixerInit()
    }

    for (uint8_t i = 0; i < 16; i++)
    {
        Dsp1toDsp2Routing[i].input = DSP_BUF_IDX_MIXBUS; // connect all 16 mixbus-channels to DSP2 Channels 1-16
        Dsp1toDsp2Routing[i].tapPoint = DSP_TAP_POST_FADER;
    }
    for (uint8_t i = 16; i < 24; i++)
    {
        Dsp1toDsp2Routing[i].input = DSP_BUF_IDX_DSPCHANNEL; // connect inputs 1-8 to DSP2 Channels 17-24
        Dsp1toDsp2Routing[i].tapPoint = DSP_TAP_POST_FADER;
    }
    for (uint8_t i = 0; i < 40; i++)
    {
        // connect MainLeft on even and MainRight on odd channels as PostFader
        Dsp1toFpga[i].input = DSP_BUF_IDX_MAINLEFT + (i % 2); // 0=OFF, 1..32=DSP-Channel, 33..40=Aux, 41..56=Mixbus, 57..62=Matrix, 63=MainL, 64=MainR, 65=MainSub, 66..68=MonL,MonR,Talkback, 69..84=FX-Return, 85..92=DSP2AUX
        Dsp1toFpga[i].tapPoint = DSP_TAP_POST_FADER;
    }
}

void DSP1::ReadAndUpdateVUMeterData(void) {
    spi->ReadData();

    // process SPI-Data
    while (spi->HasNextEvent()) {
        SpiEvent* spiEvent = spi->GetNextEvent();

        if (spiEvent->dsp == 0) {
            callbackDsp1(spiEvent->classId, spiEvent->channel, spiEvent->index, spiEvent->valueCount, spiEvent->values);
        } else {
            callbackDsp2(spiEvent->classId, spiEvent->channel, spiEvent->index, spiEvent->valueCount, spiEvent->values);
        }
    }

    // recalculate peak-hold and VU with decay
    UpdateVuMeter();
}


// set the general volume of one of the 40 DSP-channels
void DSP1::SendChannelVolume(uint8_t chan) {
    // set value to interal struct
    float balanceLeft = helper->Saturate(100.0f - Channel[chan].balance, 0.0f, 100.0f) / 100.0f;
    float balanceRight = helper->Saturate(Channel[chan].balance + 100.0f, 0.0f, 100.0f) / 100.0f;
    float volumeLR = Channel[chan].volumeLR;
    float volumeSub = Channel[chan].volumeSub;

    if (Channel[chan].muted) {
        volumeLR = -100; // dB
        volumeSub = -100; // dB
    }

    // send volume to DSP via SPI
    float values[4];
    values[0] = pow(10.0f, volumeLR/20.0f); // volume of this specific channel
    values[1] = balanceLeft; // 1 .. 1 ..  0
    values[2] = balanceRight; // 0  .. 1 .. 1
    values[3] = pow(10.0f, volumeSub/20.0f); // subwoofer

    helper->DEBUG_DSP1(DEBUGLEVEL_TRACE, "SendChannelVolume: %f, %f, %f, %f", (double)values[0], (double)values[1], (double)values[2], (double)values[3]);

    spi->SendReceiveDspParameterArray(0, 'v', chan, 0, 4, &values[0]);
}

// send BusSends
void DSP1::SendChannelSend(uint8_t chan) {
    float values[16];

    for (uint8_t i_mixbus = 0; i_mixbus < 16; i_mixbus++) {
        values[i_mixbus] = pow(10.0f, Channel[chan].sendMixbus[i_mixbus]/20.0f); // volume of this specific channel
    }

    spi->SendReceiveDspParameterArray(0, 's', chan, 0, 16, &values[0]);
}

void DSP1::SendMixbusVolume(uint8_t bus) {
    float balanceLeft = helper->Saturate(100.0f - Bus[bus].balance, 0.0f, 100.0f) / 100.0f;
    float balanceRight = helper->Saturate(Bus[bus].balance + 100.0f, 0.0f, 100.0f) / 100.0f;

    // send volume to DSP via SPI
    float values[4];
    values[0] = pow(10.0f, Bus[bus].volumeLR/20.0f); // volume of this specific channel
    values[1] = balanceLeft; // 100 .. 100 ..  0
    values[2] = balanceRight; // 0  .. 100 .. 100
    values[3] = pow(10.0f, Bus[bus].volumeSub/20.0f); // subwoofer

    spi->SendReceiveDspParameterArray(0, 'v', bus, 1, 4, &values[0]);
}

void DSP1::SendMatrixVolume(uint8_t matrix) {
    // send volume to DSP via spi->
    float values[1];

    values[0] = pow(10.0f, Matrix[matrix].volume/20.0f); // volume of this specific channel

    spi->SendReceiveDspParameterArray(0, 'v', matrix, 2, 1, &values[0]);
}

void DSP1::SendMonitorVolume() {
    // send volume to DSP via spi
    float values[1];
    
    values[0] = pow(10.0f, monitorVolume/20.0f); // volume of this specific channel

    spi->SendReceiveDspParameterArray(0, 'v', 0, 4, 1, &values[0]);
}

void DSP1::SendMainVolume() {
    float volumeLeft = (helper->Saturate(100.0f - MainChannelLR.balance, 0.0f, 100.0f) / 100.0f) * pow(10.0f, MainChannelLR.volume/20.0f);
    float volumeRight = (helper->Saturate(MainChannelLR.balance + 100.0f, 0.0f, 100.0f) / 100.0f) * pow(10.0f, MainChannelLR.volume/20.0f);
    float volumeSub = pow(10.0f, MainChannelSub.volume/20.0f);

    if (MainChannelLR.muted) {
        volumeLeft = 0; // p.u.
        volumeRight = 0; // p.u.
    }
    if (MainChannelSub.muted) {
        volumeSub = 0; // p.u.
    }

    // send volume to DSP via spi->
    float values[3];
    values[0] = volumeLeft;
    values[1] = volumeRight;
    values[2] = volumeSub;

    spi->SendReceiveDspParameterArray(0, 'v', 0, 3, 3, &values[0]);
}

void DSP1::SendGate(uint8_t chan) {
    fx->RecalcGate(&Channel[chan].gate);

    float values[5];
    values[0] = Channel[chan].gate.value_threshold;
    values[1] = Channel[chan].gate.value_gainmin;
    values[2] = Channel[chan].gate.value_coeff_attack;
    values[3] = Channel[chan].gate.value_hold_ticks;
    values[4] = Channel[chan].gate.value_coeff_release;

    spi->SendReceiveDspParameterArray(0, 'g', chan, 0, 5, &values[0]);
}

void DSP1::SendLowcut(uint8_t chan) {
    float values[1];

    // Source: https://www.dsprelated.com/showarticle/1769.php
    // alpha = 1 / (1 + 2 * pi * f_c * 1/f_s)
    // Equation for samples: output = alpha * (input + previous_output - previous_input)
    values[0] = 1.0f / (1.0f + 2.0f * M_PI * Channel[chan].lowCutFrequency * (1.0f/(float)config->GetSamplerate()));

    spi->SendReceiveDspParameterArray(0, 'e', chan, 'l', 1, &values[0]);
}

/*
void DSP1::SendHighcut(uint8_t chan) {
    float values[1];

    // alpha = (2 * pi * f_c) / (f_s + 2 * pi * f_c)
    // Equation for samples: output = previous_output + coeff * (input - previous_output)
    values[0] = (2.0f * M_PI * Channel[chan].highCutFrequency) / ((float)config->GetSamplerate() + 2.0f * M_PI * 500.0f);

    spi->SendReceiveDspParameterArray(0, 'e', chan, 'h', 1, &values[0]);
}
*/

void DSP1::SendEQ(uint8_t chan) {
    // biquad_trans() needs the coeffs in the following order
    // a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 0/1)
    // a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 2/3)
    // a0 a1 a2 b1 b2 (section 4)

    float values[MAX_CHAN_EQS * 5];

    for (int peq = 0; peq < MAX_CHAN_EQS; peq++) {
        fx->RecalcFilterCoefficients_PEQ(&Channel[chan].peq[peq]);

/*
        // send coeffiecients without interleaving for biquad() function
        int sectionIndex = peq * 5;
        values[sectionIndex + 0] = -dspChannel[chan].peq[peq].b[2]; // -b2 (poles)
        values[sectionIndex + 1] = -dspChannel[chan].peq[peq].b[1]; // -b1 (poles)
        values[sectionIndex + 2] = dspChannel[chan].peq[peq].a[2]; // a2 (zeros)
        values[sectionIndex + 3] = dspChannel[chan].peq[peq].a[1]; // a1 (zeros)
        values[sectionIndex + 4] = dspChannel[chan].peq[peq].a[0]; // a0 (zeros)
*/

        // interleave coefficients for biquad_trans()
        if (((MAX_CHAN_EQS % 2) == 0) || (peq < (MAX_CHAN_EQS - 1))) {
            // we have even number of PEQ-sections
            // or we have odd number but we are still below the last section
            // store data with interleaving
            int sectionIndex = ((peq / 2) * 2) * 5;
            if ((peq % 2) != 0) {
                // odd section index
                sectionIndex += 1;
            }
            values[sectionIndex + 0] = Channel[chan].peq[peq].a[0]; // a0 (zeros)
            values[sectionIndex + 2] = Channel[chan].peq[peq].a[1]; // a1 (zeros)
            values[sectionIndex + 4] = Channel[chan].peq[peq].a[2]; // a2 (zeros)
            values[sectionIndex + 6] = -Channel[chan].peq[peq].b[1]; // -b1 (poles)
            values[sectionIndex + 8] = -Channel[chan].peq[peq].b[2]; // -b2 (poles)
        }else{
            // last section: store without interleaving
            int sectionIndex = (MAX_CHAN_EQS - 1) * 5;
            values[sectionIndex + 0] = Channel[chan].peq[peq].a[0]; // a0 (zeros)
            values[sectionIndex + 1] = Channel[chan].peq[peq].a[1]; // a1 (zeros)
            values[sectionIndex + 2] = Channel[chan].peq[peq].a[2]; // a2 (zeros)
            values[sectionIndex + 3] = -Channel[chan].peq[peq].b[1]; // -b1 (poles)
            values[sectionIndex + 4] = -Channel[chan].peq[peq].b[2]; // -b2 (poles)
        }
    }

    spi->SendReceiveDspParameterArray(0, 'e', chan, 'e', MAX_CHAN_EQS * 5, &values[0]);
}

void DSP1::ResetEq(uint8_t chan) {
    float values[1];
    values[0] = 0;
    spi->SendReceiveDspParameterArray(0, 'e', chan, 'r', 1, &values[0]);
}

void DSP1::SendCompressor(uint8_t chan) {
    fx->RecalcCompressor(&Channel[chan].compressor);

    float values[6];
    values[0] = Channel[chan].compressor.value_threshold;
    values[1] = Channel[chan].compressor.value_ratio;
    values[2] = Channel[chan].compressor.value_makeup;
    values[3] = Channel[chan].compressor.value_coeff_attack;
    values[4] = Channel[chan].compressor.value_hold_ticks;
    values[5] = Channel[chan].compressor.value_coeff_release;

    spi->SendReceiveDspParameterArray(0, 'c', chan, 0, 6, &values[0]);
}

void DSP1::SendAll() {
    // configuration for 40 DSP-main-channel-inputs
    for (uint8_t chan = 0; chan <= 39; chan++) {
        SendLowcut(chan);
        SendGate(chan);
        SendEQ(chan);
        SendCompressor(chan);
        SetInputRouting(chan);
        SendChannelVolume(chan);
        SendChannelSend(chan);
        for (uint8_t mixbusChannel = 0; mixbusChannel <= 15; mixbusChannel++) {
            SetChannelSendTapPoints(chan, mixbusChannel, Channel[chan].sendMixbusTapPoint[mixbusChannel]);
        }
    }
    // configuration for 40 DSP-outputs to FPGA (16x output, 16x UltraNet, 8x Aux)
    for (uint8_t chan = 0; chan <= 39; chan++) {
        SetOutputRouting(chan);
    }
    // configuration for 24 channels sends to DSP2 (16x FX, 8x AUX)
    for (uint8_t fxchan = 0; fxchan <= 23; fxchan++) {
        SetFxOutputRouting(fxchan);
    }

    for (uint8_t mixbusChannel = 0; mixbusChannel <= 15; mixbusChannel++) {
        SendMixbusVolume(mixbusChannel);
        for (uint8_t matrixChannel = 0; matrixChannel <= 5; matrixChannel++) {
            SetMixbusSendTapPoints(mixbusChannel, matrixChannel, this->Bus[mixbusChannel].sendMatrixTapPoint[matrixChannel]);
        }
    }
    for (uint8_t matrixChannel = 0; matrixChannel <= 5; matrixChannel++) {
        SendMatrixVolume(matrixChannel);
        SetMainSendTapPoints(matrixChannel, MainChannelLR.sendMatrixTapPoint[matrixChannel]);
    }
    SendMainVolume();
    SendMonitorVolume();
}

void DSP1::SetInputRouting(uint8_t chan) {
    uint32_t values[2];
    values[0] = Channel[chan].input;
    values[1] = Channel[chan].inputTapPoint;
    spi->SendReceiveDspParameterArray(0, 'r', chan, 0, 2, (float*)&values[0]);
}

void DSP1::SetOutputRouting(uint8_t chan) {
    uint32_t values[2];
    values[0] = Dsp1toFpga[chan].input;
    values[1] = Dsp1toFpga[chan].tapPoint;
    spi->SendReceiveDspParameterArray(0, 'r', chan, 1, 2, (float*)&values[0]);
}

void DSP1::SetFxOutputRouting(uint8_t fxchan) {
    uint32_t values[2];
    values[0] = Dsp1toDsp2Routing[fxchan].input;
    values[1] = Dsp1toDsp2Routing[fxchan].tapPoint;
    spi->SendReceiveDspParameterArray(0, 'r', (MAX_DSP1_TO_FPGA_CHANNELS + fxchan), 1, 2, (float*)&values[0]);
}

void DSP1::SetChannelSendTapPoints(uint8_t chan, uint8_t mixbusChannel, uint8_t tapPoint) {
    Channel[chan].sendMixbusTapPoint[mixbusChannel] = tapPoint;

    uint32_t values[2];
    values[0] = mixbusChannel;
    values[1] = tapPoint;
    spi->SendReceiveDspParameterArray(0, 't', chan, 0, 2, (float*)&values[0]);
}

void DSP1::SetMixbusSendTapPoints(uint8_t mixbusChannel, uint8_t matrixChannel, uint8_t tapPoint) {
    this->Bus[mixbusChannel].sendMatrixTapPoint[matrixChannel] = tapPoint;

    uint32_t values[2];
    values[0] = matrixChannel;
    values[1] = tapPoint;
    spi->SendReceiveDspParameterArray(0, 't', mixbusChannel, 1, 2, (float*)&values[0]);
}

void DSP1::SetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint) {
    MainChannelLR.sendMatrixTapPoint[matrixChannel] = tapPoint;

    uint32_t values[2];
    values[0] = matrixChannel;
    values[1] = tapPoint;
    spi->SendReceiveDspParameterArray(0, 't', 0, 2, 2, (float*)&values[0]);
}

void DSP1::GetSourceName(char* p_nameBuffer, uint8_t dspChannel, uint8_t dspInputSource) {
    if (dspChannel < 40) {
        // we have a DSP-channel
        uint8_t channelInputSource = Channel[dspChannel].input;

        RoutingGetTapNameByIndex(p_nameBuffer, channelInputSource, dspInputSource);
    }else{
        // we have a non-DSP-channel -> no source available
        sprintf(p_nameBuffer, "<Intern>");
    }
}

void DSP1::RoutingGetInputNameByIndex(char* p_nameBuffer, uint8_t index) {
/*
    // DSP-input-channels:
    // 0-31		Full-Featured DSP-Channels
    // 32-39	Aux-Channel
*/
    if ((index >= DSP_BUF_IDX_DSPCHANNEL) && (index < (DSP_BUF_IDX_DSPCHANNEL + 32))) {
        sprintf(p_nameBuffer, "DSP-Ch %02d", index);
    }else if ((index >= DSP_BUF_IDX_AUX) && (index < (DSP_BUF_IDX_AUX + 8))) {
        sprintf(p_nameBuffer, "DSP-Aux %02d", index - 32);
    }else{
        sprintf(p_nameBuffer, "???");
    }
}

void DSP1::RoutingGetOutputNameByIndex(char* p_nameBuffer, uint8_t index) {
/*
    // DSP-output-channels:
    // 0-31		Main-Output to FPGA
    // 32-39	Aux-Output to FPGA
    // 40-56	FX-Sends 1-16 to DSP2
    // 57-64	FX-Aux to DSP2
*/
    if ((index >= DSP_BUF_IDX_DSPCHANNEL) && (index < (DSP_BUF_IDX_DSPCHANNEL + 32))) {
        sprintf(p_nameBuffer, "DSP-Out %02d", index);
    }else if ((index >= DSP_BUF_IDX_AUX) && (index < (DSP_BUF_IDX_AUX + 8))) {
        sprintf(p_nameBuffer, "DSP-AuxOut %02d", index - 32);
    }else if ((index >= DSP_BUF_IDX_DSP2_FX) && (index < (DSP_BUF_IDX_DSP2_FX + 16))) {
        sprintf(p_nameBuffer, "FX-SendOut %02d", index - 40);
    }else if ((index >= (DSP_BUF_IDX_DSP2_FX + 16)) && (index < (DSP_BUF_IDX_DSP2_FX + 24))) {
        sprintf(p_nameBuffer, "FX-AuxOut %02d", index - 56);
    }else{
        sprintf(p_nameBuffer, "???");
    }
}

void DSP1::RoutingGetTapNameByIndex(char* p_nameBuffer, uint8_t index, uint8_t source) {
/*
    // dspSource-Signals
    // 0		DSP_BUF_IDX_OFF
    // 1-33		DSP-Input 1-32 from FPGA
    // 33-40	AUX-Input 1-8 from FPGA
    // 41-56	FX-Return 1-8 from DSP2
    // 57-72	Mixbus 1-16 (internal)
    // 73-75	Main Left, Right, Sub (internal)
    // 76-81	Matrix 1-6 (internal)
    // 82-89	FX-Aux-Channel 1-8 from DSP2
    // 90-92	Monitor Left, Right, Talkback (internal)
*/
    // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
    if (index == 0) {
        // OFF
        sprintf(p_nameBuffer, "Off");
    }else if ((index >= DSP_BUF_IDX_DSPCHANNEL) && (index < (DSP_BUF_IDX_DSPCHANNEL + 40))) {
        // we are connected to one of the FPGA-DSP-inputs
        // source is set by FPGA
        //uint8_t source = mixer->fpga->Routing.dsp[index - 1];
        if (source == 0) {
            sprintf(p_nameBuffer, "DSP-Ch %02d (Off)", index);
        }else if ((source >= 1) && (source <= 32)) {
            // XLR-input
            sprintf(p_nameBuffer, "DSP-Ch %02d (XLR%02d)", index, source);
        }else if ((source >= 33) && (source <= 64)) {
            // Card input
            sprintf(p_nameBuffer, "DSP-Ch %02d (C%02d)", index, source - 32);
        }else if ((source >= 65) && (source <= 72)) {
            // Aux input
            sprintf(p_nameBuffer, "DSP-Ch %02d (Aux%02d)", index, source - 64);
        }else if ((source >= 73) && (source <= 112)) {
            // DSP input
            sprintf(p_nameBuffer, "DSP-Ch %02d (In%02d)", index, source - 72);
        }else if ((source >= 113) && (source <= 160)) {
            // AES50A input
            sprintf(p_nameBuffer, "DSP-Ch %02d (A%02d)", index, source - 112);
        }else if ((source >= 161) && (source <= 208)) {
            // AES50B input
            sprintf(p_nameBuffer, "DSP-Ch %02d (B%02d)", index, source - 160);
        }else{
            sprintf(p_nameBuffer, "???");
        }
    }else if ((index >= DSP_BUF_IDX_DSP2_FX) && (index < (DSP_BUF_IDX_DSP2_FX + 8))) {
        // FX Return 1-8 from DSP2
        sprintf(p_nameBuffer, "FX %02d", index - DSP_BUF_IDX_DSP2_FX + 1);
    }else if ((index >= (DSP_BUF_IDX_DSP2_FX + 8)) && (index < (DSP_BUF_IDX_DSP2_FX + 16))) {
        // FX Return 9-16 from DSP2
        sprintf(p_nameBuffer, "FX %02d", index - DSP_BUF_IDX_DSP2_FX + 1);
    }else if ((index >= DSP_BUF_IDX_MIXBUS) && (index < (DSP_BUF_IDX_MIXBUS + 16))) {
        // Mixbus 1-16
        sprintf(p_nameBuffer, "Bus %02d", index - DSP_BUF_IDX_MIXBUS + 1);
    }else if (index == DSP_BUF_IDX_MAINLEFT) {
        sprintf(p_nameBuffer, "Main L");
    }else if (index == DSP_BUF_IDX_MAINRIGHT) {
        sprintf(p_nameBuffer, "Main R");
    }else if (index == DSP_BUF_IDX_MAINSUB) {
        sprintf(p_nameBuffer, "Main C");
    }else if ((index >= DSP_BUF_IDX_MATRIX) && (index < (DSP_BUF_IDX_MATRIX + 6))) {
        // Matrix 1-6
        sprintf(p_nameBuffer, "Mtx %02d", index - DSP_BUF_IDX_MATRIX + 1);
    }else if ((index >= DSP_BUF_IDX_DSP2_AUX) && (index < (DSP_BUF_IDX_DSP2_AUX + 8))) {
        // DSP2 Aux Return 1-8
        sprintf(p_nameBuffer, "DSP2 Aux %02d", index - DSP_BUF_IDX_DSP2_AUX + 1);
    }else if (index == DSP_BUF_IDX_MONLEFT) {
        sprintf(p_nameBuffer, "Mon L");
    }else if (index == DSP_BUF_IDX_MONRIGHT) {
        sprintf(p_nameBuffer, "Mon R");
    }else if (index == DSP_BUF_IDX_TALKBACK) {
        sprintf(p_nameBuffer, "Talkback");
    }else{
        sprintf(p_nameBuffer, "???");
    }
}

void DSP1::RoutingGetTapPositionName(char* p_nameBuffer, uint8_t position) {
    switch(position) {
        case DSP_TAP_INPUT:
            sprintf(p_nameBuffer, "Input");
            break;
        case DSP_TAP_PRE_EQ:
            sprintf(p_nameBuffer, "Pre-EQ");
            break;
        case DSP_TAP_POST_EQ:
            sprintf(p_nameBuffer, "Post-EQ");
            break;
        case DSP_TAP_PRE_FADER:
            sprintf(p_nameBuffer, "Pre-Fader");
            break;
        case DSP_TAP_POST_FADER:
            sprintf(p_nameBuffer, "Post-Fader");
            break;
        default:
            sprintf(p_nameBuffer, "???");
            break;
    }
}

void DSP1::UpdateVuMeter() {

    uint8_t vuTreshLookupSize = 0;
    if (config->IsModelX32ProducerOrRack()) {
        vuTreshLookupSize = 18;
    } else if (config->IsModelX32FullOrCompact()) {
        vuTreshLookupSize = 24;
    }

	uint32_t data[3];
	data[0] = MainChannelLR.meter[0];
	data[1] = MainChannelLR.meter[1];
	data[2] = MainChannelSub.meter[0];

	// Step 1: Perform Peak Hold Logic for MainL/R and Sub
	uint8_t currentMeterPeakIndexMain[3]; // L, R, Sub
	for (int k = 0; k < 3; k++) {
		currentMeterPeakIndexMain[k] = 0;
		for (int i = 0; i < vuTreshLookupSize; i++) {
            if (config->IsModelX32ProducerOrRack()) {
                if (data[k] >= vuThresholds_minus45dbfs_18steps[i]) {
                    currentMeterPeakIndexMain[k] = (16 - i) + 1;
                    break;
			    }
            } else if (config->IsModelX32FullOrCompact()) {
                if (data[k] >= vuThresholds_minus60dbfs_25steps[i]) {
                    currentMeterPeakIndexMain[k] = (23 - i) + 1;
                    break;
                }
            }
		}
	}
	// MainLeft
	if (currentMeterPeakIndexMain[0] >= MainChannelLR.meterPeakIndex[0]) {
		MainChannelLR.meterPeakIndex[0] = currentMeterPeakIndexMain[0];
		MainChannelLR.meterPeakHoldTimer[0] = 100; // preload to 1000ms
	}else{
		// currentMeterPeakIndex is below current LED -> check if we have to hold the peak LED
		if (MainChannelLR.meterPeakHoldTimer[0] > 0) {
			// hold current LED
			MainChannelLR.meterPeakHoldTimer[0]--;
			MainChannelLR.meterPeakDecayTimer[0] = 10; // preload to 100ms
		}else{
			// let peak LED fall down every 100ms. It takes a maximum of 2400ms to let the peak fall down over all 24 LEDs
			if (MainChannelLR.meterPeakIndex[0] > currentMeterPeakIndexMain[0]) {
				if (MainChannelLR.meterPeakDecayTimer[0] > 0) {
					MainChannelLR.meterPeakDecayTimer[0]--;
				}else{
					MainChannelLR.meterPeakIndex[0]--;
					MainChannelLR.meterPeakDecayTimer[0] = 10; // preload for next iteration
				}
			}
		}
	}
	// MainRight
	if (currentMeterPeakIndexMain[1] >= MainChannelLR.meterPeakIndex[1]) {
		MainChannelLR.meterPeakIndex[1] = currentMeterPeakIndexMain[1];
		MainChannelLR.meterPeakHoldTimer[1] = 100; // preload to 1000ms
	}else{
		// currentMeterPeakIndex is below current LED -> check if we have to hold the peak LED
		if (MainChannelLR.meterPeakHoldTimer[1] > 0) {
			// hold current LED
			MainChannelLR.meterPeakHoldTimer[1]--;
			MainChannelLR.meterPeakDecayTimer[1] = 10; // preload to 100ms
		}else{
			// let peak LED fall down every 100ms. It takes a maximum of 400ms to let the peak fall down
			if (MainChannelLR.meterPeakIndex[1] > currentMeterPeakIndexMain[1]) {
				if (MainChannelLR.meterPeakDecayTimer[1] > 0) {
					MainChannelLR.meterPeakDecayTimer[1]--;
				}else{
					MainChannelLR.meterPeakIndex[1]--;
					MainChannelLR.meterPeakDecayTimer[1] = 10; // preload for next iteration
				}
			}
		}
	}
	// Sub
	if (currentMeterPeakIndexMain[2] >= MainChannelSub.meterPeakIndex[0]) {
		MainChannelSub.meterPeakIndex[0] = currentMeterPeakIndexMain[2];
		MainChannelSub.meterPeakHoldTimer[0] = 100; // preload to 1000ms
	}else{
		// currentMeterPeakIndex is below current LED -> check if we have to hold the peak LED
		if (MainChannelSub.meterPeakHoldTimer[0] > 0) {
			// hold current LED
			MainChannelSub.meterPeakHoldTimer[0]--;
			MainChannelSub.meterPeakDecayTimer[0] = 10; // preload to 100ms
		}else{
			// let peak LED fall down every 100ms. It takes a maximum of 400ms to let the peak fall down
			if (MainChannelSub.meterPeakIndex[0] > currentMeterPeakIndexMain[2]) {
				if (MainChannelSub.meterPeakDecayTimer[0] > 0) {
					MainChannelSub.meterPeakDecayTimer[0]--;
				}else{
					MainChannelSub.meterPeakIndex[0]--;
					MainChannelSub.meterPeakDecayTimer[0] = 10; // preload for next iteration
				}
			}
		}
	}

	// Step 2: Calculate decayed value
	// MainLeft
	if (MainChannelLR.meter[0] > MainChannelLR.meterDecay[0]) {
		// current value is above stored decay-value -> copy value immediatly
		MainChannelLR.meterDecay[0] = MainChannelLR.meter[0];
	}else{
		// current value is below -> afterglow
		MainChannelLR.meterDecay[0] -= (MainChannelLR.meterDecay[0] / 10);
	}
	// MainRight
	if (MainChannelLR.meter[1] > MainChannelLR.meterDecay[1]) {
		// current value is above stored decay-value -> copy value immediatly
		MainChannelLR.meterDecay[1] = MainChannelLR.meter[1];
	}else{
		// current value is below -> afterglow
		MainChannelLR.meterDecay[1] -= (MainChannelLR.meterDecay[1] / 10);
	}
	// MainSub
	if (MainChannelSub.meter[0] > MainChannelSub.meterDecay[0]) {
		// current value is above stored decay-value -> copy value immediatly
		MainChannelSub.meterDecay[0] = MainChannelSub.meter[0];
	}else{
		// current value is below -> afterglow
		MainChannelSub.meterDecay[0] -= (MainChannelSub.meterDecay[0] / 10);
	}

	// Step 3: Calculate real LEDs to switch on
	MainChannelLR.meterInfo[0] = 0;
	MainChannelLR.meterInfo[1] = 0;
	MainChannelSub.meterInfo[0] = 0;
	for (int i = 0; i < vuTreshLookupSize; i++) {
        if (config->IsModelX32ProducerOrRack()) {
            if (MainChannelLR.meterDecay[0] >= vuThresholds_minus45dbfs_18steps[i]) { MainChannelLR.meterInfo[0]  |= (1U << (16 - i)); }
            if (MainChannelLR.meterDecay[1] >= vuThresholds_minus45dbfs_18steps[i]) { MainChannelLR.meterInfo[1]  |= (1U << (16 - i)); }
            if (MainChannelSub.meterDecay[0] >= vuThresholds_minus45dbfs_18steps[i]) { MainChannelSub.meterInfo[0] |= (1U << (16 - i)); }
        } else if (config->IsModelX32FullOrCompact()) {
            if (MainChannelLR.meterDecay[0] >= vuThresholds_minus60dbfs_25steps[i]) { MainChannelLR.meterInfo[0]  |= (1U << (23 - i)); }
            if (MainChannelLR.meterDecay[1] >= vuThresholds_minus60dbfs_25steps[i]) { MainChannelLR.meterInfo[1]  |= (1U << (23 - i)); }
            if (MainChannelSub.meterDecay[0] >= vuThresholds_minus60dbfs_25steps[i]) { MainChannelSub.meterInfo[0] |= (1U << (23 - i)); }
        }
	}
	if (MainChannelLR.meterPeakIndex[0] > 0) {
		MainChannelLR.meterInfo[0]  |= (1U << (MainChannelLR.meterPeakIndex[0] - 1));
	}
	if (MainChannelLR.meterPeakIndex[1] > 0) {
		MainChannelLR.meterInfo[1]  |= (1U << (MainChannelLR.meterPeakIndex[1] - 1));
	}
	if (MainChannelSub.meterPeakIndex[0] > 0) {
		MainChannelSub.meterInfo[0]  |= (1U << (MainChannelSub.meterPeakIndex[0] - 1));
	}

	// Now calculate the VU Meter LEDs for each channel
	// leds Channel = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
	for (int i = 0; i < 40; i++) {
		// check if current data is above stored peak-index

        if(!(config->IsModelX32Core() || config->IsModelX32Rack())) {
            // Step 1: Perform Peak Hold Logic
            uint8_t currentMeterPeak6Index;
            currentMeterPeak6Index = GetPeak(i, 6);
            if (currentMeterPeak6Index >= rChannel[i].meterPeak6Index) {
                // currentMeterPeakIndex is above current LED -> set peakHold LED to highest value
                rChannel[i].meterPeak6Index = currentMeterPeak6Index;
                rChannel[i].meterPeak6HoldTimer = 100; // preload to 1000ms
            }else{
                // currentMeterPeakIndex is below current LED -> check if we have to hold the peak LED
                if (rChannel[i].meterPeak6HoldTimer > 0) {
                    // hold current LED
                    rChannel[i].meterPeak6HoldTimer--;
                    rChannel[i].meterPeak6DecayTimer = 10; // preload
                }else{
                    // let peak LED fall down every 100ms. It takes a maximum of 400ms to let the peak fall down
                    if (rChannel[i].meterPeak6Index > currentMeterPeak6Index) {
                        if (rChannel[i].meterPeak6DecayTimer > 0) {
                            rChannel[i].meterPeak6DecayTimer--;
                        }else{
                            rChannel[i].meterPeak6Index--;
                            rChannel[i].meterPeak6DecayTimer = 10; // preload for next iteration
                        }
                    }
                }
            }
        }

        // meter8 is used on every model
        uint8_t currentMeterPeak8Index;
        currentMeterPeak8Index = GetPeak(i, 8);
        if (currentMeterPeak8Index >= rChannel[i].meterPeak8Index) {
			// currentMeterPeakIndex is above current LED -> set peakHold LED to highest value
			rChannel[i].meterPeak8Index = currentMeterPeak8Index;
			rChannel[i].meterPeak8HoldTimer = 100; // preload to 1000ms
		}else{
			// currentMeterPeakIndex is below current LED -> check if we have to hold the peak LED
			if (rChannel[i].meterPeak8HoldTimer > 0) {
				// hold current LED
				rChannel[i].meterPeak8HoldTimer--;
				rChannel[i].meterPeak8DecayTimer = 10; // preload
			}else{
				// let peak LED fall down every 100ms. It takes a maximum of 400ms to let the peak fall down
				if (rChannel[i].meterPeak8Index > currentMeterPeak8Index) {
					if (rChannel[i].meterPeak8DecayTimer > 0) {
						rChannel[i].meterPeak8DecayTimer--;
					}else{
						rChannel[i].meterPeak8Index--;
						rChannel[i].meterPeak8DecayTimer = 10; // preload for next iteration
					}
				}
			}
		}

		// Step 2: Calculate decayed value
		if (rChannel[i].meter > rChannel[i].meterDecay) {
			// current value is above stored decay-value -> copy value immediatly
			rChannel[i].meterDecay = rChannel[i].meter;
		}else{
			// current value is below -> afterglow
			// this function is called every 10ms. A Decay-Rate of 6dB/second would be ideal, but we do a rought estimation here
			rChannel[i].meterDecay -= (rChannel[i].meterDecay / 10);
		}

		// Step 3: Calculate real LEDs to switch on
		// data contains a 32-bit sample-value
		// lets check the threshold and set meterInfo

        //####################################################################
        //
        //      6-Step Meter (Channelstrip on Full, Compact, Producer)
        //
        //      DYNAMCIS Enabled
        //      GATE/PRE Enabled
        //      CLIP
        //      -6
        //      -12
        //      -18
        //      -30
        //      -60
        //
        //
        //    8-Step Meter (Selected Channel Meter on all models)
        //
        //      CLIP
        //      -3
        //      -6
        //      -9
        //      -12
        //      -18
        //      -30
        //      SIG (-60?)
        //
        //#####################################################################


		rChannel[i].meter6Info = 0;
        rChannel[i].meter8Info = 0;
		    if (rChannel[i].meterDecay >= VUTRESH_00_DBFS_CLIP)  { 
                rChannel[i].meter6Info = 0b00111111;
                rChannel[i].meter8Info = 0b11111111;
            } else if (rChannel[i].meterDecay >= VUTRESH_MINUS_03_DBFS) {
                rChannel[i].meter6Info = 0b00011111;
                rChannel[i].meter8Info = 0b01111111;
            } else if (rChannel[i].meterDecay >= VUTRESH_MINUS_06_DBFS) {
                rChannel[i].meter6Info = 0b00011111;
                rChannel[i].meter8Info = 0b00111111;
            } else if (rChannel[i].meterDecay >= VUTRESH_MINUS_09_DBFS) {
                rChannel[i].meter6Info = 0b00001111;
                rChannel[i].meter8Info = 0b00011111;
            } else if (rChannel[i].meterDecay >= VUTRESH_MINUS_12_DBFS) {
                rChannel[i].meter6Info = 0b00001111;
                rChannel[i].meter8Info = 0b00001111;
            } else if (rChannel[i].meterDecay >= VUTRESH_MINUS_18_DBFS) {
                rChannel[i].meter6Info = 0b00000111;
                rChannel[i].meter8Info = 0b00000111;
            } else if (rChannel[i].meterDecay >= VUTRESH_MINUS_30_DBFS) {
                rChannel[i].meter6Info = 0b00000011;
                rChannel[i].meter8Info = 0b00000011;
            } else if (rChannel[i].meterDecay >= VUTRESH_MINUS_60_DBFS) {
                rChannel[i].meter6Info = 0b00000001;
                rChannel[i].meter8Info = 0b00000001;
            }

        if(!(config->IsModelX32Core() || config->IsModelX32Rack())) {
		    uint8_t peak6Bit = 0;
            if (rChannel[i].meterPeak6Index > 0) peak6Bit = 1 << (rChannel[i].meterPeak6Index -1);
		    rChannel[i].meter6Info |= peak6Bit;
        }

        uint8_t peak8Bit = 0;
        if (rChannel[i].meterPeak8Index > 0) peak8Bit = 1 << (rChannel[i].meterPeak8Index -1);
        rChannel[i].meter8Info |= peak8Bit;
        
        if(!(config->IsModelX32Core() || config->IsModelX32Rack())) {
		    // the dynamic-information is received with the 'd' information, but we will store them here
		    if (Channel[i].gate.gain < 1.0f) { rChannel[i].meter6Info |= 0b01000000; }
		    if (Channel[i].compressor.gain < 1.0f) { rChannel[i].meter6Info |= 0b10000000; }
        }

		//Channel[i].compressor.gain = floatValues[45 + i];
		//Channel[i].gate.gain = floatValues[85 + i];
	}
}

uint8_t DSP1::GetPeak(int i, uint8_t steps)
{
    if (steps==6) {
        if (rChannel[i].meter >= VUTRESH_00_DBFS_CLIP) { return 6; } // CLIP
        else if (rChannel[i].meter >= VUTRESH_MINUS_06_DBFS) { return 5; }
        else if (rChannel[i].meter >= VUTRESH_MINUS_12_DBFS) { return 4; }
        else if (rChannel[i].meter >= VUTRESH_MINUS_18_DBFS) { return 3; }
        else if (rChannel[i].meter >= VUTRESH_MINUS_30_DBFS) { return 2; }
        else if (rChannel[i].meter >= VUTRESH_MINUS_60_DBFS) { return 1; }
    }

    if (steps==8) {
        if (rChannel[i].meter >= VUTRESH_00_DBFS_CLIP) { return 8; } // CLIP
        else if (rChannel[i].meter >= VUTRESH_MINUS_03_DBFS) { return 7; }
        else if (rChannel[i].meter >= VUTRESH_MINUS_06_DBFS) { return 6; }
        else if (rChannel[i].meter >= VUTRESH_MINUS_09_DBFS) { return 5; }
        else if (rChannel[i].meter >= VUTRESH_MINUS_12_DBFS) { return 4; }
        else if (rChannel[i].meter >= VUTRESH_MINUS_18_DBFS) { return 3; }
        else if (rChannel[i].meter >= VUTRESH_MINUS_30_DBFS) { return 2; }
        else if (rChannel[i].meter >= VUTRESH_MINUS_60_DBFS) { return 1; }
    }

    return 0;
}

void DSP1::callbackDsp1(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values) {
    float* floatValues = (float*)values;
    uint32_t* intValues = (uint32_t*)values;

    helper->DEBUG_DSP1(DEBUGLEVEL_VERBOSE, "Callback - classid=%c channel=%c, index=%d, valueCount=%d", classId, channel, index, valueCount);

    switch (classId) {
        case 's': // status-feedback
            switch (channel) {
                case 'u': // Update pack
                    if (valueCount == 45) {
                        state->dspVersion[0] = floatValues[0];

                        // we are receiving 16 samples with 20.83us each resulting in 16*20.83us = 333us per interrupt
                        // DSP-Load calculation: number of used CPU-cycles for processing divided by the core-clock-frequency based on the 333us timebase
                        state->dspLoad[0] = (((float)intValues[1]/264.0f) / (16.0f/0.048f)) * 100.0f;

                        // copy meter-info to channel-struct
                        MainChannelLR.meter[0] = abs(floatValues[2]); // convert 32-bit audio-value
                        MainChannelLR.meter[1] = abs(floatValues[3]); // convert 32-bit audio-value
                        MainChannelSub.meter[0] = abs(floatValues[4]); // convert 32-bit audio-value

                        MainChannelLR.meterPu[0] = abs(floatValues[2])/2147483648.0f; // convert 32-bit audio-value to absolute p.u.
                        MainChannelLR.meterPu[1] = abs(floatValues[3])/2147483648.0f; // convert 32-bit audio-value to absolute p.u.
                        MainChannelSub.meterPu[0] = abs(floatValues[4])/2147483648.0f; // convert 32-bit audio-value to absolute p.u.
                        for (int i = 0; i < 40; i++) {
                            rChannel[i].meter = abs(floatValues[5 + i]); // convert 32-bit audio-value
                            rChannel[i].meterPu = abs(floatValues[5 + i])/2147483648.0f; // convert 32-bit audio-value to absolute p.u.
                        }
                    }
                    break;
            }
            break;
        default:
            break;
    }
}

void DSP1::callbackDsp2(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values) {
    float* floatValues = (float*)values;
    uint32_t* intValues = (uint32_t*)values;

    helper->DEBUG_DSP2(DEBUGLEVEL_VERBOSE, "Callback - classid=%c channel=%c, index=%d, valueCount=%d", classId, channel, index, valueCount);

    switch (classId) {
        case 's': // status-feedback
            switch (channel) {
                case 'u': // Update pack
                    if (valueCount == 2) {
                        state->dspVersion[1] = floatValues[0];
                        state->dspLoad[1] = (((float)intValues[1]/264.0f) / (16.0f/0.048f)) * 100.0f;
                    }
                    break;
            }
            break;
        default:
            break;
    }
}
