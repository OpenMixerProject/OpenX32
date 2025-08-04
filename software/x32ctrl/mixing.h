#ifndef MIXING_H_
#define MIXING_H_

#include "x32ctrl.h"

#define FPGA_IDX_ROUTING   0    // reserve 200 slots
#define FPGA_IDX_CH_VOL    200  // here we need 40 values here

#pragma pack(push, 1)
// size of sRouting must be multiplier of 8 to fit into 64-bit-value
typedef struct __attribute__((packed)) {
  uint8_t xlrOutput[16];
  uint8_t auxOutput[8];
  uint8_t cardOutput[32];
  uint8_t p16Output[16];
  uint8_t dspInput[40];
  //uint8_t aes50aOutput[48];
  //uint8_t aes50bOutput[48];
} sRouting;
#pragma pack(pop)

typedef struct {
  float volume; // volume in dBfs
  uint8_t balance; // balance of channel (left/right)
  bool muted;
} sDspChannel;

typedef struct {
  sRouting routing;
  float gain[32]; // XLR-Gains
  bool phantomPower[32]; // XLR-Phantom-Power
  sDspChannel dspChannel[40]; // DSP-Channels
} sOpenx32;

void mixingDefaultConfig(void);
void mixingSetRouting(uint8_t group, uint8_t dst, uint8_t src);
void mixingSetGain(uint8_t channel, float gain);
void mixingSetPhantomPower(uint8_t channel, bool active);
void mixingSetVolume(uint8_t channel, float volume);
void mixingSetBalance(uint8_t channel, uint8_t balance);
void mixingTxRoutingConfig(void);

#endif
