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
        sMainChannel MainChannelLR;
        sMainChannel MainChannelSub;
        float volumeFxReturn[8];
        float volumeDca[8];

        float volumeSpecial;
        float monitorVolume;
        float rtaData[64];
        sDsp2Oscillator dsp2Oscillator[2];

        FxMath* fxmath;
        SPI* spi;

        FxSlot* fx_slot[MAX_FX_SLOTS];

        DSP1(X32BaseParameter* basepar);
        void Init(void);

        void SendChannelVolume(uint chanIndex);
        void SendChannelSend(uint chanIndex);
        void SendMixbusVolume(uint chanIndex);
        void SendMatrixVolume(uint chanIndex);
        void SendMonitorVolume();
        void SendMainVolume();
        void SendGate(uint chanIndex);
        void SendLowcut(uint8_t chan);
        void SendEQ(uint chan);
        void ResetEq(uint8_t chan);
        void SendCompressor(uint8_t chanIndex);

        void SetInputRouting(uint dspChannel);
        void SetOutputRouting(uint dspChannel);
        void ChannelSendTapPoints(uint chanIndex);
        void SetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint);
        String RoutingGetOutputNameByIndex(uint8_t index);
        void UpdateVuMeter(uint8_t intervalMs);
        uint8_t GetPeak(int i, uint8_t steps);

        void DSP2_SetFx(int fxSlot, FX_TYPE fxType, int mode);
        void DSP2_SendFxParameter(int fxSlot);
        void DSP2_SetOscillator(uint8_t oscIndex, float frequency, float volumedB);
        void DSP2_SendOscillatorValues();

        void CallbackStateMachine();
        void callbackDsp1(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values);
        void callbackDsp2(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values);
};
