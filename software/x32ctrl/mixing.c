#include "mixing.h"

sOpenx32 openx32;

void mixingDefaultConfig(void) {
    for (uint8_t ch=1; ch<=16; ch++) {
        mixingSetRouting('x', ch, ch); // XLR-input 1-16 to XLR-output
    }
    for (uint8_t ch=1; ch<=8; ch++) {
        mixingSetRouting('a', ch, ch); // XLR-input 1-8 to AUX-output
    }
    for (uint8_t ch=1; ch<=32; ch++) {
        mixingSetRouting('c', ch, ch); // XLR-input 1-32 to Card-output
    }
    for (uint8_t ch=1; ch<=16; ch++) {
        mixingSetRouting('p', ch, ch); // XLR-input 1-16 to P16-output
    }
    for (uint8_t ch=1; ch<=40; ch++) {
        mixingSetRouting('d', ch, ch); // XLR-input 1-32 + 8 AUX-Ins to DSP-input
    }
}

void mixingSetRouting(uint8_t group, uint8_t dst, uint8_t src) {
    // sources:
    // 1-32 = XLR-inputs
    // 33-40 = AUX-inputs
    // 41-72 = Card-inputs
    // 73-120 = AES50A-inputs (planned, but not implemented)
    // 121-168 = AES50B-inputs (planned, but not implemented)

    switch (group) {
        case 'x': // XLR-Outputs
            openx32.routing.xlrOutput[dst-1] = src-1;
            break;
        case 'a': // Aux-Outputs
            openx32.routing.auxOutput[dst-1] = src-1;
            break;
        case 'c': // Card-Outputs
            openx32.routing.cardOutput[dst-1] = src-1;
            break;
        case 'p': // P16-Outputs
            openx32.routing.p16Output[dst-1] = src-1;
            break;
        case 'd': // DSP-Inputs
            openx32.routing.dspInput[dst-1] = src-1;
            break;
/*
        case 0: // AES50AOutputs
            openx32.routing.aes50aOutput[dst-1] = src-1;
            break;
        case 1: // AES50B-Outputs
            openx32.routing.aes50bOutput[dst-1] = src-1;
            break;
*/
    }

    // send new routing-configuration to FPGA
    mixingTxRoutingConfig();
}

void mixingSetGain(uint8_t channel, float gain) {
    // set value to internal struct
    openx32.gain[channel-1] = gain;

    // now send value to adda-board
    uint8_t boardId = addaGetBoardId(channel);
    uint8_t localChannel = channel;
    while (localChannel > 8) {
        localChannel -= 8;
    }
    addaSetGain(boardId, localChannel, openx32.gain[channel-1] = gain, openx32.phantomPower[channel-1]);
};

void mixingSetPhantomPower(uint8_t channel, bool active) {
    // set value to internal struct
    openx32.phantomPower[channel-1] = active;

    // now send value to adda-board
    uint8_t boardId = addaGetBoardId(channel);
    uint8_t localChannel = channel;
    while (localChannel > 8) {
        localChannel -= 8;
    }
    addaSetGain(boardId, localChannel, openx32.gain[channel-1], openx32.phantomPower[channel-1]);
};

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

void mixingSetBalance(uint8_t channel, uint8_t balance) {
    // set value to interal struct
    openx32.dspChannel[channel-1].balance = balance;

    // now update the volumes based on this balance
    mixingSetVolume(channel, openx32.dspChannel[channel-1].volume);
}

void mixingTxRoutingConfig(void) {
    data_64b routingData;

    // copy routing-struct into array
    uint8_t buf[sizeof(openx32.routing)];
    memcpy(buf, &openx32.routing, sizeof(openx32.routing));

	addaSetMute(true);
    // now copy this array into chunks of 64-bit-data and transmit it to FPGA
    for (uint8_t i = 0; i < 14; i++) {
        // copy 8 bytes from routing-data
        memcpy(&routingData.u8[0], &buf[i * 8], 8);

        // now send data to FPGA
        uartTxToFPGA(FPGA_IDX_ROUTING + (i * 8), &routingData);
    }
	addaSetMute(false);
}
