#pragma once

#include "external.h"
#include "x32ctrl_types.h"
#include "spi.h"

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


extern sDsp dsp;

void dspInit(void);
void dspSendChannelVolume(uint8_t dspChannel);
void dspSendChannelSend(uint8_t dspChannel);
void dspSendMixbusVolume(uint8_t mixbusChannel);
void dspSendMatrixVolume(uint8_t matrixChannel);
void dspSendMonitorVolume();
void dspSendMainVolume();
void dspSendGate(uint8_t dspChannel);
void dspSendLowcut(uint8_t dspChannel);
void dspSendEQ(uint8_t dspChannel);
void dspResetEq(uint8_t dspChannel);
void dspSendCompressor(uint8_t dspChannel);
void dspSendAll();
void dspSetInputRouting(uint8_t dspChannel);
void dspSetOutputRouting(uint8_t dspChannel);
void dspSetChannelSendTapPoints(uint8_t dspChannel, uint8_t mixbusChannel, uint8_t tapPoint);
void dspSetMixbusSendTapPoints(uint8_t mixbusChannel, uint8_t matrixChannel, uint8_t tapPoint);
void dspSetMainSendTapPoints(uint8_t matrixChannel, uint8_t tapPoint);
//void dspGetSourceName(char* p_nameBuffer, uint8_t dspChannel);
void callbackDsp1(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values);
void callbackDsp2(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values);