#ifndef DSP_H_
#define DSP_H_

#include "x32ctrl.h"

void dspInit(void);
void dspSendChannelVolume(uint8_t dspChannel);
void dspSendMixbusVolume(uint8_t mixbusChannel);
void dspSendMatrixVolume(uint8_t matrixChannel);
void dspSendMonitorVolume();
void dspSendMainVolume();
void dspSendGate(uint8_t dspChannel);
void dspSendLowcut(uint8_t dspChannel);
void dspSendEQ(uint8_t dspChannel);
void dspSendCompressor(uint8_t dspChannel);
void dspSendAll();
void dspSetInputRouting(uint8_t dspChannel);
void dspSetOutputRouting(uint8_t dspChannel);
void dspSetChannelSendTapPoints(uint8_t dspChannel, uint8_t mixbusChannel, uint8_t tapPoint);
void dspSetMixbusSendTapPoints(uint8_t mixbusChannel, uint8_t matrixChannel, uint8_t tapPoint);
void dspSetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint);
void dspGetSourceName(char* p_nameBuffer, uint8_t dspChannel);

#endif
