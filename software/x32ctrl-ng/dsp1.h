#pragma once

#include "external.h"
#include "x32ctrl_types.h"
#include "x32base.h"
#include "spi.h"
#include "fx.h"

#define DSP_BUF_IDX_OFF			0	// no audio
#define DSP_BUF_IDX_DSPCHANNEL	1	// DSP-Channel 1-32
#define DSP_BUF_IDX_AUX			33	// Aux-Channel 1-8
#define DSP_BUF_IDX_MIXBUS		41	// Mixbus 1-16
#define DSP_BUF_IDX_MATRIX		57	// Matrix 1-6
#define DSP_BUF_IDX_MAINLEFT	63	// main left
#define DSP_BUF_IDX_MAINRIGHT	64	// main right
#define DSP_BUF_IDX_MAINSUB		65	// main sub
#define DSP_BUF_IDX_MONLEFT		66	// Monitor Left
#define DSP_BUF_IDX_MONRIGHT	67	// Monitor Right
#define DSP_BUF_IDX_TALKBACK	68	// Talkback
#define DSP_BUF_IDX_DSP2_FX		69  // FXDSP2 FX-Channel 1-16
#define DSP_BUF_IDX_DSP2_AUX	85	// FXDSP2 AUX-Channel 1-8

class DSP1 : X32Base {

    private:
        uint8_t monitorTapPoint;

        // status messages
        float dspLoad[2];
        float dspVersion[2];

    public:

        sDspChannel dspChannel[40];
        sMixbusChannel mixbusChannel[16];
        sMatrixChannel matrixChannel[6];
        sFxChannel fxChannel[16];
        sDsp2AuxChannel dsp2AuxChannel[8];
        sMainChannel mainChannelLR;
        sMainChannel mainChannelSub;
        float volumeFxReturn[8];
        float volumeDca[8];

        float volumeSpecial;
        float monitorVolume;

        FX* fx;
        SPI* spi;

        DSP1(Config* config, State* state);
        void dspInit(void);

        void SendChannelVolume(uint8_t chan);
        void SendChannelSend(uint8_t chan);
        void SendMixbusVolume(uint8_t bus);
        void SendMatrixVolume(uint8_t matrix);
        void SendMonitorVolume();
        void SendMainVolume();
        void SendGate(uint8_t chan);
        void SendLowcut(uint8_t chan);
        void SendEQ(uint8_t chan);
        void ResetEq(uint8_t chan);
        void SendCompressor(uint8_t chan);
        void SendAll();

        void SetInputRouting(uint8_t dspChannel);
        void SetOutputRouting(uint8_t dspChannel);
        void SetChannelSendTapPoints(uint8_t dspChannel, uint8_t mixbusChannel, uint8_t tapPoint);
        void SetMixbusSendTapPoints(uint8_t mixbusChannel, uint8_t matrixChannel, uint8_t tapPoint);
        void SetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint);
        //void dspGetSourceName(char* p_nameBuffer, uint8_t dspChannel);
};