#ifndef DSP_H_
#define DSP_H_

#include "x32ctrl.h"

extern sDsp dsp;

void dspInit(void);
void dspSendChannelVolume(uint8_t dspChannel, float volumeLR, float volumeSub, float balance);
void dspSendMixbusVolume(uint8_t mixbusChannel, float volumeLR, float volumeSub, float balance);
void dspSendMatrixVolume(uint8_t matrixChannel, float volume);
void dspSendMonitorVolume(float volume);
void dspSendMainVolume(float volumeLR, float volumeSub, float balance);
void dspSendGate(uint8_t dspChannel);
void dspSendLowcut(uint8_t dspChannel);
void dspSendEQ(uint8_t dspChannel);
void dspSendCompressor(uint8_t dspChannel);
void dspSendAll();
void dspSetInputRouting(uint8_t dspChannel);
void dspSetOutputRouting(uint8_t dspOutputChannel);
void dspSetChannelSendTapPoints(uint8_t dspChannel, uint8_t mixbusChannel, uint8_t tapPoint);
void dspSetMixbusSendTapPoints(uint8_t mixbusChannel, uint8_t matrixChannel, uint8_t tapPoint);
void dspSetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint);

#endif
