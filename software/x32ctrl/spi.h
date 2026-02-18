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

#include "eez/src/ui/screens.h"


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
#define SPI_TX_BUFFER_SIZE      3000
#define SPI_TX_MAX_WORD_COUNT   50  // multiband-compressor sends 41 values at once, so set to 50 to be safe
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

typedef struct {
  uint8_t classId;
  uint8_t channel;
  uint8_t index;
  uint8_t valueCount;
  float values[SPI_TX_MAX_WORD_COUNT];
} sSpiTxBufferElement;

typedef struct {
  sSpiTxBufferElement buffer[SPI_TX_BUFFER_SIZE];
  int head;
  int level; // tail will be calculated based on the level
} sSpiTxRingBuffer;

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
    bool connected; // SPI ist ready to use

    uint32_t dataToRead[2];
    const unsigned int SPI_START_MARKER = 0x0000002A; // '*'
    const unsigned int SPI_END_MARKER = 0x00000023; // '#'
    sSpiRxRingBuffer spiRxRingBuffer[2]; // for both DSPs
    sSpiTxRingBuffer spiTxRingBuffer[2]; // for both DSPs
    int spiDspHandle[2];
    int spiFpgaHandle;
    std::list<SpiEvent*> eventBuffer;

    void toggleFpgaProgramnPin(uint32_t assertTime, uint32_t waitTime);
    void setFpgaChipSelectPin(bool state);
    void setFpgaDonePin(bool state);

    int fpgaLatticeReadData(int* spi_fd, uint8_t cmd);
    uint32_t fpgaLatticeReverseByteOrder_uint32(uint32_t x);
    void fpgaLatticePrintBits(uint32_t status);
    bool fpgaLatticePollBusyFlag(int* spi_fd);
    bool fpgaLatticeSendCommand(int* spi_fd, uint8_t cmd, bool keepCS, bool checkBusyAndStatus);
    int fpgaLatticeTransferCommand(int* spi_fd, uint8_t cmd);

  public:
    SPI(X32BaseParameter* basepar);

    // functions for configuring FPGA and DSPs
    int UploadBitstreamFpgaXilinx();
    bool UploadBitstreamFpgaLattice();
    int UploadBitstreamDsps(bool useCli);

    // functions for general communication
    bool OpenConnectionFpga();
    bool CloseConnectionFpga();
    bool OpenConnectionDsps();
    bool CloseConnectionDsps();

    bool SendFpgaData(uint8_t txData[], uint8_t rxData[], uint8_t len);

    void QueueDspData(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, float values[]);
    void ProcessDspTxQueue(uint8_t dsp);
    uint32_t GetDspTxQueueLength(uint8_t dsp);
    bool SendDspData(uint8_t dsp, sSpiTxBufferElement* buffer);
    void UpdateNumberOfExpectedReadBytes(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index);
    bool ReadDspData(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index);
    void PushValuesToRxBuffer(uint8_t dsp, uint32_t valueCount, uint32_t values[]);
    void ProcessRxData(uint8_t dsp);

    bool HasNextEvent(void);
    SpiEvent* GetNextEvent(void);
};
