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
    fx = new FX(basepar);
};

void DSP1::dspInit(void) {

    MainChannelLR.volume = VOLUME_MIN; // dB
    MainChannelLR.balance = 0; // -100 .. 0 .. +100
    MainChannelSub.volume = VOLUME_MIN; // dB
    MainChannelSub.balance = 0; // -100 .. 0 .. +100

    for (uint8_t i = 0; i < 40; i++) {
        Channel[i].lowCutFrequency = 100.0f; // Hz

        Channel[i].gate.threshold = -80; // dB -> no gate
        Channel[i].gate.range = 60; // dB -> full range
        Channel[i].gate.attackTime_ms = 10;
        Channel[i].gate.holdTime_ms = 50;
        Channel[i].gate.releaseTime_ms = 250;

        Channel[i].compressor.threshold = 0; // dB -> no compression
        Channel[i].compressor.ratio = 1.0f/3.0f; // 1:3
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
        }

        monitorVolume = 0; // dBfs
        monitorTapPoint = DSP_TAP_INPUT;

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
        Channel[i].inputSource = DSP_BUF_IDX_DSPCHANNEL + i; // 0=OFF, 1..32=DSP-Channel, 33..40=Aux, 41..56=Mixbus, 57..62=Matrix, 63=MainL, 64=MainR, 65=MainSub, 66..68=MonL,MonR,Talkback
        Channel[i].inputTapPoint = DSP_TAP_INPUT;

        // Volumes, Balance and Mute/Solo is setup in mixerInit()
    }

    for (uint8_t i = 0; i < 16; i++) {
        Dsp2FxChannel[i].outputSource = DSP_BUF_IDX_MIXBUS; // connect all 16 mixbus-channels to DSP2
        Dsp2FxChannel[i].outputTapPoint = DSP_TAP_POST_FADER;
    }
    for (uint8_t i = 0; i < 8; i++) {
        Dsp2AuxChannel[i].outputSource = DSP_BUF_IDX_DSPCHANNEL; // connect inputs 1-8 to DSP2 Aux-Channels 1-8
        Dsp2AuxChannel[i].outputTapPoint = DSP_TAP_POST_FADER;
    }
    for (uint8_t i = 0; i < 40; i++) {
        // connect MainLeft on even and MainRight on odd channels as PostFader
        Dsp2FpgaChannel[i].outputSource = DSP_BUF_IDX_MAINLEFT + (i % 2); // 0=OFF, 1..32=DSP-Channel, 33..40=Aux, 41..56=Mixbus, 57..62=Matrix, 63=MainL, 64=MainR, 65=MainSub, 66..68=MonL,MonR,Talkback, 69..84=FX-Return, 85..92=DSP2AUX
        Dsp2FpgaChannel[i].outputTapPoint = DSP_TAP_POST_FADER;
    }
}

void DSP1::Tick10ms(void) {
    spi->Tick10ms();

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

void DSP1::Tick100ms(void) {
    spi->Tick100ms();
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

    spi->SendDspParameterArray(0, 'v', chan, 0, 4, &values[0]);
}

// send BusSends
void DSP1::SendChannelSend(uint8_t chan) {
    float values[16];

    for (uint8_t i_mixbus = 0; i_mixbus < 16; i_mixbus++) {
        values[i_mixbus] = pow(10.0f, Channel[chan].sendMixbus[i_mixbus]/20.0f); // volume of this specific channel
    }

    spi->SendDspParameterArray(0, 's', chan, 0, 16, &values[0]);
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

    spi->SendDspParameterArray(0, 'v', bus, 1, 4, &values[0]);
}

void DSP1::SendMatrixVolume(uint8_t matrix) {
    // send volume to DSP via spi->
    float values[1];

    values[0] = pow(10.0f, Matrix[matrix].volume/20.0f); // volume of this specific channel

    spi->SendDspParameterArray(0, 'v', matrix, 2, 1, &values[0]);
}

void DSP1::SendMonitorVolume() {
    // send volume to DSP via spi
    float values[1];
    
    values[0] = pow(10.0f, monitorVolume/20.0f); // volume of this specific channel

    spi->SendDspParameterArray(0, 'v', 0, 4, 1, &values[0]);
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

    spi->SendDspParameterArray(0, 'v', 0, 3, 3, &values[0]);
}

void DSP1::SendGate(uint8_t chan) {
    fx->RecalcGate(&Channel[chan].gate);

    float values[5];
    values[0] = Channel[chan].gate.value_threshold;
    values[1] = Channel[chan].gate.value_gainmin;
    values[2] = Channel[chan].gate.value_coeff_attack;
    values[3] = Channel[chan].gate.value_hold_ticks;
    values[4] = Channel[chan].gate.value_coeff_release;

    spi->SendDspParameterArray(0, 'g', chan, 0, 5, &values[0]);
}

void DSP1::SendLowcut(uint8_t chan) {
    float values[1];

    values[0] = 1.0f / (1.0f + 2.0f * M_PI * Channel[chan].lowCutFrequency * (1.0f/config->GetSamplerate()));

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

    spi->SendDspParameterArray(0, 'e', chan, 'e', MAX_CHAN_EQS * 5, &values[0]);
}

void DSP1::ResetEq(uint8_t chan) {
    float values[1];
    values[0] = 0;
    spi->SendDspParameterArray(0, 'e', chan, 'r', 1, &values[0]);
}

void DSP1::Reset() {
    float values[1];
    values[0] = 0;
    spi->SendDspParameterArray(0, 'a', 0, 'r', 1, &values[0]);
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

    spi->SendDspParameterArray(0, 'c', chan, 0, 6, &values[0]);
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
    // configuration for 16 FX sends to DSP2
    for (uint8_t fxchan = 0; fxchan <= 15; fxchan++) {
        SetFxOutputRouting(fxchan);
    }
    // configuration for 8 AUX-sends to DSP2
    for (uint8_t auxchan = 0; auxchan <= 7; auxchan++) {
        SetAuxOutputRouting(auxchan);
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
    values[0] = Channel[chan].inputSource;
    values[1] = Channel[chan].inputTapPoint;
    spi->SendDspParameterArray(0, 'r', chan, 0, 2, (float*)&values[0]);
}

void DSP1::SetOutputRouting(uint8_t chan) {
    uint32_t values[2];
    values[0] = Dsp2FpgaChannel[chan].outputSource;
    values[1] = Dsp2FpgaChannel[chan].outputTapPoint;
    spi->SendDspParameterArray(0, 'r', chan, 1, 2, (float*)&values[0]);
}

void DSP1::SetFxOutputRouting(uint8_t fxchan) {
    uint32_t values[2];
    values[0] = Dsp2FxChannel[fxchan].outputSource;
    values[1] = Dsp2FxChannel[fxchan].outputTapPoint;
    spi->SendDspParameterArray(0, 'r', (MAX_DSP_OUTPUTCHANNELS + fxchan), 1, 2, (float*)&values[0]);
}

void DSP1::SetAuxOutputRouting(uint8_t auxchan) {
    uint32_t values[2];
    values[0] = Dsp2AuxChannel[auxchan].outputSource;
    values[1] = Dsp2AuxChannel[auxchan].outputTapPoint;
    spi->SendDspParameterArray(0, 'r', (MAX_DSP_OUTPUTCHANNELS + MAX_DSP_FXCHANNELS + auxchan), 1, 2, (float*)&values[0]);
}

void DSP1::SetChannelSendTapPoints(uint8_t chan, uint8_t mixbusChannel, uint8_t tapPoint) {
    Channel[chan].sendMixbusTapPoint[mixbusChannel] = tapPoint;

    uint32_t values[2];
    values[0] = mixbusChannel;
    values[1] = tapPoint;
    spi->SendDspParameterArray(0, 't', chan, 0, 2, (float*)&values[0]);
}

void DSP1::SetMixbusSendTapPoints(uint8_t mixbusChannel, uint8_t matrixChannel, uint8_t tapPoint) {
    this->Bus[mixbusChannel].sendMatrixTapPoint[matrixChannel] = tapPoint;

    uint32_t values[2];
    values[0] = matrixChannel;
    values[1] = tapPoint;
    spi->SendDspParameterArray(0, 't', mixbusChannel, 1, 2, (float*)&values[0]);
}

void DSP1::SetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint) {
    MainChannelLR.sendMatrixTapPoint[matrixChannel] = tapPoint;

    uint32_t values[2];
    values[0] = matrixChannel;
    values[1] = tapPoint;
    spi->SendDspParameterArray(0, 't', 0, 2, 2, (float*)&values[0]);
}

void DSP1::GetSourceName(char* p_nameBuffer, uint8_t dspChannel, uint8_t dspInputSource) {
    if (dspChannel < 40) {
        // we have a DSP-channel
        uint8_t channelInputSource = Channel[dspChannel].inputSource;

        // check if we are using an external signal (possibly with gain) or DSP-internal (no gain)
        if (channelInputSource == 0) {
            // OFF
            sprintf(p_nameBuffer, "Off");
        }else if ((channelInputSource >= 1) && (channelInputSource <= 40)) {
            // we are connected to one of the DSP-inputs
            //uint8_t dspInputSource = mixer->fpga->Routing.dsp[channelInputSource - 1];
            if (dspInputSource == 0) {
                sprintf(p_nameBuffer, "Off");
            }else if ((dspInputSource >= 1) && (dspInputSource <= 32)) {
                // XLR-input
                sprintf(p_nameBuffer, "XLR%02d", dspInputSource);
            }else if ((dspInputSource >= 33) && (dspInputSource <= 64)) {
                // Card input
                sprintf(p_nameBuffer, "C%02d", dspInputSource - 32);
            }else if ((dspInputSource >= 65) && (dspInputSource <= 72)) {
                // Aux input
                sprintf(p_nameBuffer, "Aux%02d", dspInputSource - 64);
            }else if ((dspInputSource >= 73) && (dspInputSource <= 112)) {
                // DSP input
                sprintf(p_nameBuffer, "In%02d", dspInputSource - 72);
            }else if ((dspInputSource >= 113) && (dspInputSource <= 160)) {
                // AES50A input
                sprintf(p_nameBuffer, "A%02d", dspInputSource - 112);
            }else if ((dspInputSource >= 161) && (dspInputSource <= 208)) {
                // AES50B input
                sprintf(p_nameBuffer, "B%02d", dspInputSource - 160);
            }else{
                sprintf(p_nameBuffer, "???");
            }
        }else if ((channelInputSource >= 41) && (channelInputSource <= 56)) {
            // Mixbus 1-16
            sprintf(p_nameBuffer, "Bus %02d", channelInputSource - 40);
        }else if ((channelInputSource >= 57) && (channelInputSource <= 62)) {
            // Matrix 1-6
            sprintf(p_nameBuffer, "Mtx %02d", channelInputSource - 56);
        }else if (channelInputSource == 63) {
            sprintf(p_nameBuffer, "Main L");
        }else if (channelInputSource == 64) {
            sprintf(p_nameBuffer, "Main R");
        }else if (channelInputSource == 65) {
            sprintf(p_nameBuffer, "Main C");
        }else if (channelInputSource == 66) {
            sprintf(p_nameBuffer, "Mon L");
        }else if (channelInputSource == 67) {
            sprintf(p_nameBuffer, "Mon R");
        }else if (channelInputSource == 68) {
            sprintf(p_nameBuffer, "Talkback");
        }else{
            sprintf(p_nameBuffer, "???");
        }
    }else{
        // we have a non-DSP-channel -> no source available
        sprintf(p_nameBuffer, "<Intern>");
    }
}

void DSP1::UpdateVuMeter() {
	// leds Main   = 32-bit bitwise (bit 0=-57dB ... 22=-2, 23=-1, 24=Clip)

	float data[3];
	data[0] = MainChannelLR.meterPu[0];
	data[1] = MainChannelLR.meterPu[1];
	data[2] = MainChannelSub.meterPu[0];

	// Step 1: Perform Peak Hold Logic for MainL/R and Sub
	uint8_t currentMeterPeakIndexMain[3]; // L, R, Sub
	for (int k = 0; k < 3; k++) {
		currentMeterPeakIndexMain[k] = 0;
		for (int i = 0; i < 24; i++) {
			if (data[k] >= vuThresholdsPu[i]) {
				currentMeterPeakIndexMain[k] = (23 - i) + 1;
				break;
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
	if (MainChannelLR.meterPu[0] > MainChannelLR.meterDecay[0]) {
		// current value is above stored decay-value -> copy value immediatly
		MainChannelLR.meterDecay[0] = MainChannelLR.meterPu[0];
	}else{
		// current value is below -> afterglow
		MainChannelLR.meterDecay[0] -= (MainChannelLR.meterDecay[0] / 20.0f);
	}
	// MainRight
	if (MainChannelLR.meterPu[1] > MainChannelLR.meterDecay[1]) {
		// current value is above stored decay-value -> copy value immediatly
		MainChannelLR.meterDecay[1] = MainChannelLR.meterPu[1];
	}else{
		// current value is below -> afterglow
		MainChannelLR.meterDecay[1] -= (MainChannelLR.meterDecay[1] / 20.0f);
	}
	// MainSub
	if (MainChannelSub.meterPu[0] > MainChannelSub.meterDecay[0]) {
		// current value is above stored decay-value -> copy value immediatly
		MainChannelSub.meterDecay[0] = MainChannelSub.meterPu[0];
	}else{
		// current value is below -> afterglow
		MainChannelSub.meterDecay[0] -= (MainChannelSub.meterDecay[0] / 20.0f);
	}

	// Step 3: Calculate real LEDs to switch on
	MainChannelLR.meterInfo[0] = 0;
	MainChannelLR.meterInfo[1] = 0;
	MainChannelSub.meterInfo[0] = 0;
	for (int i = 0; i < 24; i++) {
		if (MainChannelLR.meterDecay[0] >= vuThresholdsPu[i]) { MainChannelLR.meterInfo[0]  |= (1U << (23 - i)); }
		if (MainChannelLR.meterDecay[1] >= vuThresholdsPu[i]) { MainChannelLR.meterInfo[1]  |= (1U << (23 - i)); }
		if (MainChannelSub.meterDecay[0] >= vuThresholdsPu[i]) { MainChannelSub.meterInfo[0] |= (1U << (23 - i)); }
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

		// Step 1: Perform Peak Hold Logic
		uint8_t currentMeterPeakIndex;
		if (rChannel[i].meterPu >= vuThresholdsPu[0]) {currentMeterPeakIndex = 6;}        // CLIP
		else if (rChannel[i].meterPu >= vuThresholdsPu[5]) {currentMeterPeakIndex = 5;}   // -6dBfs
		else if (rChannel[i].meterPu >= vuThresholdsPu[8]) {currentMeterPeakIndex = 4;}   // -12dBfs
		else if (rChannel[i].meterPu >= vuThresholdsPu[10]) {currentMeterPeakIndex = 3;}  // -18dBfs
		else if (rChannel[i].meterPu >= vuThresholdsPu[14]) {currentMeterPeakIndex = 2;}  // -30dBfs
		else if (rChannel[i].meterPu >= vuThresholdsPu[24]) {currentMeterPeakIndex = 1;}  // -60dBfs
		else {currentMeterPeakIndex = 0;} // below -60dBfs

		if (currentMeterPeakIndex >= rChannel[i].meterPeakIndex) {
			// currentMeterPeakIndex is above current LED -> set peakHold LED to highest value
			rChannel[i].meterPeakIndex = currentMeterPeakIndex;
			rChannel[i].meterPeakHoldTimer = 100; // preload to 1000ms
		}else{
			// currentMeterPeakIndex is below current LED -> check if we have to hold the peak LED
			if (rChannel[i].meterPeakHoldTimer > 0) {
				// hold current LED
				rChannel[i].meterPeakHoldTimer--;
				rChannel[i].meterPeakDecayTimer = 10; // preload
			}else{
				// let peak LED fall down every 100ms. It takes a maximum of 400ms to let the peak fall down
				if (rChannel[i].meterPeakIndex > currentMeterPeakIndex) {
					if (rChannel[i].meterPeakDecayTimer > 0) {
						rChannel[i].meterPeakDecayTimer--;
					}else{
						rChannel[i].meterPeakIndex--;
						rChannel[i].meterPeakDecayTimer = 10; // preload for next iteration
					}
				}
			}
		}

		// Step 2: Calculate decayed value
		if (rChannel[i].meterPu > rChannel[i].meterDecay) {
			// current value is above stored decay-value -> copy value immediatly
			rChannel[i].meterDecay = rChannel[i].meterPu;
		}else{
			// current value is below -> afterglow
			// this function is called every 10ms. A Decay-Rate of 6dB/second would be ideal, but we do a rought estimation here
			rChannel[i].meterDecay -= (rChannel[i].meterDecay / 20.0f);
		}

		// Step 3: Calculate real LEDs to switch on
		// data contains a 32-bit sample-value
		// lets check the threshold and set meterInfo
		rChannel[i].meterInfo = 0;
		if (rChannel[i].meterDecay >= vuThresholdsPu[0])  { rChannel[i].meterInfo |= 0b00100000; } // CLIP
		if (rChannel[i].meterDecay >= vuThresholdsPu[5])  { rChannel[i].meterInfo |= 0b00010000; } // -6dBfs
		if (rChannel[i].meterDecay >= vuThresholdsPu[8])  { rChannel[i].meterInfo |= 0b00001000; } // -12dBfs
		if (rChannel[i].meterDecay >= vuThresholdsPu[10]) { rChannel[i].meterInfo |= 0b00000100; } // -18dBfs
		if (rChannel[i].meterDecay >= vuThresholdsPu[14]) { rChannel[i].meterInfo |= 0b00000010; } // -30dBfs
		if (rChannel[i].meterDecay >= vuThresholdsPu[24]) { rChannel[i].meterInfo |= 0b00000001; } // -60dBfs

		uint8_t peakBit = 0;
		switch (rChannel[i].meterPeakIndex) {
			case 6: peakBit = 0b00100000; break; // CLIP
			case 5: peakBit = 0b00010000; break; // -6dBfs
			case 4: peakBit = 0b00001000; break; // -12dBfs
			case 3: peakBit = 0b00000100; break; // -18dBfs
			case 2: peakBit = 0b00000010; break; // -30dBfs
			case 1: peakBit = 0b00000001; break; // -60dBfs
			default: peakBit = 0; break;
		}
		rChannel[i].meterInfo |= peakBit;

		// the dynamic-information is received with the 'd' information, but we will store them here
		if (Channel[i].gate.gain < 1.0f) { rChannel[i].meterInfo |= 0b01000000; }
		if (Channel[i].compressor.gain < 1.0f) { rChannel[i].meterInfo |= 0b10000000; }

		//Channel[i].compressor.gain = floatValues[45 + i];
		//Channel[i].gate.gain = floatValues[85 + i];
	}
}

void DSP1::callbackDsp1(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values) {
    float* floatValues = (float*)values;
    uint32_t* intValues = (uint32_t*)values;

    helper->DEBUG_DSP1(DEBUGLEVEL_TRACE, "Callback - classid=%c channel=%c, index=%d, valueCount=%d", classId, channel, index, valueCount);

    switch (classId) {
        case 's': // status-feedback
            switch (channel) {
                case 'u': // Update pack
                    if (valueCount == 45) {
                        state->dspVersion[0] = floatValues[0];
                        state->dspLoad[0] = (((float)intValues[1]/264.0f) / (16.0f/0.048f)) * 100.0f;

                        // copy meter-info to channel-struct
                        MainChannelLR.meterPu[0] = abs(floatValues[2])/2147483648.0f; // convert 32-bit audio-value to absolute p.u.
                        MainChannelLR.meterPu[1] = abs(floatValues[3])/2147483648.0f; // convert 32-bit audio-value to absolute p.u.
                        MainChannelSub.meterPu[0] = abs(floatValues[4])/2147483648.0f; // convert 32-bit audio-value to absolute p.u.
                        for (int i = 0; i < 40; i++) {
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

    helper->DEBUG_DSP2(DEBUGLEVEL_TRACE, "Callback - classid=%c channel=%c, index=%d, valueCount=%d", classId, channel, index, valueCount);

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
