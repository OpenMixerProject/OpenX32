#pragma once

#include "external.h"
#include "x32ctrl_types.h"
#include "auxiliary.h"
#include "dsp.h"

/*
  SPI ClassIds:
  'v' = Volume
  'e' = PEQ
  'g' = Gate
  'c' = Compressor
  'a' = Auxiliary
*/

// variables
extern uint32_t dataToRead[2];

// function prototypes
int spiConfigureFpga(const char* bitstream_path);
int spiConfigureDsp(const char* bitstream_path_a, const char* bitstream_path_b, uint8_t numStreams);
bool spiOpenDspConnections();
bool spiCloseDspConnections();
void spiProcessRxData(uint8_t dsp);
void spiUpdateNumberOfExpectedReadBytes(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index);
void spiPushValuesToRxBuffer(uint8_t dsp, uint32_t valueCount, uint32_t values[]);
bool spiSendDspParameterArray(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, float values[]);
bool spiSendDspParameter(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, float value);
bool spiSendDspParameter_uint32(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint32_t value);

