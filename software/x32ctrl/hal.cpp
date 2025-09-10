#include "hal.h"
#include "uart.h"
#include "adda.h"
#include "mixer.h"
#include "spi.h"

void halSyncVChannelConfigFromMixer(void){
    // loop trough all vChannels
    uint8_t group = 0;
    uint8_t channel = 0;
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        if (mixer.vChannel[i].inputSource.dspChannel == 0){
            continue;
        } else {
            s_vChannel* chan = &mixer.vChannel[i];

            // only the first 40 channels are able to control gain and phantomPower. DSP-Channels 41..64 do not support this
            if ((mixer.vChannel[i].inputSource.dspChannel >= 1) && (mixer.vChannel[i].inputSource.dspChannel <= 40)) {
                routingGetSourceGroupAndChannelByDspChannel(mixer.vChannel[i].inputSource.dspChannel, &group, &channel);

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_GAIN)){
                    halSetGain(group, channel, chan->inputSource.gain);
                }

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_PHANTOM)){
                    halSetPhantomPower(group, channel, chan->inputSource.phantomPower);
                }

                if (mixerHasVChannelChanged(chan, X32_VCHANNEL_CHANGED_VOLUME)){
                    halSetVolume(chan->inputSource.dspChannel, chan->volume);
                }
            }
        }
    }

    x32debug("Mixer gain to hardware synced\n");
}

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

    // send volume to DSP via SPI
    float value = pow(10, volume/20.0f);
    uint32_t valueRaw;
    memcpy(&valueRaw, &value, 4);
    spiSendDspParameter(0, 10 + (channel - 1), valueRaw);
}

// set the balance of one of the 40 DSP-channels
void halSetBalance(uint8_t channel, uint8_t balance) {
    // set value to interal struct
    openx32.dspChannel[channel-1].balance = balance;

    // now update the volumes based on this balance
//    halSetVolume(channel, openx32.dspChannel[channel-1].volume);
    //TODO: send balance to DSP via SPI
}
