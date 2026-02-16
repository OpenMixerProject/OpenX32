#pragma once

#include "external.h"
#include "types.h"
#include "const.h"

#include "base.h"
#include "spi.h"
#include "fxmath.h"

class DSP1 : X32Base {

    using enum MP_ID;

    private:
        uint8_t monitorTapPoint;
        uint8_t readState;

    public:
        srDspChannel rChannel[MAX_FPGA_TO_DSP1_CHANNELS + 8 + 8];  // values used only during runtime
        sDspChannel Channel[MAX_FPGA_TO_DSP1_CHANNELS]; // values stored in config
        sMixbusChannel Bus[16];
        sMatrixChannel Matrix[6];
        //sFxChannel Dsp1toDsp2Routing[MAX_DSP1_TO_DSP2_CHANNELS];
        sMainChannel MainChannelLR;
        sMainChannel MainChannelSub;
        sDspOutchannel Dsp1Routing[MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS];
        float volumeFxReturn[8];
        float volumeDca[8];

        float volumeSpecial;
        float monitorVolume;
        float rtaData[64];

        FxMath* fxmath;
        SPI* spi;

        FxSlot* fx_slot[MAX_FX_SLOTS];

        DSP1(X32BaseParameter* basepar);
        void Init(void);
        void LoadRouting_X32Default();

        void SendChannelVolume(uint8_t chan);
        void SendChannelSend(uint8_t chan);
        void SendMixbusVolume(uint8_t bus);
        void SendMatrixVolume(uint8_t matrix);
        void SendMonitorVolume();
        void SendMainVolume();
        void SendGate(uint chan);
        void SendLowcut(uint8_t chan);
        void SendEQ(uint chan);
        void NewFunction(sPEQ &eq, int peqIndex, uint chanIndex);
        void ResetEq(uint8_t chan);
        void SendCompressor(uint8_t chan);
        void SendAll();

        void SetInputRouting(uint dspChannel);
        void SetOutputRouting(uint dspChannel);
        void SetChannelSendTapPoints(uint8_t dspChannel, uint8_t mixbusChannel, uint8_t tapPoint);
        void SetMixbusSendTapPoints(uint8_t mixbusChannel, uint8_t matrixChannel, uint8_t tapPoint);
        void SetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint);
        void GetSourceName(char* p_nameBuffer, uint8_t dspChannel, uint8_t dspInputSource);
        String RoutingGetOutputNameByIndex(uint8_t index);
        void RoutingGetTapNameByIndex(char* p_nameBuffer, uint8_t index, uint8_t source);
        void RoutingGetTapPositionName(char* p_nameBuffer, uint8_t position);
        void UpdateVuMeter(uint8_t intervalMs);
        uint8_t GetPeak(int i, uint8_t steps);

        void DSP2_SetFx(int fxSlot, FX_TYPE fxType, int mode);
        void DSP2_SendFxParameter(int fxSlot);

        void CallbackStateMachine();
        void callbackDsp1(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values);
        void callbackDsp2(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values);
};
