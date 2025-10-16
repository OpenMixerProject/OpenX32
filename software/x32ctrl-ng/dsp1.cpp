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

DSP1::DSP1(Config* config, State* state) : X32Base(config, state) {
    spi = new SPI(config, state);
    fx = new FX(config, state);
};

void DSP1::dspInit(void) {

    mainChannelLR.volume = -100; // dB
    mainChannelLR.balance = 0; // -100 .. 0 .. +100
    mainChannelSub.volume = -100; // dB
    mainChannelSub.balance = 0; // -100 .. 0 .. +100

    for (uint8_t i = 0; i < 40; i++) {
        dspChannel[i].lowCutFrequency = 100.0f;

        dspChannel[i].gate.threshold = -80; // dB -> no gate
        dspChannel[i].gate.range = 60; // full range
        dspChannel[i].gate.attackTime_ms = 10;
        dspChannel[i].gate.holdTime_ms = 50;
        dspChannel[i].gate.releaseTime_ms = 250;

        dspChannel[i].compressor.threshold = 0; // dB -> no compression
        dspChannel[i].compressor.ratio = 1.0f/3.0f; // 1:3
        dspChannel[i].compressor.makeup = 0; // dB
        dspChannel[i].compressor.attackTime_ms = 10;
        dspChannel[i].compressor.holdTime_ms = 10;
        dspChannel[i].compressor.releaseTime_ms = 150;

        for (uint8_t peq = 0; peq < MAX_CHAN_EQS; peq++) {
            dspChannel[i].peq[peq].type = 1;
            dspChannel[i].peq[peq].fc = 3000;
            dspChannel[i].peq[peq].Q = 2.0;
            dspChannel[i].peq[peq].gain = 0;
        }

        for (uint8_t i_mixbus = 0; i_mixbus < 16; i_mixbus++) {
            dspChannel[i].sendMixbus[i_mixbus] = VOLUME_MIN;
        }

        monitorVolume = 0; // dB
        monitorTapPoint = 0; // TAP_INPUT

        // initialize dsp-routing
        // route output 1-14 to Mixbus 1-14
        // route output 15/16 to MainLeft/MainRight
        // route output 17-32 to DirectOut 1-16
        // route output 33-37 to AuxIn
        // route output 38-39 to ???
        //
        // audioOutputChannelBuffer contains this data
        // 0:       OFF
        // 1..32:   Input 1-32
        // 33..40:  AuxIn 1-6, TalkbackA, TalkbackB
        // 41..56:  MixBus 1-16
        // 57..62:   Matrix 1-6
        // 63: MainL
        // 64: MainR
        // 65: MainSub
        // 66..68: Monitor L/R/TB
        // 69..84: FX Return 1-16
        // 85..92: DSP2 Aux-Channel 1-8
        //
        // connect DSP-inputs 1-40 to all 40 input-sources from FPGA
        dspChannel[i].inputSource = DSP_BUF_IDX_DSPCHANNEL + i; // 0=OFF, 1..32=DSP-Channel, 33..40=Aux, 41..56=Mixbus, 57..62=Matrix, 63=MainL, 64=MainR, 65=MainSub, 66..68=MonL,MonR,Talkback
        dspChannel[i].inputTapPoint = 0; // 0=INPUT, 1=PreEQ, 2=PostEQ, 3=PreFader, 4=PostFader
        // connect MainLeft on even and MainRight on odd channels as PostFader
        dspChannel[i].outputSource = DSP_BUF_IDX_MAINLEFT + (i % 2); // 0=OFF, 1..32=DSP-Channel, 33..40=Aux, 41..56=Mixbus, 57..62=Matrix, 63=MainL, 64=MainR, 65=MainSub, 66..68=MonL,MonR,Talkback, 69..84=FX-Return, 85..92=DSP2AUX
        dspChannel[i].outputTapPoint = 4; // 0=INPUT, 1=PreEQ, 2=PostEQ, 3=PreFader, 4=PostFader

        // Volumes, Balance and Mute/Solo is setup in mixerInit()
    }

    for (uint8_t i = 0; i < 15; i++) {
        fxChannel[i].inputSource = DSP_BUF_IDX_MIXBUS; // connect all 16 mixbus-channels to DSP2
    }
    for (uint8_t i = 0; i < 8; i++) {
        dsp2AuxChannel[i].inputSource = DSP_BUF_IDX_DSPCHANNEL; // connect inputs 1-8 to DSP2 Aux-Channels 1-8
    }
}

// set the general volume of one of the 40 DSP-channels
void DSP1::SendChannelVolume(uint8_t chan) {
    // set value to interal struct
    float balanceLeft = helper->Saturate(100.0f - dspChannel[chan].balance, 0.0f, 100.0f) / 100.0f;
    float balanceRight = helper->Saturate(dspChannel[chan].balance + 100.0f, 0.0f, 100.0f) / 100.0f;
    float volumeLR = dspChannel[chan].volumeLR;
    float volumeSub = dspChannel[chan].volumeSub;

    if (dspChannel[chan].muted) {
        volumeLR = -100; // dB
        volumeSub = -100; // dB
    }

    // send volume to DSP via SPI
    float values[4];
    values[0] = pow(10.0f, volumeLR/20.0f); // volume of this specific channel
    values[1] = balanceLeft; // 100 .. 100 ..  0
    values[2] = balanceRight; // 0  .. 100 .. 100
    values[3] = pow(10.0f, volumeSub/20.0f); // subwoofer

    spi->SendDspParameterArray(0, 'v', chan, 0, 4, &values[0]);
}

// send BusSends
void DSP1::SendChannelSend(uint8_t chan) {
    float values[16];

    for (uint8_t i_mixbus = 0; i_mixbus < 16; i_mixbus++) {
        values[i_mixbus] = pow(10.0f, dspChannel[chan].sendMixbus[i_mixbus]/20.0f); // volume of this specific channel
    }

    spi->SendDspParameterArray(0, 's', chan, 0, 16, &values[0]);
}

void DSP1::SendMixbusVolume(uint8_t bus) {
    float balanceLeft = helper->Saturate(100.0f - mixbusChannel[bus].balance, 0.0f, 100.0f) / 100.0f;
    float balanceRight = helper->Saturate(mixbusChannel[bus].balance + 100.0f, 0.0f, 100.0f) / 100.0f;

    // send volume to DSP via SPI
    float values[4];
    values[0] = pow(10.0f, mixbusChannel[bus].volumeLR/20.0f); // volume of this specific channel
    values[1] = balanceLeft; // 100 .. 100 ..  0
    values[2] = balanceRight; // 0  .. 100 .. 100
    values[3] = pow(10.0f, mixbusChannel[bus].volumeSub/20.0f); // subwoofer

    spi->SendDspParameterArray(0, 'v', bus, 1, 4, &values[0]);
}

void DSP1::SendMatrixVolume(uint8_t matrix) {
    // send volume to DSP via spi->
    float values[1];

    values[0] = pow(10.0f, matrixChannel[matrix].volume/20.0f); // volume of this specific channel

    spi->SendDspParameterArray(0, 'v', matrix, 2, 1, &values[0]);
}

void DSP1::SendMonitorVolume() {
    // send volume to DSP via spi
    float values[1];
    
    values[0] = pow(10.0f, monitorVolume/20.0f); // volume of this specific channel

    spi->SendDspParameterArray(0, 'v', 0, 4, 1, &values[0]);
}

void DSP1::SendMainVolume() {
    float volumeLeft = (helper->Saturate(100.0f - mainChannelLR.balance, 0.0f, 100.0f) / 100.0f) * pow(10.0f, mainChannelLR.volume/20.0f);
    float volumeRight = (helper->Saturate(mainChannelLR.balance + 100.0f, 0.0f, 100.0f) / 100.0f) * pow(10.0f, mainChannelLR.volume/20.0f);
    float volumeSub = pow(10.0f, mainChannelSub.volume/20.0f);

    if (mainChannelLR.muted) {
        volumeLeft = 0; // p.u.
        volumeRight = 0; // p.u.
    }
    if (mainChannelSub.muted) {
        volumeSub = 0; // p.u.
    }

    // send volume to DSP via spi->
    float values[3];
    values[0] = volumeLeft;
    values[1] = volumeRight;
    values[2] = volumeSub;

    spi->SendDspParameterArray(0, 'v', 0, 3, 3, &values[0]);
}

void DSP1::SendGate(uint8_t chan) {
    fx->RecalcGate(&dspChannel[chan].gate);

    float values[5];
    values[0] = dspChannel[chan].gate.value_threshold;
    values[1] = dspChannel[chan].gate.value_gainmin;
    values[2] = dspChannel[chan].gate.value_coeff_attack;
    values[3] = dspChannel[chan].gate.value_hold_ticks;
    values[4] = dspChannel[chan].gate.value_coeff_release;

    spi->SendDspParameterArray(0, 'g', chan, 0, 5, &values[0]);
}

void DSP1::SendLowcut(uint8_t chan) {
    float values[1];

    values[0] = 1.0f / (1.0f + 2.0f * M_PI * dspChannel[chan].lowCutFrequency * (1.0f/config->GetSamplerate()));

    spi->SendDspParameterArray(0, 'e', chan, 'l', 1, &values[0]);
}

/*
void dspSendHighcut(uint8_t dspChannel) {
    float values[1];

    values[0] = (2.0f * M_PI * dspChannel[dspChannel].highCutFrequency) / (samplerate + 2.0f * M_PI * 500.0f);

    spi->SendDspParameterArray(0, 'e', dspChannel, 'h', 1, &values[0]);
}
*/

void DSP1::SendEQ(uint8_t chan) {
    // biquad_trans() needs the coeffs in the following order
    // a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 0/1)
    // a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 2/3)
    // a0 a1 a2 b1 b2 (section 4)

    float values[MAX_CHAN_EQS * 5];

    for (int peq = 0; peq < MAX_CHAN_EQS; peq++) {
        fx->RecalcFilterCoefficients_PEQ(&dspChannel[chan].peq[peq]);

/*
        // send coeffiecients without interleaving for biquad() function
        int sectionIndex = peq * 5;
        values[sectionIndex + 0] = -dspChannel[dspChannel].peq[peq].b[2]; // -b2 (poles)
        values[sectionIndex + 1] = -dspChannel[dspChannel].peq[peq].b[1]; // -b1 (poles)
        values[sectionIndex + 2] = dspChannel[dspChannel].peq[peq].a[2]; // a2 (zeros)
        values[sectionIndex + 3] = dspChannel[dspChannel].peq[peq].a[1]; // a1 (zeros)
        values[sectionIndex + 4] = dspChannel[dspChannel].peq[peq].a[0]; // a0 (zeros)
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
            values[sectionIndex + 0] = dspChannel[chan].peq[peq].a[0]; // a0 (zeros)
            values[sectionIndex + 2] = dspChannel[chan].peq[peq].a[1]; // a1 (zeros)
            values[sectionIndex + 4] = dspChannel[chan].peq[peq].a[2]; // a2 (zeros)
            values[sectionIndex + 6] = -dspChannel[chan].peq[peq].b[1]; // -b1 (poles)
            values[sectionIndex + 8] = -dspChannel[chan].peq[peq].b[2]; // -b2 (poles)
        }else{
            // last section: store without interleaving
            int sectionIndex = (MAX_CHAN_EQS - 1) * 5;
            values[sectionIndex + 0] = dspChannel[chan].peq[peq].a[0]; // a0 (zeros)
            values[sectionIndex + 1] = dspChannel[chan].peq[peq].a[1]; // a1 (zeros)
            values[sectionIndex + 2] = dspChannel[chan].peq[peq].a[2]; // a2 (zeros)
            values[sectionIndex + 3] = -dspChannel[chan].peq[peq].b[1]; // -b1 (poles)
            values[sectionIndex + 4] = -dspChannel[chan].peq[peq].b[2]; // -b2 (poles)
        }
    }

    spi->SendDspParameterArray(0, 'e', chan, 'e', MAX_CHAN_EQS * 5, &values[0]);
}

void DSP1::ResetEq(uint8_t chan) {
    float values[1];
    values[0] = 0;
    spi->SendDspParameterArray(0, 'e', chan, 'r', 1, &values[0]);
}

void DSP1::SendCompressor(uint8_t chan) {
    fx->RecalcCompressor(&dspChannel[chan].compressor);

    float values[6];
    values[0] = dspChannel[chan].compressor.value_threshold;
    values[1] = dspChannel[chan].compressor.value_ratio;
    values[2] = dspChannel[chan].compressor.value_makeup;
    values[3] = dspChannel[chan].compressor.value_coeff_attack;
    values[4] = dspChannel[chan].compressor.value_hold_ticks;
    values[5] = dspChannel[chan].compressor.value_coeff_release;

    spi->SendDspParameterArray(0, 'c', chan, 0, 6, &values[0]);
}

void DSP1::SendAll() {
    for (uint8_t chan = 0; chan <= 39; chan++) {
        SendLowcut(chan);
        SendGate(chan);
        SendEQ(chan);
        SendCompressor(chan);
        SetInputRouting(chan);
        SetOutputRouting(chan);
        SendChannelVolume(chan);
        SendChannelSend(chan);
        for (uint8_t mixbusChannel = 0; mixbusChannel <= 15; mixbusChannel++) {
            SetChannelSendTapPoints(chan, mixbusChannel, dspChannel[chan].sendMixbusTapPoint[mixbusChannel]);
        }
    }
    for (uint8_t mixbusChannel = 0; mixbusChannel <= 15; mixbusChannel++) {
        SendMixbusVolume(mixbusChannel);
        for (uint8_t matrixChannel = 0; matrixChannel <= 5; matrixChannel++) {
            SetMixbusSendTapPoints(mixbusChannel, matrixChannel, this->mixbusChannel[mixbusChannel].sendMatrixTapPoint[matrixChannel]);
        }
    }
    for (uint8_t matrixChannel = 0; matrixChannel <= 5; matrixChannel++) {
        SendMatrixVolume(matrixChannel);
        SetMainSendTapPoints(matrixChannel, mainChannelLR.sendMatrixTapPoint[matrixChannel]);
    }
    for (uint8_t fxChannel = 0; fxChannel <= 15; fxChannel++) {
        SetInputRouting(fxChannel + 40);
    }
    for (uint8_t dsp2AuxChannel = 0; dsp2AuxChannel <= 7; dsp2AuxChannel++) {
        SetInputRouting(dsp2AuxChannel + 56);
    }
    SendMainVolume();
    SendMonitorVolume();
}

void DSP1::SetInputRouting(uint8_t chan) {
    uint32_t values[2];
    values[0] = dspChannel[chan].inputSource;
    values[1] = dspChannel[chan].inputTapPoint;
    spi->SendDspParameterArray(0, 'r', chan, 0, 2, (float*)&values[0]);
}

void DSP1::SetOutputRouting(uint8_t chan) {
    uint32_t values[2];
    values[0] = dspChannel[chan].outputSource;
    values[1] = dspChannel[chan].outputTapPoint;
    spi->SendDspParameterArray(0, 'r', chan, 1, 2, (float*)&values[0]);
}

void DSP1::SetChannelSendTapPoints(uint8_t chan, uint8_t mixbusChannel, uint8_t tapPoint) {
    dspChannel[chan].sendMixbusTapPoint[mixbusChannel] = tapPoint;

    uint32_t values[2];
    values[0] = mixbusChannel;
    values[1] = tapPoint;
    spi->SendDspParameterArray(0, 't', chan, 0, 2, (float*)&values[0]);
}

void DSP1::SetMixbusSendTapPoints(uint8_t mixbusChannel, uint8_t matrixChannel, uint8_t tapPoint) {
    this->mixbusChannel[mixbusChannel].sendMatrixTapPoint[matrixChannel] = tapPoint;

    uint32_t values[2];
    values[0] = matrixChannel;
    values[1] = tapPoint;
    spi->SendDspParameterArray(0, 't', mixbusChannel, 1, 2, (float*)&values[0]);
}

void DSP1::SetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint) {
    mainChannelLR.sendMatrixTapPoint[matrixChannel] = tapPoint;

    uint32_t values[2];
    values[0] = matrixChannel;
    values[1] = tapPoint;
    spi->SendDspParameterArray(0, 't', 0, 2, 2, (float*)&values[0]);
}

// void dspGetSourceName(char* p_nameBuffer, uint8_t dspChannel) {
//     if ((dspChannel >= 0) && (dspChannel < 40)) {
//         // we have a DSP-channel
//         uint8_t channelInputSource = dspChannel[dspChannel].inputSource;

//         // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
//         if (channelInputSource == 0) {
//             // OFF
//             sprintf(p_nameBuffer, "Off");
//         }else if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
//             // we are connected to one of the DSP-inputs
//             uint8_t dspInputSource = mixer->fpgaRouting.dsp[channelInputSource - 1];
//             if (dspInputSource == 0) {
//                 sprintf(p_nameBuffer, "Off");
//             }else if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
//                 // XLR-input
//                 sprintf(p_nameBuffer, "XLR%02d", dspInputSource);
//             }else if ((dspInputSource >= 33) && (dspInputSource <= 64)) {
//                 // Card input
//                 sprintf(p_nameBuffer, "C%02d", dspInputSource - 32);
//             }else if ((dspInputSource >= 65) && (dspInputSource <= 72)) {
//                 // Aux input
//                 sprintf(p_nameBuffer, "Aux%02d", dspInputSource - 64);
//             }else if ((dspInputSource >= 73) && (dspInputSource <= 112)) {
//                 // DSP input
//                 sprintf(p_nameBuffer, "In%02d", dspInputSource - 72);
//             }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
//                 // AES50A input
//                 sprintf(p_nameBuffer, "A%02d", dspInputSource - 112);
//             }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
//                 // AES50B input
//                 sprintf(p_nameBuffer, "B%02d", dspInputSource - 160);
//             }else{
//                 sprintf(p_nameBuffer, "???");
//             }
//         }else if ((channelInputSource >= 41) && (channelInputSource <= 56)) {
//             // Mixbus 1-16
//             sprintf(p_nameBuffer, "Bus %02d", channelInputSource - 40);
//         }else if ((channelInputSource >= 57) && (channelInputSource <= 62)) {
//             // Matrix 1-6
//             sprintf(p_nameBuffer, "Mtx %02d", channelInputSource - 56);
//         }else if (channelInputSource == 63) {
//             sprintf(p_nameBuffer, "Main L");
//         }else if (channelInputSource == 64) {
//             sprintf(p_nameBuffer, "Main R");
//         }else if (channelInputSource == 65) {
//             sprintf(p_nameBuffer, "Main C");
//         }else if (channelInputSource == 66) {
//             sprintf(p_nameBuffer, "Mon L");
//         }else if (channelInputSource == 67) {
//             sprintf(p_nameBuffer, "Mon R");
//         }else if (channelInputSource == 68) {
//             sprintf(p_nameBuffer, "Talkback");
//         }else{
//             sprintf(p_nameBuffer, "???");
//         }
//     }else{
//         // we have a non-DSP-channel -> no source available
//         sprintf(p_nameBuffer, "<Intern>");
//     }
// }

