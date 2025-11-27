// FPGA Configuration-tool for the Lattice ECP5 FPGAs
// v0.4.0, 27.11.2025
//
// This software reads a bitstream from Lattice Diamond and sends it using
// the SPI-connection /dev/spidev2.0 (CSPI3-connection of i.MX25)
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

// SPI configuration for i.MX25
#define SPI_DEVICE "/dev/spidev2.0"
#define SPI_SPEED_HZ 100000 // 100kHz (Lattice ECP5 should be able up to 60 MHz)

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

#define STATUS_DONE_BIT			0x00000100
#define STATUS_ERROR_BITS		0x00020040
#define STATUS_FEA_OTP			0x00004000
#define STATUS_FAIL_FLAG		0x00002000
#define STATUS_BUSY_FLAG		0x00001000
#define REGISTER_ALL_BITS_1		0xffffffff

// ----------------------------------------------

long get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1; // Error
}

uint32_t ReverseBitOrder_uint32(uint32_t n) {
	n = ((n >> 1) & 0x55555555) | ((n & 0x55555555) << 1);
	n = ((n >> 2) & 0x33333333) | ((n & 0x33333333) << 2);
	n = ((n >> 4) & 0x0F0F0F0F) | ((n & 0x0F0F0F0F) << 4);
	n = ((n >> 8) & 0x00FF00FF) | ((n & 0x00FF00FF) << 8);
	n = (n >> 16) | (n << 16);
	return n;
}

uint8_t ReverseBitOrder_uint8(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

uint32_t ReverseByteOrder_uint32(uint32_t x) { 
  return 
      ((x >> 24) & 0x000000FFul) | 
      ((x >>  8) & 0x0000FF00ul) | 
      ((x <<  8) & 0x00FF0000ul) | 
      ((x << 24) & 0xFF000000ul); 
} 

void ReverseBitOrderArray(uint8_t* data, uint32_t len) {
	// reverse bits in array
	uint8_t* pData = (uint8_t*)data;
	for (uint32_t i = 0; i < len; i++) {
		*pData = ReverseBitOrder_uint8(*pData);
		pData++;
	}
}

void ReverseBitAndByteOrderArray(uint8_t* data, uint32_t len) {
	// reverse bits in array
	uint32_t* pData = (uint32_t*)data;
	for (uint32_t i = 0; i < (len/4); i++) {
		*pData = ReverseBitOrder_uint32(*pData);
		pData++;
	}
}

void ReverseByteOrderArray(uint8_t* data, uint32_t len) {
	// reverse bits in array
	uint32_t* pData = (uint32_t*)data;
	for (uint32_t i = 0; i < (len/4); i++) {
		*pData = ReverseByteOrder_uint32(*pData);
		pData++;
	}
}

#define SHOWSTATUSBIT(status, bit, desc) printf("| %2d | %-30s | %1s |\n", bit, desc, ((status) & (1<<(bit))) ? "X" : " ");

// Assumes little endian
void printBits(uint32_t status)
{
    int i;
    
    printf("\n");
    printf("+-----------------------------------------+\n");
    printf("|      Lattice FPGA Status Register       |\n");
    printf("+----+--------------------------------+---+\n");
    printf("|Bit |Description                     |Set|\n");
    printf("+----+--------------------------------+---+\n");
      
    SHOWSTATUSBIT(status, 0, "JTAG Transparent");
    SHOWSTATUSBIT(status, 1, "Config Target Selection [0]");
    SHOWSTATUSBIT(status, 2, "Config Target Selection [1]");
    SHOWSTATUSBIT(status, 3, "Config Target Selection [2]");
    SHOWSTATUSBIT(status, 4, "JTAG Active");
    SHOWSTATUSBIT(status, 5, "PWD Protection");
    SHOWSTATUSBIT(status, 6, "(Internal use)");
    SHOWSTATUSBIT(status, 7, "Decrypt Enable");
    SHOWSTATUSBIT(status, 8, "DONE");
    SHOWSTATUSBIT(status, 9, "ISC Enable");
    SHOWSTATUSBIT(status, 10, "Write Enable");
    SHOWSTATUSBIT(status, 11, "Read Enable");
    SHOWSTATUSBIT(status, 12, "Busy Flag");
    SHOWSTATUSBIT(status, 13, "Fail Flag");
    SHOWSTATUSBIT(status, 14, "FEA OTP");
    SHOWSTATUSBIT(status, 15, "Decrypt Only");
    SHOWSTATUSBIT(status, 16, "PWD Enable");
    SHOWSTATUSBIT(status, 17, "(Internal use)");
    SHOWSTATUSBIT(status, 18, "(Internal use)");
    SHOWSTATUSBIT(status, 19, "(Internal use)");
    SHOWSTATUSBIT(status, 20, "Encrypt Preamble");
    SHOWSTATUSBIT(status, 21, "Std Preamble");
    SHOWSTATUSBIT(status, 22, "SPIm Fail 1");
    SHOWSTATUSBIT(status, 23, "BSE Error Code [0]");
    SHOWSTATUSBIT(status, 24, "BSE Error Code [1]");
    SHOWSTATUSBIT(status, 25, "BSE Error Code [2]");
    SHOWSTATUSBIT(status, 26, "ID Error");
    SHOWSTATUSBIT(status, 27, "ID Error");
    SHOWSTATUSBIT(status, 28, "Invalid Command");
    SHOWSTATUSBIT(status, 29, "SED Error");
    SHOWSTATUSBIT(status, 30, "Bypass Mode");
    SHOWSTATUSBIT(status, 31, "Flow Through Mode");
    
    printf("+----+--------------------------------+---+\n");
}

int readData(int* spi_fd, uint8_t cmd) {
    uint8_t tx_buf[8] = {0};
    uint8_t rx_buf[8] = {0};
    struct spi_ioc_transfer tr_cmd = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = 8, // 4 byte for command + 4 byte for read
        .bits_per_word = 8,
        .speed_hz = SPI_SPEED_HZ,
    };

    tx_buf[0] = cmd; // command
    ioctl(*spi_fd, SPI_IOC_MESSAGE(1), &tr_cmd);
    int rx_data;

	// copy desired data to 32-bit word
    memcpy(&rx_data, &rx_buf[4], 4);
	
	// reverse the byte-order
	rx_data = ReverseByteOrder_uint32(rx_data);

	return rx_data;
}

bool pollBusyFlag(int* spi_fd) {
	uint32_t busyState;
	int timeout = 0;
	
	do {
		busyState = readData(spi_fd, CMD_LSC_CHECK_BUSY);
		
		timeout++;
		if (timeout == 100000000) {
			return false;
		}
	} while(busyState != 0);
		
	return true;
}

bool flashErase(int* spi_fd) {
    uint8_t tx_buf[4] = {CMD_ISC_ERASE, 0x01, 0x00, 0x00};
    uint8_t rx_buf[4] = {0};
	
    struct spi_ioc_transfer tr_cmd = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = 4, // Standard 4-Byte-Befehl (8-Bit Cmd + 24-Bit Dummy)
        .bits_per_word = 8,
        .speed_hz = SPI_SPEED_HZ,
    };
	int ret = ioctl(*spi_fd, SPI_IOC_MESSAGE(1), &tr_cmd);
	
	if (!pollBusyFlag(spi_fd)) {
		return false;
	}
	
	return true;
}

bool sendCommand(int* spi_fd, uint8_t cmd, bool keepCS, bool checkBusyAndStatus) {
    uint8_t tx_buf[4] = {0};
    uint8_t rx_buf[4] = {0};
    struct spi_ioc_transfer tr_cmd = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = 4, // Standard 4-Byte-Befehl (8-Bit Cmd + 24-Bit Dummy)
        .bits_per_word = 8,
        .speed_hz = SPI_SPEED_HZ,
    };
	
	if (keepCS) {
		tr_cmd.cs_change = 1; // inverted logic here: 1 = no change=keep CS asserted, 0 = deassert CS after command
	}else{
		tr_cmd.cs_change = 0; // inverted logic here: 1 = no change=keep CS asserted, 0 = deassert CS after command
	}

    tx_buf[0] = cmd; // command
    int ret = ioctl(*spi_fd, SPI_IOC_MESSAGE(1), &tr_cmd);
	
	if (checkBusyAndStatus) {
		if (!pollBusyFlag(spi_fd)) {
			return false;
		}
	}else{
		return (ret >= 0);
	}
};

// configures a Lattice ECP5 via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
int configure_lattice_spi(const char *bitstream_path, const char *parameter) {
    int spi_fd = -1;
	uint32_t status;
    FILE *bitstream_file = NULL;
    int ret = -1;

    uint8_t spiMode = SPI_MODE_0; // Lattice ECP5 uses MODE 0
    uint8_t spiBitsPerWord = 8;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    fprintf(stdout, "FPGA Configuration Tool v0.4.0\n");

    fprintf(stdout, "  Connecting to SPI...\n");
    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0) {
        perror("Error: Could not open SPI-device");
        return -1;
    }

    // SPI-Modus (0 = CPOL=0, CPHA=0)
    ioctl(spi_fd, SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    fprintf(stdout, "  SPI-Bus '%s' initialized. (Mode %d, Speed %d Hz).\n", SPI_DEVICE, spiMode, spiSpeed);

    // open the file
    long bitstream_size = get_file_size(bitstream_path);
    bitstream_file = fopen(bitstream_path, "rb");
    if (!bitstream_file) {
        perror("Error: Could not open bitstream-file");
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        return -1;
    }

    // configuration-process
    fprintf(stdout, "Configuring Lattice FPGA...\n");
    fprintf(stdout, "  Setting PROGRAMN-Sequence HIGH -> LOW -> HIGH and start upload...\n");
    int fd = open("/sys/class/leds/reset_fpga/brightness", O_WRONLY);
    write(fd, "1", 1); // inverted logic in DeviceTree -> this sets the PROGRAMN to LOW
    usleep(500); // we have to keep at least 25ns. So 500us is more than enough
    write(fd, "0", 1); // inverted logic in DeviceTree -> this sets the PROGRAMN to HIGH again
    close(fd);
    usleep(50000); // we have to wait 50ms until we can send commands

    // Virtual toggle of PROGRAMN: send CMD_LSC_REFRESH command [class D command]
    fprintf(stdout, "  Sending LSC_REFRESH...");
    sendCommand(&spi_fd, CMD_LSC_REFRESH, false, false);
    fprintf(stdout, "OK\n");
    usleep(50000); // we have to wait 50ms until we can send commands

    // read IDCODE
    uint32_t idcode = readData(&spi_fd, CMD_READ_ID);
    fprintf(stdout, "  Read IDCODE: 0x%08X\n", idcode);
	// check if we've found the ID for the Lattice LFE5U-25 FPGA
	if (idcode != 0x41111043) {
		perror("Error: Unexpected IDCODE");
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        return -1;
	}

    // Enable SRAM Programming: send ISC_ENABLE command [class C command]
    fprintf(stdout, "  Sending ISC_ENABLE...");
    sendCommand(&spi_fd, CMD_ISC_ENABLE, false, true);
    fprintf(stdout, "OK\n");

    status = readData(&spi_fd, CMD_LSC_READ_STATUS);
    fprintf(stdout, "    Status Register [31..0]: ");
    printBits(status);
    fprintf(stdout, "\n");

    // Erase SRAM: send ISC_ERASE command [class D command]
    fprintf(stdout, "  Sending ISC_ERASE...");
    flashErase(&spi_fd); // ISC_ERASE seems to need a 0x01 after the command. See sources of openFPGAloader
	fprintf(stdout, "OK\n");

    status = readData(&spi_fd, CMD_LSC_READ_STATUS);
    fprintf(stdout, "    Status Register [31..0]: ");
    printBits(status);
    fprintf(stdout, "\n");

    // Initialize Address-Shift-Register: send LSC_INIT_ADDRESS command [class C command]
    fprintf(stdout, "  Sending LSC_INIT_ADDRESS...");
    sendCommand(&spi_fd, CMD_LSC_INIT_ADDRESS, false, false);
    fprintf(stdout, "OK\n");

	// =========== Here starts a long part without deasserting ChipSelect ===========
	
    // Program Config MAP: send LSC_BITSTREAM_BURST [class C command]
    fprintf(stdout, "  Sending LSC_BITSTREAM_BURST...");
    sendCommand(&spi_fd, CMD_LSC_BITSTREAM_BURST, true, false); // keep CS asserted after this command
    fprintf(stdout, "OK\n");

    // transmit large bitstream in chunks but without deasserting CS
    fseek(bitstream_file, 0, SEEK_SET); 
    const size_t CHUNK_SIZE = 1024; // 1 KB as maximum chunk-size
    uint8_t tx_chunk[CHUNK_SIZE];
    const int MAX_TRANSFERS = (int)(bitstream_size / CHUNK_SIZE) + 1;
	
	// dynamic allocation of memory for the transfer-array
    struct spi_ioc_transfer *transfers = calloc(MAX_TRANSFERS, sizeof(struct spi_ioc_transfer));
    if (!transfers) {
        perror("Error: Failed to allocate memory for transfer structures");
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        return -1;
    }
    // buffer for data-payload
    uint8_t *bitstream_payload = (uint8_t *)malloc(bitstream_size);
    if (!bitstream_payload) {
        perror("Error: Failed to allocate memory for bitstream payload");
        free(transfers);
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        return -1;
    }
    // read buffer into large payload-buffer
    size_t total_bytes_read = fread(bitstream_payload, 1, bitstream_size, bitstream_file);
    if (total_bytes_read != bitstream_size) {
        fprintf(stderr, "Error: Failed to read complete bitstream file.\n");
        free(bitstream_payload);
        free(transfers);
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        return -1;
    }
	
	// reverse bit-order
	if (strcmp(parameter, "1") == 0) {
		ReverseBitOrderArray(&bitstream_payload[0], total_bytes_read);
	}	
	// reverse bit- and byte-order
	if (strcmp(parameter, "2") == 0) {
		ReverseBitAndByteOrderArray(&bitstream_payload[0], total_bytes_read);
	}
	// reverse byte-order
	if (strcmp(parameter, "3") == 0) {
		ReverseByteOrderArray(&bitstream_payload[0], total_bytes_read);
	}
	
    // configure transfer
    int num_transfers = 0;
    size_t current_offset = 0;
    while (current_offset < bitstream_size) {
        size_t len = bitstream_size - current_offset;
        if (len > CHUNK_SIZE) {
            len = CHUNK_SIZE;
        }

        struct spi_ioc_transfer *tr = &transfers[num_transfers];
        
		tr->tx_buf = (unsigned long)(&bitstream_payload[0] + current_offset);
        tr->rx_buf = 0; // Kein Rx erforderlich
        tr->len = len;
        tr->bits_per_word = spiBitsPerWord;
        tr->speed_hz = spiSpeed;
        tr->cs_change = 0; // keep CS asserted between individual chunks, but deassert after last chunk. Behavior tested on RaspberryPi 4.
        
		// we dont set no flags here. The kernel keeps CS asserted within this transmission-chain
        
        current_offset += len;
        num_transfers++;
    }

    // send of the whole data-chain within a single ioctl-call
	fprintf(stdout, "  Sending Bitstream in %d chunks (Max %zu B/chunk)...\n", num_transfers, CHUNK_SIZE);
    ret = ioctl(spi_fd, SPI_IOC_MESSAGE(num_transfers), transfers);

    // freeing allocated dynamic memory
    free(bitstream_payload);
    free(transfers);

	// =========== End of transmission ===========
    
    if (ret < 0) {
        perror("Error: SPI BITSTREAM CHAIN failed");
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        return ret;
    }
    fprintf(stdout, "\r[██████████████████████████████████████████████████] %ld/%ld Bytes (100.00%%) - **COMPLETE**\n", bitstream_size, bitstream_size);
    usleep(10000); // wait 10ms

    // Exit Programming Mode: send ISC_DISABLE
    fprintf(stdout, "  Sending ISC_DISABLE...");
	sendCommand(&spi_fd, CMD_ISC_DISABLE, false, true);
    fprintf(stdout, "OK\n");

    // check Status-Bits
    // Bit 8 (DONE) must be 1, Bit 9 (ISC ENABLED) must be 1 sein, Bit 26 (EXECUTION ERROR) must be 0 sein
	status = readData(&spi_fd, CMD_LSC_READ_STATUS);
    int done = (status & (1 << 8)) > 0;
    int isc_enabled = (status & (1 << 9)) > 0;
    int exec_error = (status & (1 << 26)) > 0;

    fprintf(stdout, "  Status Register Check:\n");
    fprintf(stdout, "    DONE: %d\n", done);
    fprintf(stdout, "    ISC Enabled: %d\n", isc_enabled);
    fprintf(stdout, "    Execution Error: %d\n", exec_error);
    printBits(status);
    fprintf(stdout, "\n");

    if (!done || exec_error) {
        fprintf(stderr, "Configuration failed! Status indicates error or not done.\n");
        ret = -1;
    } else {
        fprintf(stdout, "\n✅ **SUCCESS:** Configuration complete and DONE bit set.\n");
        ret = 0;
    }


	if (bitstream_file) fclose(bitstream_file);
	if (spi_fd >= 0) close(spi_fd);
	return ret;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <bitstream.bit> <option>\n", argv[0]);
        fprintf(stderr, "  <option> == 0: regular Bitstream\n");
        fprintf(stderr, "  <option> == 1: reverse Bits per Byte in Bitstream\n");
        fprintf(stderr, "  <option> == 2: reverse Bits per Byte and reverse Byteorder per 32-bit Word in Bitstream\n");
        fprintf(stderr, "  <option> == 3: reverse Byteorder per 32-bit Word in Bitstream\n");
        return 1;
    }

    return configure_lattice_spi(argv[1], argv[2]);
}
