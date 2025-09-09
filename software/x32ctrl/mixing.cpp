#include "mixing.h"
#include "uart.h"
#include "adda.h"
#include "mixer.h"

sOpenx32 openx32;

void mixingInit(void) {
    // reset routing-configuration and dsp-configuration
    mixingDefaultRoutingConfig();
}

// set a default configuration for the audio-routing-matrix
void mixingDefaultRoutingConfig(void) {
    // XLR-inputs 1-32 to DSP-inputs 1-32
    for (uint8_t ch=1; ch<=32; ch++) {
        mixingSetRouting('d', ch, mixingGetInputSource('x', ch));
    }
    // AUX-inputs 1-8 to DSP-inputs 33-40
    for (uint8_t ch=1; ch<=8; ch++) {
        mixingSetRouting('d', ch + 32, mixingGetInputSource('a', ch));
    }


    // DSP-outputs 1-16 to XLR-outputs 1-16
    for (uint8_t ch=1; ch<=16; ch++) {
        mixingSetRouting('x', ch, mixingGetInputSource('d', ch));
    }
    // DSP-outputs 17-32 to P16-output 1-16
    for (uint8_t ch=1; ch<=16; ch++) {
        mixingSetRouting('p', ch, mixingGetInputSource('d', ch + 16));
    }
    // DSP-outputs 33-40 to AUX-outputs 1-6 + ControlRoom L/R
    for (uint8_t ch=1; ch<=8; ch++) {
        mixingSetRouting('a', ch, mixingGetInputSource('d', ch + 32));
    }


    // XLR-inputs 1-32 to Card-outputs 1-32
    for (uint8_t ch=1; ch<=32; ch++) {
        mixingSetRouting('c', ch, mixingGetInputSource('x', ch));
    }


    // transmit routing-configuration to FPGA
    mixingTxRoutingConfig();
}

// "compiles" the current mixer state into mixing routing
void mixingSyncRoutingConfigFromMixer(void) {
    if (
        mixerHasChanged(X32_MIXER_CHANGED_ROUTING) ||
        mixerHasChanged(X32_MIXER_CHANGED_VCHANNEL)
        ){

        // loop trough all vChannels
        for (int i = 0; i < MAX_VCHANNELS; i++)
        {
            //x32debug("Mixer sync: vChannel%d - %-30s - ", i, mixer.vChannel[i].name); 

            if((mixer.vChannel[i].outputDestination.hardwareChannel == 0)||
                (mixer.vChannel[i].inputSource.hardwareChannel == 0))
                {
                    //x32debug("no routing\n");
                    continue;
                } else {

                    // TODO: routing within the FPGA is not the same as mixer-channels!
                    // Each input (XLR, AUX, AES50, DSP) can be routed to each output (XLR, AUX, AES50, DSP)
                    // but each vChannel belongs to a dedicated DSP-channel. So we should change vChannel to dspChannel
                    // and separate dspChannel-routing from channel routing

                    // vChannel inputs can only be selected from one of the 40 channels routed to the DSP
/*
                    mixingSetRouting(
                        mixer.vChannel[i].outputDestination.hardwareGroup,
                        mixer.vChannel[i].outputDestination.hardwareChannel,
                        mixingGetInputSource(
                            mixer.vChannel[i].inputSource.hardwareGroup,
                            mixer.vChannel[i].inputSource.hardwareChannel
                        )
                    );
*/

            // x32debug(" input(%c,%d) -> output(%c,%d)\n", 
            //         mixer.vChannel[i].inputSource.hardwareGroup,
            //         mixer.vChannel[i].inputSource.hardwareChannel,
            //             mixer.vChannel[i].outputDestination.hardwareGroup,
            //             mixer.vChannel[i].outputDestination.hardwareChannel
            //         );
                }
        }
        // transmit routing-configuration to FPGA
//        mixingTxRoutingConfig();

        x32debug("Mixer routing to hardware synced\n");
    }
}

void mixingSyncVChannelConfigFromMixer(void){
    // loop trough all vChannels
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        if(mixer.vChannel[i].inputSource.hardwareChannel == 0){
            continue;
        } else {

            s_vChannel* chan = &mixer.vChannel[i];

            if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_GAIN)){
                mixingSetGain(chan->inputSource.hardwareGroup, chan->inputSource.hardwareChannel, chan->inputSource.gain);
            }

            if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_PHANTOM)){
                mixingSetPhantomPower(chan->inputSource.hardwareGroup, chan->inputSource.hardwareChannel, chan->inputSource.phantomPower);
            }

            // TODO: Volume (a bit complicated)
        }
    }

    x32debug("Mixer gain to hardware synced\n");
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

    if (channel == 0){
        return;
    }

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

    if (channel == 0){
        return 0;
    }

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


// get the input-source name
void mixingGetInputName(char* p_nameBuffer, uint8_t group, uint8_t channel) {
    // input-sources:
    // 1-32 = XLR-inputs
    // 33-64 = Card-inputs
    // 65-72 = AUX-inputs
    // 73-112 = DSP-outputs
    // 113-160 = AES50A-inputs
    // 161-208 = AES50B-inputs

    if (channel == 0){
        sprintf(p_nameBuffer, "Off");
        return;
    }

    switch (group) {
        case 'x': // XLR-Inputs 1-32
            sprintf(p_nameBuffer, "XLR%d", channel);
            break;
        case 'c': // Card-Inputs 1-32
            sprintf(p_nameBuffer, "Card%d", channel);
            break;
        case 'a': // Aux-Inputs 1-8
            sprintf(p_nameBuffer, "AUX%d", channel);
            break;
        case 'd': // DSP-Outputs 1-40
            sprintf(p_nameBuffer, "DSP%d", channel);
            break;
        case 0: // AES50A-Inputs 1-48
            sprintf(p_nameBuffer, "AESA%d", channel);
            break;
        case 1: // AES50B-Inputs 1-48
            sprintf(p_nameBuffer, "AESB%d", channel);
            break;
    }
}

// get the output destination name name
void mixingGetOutputName(char* p_nameBuffer, s_vChannel *p_chan) {
    if (p_chan->outputDestination.hardwareChannel == 0) {
        sprintf(p_nameBuffer, "Off");
        return;
    }

    uint8_t channel = p_chan->outputDestination.hardwareChannel;

    switch(p_chan->outputDestination.hardwareGroup){
        case 'x':
            sprintf(p_nameBuffer, "XLR%d", channel);
            break;
        case 'p':
            sprintf(p_nameBuffer, "P16-%d", channel);
            break;
        case 'c':
            sprintf(p_nameBuffer, "Card%d", channel);
            break;
        case 'a':
            sprintf(p_nameBuffer, "AUX%d", channel);
            break;
        case 'd':
            sprintf(p_nameBuffer, "DSP%d", channel);
            break;
        case 0:
            sprintf(p_nameBuffer, "AESA%d", channel);
            break;
        case 1:
            sprintf(p_nameBuffer, "AESB%d", channel);
            break;
    }
}


// set the gain of the local XLR head-amp-control
void mixingSetGain(uint8_t group, uint8_t channel, float gain) {
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
        case 0: // AES50A
            openx32.preamps.gainAes50a[channel - 1] = gain;
            break;
        case 1: // AES50B
            openx32.preamps.gainAes50b[channel - 1] = gain;
            break;
    }
}


// enable or disable phatom-power of local XLR-inputs
void mixingSetPhantomPower(uint8_t group, uint8_t channel, bool active) {
    uint8_t boardId;
    uint8_t addaChannel;

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
        case 0: // AES50A
            openx32.preamps.phantomPowerAes50a[channel - 1] = active;
            break;
        case 1: // AES50B
            openx32.preamps.phantomPowerAes50b[channel - 1] = active;
            break;
    }
}

// set the general volume of one of the 40 DSP-channels
void mixingSetVolume(uint8_t channel, float volume) {
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
void mixingSetBalance(uint8_t channel, uint8_t balance) {
    // set value to interal struct
    openx32.dspChannel[channel-1].balance = balance;

    // now update the volumes based on this balance
//    mixingSetVolume(channel, openx32.dspChannel[channel-1].volume);
    //TODO: send balance to DSP via SPI
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
