// FPGA Configuration-tool for the Lattice ECP5 FPGAs
// v0.2.0, 02.11.2025
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
#define SPI_SPEED_HZ 10000000 // 10 MHz (Lattice ECP5 should be able up to 60 MHz)

// ----------------------------------------------

long get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1; // Error
}

// Assumes little endian
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
        printf("|");
    }
    puts("");
}

// configures a Lattice ECP5 via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
int configure_lattice_spi(const char *bitstream_path) {
    int spi_fd = -1;
    FILE *bitstream_file = NULL;
    uint8_t *full_tx_buffer = NULL; // pointer for the whole transmit-buffer
    int ret = -1;

    uint8_t spiMode = SPI_MODE_0; // Lattice ECP5 uses MODE 0
    uint8_t spiBitsPerWord = 8;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    fprintf(stdout, "FPGA Configuration Tool v0.2.0\n");

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
        ret = -1; goto cleanup;
    }

    // configuration-process
    const size_t CMD_OVERHEAD = 4;
    size_t total_transfer_size = bitstream_size + CMD_OVERHEAD;

    // allocate memory for the whole burst-transfer
    full_tx_buffer = (uint8_t *)malloc(total_transfer_size);
    if (!full_tx_buffer) {
        perror("Error: Failed to allocate memory for bitstream");
        goto cleanup;
    }
    memset(full_tx_buffer, 0x00, total_transfer_size); // initialize memory with zeros
    full_tx_buffer[0] = 0x7A; // set command to LSC_BITSTREAM_BURST

    // read bitstream to memory right behind the command
    size_t bytes_read = fread(full_tx_buffer + CMD_OVERHEAD, 1, bitstream_size, bitstream_file);
    if (bytes_read != bitstream_size) {
        fprintf(stderr, "Error: Failed to read complete bitstream file (Read %zu of %ld bytes).\n", bytes_read, bitstream_size);
        goto cleanup;
    }

    fprintf(stdout, "Configuring Lattice FPGA...\n");
    fprintf(stdout, "  Setting PROGRAMN-Sequence HIGH -> LOW -> HIGH and start upload...\n");
    int fd = open("/sys/class/leds/reset_fpga/brightness", O_WRONLY);
    write(fd, "1", 1);
    usleep(500); // we have to keep at least 25ns. So 500us is more than enough
    write(fd, "0", 1);
    close(fd);
    usleep(50000); // we have to wait 50ms until we can send commands
	
	
	// prepare struct to send commands
    uint8_t cmd_buf[8] = {0};
    uint8_t rx_buf[8] = {0};
    struct spi_ioc_transfer tr_cmd = {
        .tx_buf = (unsigned long)cmd_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = 4, // Standard 4-Byte-Befehl (8-Bit Cmd + 24-Bit Dummy)
        .bits_per_word = spiBitsPerWord,
        .speed_hz = spiSpeed,
    };
	
    // read IDCODE
    cmd_buf[0] = 0xE0; // READ_ID
    tr_cmd.len = 8; // 8 Byte für READ_ID (Cmd + 3 Dummy + 4 Daten)
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr_cmd) < 0) {
        perror("Error: SPI READ_ID failed"); goto cleanup;
    }
    uint32_t idcode;
    memcpy(&idcode, &rx_buf[4], 4);
    fprintf(stdout, "  Read IDCODE: 0x%08X\n", idcode);
//	if (idcode != 0x00000000) {
//		perror("Error: Unexpected IDCODE");
//		goto cleanup;
//	}

    // send ISC_ENABLE command [class C command]
    memset(cmd_buf, 0, sizeof(cmd_buf));
    cmd_buf[0] = 0xC6; // ISC_ENABLE
    tr_cmd.len = 4;
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr_cmd) < 0) {
        perror("Error: SPI ISC_ENABLE failed"); goto cleanup;
    }
    fprintf(stdout, "  ISC_ENABLE sent.\n");
    usleep(100);

    fprintf(stdout, "  Sending Bitstream Burst (%zu bytes)...\n", total_transfer_size);
    struct spi_ioc_transfer tr_burst = {
        .tx_buf = (unsigned long)full_tx_buffer,
        .rx_buf = 0, // no feedback necessary
        .len = total_transfer_size,
        .bits_per_word = spiBitsPerWord,
        .speed_hz = spiSpeed,
    };
    // using bulk-transfer we have no option to get the current status of the transmission
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr_burst) < 0) {
        perror("Error: SPI BITSTREAM BURST failed");
        ret = -1;
        goto cleanup;
    }
    fprintf(stdout, "\r[██████████████████████████████████████████████████] %ld/%ld Bytes (100.00%%) - **COMPLETE**\n", bitstream_size, bitstream_size);
	
	// wait 10ms
	usleep(10000);
	
    // read Statusregister
    memset(cmd_buf, 0, sizeof(cmd_buf));
    cmd_buf[0] = 0x3C; // LSC_READ_STATUS
    tr_cmd.len = 8; // 8 Byte for Status-Read
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr_cmd) < 0) {
        perror("Error: SPI LSC_READ_STATUS failed"); goto cleanup;
    }
    uint32_t status;
    memcpy(&status, &rx_buf[4], 4);
    
    // check Status-Bits
    // Bit 8 (DONE) must be 1, Bit 9 (ISC ENABLED) must be 1 sein, Bit 26 (EXECUTION ERROR) must be 0 sein
    int done = (status & (1 << 8)) > 0;
    int isc_enabled = (status & (1 << 9)) > 0;
    int exec_error = (status & (1 << 26)) > 0;

    fprintf(stdout, "  Status Register Check:\n");
    fprintf(stdout, "    DONE: %d\n", done);
    fprintf(stdout, "    ISC Enabled: %d\n", isc_enabled);
    fprintf(stdout, "    Execution Error: %d\n", exec_error);
    fprintf(stdout, "    Status Register [31..0]: ");
    printBits(sizeof(uint32_t), &status);
    fprintf(stdout, "\n");

    if (!done || exec_error) {
        fprintf(stderr, "Configuration failed! Status indicates error or not done.\n");
        ret = -1;
    } else {
        fprintf(stdout, "\n✅ **SUCCESS:** Configuration complete and DONE bit set.\n");
        ret = 0;
    }

    // send ISC_DISABLE
    memset(cmd_buf, 0, sizeof(cmd_buf));
    cmd_buf[0] = 0x26; // ISC_DISABLE
    tr_cmd.len = 4;
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr_cmd) < 0) {
        perror("Error: SPI ISC_DISABLE failed");
    }
    fprintf(stdout, "  ISC_DISABLE sent. FPGA should now be configured.\n");
	
cleanup:
    if (bitstream_file) fclose(bitstream_file);
    if (spi_fd >= 0) close(spi_fd);

    return ret;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <bitstream.bit>\n", argv[0]);
        return 1;
    }

    return configure_lattice_spi(argv[1]);
}
