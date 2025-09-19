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
#include "mixer.h"
#include "fx.h"
#include "spi.h"

sDsp dsp;

void dspInit(void) {
    dsp.samplerate = 48000;

    dsp.mainLRVolume = 0; // dB
    dsp.mainSubVolume = -100; // dB
    dsp.mainBalance = 0; // -100 .. 0 .. +100

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

        dsp.dspChannel[i].volumeLR = -100; // dB
        dsp.dspChannel[i].volumeSub = -100; // dB
        dsp.dspChannel[i].balance = 0;

        dsp.monitorVolume = 1; // dB
        dsp.monitorTapPoint = 0; // TAP_INPUT

        // initialize dsp-routing
        // route output 1-14 to Mixbus 1-14
        // route output 15/16 to MainLeft/MainRight
        // route output 17-32 to DirectOut 1-16
        // route output 33-37 to AuxIn
        // route output 38-39 to ???
        //
        // audioOutputChannelBuffer contains this data
        // 0: MainL
        // 1: MainR
        // 2: MainSub
        // 3..18: MixBus 1-16
        // 19..24: Matrix 1-6
        // 25..56: Input 1-32
        // 57..64: AuxIn 1-6, TalkbackA, TalkbackB
        // 65..67: Monitor L/R/TB
        //
        // route MainLeft on even and MainRight on odd channels as PostFader
        dsp.outputSource[i] = i % 2; // 0=MainL, 1=MainR, 2=MainSub, 3..18=Mixbus, 19..24=Matrix, 25..56=DSP-Channel, 57..64=Aux, 65..67=MonL,MonR,Talkback
        dsp.outputTapPoint[i] = 4; // 0=INPUT, 1=PreEQ, 2=PostEQ, 3=PreFader, 4=PostFader
    }

    for (uint8_t i = 0; i < 16; i++) {
        dsp.mixbusChannel[i].volumeLR = 0; // dB
        dsp.mixbusChannel[i].volumeSub = -100; // dB
        dsp.mixbusChannel[i].balance = 0;
    }

    for (uint8_t i = 0; i < 6; i++) {
        dsp.matrixChannel[i].volume = 0; // dB
    }

    // some testvalues for channel 1
    dsp.dspChannel[0].gate.threshold = -80; // dB -> no gate
    dsp.dspChannel[0].gate.range = 60; // full range
    dsp.dspChannel[0].gate.attackTime_ms = 10;
    dsp.dspChannel[0].gate.holdTime_ms = 50;
    dsp.dspChannel[0].gate.releaseTime_ms = 250;

    dsp.dspChannel[0].compressor.threshold = -40; // dB -> full compression
    dsp.dspChannel[0].compressor.ratio = 1.0f/10.0f; // 1:100 -> limiter
    dsp.dspChannel[0].compressor.makeup = 10; // dB
    dsp.dspChannel[0].compressor.attackTime_ms = 10;
    dsp.dspChannel[0].compressor.holdTime_ms = 50;
    dsp.dspChannel[0].compressor.releaseTime_ms = 100;
}

// set the general volume of one of the 40 DSP-channels
void dspSendChannelVolume(uint8_t dspChannel, float volumeLR, float volumeSub, float balance) {
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

void dspSendMixbusVolume(uint8_t mixbusChannel, float volumeLR, float volumeSub, float balance) {
    // set value to interal struct
    dsp.mixbusChannel[mixbusChannel-1].volumeLR = volumeLR;
    dsp.mixbusChannel[mixbusChannel-1].volumeSub = volumeSub;
    dsp.mixbusChannel[mixbusChannel-1].balance = balance;

    float balanceLeft = saturate(100.0f - balance, 0.0f, 100.0f) / 100.0f;
    float balanceRight = saturate(balance + 100.0f, 0.0f, 100.0f) / 100.0f;

    // send volume to DSP via SPI
    float values[4];
    values[0] = pow(10.0f, volumeLR/20.0f); // volume of this specific channel
    values[1] = balanceLeft; // 100 .. 100 ..  0
    values[2] = balanceRight; // 0  .. 100 .. 100
    values[3] = pow(10.0f, volumeSub/20.0f); // subwoofer

    spiSendDspParameterArray(0, 'v', mixbusChannel-1, 1, 4, &values[0]);
}

void dspSendMatrixVolume(uint8_t matrixChannel, float volume) {
    // set value to interal struct
    dsp.matrixChannel[matrixChannel-1].volume = volume;

    // send volume to DSP via SPI
    float values[1];
    values[0] = pow(10.0f, volume/20.0f); // volume of this specific channel

    spiSendDspParameterArray(0, 'v', matrixChannel-1, 2, 1, &values[0]);
}

void dspSendMonitorVolume(float volume) {
    // set value to interal struct
    dsp.monitorVolume = volume;

    // send volume to DSP via SPI
    float values[1];
    values[0] = pow(10.0f, volume/20.0f); // volume of this specific channel

    spiSendDspParameterArray(0, 'v', 0, 4, 1, &values[0]);
}

void dspSendMainVolume(float volumeLR, float volumeSub, float balance) {
    // set value to interal struct
    dsp.mainLRVolume = volumeLR;
    dsp.mainSubVolume = volumeSub;
    dsp.mainBalance = balance;

    float volumeLeft = (saturate(100.0f - balance, 0.0f, 100.0f) / 100.0f) * pow(10.0f, volumeLR/20.0f);
    float volumeRight = (saturate(balance + 100.0f, 0.0f, 100.0f) / 100.0f) * pow(10.0f, volumeLR/20.0f);

    // send volume to DSP via SPI
    float values[3];
    values[0] = volumeLeft;
    values[1] = volumeRight;
    values[2] = pow(10.0f, volumeSub/20.0f); // subwoofer

    spiSendDspParameterArray(0, 'v', 0, 3, 3, &values[0]);
}

void dspSendGate(uint8_t dspChannel) {
    fxRecalcGate(&dsp.dspChannel[dspChannel-1].gate);

    float values[5];
    values[0] = dsp.dspChannel[dspChannel-1].gate.value_threshold;
    values[1] = dsp.dspChannel[dspChannel-1].gate.value_gainmin;
    values[2] = dsp.dspChannel[dspChannel-1].gate.value_coeff_attack;
    values[3] = dsp.dspChannel[dspChannel-1].gate.value_hold_ticks;
    values[4] = dsp.dspChannel[dspChannel-1].gate.value_coeff_release;

    spiSendDspParameterArray(0, 'g', dspChannel-1, 0, 5, &values[0]);
}

void dspSendLowcut(uint8_t dspChannel) {
    float values[1];

    values[0] = 1.0f / (1.0f + 2.0f * M_PI * dsp.dspChannel[dspChannel-1].lowCutFrequency * (1.0f/dsp.samplerate));

    spiSendDspParameterArray(0, 'e', dspChannel-1, 0, 1, &values[0]);
}

/*
void dspSendHighcut(uint8_t dspChannel) {
    float values[1];

    values[0] = (2.0f * M_PI * dsp.dspChannel[dspChannel-1].highCutFrequency) / (dsp.samplerate + 2.0f * M_PI * 500.0f);

    spiSendDspParameterArray(0, 'e', dspChannel-1, 2, 1, &values[0]);
}
*/

void dspSendEQ(uint8_t dspChannel) {
    // biquad_trans() needs the coeffs in the following order
    // a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 0/1)
    // a0 a0 a1 a1 a2 a2 b1 b1 b2 b2 (section 2/3)
    // a0 a1 a2 b1 b2 (section 4)

    float values[MAX_CHAN_EQS * 5];

    for (int peq = 0; peq < MAX_CHAN_EQS; peq++) {
        fxRecalcFilterCoefficients_PEQ(&dsp.dspChannel[dspChannel-1].peq[peq]);

/*
        // send coeffiecients without interleaving for biquad() function
        int sectionIndex = peq * 5;
        values[sectionIndex + 0] = -dsp.dspChannel[dspChannel-1].peq[peq].b[2]; // -b2 (poles)
        values[sectionIndex + 1] = -dsp.dspChannel[dspChannel-1].peq[peq].b[1]; // -b1 (poles)
        values[sectionIndex + 2] = dsp.dspChannel[dspChannel-1].peq[peq].a[2]; // a2 (zeros)
        values[sectionIndex + 3] = dsp.dspChannel[dspChannel-1].peq[peq].a[1]; // a1 (zeros)
        values[sectionIndex + 4] = dsp.dspChannel[dspChannel-1].peq[peq].a[0]; // a0 (zeros)
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

    spiSendDspParameterArray(0, 'e', dspChannel-1, 1, MAX_CHAN_EQS * 5, &values[0]);
}


void dspSendCompressor(uint8_t dspChannel) {
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

void dspSendAll() {
    for (uint8_t dspChannel = 1; dspChannel <= 40; dspChannel++) {
        dspSendLowcut(dspChannel);
        dspSendGate(dspChannel);
        dspSendEQ(dspChannel);
        dspSendCompressor(dspChannel);
        dspSetRouting(dspChannel);
        dspSendChannelVolume(dspChannel, dsp.dspChannel[dspChannel-1].volumeLR, dsp.dspChannel[dspChannel-1].volumeSub, dsp.dspChannel[dspChannel-1].balance);
        for (uint8_t mixbusChannel = 1; mixbusChannel <= 16; mixbusChannel++) {
            dspSetChannelSendTapPoints(dspChannel, mixbusChannel, dsp.dspChannel[dspChannel-1].sendMixbusTapPoint[mixbusChannel-1]);
        }
    }
    for (uint8_t mixbusChannel = 1; mixbusChannel <= 16; mixbusChannel++) {
        dspSendMixbusVolume(mixbusChannel, dsp.mixbusChannel[mixbusChannel-1].volumeLR, dsp.mixbusChannel[mixbusChannel-1].volumeSub, dsp.mixbusChannel[mixbusChannel-1].balance);
        for (uint8_t matrixChannel = 1; matrixChannel <= 6; matrixChannel++) {
            dspSetMixbusSendTapPoints(mixbusChannel, matrixChannel, dsp.mixbusChannel[mixbusChannel-1].sendMatrixTapPoint[matrixChannel-1]);
        }
    }
    for (uint8_t matrixChannel = 1; matrixChannel <= 6; matrixChannel++) {
        dspSendMatrixVolume(matrixChannel, dsp.matrixChannel[matrixChannel-1].volume);
        dspSetMainSendTapPoints(matrixChannel, dsp.mainSendMatrixTapPoint[matrixChannel-1]);
    }
    dspSendMainVolume(dsp.mainLRVolume, dsp.mainSubVolume, dsp.mainBalance);
    dspSendMonitorVolume(dsp.monitorVolume);
}

void dspSetRouting(uint8_t dspOutputChannel) {
    uint32_t values[2];
    values[0] = dsp.outputSource[dspOutputChannel-1];
    values[1] = dsp.outputTapPoint[dspOutputChannel-1];
    spiSendDspParameterArray(0, 'r', dspOutputChannel-1, 0, 2, (float*)&values[0]);
}

void dspSetChannelSendTapPoints(uint8_t dspChannel, uint8_t mixbusChannel, uint8_t tapPoint) {
    dsp.dspChannel[dspChannel-1].sendMixbusTapPoint[mixbusChannel-1] = tapPoint;

    uint32_t values[2];
    values[0] = mixbusChannel;
    values[1] = tapPoint;
    spiSendDspParameterArray(0, 't', dspChannel-1, 0, 2, (float*)&values[0]);
}

void dspSetMixbusSendTapPoints(uint8_t mixbusChannel, uint8_t matrixChannel, uint8_t tapPoint) {
    dsp.mixbusChannel[mixbusChannel-1].sendMatrixTapPoint[matrixChannel-1] = tapPoint;

    uint32_t values[2];
    values[0] = matrixChannel;
    values[1] = tapPoint;
    spiSendDspParameterArray(0, 't', mixbusChannel-1, 1, 2, (float*)&values[0]);
}

void dspSetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint) {
    dsp.mainSendMatrixTapPoint[matrixChannel-1] = tapPoint;

    uint32_t values[2];
    values[0] = matrixChannel;
    values[1] = tapPoint;
    spiSendDspParameterArray(0, 't', 0, 2, 2, (float*)&values[0]);
}
