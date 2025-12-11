#pragma once

#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <list>

#include "external.h"
#include "defines.h"
#include "types.h"
#include "base.h"
#include "state.h"
#include "spi-event.h"

#define SPI_DEVICE_DSP1 "/dev/spidev0.0"
#define SPI_DEVICE_DSP2 "/dev/spidev0.1"
#define SPI_DEVICE_FPGA "/dev/spidev2.0"
#define MHz * 1000000
#define SPI_FPGA_XILINX_SPEED_HZ 8 MHz // 10 MHz for the FPGA, seems to be fine
#define SPI_FPGA_LATTICE_SPEED_HZ 8 MHz // 8MHz
#define SPI_DSP_CONF_SPEED_HZ 1 MHz
#define SPI_DSP_SPEED_HZ 32 MHz // 32 MHz is diveded by 2 -> 16 MHz is the regular SPI-clock on the original X32

// defines for FPGA-configuration via SPI
#define PROG_B_GPIO_OFFSET      30
#define DONE_GPIO_OFFSET        2
#define FPGA_BUFFER_SIZE        4096
#define FPGA_FILE_BUFFER_SIZE   1024

// Lattice Commands
#define CMD_ISC_NOOP			0xFF
#define CMD_READ_ID				0xE0
#define CMD_USERCODE			0xC0
#define CMD_LSC_READ_STATUS		0x3C
#define CMD_LSC_CHECK_BUSY		0xF0
#define CMD_LSC_REFRESH			0x79
#define CMD_ISC_ENABLE			0xC6
#define CMD_ISC_DISABLE			0x26
#define CMD_ISC_ERASE			0x0E
#define CMD_ISC_PROGRAM_DONE	0x5E
#define CMD_LSC_INIT_ADDRESS	0x46
#define CMD_LSC_BITSTREAM_BURST	0x7A


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

    void fpgaLatticeToggleProgramnPin();
    void fpgaLatticeChipSelectPin(bool state);
    void fpgaLatticeDonePin(bool state);
    int fpgaLatticeReadData(int* spi_fd, uint8_t cmd);
    uint32_t fpgaLatticeReverseByteOrder_uint32(uint32_t x);
    void fpgaLatticePrintBits(uint32_t status);
    bool fpgaLatticePollBusyFlag(int* spi_fd);
    bool fpgaLatticeSendCommand(int* spi_fd, uint8_t cmd, bool keepCS, bool checkBusyAndStatus);
    int fpgaLatticeTransferCommand(int* spi_fd, uint8_t cmd);

  public:
    SPI(X32BaseParameter* basepar);
    int ConfigureFpgaXilinx();
    bool ConfigureFpgaLattice();
    int ConfigureDsp();
    void Tick10ms(void);
    void Tick100ms(void);
    bool OpenDspConnections();
    bool CloseDspConnections();
    void ProcessRxData(uint8_t dsp);
    void UpdateNumberOfExpectedReadBytes(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index);
    void PushValuesToRxBuffer(uint8_t dsp, uint32_t valueCount, uint32_t values[]);
    bool SendDspParameterArray(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, float values[]);
    bool SendReceiveDspParameterArray(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, float values[]);
    bool SendDspParameter(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, float value);
    bool SendDspParameter_uint32(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint32_t value);

    bool HasNextEvent(void);
    SpiEvent* GetNextEvent(void);
};
