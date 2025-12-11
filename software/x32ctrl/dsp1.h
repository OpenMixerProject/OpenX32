#pragma once

#include "external.h"
#include "types.h"
#include "const.h"

#include "base.h"
#include "spi.h"
#include "fx.h"

#define DSP_BUF_IDX_OFF			0	// no audio
#define DSP_BUF_IDX_DSPCHANNEL	1	// DSP-Channel 1-32
#define DSP_BUF_IDX_AUX			33	// Aux-Channel 1-8
#define DSP_BUF_IDX_DSP2_FX		41  // FXDSP2 FX-Channel 1-16
#define DSP_BUF_IDX_MIXBUS		57	// Mixbus 1-16
#define DSP_BUF_IDX_MAINLEFT	73	// main left
#define DSP_BUF_IDX_MAINRIGHT	74	// main right
#define DSP_BUF_IDX_MAINSUB		75	// main sub
#define DSP_BUF_IDX_MATRIX		76	// Matrix 1-6
#define DSP_BUF_IDX_DSP2_AUX	82	// FXDSP2 AUX-Channel 1-8
#define DSP_BUF_IDX_MONLEFT		90	// Monitor Left
#define DSP_BUF_IDX_MONRIGHT	91	// Monitor Right
#define DSP_BUF_IDX_TALKBACK	92	// Talkback

#define DSP_TAP_INPUT           0
#define DSP_TAP_PRE_EQ          1
#define DSP_TAP_POST_EQ         2
#define DSP_TAP_PRE_FADER       3
#define DSP_TAP_POST_FADER      4

class DSP1 : X32Base {

    private:
        uint8_t monitorTapPoint;

    public:
        srDspChannel rChannel[MAX_DSP_INPUTCHANNELS];  // values used only during runtime
        sDspChannel Channel[MAX_DSP_INPUTCHANNELS]; // values stored in config
        sMixbusChannel Bus[16];
        sMatrixChannel Matrix[6];
        sFxChannel Dsp2FxChannel[MAX_DSP_FXCHANNELS];
        sDsp2AuxChannel Dsp2AuxChannel[MAX_DSP_AUXCHANNELS];
        sMainChannel MainChannelLR;
        sMainChannel MainChannelSub;
        sDspOutchannel Dsp2FpgaChannel[MAX_DSP_OUTPUTCHANNELS];
        float volumeFxReturn[8];
        float volumeDca[8];

        float volumeSpecial;
        float monitorVolume;

        FX* fx;
        SPI* spi;

        DSP1(X32BaseParameter* basepar);
        void dspInit(void);
        void Tick10ms(void);
        void Tick100ms(void);

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
        void SetFxOutputRouting(uint8_t fxChannel);
        void SetAuxOutputRouting(uint8_t auxChannel);
        void SetChannelSendTapPoints(uint8_t dspChannel, uint8_t mixbusChannel, uint8_t tapPoint);
        void SetMixbusSendTapPoints(uint8_t mixbusChannel, uint8_t matrixChannel, uint8_t tapPoint);
        void SetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint);
        void GetSourceName(char* p_nameBuffer, uint8_t dspChannel, uint8_t dspInputSource);
        void RoutingGetInputNameByIndex(char* p_nameBuffer, uint8_t index);
        void RoutingGetOutputNameByIndex(char* p_nameBuffer, uint8_t index);
        void RoutingGetTapNameByIndex(char* p_nameBuffer, uint8_t index, uint8_t source);
        void RoutingGetTapPositionName(char* p_nameBuffer, uint8_t position);
        void UpdateVuMeter();

        void callbackDsp1(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values);
        void callbackDsp2(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values);
};
