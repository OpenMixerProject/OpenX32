#ifndef ROUTING_H_
#define ROUTING_H_

#include "x32ctrl.h"
#include "hal.h"

#define FPGA_IDX_ROUTING   0    // reserve 200 slots
#define FPGA_IDX_CH_VOL    200  // here we need 40 values here
#define NUM_INPUT_CHANNEL  112  // at the moment we are not using AES50A and AES50B
#define NUM_OUTPUT_CHANNEL 112  // at the moment we are not using AES50A and AES50B
#define NUM_DSP_CHANNEL    64   // 32 in + 8 aux + 8 FX return + 16 bus

#pragma pack(push, 1)
// size of sRouting must be multiplier of 8 to fit into 64-bit-value
typedef struct __attribute__((packed,aligned(1))) {
  uint8_t xlr[16];
  uint8_t p16[16];
  uint8_t card[32];
  uint8_t aux[8];
  uint8_t dsp[40];
  uint8_t aes50a[48]; // not used in FPGA at the moment
  uint8_t aes50b[48]; // not used in FPGA at the moment
} sRouting;
#pragma pack(pop)

typedef struct {
  float gainXlr[32];
  float gainAes50a[48];
  float gainAes50b[48];

  bool phantomPowerXlr[32];
  bool phantomPowerAes50a[48];
  bool phantomPowerAes50b[48];
} sPreamps;

typedef struct {
  float volume; // volume in dBfs
  uint8_t balance; // balance of channel (left/right)
  bool muted;
} sDspChannel;

typedef struct {
  sRouting routing;
  sPreamps preamps;
  sDspChannel dspChannel[40]; // DSP-Channels with controls
} sOpenx32;

extern sOpenx32 openx32;

void routingInit(void);
void routingDefaultConfig(void);
void routingSyncConfigFromMixer(void);

void routingSetOutputSource(uint8_t group, uint8_t channel, uint8_t inputsource);
void routingSetOutputSourceByIndex(uint8_t index, uint8_t inputsource);
uint8_t routingGetOutputSource(uint8_t group, uint8_t channel);
uint8_t routingGetOutputSourceByIndex(uint8_t index);

uint8_t routingGetSourceIndex(uint8_t group, uint8_t channel);
uint8_t routingGetOutputIndex(uint8_t group, uint8_t channel);
void routingGetSourceGroupAndChannelByIndex(uint8_t index, uint8_t* group, uint8_t* channel);
void routingGetOutputGroupAndChannelByIndex(uint8_t index, uint8_t* group, uint8_t* channel);
void routingGetSourceGroupAndChannelByDspChannel(uint8_t dspChannel, uint8_t* group, uint8_t* channel);

void routingGetSourceName(char* p_nameBuffer, uint8_t group, uint8_t channel);
void routingGetSourceNameByIndex(char* p_nameBuffer, uint8_t index);
void routingGetOutputName(char* p_nameBuffer, s_vChannel *p_chan);
void routingGetOutputNameByIndex(char* p_nameBuffer, uint8_t index);
void routingGetDspSourceName(char* p_nameBuffer, uint8_t dspChannel);

void routingSendConfigToFpga(void);

#endif
