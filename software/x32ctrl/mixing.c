#include "mixing.h"

sOpenx32 openx32;

// set a default configuration for the audio-routing-matrix
void mixingDefaultRoutingConfig(void) {
    // XLR-input 1-16 to XLR-output
    for (uint8_t ch=1; ch<=16; ch++) {
        mixingSetRouting('x', ch, mixingGetInputSource('x', ch));
    }

    // XLR-input 1-16 to P16-output
    for (uint8_t ch=1; ch<=16; ch++) {
        mixingSetRouting('p', ch, mixingGetInputSource('x', ch));
    }

    // XLR-input 1-32 to Card-output
    for (uint8_t ch=1; ch<=32; ch++) {
        mixingSetRouting('c', ch, mixingGetInputSource('x', ch));
    }

    // XLR-input 1-8 to AUX-output
    for (uint8_t ch=1; ch<=8; ch++) {
        mixingSetRouting('a', ch, mixingGetInputSource('x', ch));
    }

    // XLR-input 1-32 and AUX-Input 1-8 to DSP-input
    for (uint8_t ch=1; ch<=32; ch++) {
        mixingSetRouting('d', ch, mixingGetInputSource('x', ch));
    }
    for (uint8_t ch=1; ch<=8; ch++) {
        mixingSetRouting('d', ch+32, mixingGetInputSource('a', ch));
    }

    // transmit routing-configuration to FPGA
    mixingTxRoutingConfig();
}

// set the outputs of the audio-routing-matrix to desired input-sources
void mixingSetRouting(uint8_t group, uint8_t channel, uint8_t inputsource) {
    // input-sources:
    // 1-32 = XLR-inputs
    // 33-64 = Card-inputs
    // 65-72 = AUX-inputs
    // 73-112 = DSP-outputs
    // 113-160 = AES50A-inputs
    // 161-208 = AES50B-inputs

    // output-taps
    // 1-16 = XLR-outputs
    // 17-32 = UltraNet/P16-outputs
    // 33-64 = Card-outputs
    // 65-72 = AUX-outputs
    // 73-112 = DSP-inputs
    // 113-160 = AES50A-outputs
    // 161-208 = AES50B-outputs

    switch (group) {
        case 'x': // XLR-Outputs 1-16
            openx32.routing.xlr[channel - 1] = inputsource - 1;
            break;
        case 'p': // P16-Outputs 1-16
            openx32.routing.p16[channel - 1] = inputsource - 1;
            break;
        case 'c': // Card-Outputs 1-32
            openx32.routing.card[channel - 1] = inputsource - 1;
            break;
        case 'a': // Aux-Outputs 1-8
            openx32.routing.aux[channel - 1] = inputsource - 1;
            break;
        case 'd': // DSP-Inputs 1-40
            openx32.routing.dsp[channel - 1] = inputsource - 1;
            break;
        case 0: // AES50A-Outputs
            //openx32.routing.aes50a[channel - 1] = inputsource - 1;
            break;
        case 1: // AES50B-Outputs
            //openx32.routing.aes50b[channel - 1] = inputsource - 1;
            break;
    }
}

// get the absolute input-source (global channel-number)
uint8_t mixingGetInputSource(uint8_t group, uint8_t channel) {
    // input-sources:
    // 1-32 = XLR-inputs
    // 33-64 = Card-inputs
    // 65-72 = AUX-inputs
    // 73-112 = DSP-outputs
    // 113-160 = AES50A-inputs
    // 161-208 = AES50B-inputs

    switch (group) {
        case 'x': // XLR-Inputs 1-32
            return channel;
            break;
        case 'c': // Card-Inputs 1-32
            return channel + 32;
            break;
        case 'a': // Aux-Inputs 1-8
            return channel + 64;
            break;
        case 'd': // DSP-Outputs 1-40
            return channel + 72;
            break;
        case 0: // AES50A-Inputs 1-48
            return channel + 112;
            break;
        case 1: // AES50B-Inputs 1-48
            return channel + 160;
            break;
    }

    return 0;
}

// set the gain of the local XLR head-amp-control
void mixingSetGain(uint8_t group, uint8_t channel, float gain) {
    switch (group) {
        case 'x': // local XLR-inputs
            // set value to internal struct
            openx32.preamps.gainXlr[channel - 1] = gain;

            // now send value to adda-board
            uint8_t boardId = addaGetBoardId(channel);
            uint8_t addaChannel = channel;
            while (addaChannel > 8) {
                addaChannel -= 8;
            }
            addaSetGain(boardId, addaChannel, openx32.preamps.gainXlr[channel - 1], openx32.preamps.phantomPowerXlr[channel - 1]);
            break;
        case 0: // AES50A
            openx32.preamps.gainAes50a[channel - 1] = gain;
            break;
        case 1: // AES50B
            openx32.preamps.gainAes50b[channel - 1] = gain;
            break;
    }
};


// enable or disable phatom-power of local XLR-inputs
void mixingSetPhantomPower(uint8_t group, uint8_t channel, bool active) {
    switch (group) {
        case 'x': // local XLR-inputs
            // set value to internal struct
            openx32.preamps.phantomPowerXlr[channel - 1] = active;

            // now send value to adda-board
            uint8_t boardId = addaGetBoardId(channel);
            uint8_t addaChannel = channel;
            while (addaChannel > 8) {
               addaChannel -= 8;
            }
            addaSetGain(boardId, addaChannel, openx32.preamps.gainXlr[channel - 1], openx32.preamps.phantomPowerXlr[channel - 1]);
            break;
        case 0: // AES50A
            openx32.preamps.phantomPowerAes50a[channel - 1] = active;
            break;
        case 1: // AES50B
            openx32.preamps.phantomPowerAes50b[channel - 1] = active;
            break;
    }
};

// set the general volume of one of the 40 DSP-channels
void mixingSetVolume(uint8_t channel, float volume) {
    // set value to interal struct
    openx32.dspChannel[channel-1].volume = volume;

    // send new volume to FPGA (later the DSP)
    data_64b fpga_data;
    float volume_left = ((float)pow(10, openx32.dspChannel[channel-1].volume/20.0f) * 128.0f) * limitMax((100 - openx32.dspChannel[channel-1].balance) * 2, 100) / 100.0f;
    float volume_right = ((float)pow(10, openx32.dspChannel[channel-1].volume/20.0f) * 128.0f) * limitMax(openx32.dspChannel[channel-1].balance * 2, 100) / 100.0f;

    fpga_data.u8[0] = trunc(volume_left);
    fpga_data.u8[1] = trunc(volume_right);
    uartTxToFPGA(FPGA_IDX_CH_VOL + (channel-1), &fpga_data);
}

// set the balance of one of the 40 DSP-channels
void mixingSetBalance(uint8_t channel, uint8_t balance) {
    // set value to interal struct
    openx32.dspChannel[channel-1].balance = balance;

    // now update the volumes based on this balance
    mixingSetVolume(channel, openx32.dspChannel[channel-1].volume);
}

// helper-function to send the audio-routing to the fpga
void mixingTxRoutingConfig(void) {
    data_64b routingData;

    // copy routing-struct into array
    uint8_t buf[sizeof(openx32.routing)];
    memcpy(buf, &openx32.routing, sizeof(openx32.routing));

    // now copy this array into chunks of 64-bit-data and transmit it to FPGA
    for (uint8_t i = 0; i < (NUM_INPUT_CHANNEL/8); i++) {
        // copy 8 bytes from routing-data
        memcpy(&routingData.u8[0], &buf[i * 8], 8);

        // now send data to FPGA
        uartTxToFPGA(FPGA_IDX_ROUTING + (i * 8), &routingData);
    }
}
