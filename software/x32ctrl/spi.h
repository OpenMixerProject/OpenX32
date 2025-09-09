#ifndef SPI_H_
#define SPI_H_

// variables

// function prototypes
int spiConfigureFpga(const char *bitstream_path);
int spiConfigureDsp(const char *bitstream_path_a, const char *bitstream_path_b, uint8_t numStreams);
bool spiOpenDspConnections();
bool spiCloseDspConnections();
bool spiSendDspParameter(uint8_t dsp, uint32_t parameter, uint32_t value);

#endif
