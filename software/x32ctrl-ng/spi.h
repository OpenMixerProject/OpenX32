#pragma once

#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <list>

#include "external.h"
#include "constants.h"
#include "x32ctrl_types.h"
#include "x32base.h"
#include "state.h"
#include "spi-event.h"

// defines for FPGA-configuration via SPI
#define PROG_B_GPIO_OFFSET      30
#define DONE_GPIO_OFFSET        2
#define FPGA_BUFFER_SIZE        4096
#define FPGA_FILE_BUFFER_SIZE   1024

// defines for SPI-communication with DSPs
#define SPI_MAX_RX_PAYLOAD_SIZE 200 // 197 int-values + * + # + parameter
#define SPI_RX_BUFFER_SIZE      (SPI_MAX_RX_PAYLOAD_SIZE * 3) // store up to 3 payload-sets
typedef enum {
	LOOKING_FOR_START_MARKER,
	COLLECTING_PAYLOAD,
	LOOKING_FOR_END_MARKER
} spiParserState;

typedef struct {
	uint32_t buffer[SPI_RX_BUFFER_SIZE];
    int head;
    int tail;

    spiParserState state = LOOKING_FOR_START_MARKER;
    uint32_t payload[SPI_MAX_RX_PAYLOAD_SIZE];
    int payloadIdx;
    int payloadLength;
} sSpiRxRingBuffer;

class SPI : public X32Base {

  /*
    SPI ClassIds:
    'v' = Volume
    'e' = PEQ
    'g' = Gate
    'c' = Compressor
    'a' = Auxiliary
  */

  private:
    uint32_t dataToRead[2];
    const unsigned int SPI_START_MARKER = 0x0000002A; // '*'
    const unsigned int SPI_END_MARKER = 0x00000023; // '#'
    sSpiRxRingBuffer spiRxRingBuffer[2]; // for both DSPs
    int spiDspHandle[2];
    std::list<SpiEvent*> eventBuffer;

  public:
    SPI(Config* config, State* state);
    int ConfigureFpga(const char* bitstream_path);
    int ConfigureDsp(const char* bitstream_path_a, const char* bitstream_path_b, uint8_t numStreams);
    bool OpenDspConnections();
    bool CloseDspConnections();
    void ProcessRxData(uint8_t dsp);
    void UpdateNumberOfExpectedReadBytes(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index);
    void PushValuesToRxBuffer(uint8_t dsp, uint32_t valueCount, uint32_t values[]);
    bool SendDspParameterArray(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, float values[]);
    bool SendDspParameter(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, float value);
    bool SendDspParameter_uint32(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint32_t value);

    bool HasNextEvent(void);
    SpiEvent* GetNextEvent(void);
};