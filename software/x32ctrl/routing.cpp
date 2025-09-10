#include "routing.h"
#include "uart.h"
#include "adda.h"
#include "mixer.h"

sOpenx32 openx32;

void routingInit(void) {
    // reset routing-configuration and dsp-configuration
    routingDefaultConfig();
}

// set a default configuration for the audio-routing-matrix
void routingDefaultConfig(void) {
    // XLR-inputs 1-32 to DSP-inputs 1-32
    for (uint8_t ch=1; ch<=32; ch++) {
        routingSetOutputSource('d', ch, routingGetSourceIndex('x', ch));
    }
    // AUX-inputs 1-8 to DSP-inputs 33-40
    for (uint8_t ch=1; ch<=8; ch++) {
        routingSetOutputSource('d', ch + 32, routingGetSourceIndex('a', ch));
    }

    // DSP-outputs 1-16 to XLR-outputs 1-16
    for (uint8_t ch=1; ch<=16; ch++) {
        routingSetOutputSource('x', ch, routingGetSourceIndex('d', ch));
    }
    // DSP-outputs 17-32 to P16-output 1-16
    for (uint8_t ch=1; ch<=16; ch++) {
        routingSetOutputSource('p', ch, routingGetSourceIndex('d', ch + 16));
    }
    // DSP-outputs 33-40 to AUX-outputs 1-6 + ControlRoom L/R
    for (uint8_t ch=1; ch<=8; ch++) {
        routingSetOutputSource('a', ch, routingGetSourceIndex('d', ch + 32));
    }

    // XLR-inputs 1-32 to Card-outputs 1-32
    for (uint8_t ch=1; ch<=32; ch++) {
        routingSetOutputSource('c', ch, routingGetSourceIndex('x', ch));
    }


    // transmit routing-configuration to FPGA
    routingSendConfigToFpga();
}

// "compiles" the current mixer state into mixing routing
void routingSyncConfigFromMixer(void) {
	// TODO: this function is not working at the moment as outputchannels are routed independently to the vChannels (DSP-Channels)
	
/*
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
                    routingSetOutputSource(
                        mixer.vChannel[i].outputDestination.hardwareGroup,
                        mixer.vChannel[i].outputDestination.hardwareChannel,
                        routingGetSourceIndex(
                            mixer.vChannel[i].inputSource.hardwareGroup,
                            mixer.vChannel[i].inputSource.hardwareChannel
                        )
                    );

            // x32debug(" input(%c,%d) -> output(%c,%d)\n", 
            //         mixer.vChannel[i].inputSource.hardwareGroup,
            //         mixer.vChannel[i].inputSource.hardwareChannel,
            //             mixer.vChannel[i].outputDestination.hardwareGroup,
            //             mixer.vChannel[i].outputDestination.hardwareChannel
            //         );
                }
        }
        // transmit routing-configuration to FPGA
//        routingSendConfigToFpga();

        x32debug("Mixer routing to hardware synced\n");
    }
*/
}

// set the outputs of the audio-routing-matrix to desired input-sources
void routingSetOutputSource(uint8_t group, uint8_t channel, uint8_t inputsource) {
    // input-sources:
    // 0 = OFF
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
            openx32.routing.xlr[channel - 1] = inputsource;
            break;
        case 'p': // P16-Outputs 1-16
            openx32.routing.p16[channel - 1] = inputsource;
            break;
        case 'c': // Card-Outputs 1-32
            openx32.routing.card[channel - 1] = inputsource;
            break;
        case 'a': // Aux-Outputs 1-8
            openx32.routing.aux[channel - 1] = inputsource;
            break;
        case 'd': // DSP-Inputs 1-40
            openx32.routing.dsp[channel - 1] = inputsource;
            break;
        case 'A': // AES50A-Outputs
            openx32.routing.aes50a[channel - 1] = inputsource;
            break;
        case 'B': // AES50B-Outputs
            openx32.routing.aes50b[channel - 1] = inputsource;
            break;
    }
}

void routingSetOutputSourceByIndex(uint8_t index, uint8_t inputsource) {
    uint8_t group = 0;
    uint8_t channel = 0;
    routingGetOutputGroupAndChannelByIndex(index, &group, &channel);
    routingSetOutputSource(group, channel, inputsource);
}

uint8_t routingGetOutputSource(uint8_t group, uint8_t channel) {
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

    if (channel == 0) {
        return 0;
    }

    switch (group) {
        case 'x': // XLR-Outputs 1-16
            return openx32.routing.xlr[channel - 1];
            break;
        case 'p': // P16-Outputs 1-16
            return openx32.routing.p16[channel - 1];
            break;
        case 'c': // Card-Outputs 1-32
            return openx32.routing.card[channel - 1];
            break;
        case 'a': // Aux-Outputs 1-8
            return openx32.routing.aux[channel - 1];
            break;
        case 'd': // DSP-Inputs 1-40
            return openx32.routing.dsp[channel - 1];
            break;
        case 'A': // AES50A-Outputs
            return openx32.routing.aes50a[channel - 1];
            break;
        case 'B': // AES50B-Outputs
            return openx32.routing.aes50b[channel - 1];
            break;
    }

    return 0;
}

uint8_t routingGetOutputSourceByIndex(uint8_t index) {
    uint8_t group = 0;
    uint8_t channel = 0;
    routingGetOutputGroupAndChannelByIndex(index, &group, &channel);
    return routingGetOutputSource(group, channel);
}

// get the absolute input-source (global channel-number)
uint8_t routingGetSourceIndex(uint8_t group, uint8_t channel) {
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
        case 'A': // AES50A-Inputs 1-48
            return channel + 112;
            break;
        case 'B': // AES50B-Inputs 1-48
            return channel + 160;
            break;
    }

    return 0;
}

uint8_t routingGetOutputIndex(uint8_t group, uint8_t channel) {
    // output-taps
    // 1-16 = XLR-outputs
    // 17-32 = UltraNet/P16-outputs
    // 33-64 = Card-outputs
    // 65-72 = AUX-outputs
    // 73-112 = DSP-inputs
    // 113-160 = AES50A-outputs
    // 161-208 = AES50B-outputs

    if (channel == 0){
        return 0;
    }

    switch (group) {
        case 'x': // XLR-Inputs 1-16
            return channel;
            break;
        case 'p': // UltraNet/P16-outputs 1-16
            return channel + 16;
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
        case 'A': // AES50A-Inputs 1-48
            return channel + 112;
            break;
        case 'B': // AES50B-Inputs 1-48
            return channel + 160;
            break;
    }

    return 0;
}

void routingGetSourceGroupAndChannelByIndex(uint8_t index, uint8_t* group, uint8_t* channel) {
    // input-sources:
    // 1-32 = XLR-inputs
    // 33-64 = Card-inputs
    // 65-72 = AUX-inputs
    // 73-112 = DSP-outputs
    // 113-160 = AES50A-inputs
    // 161-208 = AES50B-inputs

	if ((index >= 1) && ( index <= 32)) {
		*group = 'x';
		*channel = index;
	}else if ((index >= 33) && ( index <= 64)) {
		*group = 'c';
		*channel = index - 32;
	}else if ((index >= 65) && ( index <= 72)) {
		*group = 'a';
		*channel = index - 64;
	}else if ((index >= 73) && ( index <= 112)) {
		*group = 'd';
		*channel = index - 72;
	}else if ((index >= 113) && ( index <= 160)) {
		*group = 'A';
		*channel = index - 112;
	}else if ((index >= 161) && ( index <= 208)) {
		*group = 'B';
		*channel = index - 160;
	}
}

void routingGetOutputGroupAndChannelByIndex(uint8_t index, uint8_t* group, uint8_t* channel) {
    // output-taps
    // 1-16 = XLR-outputs
    // 17-32 = UltraNet/P16-outputs
    // 33-64 = Card-outputs
    // 65-72 = AUX-outputs
    // 73-112 = DSP-inputs
    // 113-160 = AES50A-outputs
    // 161-208 = AES50B-outputs

	if ((index >= 1) && ( index <= 16)) {
		*group = 'x';
		*channel = index;
	}else if ((index >= 17) && ( index <= 32)) {
		*group = 'p';
		*channel = index - 16;
	}else if ((index >= 33) && ( index <= 64)) {
		*group = 'c';
		*channel = index - 32;
	}else if ((index >= 65) && ( index <= 72)) {
		*group = 'a';
		*channel = index - 64;
	}else if ((index >= 73) && ( index <= 112)) {
		*group = 'd';
		*channel = index - 72;
	}else if ((index >= 113) && ( index <= 160)) {
		*group = 'A';
		*channel = index - 112;
	}else if ((index >= 161) && ( index <= 208)) {
		*group = 'B';
		*channel = index - 160;
	}
}

void routingGetSourceGroupAndChannelByDspChannel(uint8_t dspChannel, uint8_t* group, uint8_t* channel) {
    // first lets find the connected source to this dspChannel
    uint8_t index = routingGetOutputSource('d', dspChannel);
    routingGetSourceGroupAndChannelByIndex(index, group, channel);
}

// get the input-source name
void routingGetSourceName(char* p_nameBuffer, uint8_t group, uint8_t channel) {
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
        case 'A': // AES50A-Inputs 1-48
            sprintf(p_nameBuffer, "AESA%d", channel);
            break;
        case 'B': // AES50B-Inputs 1-48
            sprintf(p_nameBuffer, "AESB%d", channel);
            break;
    }
}

void routingGetSourceNameByIndex(char* p_nameBuffer, uint8_t index) {
    uint8_t group = 0;
    uint8_t channel = 0;
    routingGetSourceGroupAndChannelByIndex(index, &group, &channel);
    routingGetSourceName(p_nameBuffer, group, channel);
}

// get the output destination name
void routingGetOutputName(char* p_nameBuffer, uint8_t group, uint8_t channel) {
    // output-taps
    // 1-16 = XLR-outputs
    // 17-32 = UltraNet/P16-outputs
    // 33-64 = Card-outputs
    // 65-72 = AUX-outputs
    // 73-112 = DSP-inputs
    // 113-160 = AES50A-outputs
    // 161-208 = AES50B-outputs

    if (channel == 0) {
        sprintf(p_nameBuffer, "Off");
        return;
    }

    switch(group){
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
        case 'A':
            sprintf(p_nameBuffer, "AESA%d", channel);
            break;
        case 'B':
            sprintf(p_nameBuffer, "AESB%d", channel);
            break;
    }
}

void routingGetOutputNameByIndex(char* p_nameBuffer, uint8_t index) {
    uint8_t group = 0;
    uint8_t channel = 0;
    routingGetOutputGroupAndChannelByIndex(index, &group, &channel);
    routingGetOutputName(p_nameBuffer, group, channel);
}

void routingGetDspSourceName(char* p_nameBuffer, uint8_t dspChannel) {
    uint8_t group = 0;
    uint8_t channel = 0;
    if (dspChannel == 0) {
        sprintf(p_nameBuffer, "OFF");
    }else if ((dspChannel >=1 ) && (dspChannel <= 40)) {
        // we have one of the regular DSP channels which are regular inputs
        routingGetSourceGroupAndChannelByDspChannel(dspChannel, &group, &channel);
        routingGetOutputName(p_nameBuffer, group, channel);
    }else if ((dspChannel >=41 ) && (dspChannel <= 48)) {
       sprintf(p_nameBuffer, "FX %d", dspChannel - 40);
    }else if ((dspChannel >=49 ) && (dspChannel <= 64)) {
       sprintf(p_nameBuffer, "Bus %d", dspChannel - 48);
    }
}

// helper-function to send the audio-routing to the fpga
void routingSendConfigToFpga(void) {
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
