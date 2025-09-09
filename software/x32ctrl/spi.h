#ifndef SPI_H_
#define SPI_H_

// variables

// function prototypes
int spiConfigureFpga(const char *bitstream_path);
int spiConfigureDsp(const char *bitstream_path_a, const char *bitstream_path_b, uint8_t numStreams);

#endif
