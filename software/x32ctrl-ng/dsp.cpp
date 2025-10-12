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

#include "dsp.h"

DSP::DSP(Config* config) : X32Base(config) {

};

void DSP::dspInit(void) {

    dsp.mainChannelLR.volume = -100; // dB
    dsp.mainChannelLR.balance = 0; // -100 .. 0 .. +100
    dsp.mainChannelSub.volume = -100; // dB
    dsp.mainChannelSub.balance = 0; // -100 .. 0 .. +100

    for (uint8_t i = 0; i < 40; i++) {
        dsp.dspChannel[i].lowCutFrequency = 100.0f;

        dsp.dspChannel[i].gate.threshold = -80; // dB -> no gate
        dsp.dspChannel[i].gate.range = 60; // full range
        dsp.dspChannel[i].gate.attackTime_ms = 10;
        dsp.dspChannel[i].gate.holdTime_ms = 50;
        dsp.dspChannel[i].gate.releaseTime_ms = 250;

        dsp.dspChannel[i].compressor.threshold = 0; // dB -> no compression
        dsp.dspChannel[i].compressor.ratio = 1.0f/3.0f; // 1:3
        dsp.dspChannel[i].compressor.makeup = 0; // dB
        dsp.dspChannel[i].compressor.attackTime_ms = 10;
        dsp.dspChannel[i].compressor.holdTime_ms = 10;
        dsp.dspChannel[i].compressor.releaseTime_ms = 150;

        for (uint8_t peq = 0; peq < MAX_CHAN_EQS; peq++) {
            dsp.dspChannel[i].peq[peq].type = 1;
            dsp.dspChannel[i].peq[peq].fc = 3000;
            dsp.dspChannel[i].peq[peq].Q = 2.0;
            dsp.dspChannel[i].peq[peq].gain = 0;
        }

        for (uint8_t i_mixbus = 0; i_mixbus < 16; i_mixbus++) {
            dsp.dspChannel[i].sendMixbus[i_mixbus] = VOLUME_MIN;
        }

        dsp.monitorVolume = 0; // dB
        dsp.monitorTapPoint = 0; // TAP_INPUT

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
        dsp.dspChannel[i].inputSource = DSP_BUF_IDX_DSPCHANNEL + i; // 0=OFF, 1..32=DSP-Channel, 33..40=Aux, 41..56=Mixbus, 57..62=Matrix, 63=MainL, 64=MainR, 65=MainSub, 66..68=MonL,MonR,Talkback
        dsp.dspChannel[i].inputTapPoint = 0; // 0=INPUT, 1=PreEQ, 2=PostEQ, 3=PreFader, 4=PostFader
        // connect MainLeft on even and MainRight on odd channels as PostFader
        dsp.dspChannel[i].outputSource = DSP_BUF_IDX_MAINLEFT + (i % 2); // 0=OFF, 1..32=DSP-Channel, 33..40=Aux, 41..56=Mixbus, 57..62=Matrix, 63=MainL, 64=MainR, 65=MainSub, 66..68=MonL,MonR,Talkback, 69..84=FX-Return, 85..92=DSP2AUX
        dsp.dspChannel[i].outputTapPoint = 4; // 0=INPUT, 1=PreEQ, 2=PostEQ, 3=PreFader, 4=PostFader

        // Volumes, Balance and Mute/Solo is setup in mixerInit()
    }

    for (uint8_t i = 0; i < 15; i++) {
        dsp.fxChannel[i].inputSource = DSP_BUF_IDX_MIXBUS; // connect all 16 mixbus-channels to DSP2
    }
    for (uint8_t i = 0; i < 8; i++) {
        dsp.dsp2AuxChannel[i].inputSource = DSP_BUF_IDX_DSPCHANNEL; // connect inputs 1-8 to DSP2 Aux-Channels 1-8
    }
}

// set the general volume of one of the 40 DSP-channels
void DSP::dspSendChannelVolume(uint8_t dspChannel) {
    // set value to interal struct
    float balanceLeft = helper->Saturate(100.0f - dsp.dspChannel[dspChannel].balance, 0.0f, 100.0f) / 100.0f;
    float balanceRight = helper->Saturate(dsp.dspChannel[dspChannel].balance + 100.0f, 0.0f, 100.0f) / 100.0f;
    float volumeLR = dsp.dspChannel[dspChannel].volumeLR;
    float volumeSub = dsp.dspChannel[dspChannel].volumeSub;

    if (dsp.dspChannel[dspChannel].muted) {
        volumeLR = -100; // dB
        volumeSub = -100; // dB
    }

    // send volume to DSP via SPI
    float values[4];
    values[0] = pow(10.0f, volumeLR/20.0f); // volume of this specific channel
    values[1] = balanceLeft; // 100 .. 100 ..  0
    values[2] = balanceRight; // 0  .. 100 .. 100
    values[3] = pow(10.0f, volumeSub/20.0f); // subwoofer

    spiSendDspParameterArray(0, 'v', dspChannel, 0, 4, &values[0]);
}

// send BusSends
void DSP::dspSendChannelSend(uint8_t dspChannel) {
    float values[16];

    for (uint8_t i_mixbus = 0; i_mixbus < 16; i_mixbus++) {
        values[i_mixbus] = pow(10.0f, dsp.dspChannel[dspChannel].sendMixbus[i_mixbus]/20.0f); // volume of this specific channel
    }

    spiSendDspParameterArray(0, 's', dspChannel, 0, 16, &values[0]);
}

void DSP::dspSendMixbusVolume(uint8_t mixbusChannel) {
    float balanceLeft = helper->Saturate(100.0f - dsp.mixbusChannel[mixbusChannel].balance, 0.0f, 100.0f) / 100.0f;
    float balanceRight = helper->Saturate(dsp.mixbusChannel[mixbusChannel].balance + 100.0f, 0.0f, 100.0f) / 100.0f;

    // send volume to DSP via SPI
    float values[4];
    values[0] = pow(10.0f, dsp.mixbusChannel[mixbusChannel].volumeLR/20.0f); // volume of this specific channel
    values[1] = balanceLeft; // 100 .. 100 ..  0
    values[2] = balanceRight; // 0  .. 100 .. 100
    values[3] = pow(10.0f, dsp.mixbusChannel[mixbusChannel].volumeSub/20.0f); // subwoofer

    spiSendDspParameterArray(0, 'v', mixbusChannel, 1, 4, &values[0]);
}

void DSP::dspSendMatrixVolume(uint8_t matrixChannel) {
    // send volume to DSP via SPI
    float values[1];

    values[0] = pow(10.0f, dsp.matrixChannel[matrixChannel].volume/20.0f); // volume of this specific channel

    spiSendDspParameterArray(0, 'v', matrixChannel, 2, 1, &values[0]);
}

void DSP::dspSendMonitorVolume() {
    // send volume to DSP via SPI
    float values[1];
    
    values[0] = pow(10.0f, dsp.monitorVolume/20.0f); // volume of this specific channel

    spiSendDspParameterArray(0, 'v', 0, 4, 1, &values[0]);
}

void DSP::dspSendMainVolume() {
    float volumeLeft = (helper->Saturate(100.0f - dsp.mainChannelLR.balance, 0.0f, 100.0f) / 100.0f) * pow(10.0f, dsp.mainChannelLR.volume/20.0f);
    float volumeRight = (helper->Saturate(dsp.mainChannelLR.balance + 100.0f, 0.0f, 100.0f) / 100.0f) * pow(10.0f, dsp.mainChannelLR.volume/20.0f);
    float volumeSub = pow(10.0f, dsp.mainChannelSub.volume/20.0f);

    if (dsp.mainChannelLR.muted) {
        volumeLeft = 0; // p.u.
        volumeRight = 0; // p.u.
    }
    if (dsp.mainChannelSub.muted) {
        volumeSub = 0; // p.u.
    }

    // send volume to DSP via SPI
    float values[3];
    values[0] = volumeLeft;
    values[1] = volumeRight;
    values[2] = volumeSub;

    spiSendDspParameterArray(0, 'v', 0, 3, 3, &values[0]);
}

void DSP::dspSendGate(uint8_t dspChannel) {
    helper->FxRecalcGate(&dsp.dspChannel[dspChannel].gate);

    float values[5];
    values[0] = dsp.dspChannel[dspChannel].gate.value_threshold;
    values[1] = dsp.dspChannel[dspChannel].gate.value_gainmin;
    values[2] = dsp.dspChannel[dspChannel].gate.value_coeff_attack;
    values[3] = dsp.dspChannel[dspChannel].gate.value_hold_ticks;
    values[4] = dsp.dspChannel[dspChannel].gate.value_coeff_release;

    spiSendDspParameterArray(0, 'g', dspChannel, 0, 5, &values[0]);
}

void DSP::dspSendLowcut(uint8_t dspChannel) {
    float values[1];

    values[0] = 1.0f / (1.0f + 2.0f * M_PI * dsp.dspChannel[dspChannel].lowCutFrequency * (1.0f/config.GetSamplerate()));

    spiSendDspParameterArray(0, 'e', dspChannel, 'l', 1, &values[0]);
}

/*
void dspSendHighcut(uint8_t dspChannel) {
    float values[1];

    values[0] = (2.0f * M_PI * dsp.dspChannel[dspChannel].highCutFrequency) / (dsp.samplerate + 2.0f * M_PI * 500.0f);

    spiSendDspParameterArray(0, 'e', dspChannel, 'h', 1, &values[0]);
}
*/

void DSP::dspSendEQ(uint8_t dspChannel) {
    // biquad_trans() needs the coeffs in the following order
    // a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 0/1)
    // a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 2/3)
    // a0 a1 a2 b1 b2 (section 4)

    float values[MAX_CHAN_EQS * 5];

    for (int peq = 0; peq < MAX_CHAN_EQS; peq++) {
        fxRecalcFilterCoefficients_PEQ(&dsp.dspChannel[dspChannel].peq[peq]);

/*
        // send coeffiecients without interleaving for biquad() function
        int sectionIndex = peq * 5;
        values[sectionIndex + 0] = -dsp.dspChannel[dspChannel].peq[peq].b[2]; // -b2 (poles)
        values[sectionIndex + 1] = -dsp.dspChannel[dspChannel].peq[peq].b[1]; // -b1 (poles)
        values[sectionIndex + 2] = dsp.dspChannel[dspChannel].peq[peq].a[2]; // a2 (zeros)
        values[sectionIndex + 3] = dsp.dspChannel[dspChannel].peq[peq].a[1]; // a1 (zeros)
        values[sectionIndex + 4] = dsp.dspChannel[dspChannel].peq[peq].a[0]; // a0 (zeros)
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
            values[sectionIndex + 0] = dsp.dspChannel[dspChannel].peq[peq].a[0]; // a0 (zeros)
            values[sectionIndex + 2] = dsp.dspChannel[dspChannel].peq[peq].a[1]; // a1 (zeros)
            values[sectionIndex + 4] = dsp.dspChannel[dspChannel].peq[peq].a[2]; // a2 (zeros)
            values[sectionIndex + 6] = -dsp.dspChannel[dspChannel].peq[peq].b[1]; // -b1 (poles)
            values[sectionIndex + 8] = -dsp.dspChannel[dspChannel].peq[peq].b[2]; // -b2 (poles)
        }else{
            // last section: store without interleaving
            int sectionIndex = (MAX_CHAN_EQS - 1) * 5;
            values[sectionIndex + 0] = dsp.dspChannel[dspChannel].peq[peq].a[0]; // a0 (zeros)
            values[sectionIndex + 1] = dsp.dspChannel[dspChannel].peq[peq].a[1]; // a1 (zeros)
            values[sectionIndex + 2] = dsp.dspChannel[dspChannel].peq[peq].a[2]; // a2 (zeros)
            values[sectionIndex + 3] = -dsp.dspChannel[dspChannel].peq[peq].b[1]; // -b1 (poles)
            values[sectionIndex + 4] = -dsp.dspChannel[dspChannel].peq[peq].b[2]; // -b2 (poles)
        }
    }

    spiSendDspParameterArray(0, 'e', dspChannel, 'e', MAX_CHAN_EQS * 5, &values[0]);
}

void DSP::dspResetEq(uint8_t dspChannel) {
    float values[1];
    values[0] = 0;
    spiSendDspParameterArray(0, 'e', dspChannel, 'r', 1, &values[0]);
}

void DSP::dspSendCompressor(uint8_t dspChannel) {
    fxRecalcCompressor(&dsp.dspChannel[dspChannel].compressor);

    float values[6];
    values[0] = dsp.dspChannel[dspChannel].compressor.value_threshold;
    values[1] = dsp.dspChannel[dspChannel].compressor.value_ratio;
    values[2] = dsp.dspChannel[dspChannel].compressor.value_makeup;
    values[3] = dsp.dspChannel[dspChannel].compressor.value_coeff_attack;
    values[4] = dsp.dspChannel[dspChannel].compressor.value_hold_ticks;
    values[5] = dsp.dspChannel[dspChannel].compressor.value_coeff_release;

    spiSendDspParameterArray(0, 'c', dspChannel, 0, 6, &values[0]);
}

void DSP::dspSendAll() {
    for (uint8_t dspChannel = 0; dspChannel <= 39; dspChannel++) {
        dspSendLowcut(dspChannel);
        dspSendGate(dspChannel);
        dspSendEQ(dspChannel);
        dspSendCompressor(dspChannel);
        dspSetInputRouting(dspChannel);
        dspSetOutputRouting(dspChannel);
        dspSendChannelVolume(dspChannel);
        dspSendChannelSend(dspChannel);
        for (uint8_t mixbusChannel = 0; mixbusChannel <= 15; mixbusChannel++) {
            dspSetChannelSendTapPoints(dspChannel, mixbusChannel, dsp.dspChannel[dspChannel].sendMixbusTapPoint[mixbusChannel]);
        }
    }
    for (uint8_t mixbusChannel = 0; mixbusChannel <= 15; mixbusChannel++) {
        dspSendMixbusVolume(mixbusChannel);
        for (uint8_t matrixChannel = 0; matrixChannel <= 5; matrixChannel++) {
            dspSetMixbusSendTapPoints(mixbusChannel, matrixChannel, dsp.mixbusChannel[mixbusChannel].sendMatrixTapPoint[matrixChannel]);
        }
    }
    for (uint8_t matrixChannel = 0; matrixChannel <= 5; matrixChannel++) {
        dspSendMatrixVolume(matrixChannel);
        dspSetMainSendTapPoints(matrixChannel, dsp.mainChannelLR.sendMatrixTapPoint[matrixChannel]);
    }
    for (uint8_t fxChannel = 0; fxChannel <= 15; fxChannel++) {
        dspSetInputRouting(fxChannel + 40);
    }
    for (uint8_t dsp2AuxChannel = 0; dsp2AuxChannel <= 7; dsp2AuxChannel++) {
        dspSetInputRouting(dsp2AuxChannel + 56);
    }
    dspSendMainVolume();
    dspSendMonitorVolume();
}

void DSP::dspSetInputRouting(uint8_t dspChannel) {
    uint32_t values[2];
    values[0] = dsp.dspChannel[dspChannel].inputSource;
    values[1] = dsp.dspChannel[dspChannel].inputTapPoint;
    spiSendDspParameterArray(0, 'r', dspChannel, 0, 2, (float*)&values[0]);
}

void DSP::dspSetOutputRouting(uint8_t dspChannel) {
    uint32_t values[2];
    values[0] = dsp.dspChannel[dspChannel].outputSource;
    values[1] = dsp.dspChannel[dspChannel].outputTapPoint;
    spiSendDspParameterArray(0, 'r', dspChannel, 1, 2, (float*)&values[0]);
}

void DSP::dspSetChannelSendTapPoints(uint8_t dspChannel, uint8_t mixbusChannel, uint8_t tapPoint) {
    dsp.dspChannel[dspChannel].sendMixbusTapPoint[mixbusChannel] = tapPoint;

    uint32_t values[2];
    values[0] = mixbusChannel;
    values[1] = tapPoint;
    spiSendDspParameterArray(0, 't', dspChannel, 0, 2, (float*)&values[0]);
}

void DSP::dspSetMixbusSendTapPoints(uint8_t mixbusChannel, uint8_t matrixChannel, uint8_t tapPoint) {
    dsp.mixbusChannel[mixbusChannel].sendMatrixTapPoint[matrixChannel] = tapPoint;

    uint32_t values[2];
    values[0] = matrixChannel;
    values[1] = tapPoint;
    spiSendDspParameterArray(0, 't', mixbusChannel, 1, 2, (float*)&values[0]);
}

void DSP::dspSetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint) {
    dsp.mainChannelLR.sendMatrixTapPoint[matrixChannel] = tapPoint;

    uint32_t values[2];
    values[0] = matrixChannel;
    values[1] = tapPoint;
    spiSendDspParameterArray(0, 't', 0, 2, 2, (float*)&values[0]);
}

// void dspGetSourceName(char* p_nameBuffer, uint8_t dspChannel) {
//     if ((dspChannel >= 0) && (dspChannel < 40)) {
//         // we have a DSP-channel
//         uint8_t channelInputSource = dsp.dspChannel[dspChannel].inputSource;

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

