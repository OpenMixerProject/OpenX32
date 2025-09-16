#ifndef SPI_H_
#define SPI_H_

/*
  SPI ClassIds:
  'v' = Volume
  'e' = PEQ
  'g' = Gate
  'c' = Compressor
  'a' = Auxiliary
*/

// variables

// function prototypes
int spiConfigureFpga(const char* bitstream_path);
int spiConfigureDsp(const char* bitstream_path_a, const char* bitstream_path_b, uint8_t numStreams);
bool spiOpenDspConnections();
bool spiCloseDspConnections();
bool spiSendReceiveDspParameterArray(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, float values[], void* inputValue);
bool spiSendDspParameterArray(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, float values[]);
bool spiSendDspParameter(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, float value);
bool spiSendDspParameter_uint32(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint32_t value);
float spiReadDspParameter(uint8_t dsp, uint8_t channel, uint8_t index);
uint32_t spiReadDspParameter_uint32(uint8_t dsp, uint8_t channel, uint8_t index);

#endif
