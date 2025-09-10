#include "hal.h"
#include "uart.h"
#include "adda.h"
#include "mixer.h"

// set the gain of the local XLR head-amp-control
void halSetGain(uint8_t group, uint8_t channel, float gain) {
    uint8_t boardId;
    uint8_t addaChannel;

    switch (group) {
        case 'x': // local XLR-inputs
            // set value to internal struct
            openx32.preamps.gainXlr[channel - 1] = gain;

            // now send value to adda-board
            boardId = addaGetBoardId(channel);
            addaChannel = channel;
            while (addaChannel > 8) {
                addaChannel -= 8;
            }
            addaSetGain(boardId, addaChannel, openx32.preamps.gainXlr[channel - 1], openx32.preamps.phantomPowerXlr[channel - 1]);
            break;
        case 'A': // AES50A
            openx32.preamps.gainAes50a[channel - 1] = gain;
            break;
        case 'B': // AES50B
            openx32.preamps.gainAes50b[channel - 1] = gain;
            break;
    }
}


// enable or disable phatom-power of local XLR-inputs
void halSetPhantomPower(uint8_t group, uint8_t channel, bool active) {
    uint8_t boardId = 0;
    uint8_t addaChannel = 0;

    switch (group) {
        case 'x': // local XLR-inputs
            // set value to internal struct
            openx32.preamps.phantomPowerXlr[channel - 1] = active;

            // now send value to adda-board
            boardId = addaGetBoardId(channel);
            addaChannel = channel;
            while (addaChannel > 8) {
               addaChannel -= 8;
            }
            addaSetGain(boardId, addaChannel, openx32.preamps.gainXlr[channel - 1], openx32.preamps.phantomPowerXlr[channel - 1]);
            break;
        case 'A': // AES50A
            openx32.preamps.phantomPowerAes50a[channel - 1] = active;
            break;
        case 'B': // AES50B
            openx32.preamps.phantomPowerAes50b[channel - 1] = active;
            break;
    }
}

// set the general volume of one of the 40 DSP-channels
void halSetVolume(uint8_t channel, float volume) {
    // set value to interal struct
    openx32.dspChannel[channel-1].volume = volume;

/*
    // send new volume to FPGA (later the DSP)
    data_64b fpga_data;
    float volume_left = ((float)pow(10, openx32.dspChannel[channel-1].volume/20.0f) * 128.0f) * limitMax((100 - openx32.dspChannel[channel-1].balance) * 2, 100) / 100.0f;
    float volume_right = ((float)pow(10, openx32.dspChannel[channel-1].volume/20.0f) * 128.0f) * limitMax(openx32.dspChannel[channel-1].balance * 2, 100) / 100.0f;

    fpga_data.u8[0] = trunc(volume_left);
    fpga_data.u8[1] = trunc(volume_right);
    uartTxToFPGA(FPGA_IDX_CH_VOL + (channel-1), &fpga_data);
*/
    //TODO: send volume to DSP via SPI
}

// set the balance of one of the 40 DSP-channels
void halSetBalance(uint8_t channel, uint8_t balance) {
    // set value to interal struct
    openx32.dspChannel[channel-1].balance = balance;

    // now update the volumes based on this balance
//    halSetVolume(channel, openx32.dspChannel[channel-1].volume);
    //TODO: send balance to DSP via SPI
}
