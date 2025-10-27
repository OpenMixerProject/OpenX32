// FPGA Configuration-tool for the Lattice ECP5 FPGAs
// v0.1.0, 27.10.2025
//
// This software reads a bitstream from Lattice Diamond and sends it using
// the SPI-connection /dev/spidev2.0 (CSPI3-connection of i.MX25)
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
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

// configures a Lattice ECP5 via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
int configure_lattice_spi(const char *bitstream_path) {
    int spi_fd = -1;
    FILE *bitstream_file = NULL;
    struct spi_ioc_transfer tr = {0};
    uint8_t tx_buffer[4096];
    uint8_t rx_buffer[sizeof(tx_buffer)]; // not used here, but necessary
    int ret = 0;
    uint8_t buf[1024];
    uint16_t offset = 0;
    size_t bytes_read;

    uint8_t spiMode = SPI_MODE_0; // Lattice ECP5 uses MODE 0
    uint8_t spiBitsPerWord = 8;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    fprintf(stdout, "FPGA Configuration Tool v0.1.0\n");

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
    fprintf(stdout, "Configuring Lattice FPGA...\n");
    long file_size = get_file_size(bitstream_path);
    bitstream_file = fopen(bitstream_path, "rb");
    if (!bitstream_file) {
        perror("Error: Could not open bitstream-file");
        ret = -1; goto cleanup;
    }

    // configuration-process
    fprintf(stdout, "Send bitstream '%s' to FPGA...\n", bitstream_path);
    tr.tx_buf = (unsigned long)tx_buffer;
    tr.rx_buf = (unsigned long)rx_buffer;
    tr.bits_per_word = spiBitsPerWord;
    tr.speed_hz = spiSpeed;

    fprintf(stdout, "  Setting PROGRAMN-Sequence HIGH -> LOW -> HIGH and start upload...\n");
    int fd = open("/sys/class/leds/reset_fpga/brightness", O_WRONLY);
    write(fd, "1", 1);
    usleep(500); // we have to keep at least 25ns. So 500us is more than enough
    write(fd, "0", 1);
    close(fd);
    usleep(50000); // we have to wait 50ms until we can send commands
	
	// send READ_ID command [class A command] -> read 32-bit IDCODE and check it
	tr.len = 8; // 8-bit command + 24-bit dummy-data + 32-bit read-data
	memset(&tx_buffer[0], 0, 8); // reset tx-buffer to 0x00
	tx_buffer[0] = 0xE0; // command = READ_ID
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 0) {
		perror("Error: SPI-transmission failed");
		goto cleanup;
	}
	uint32_t idcode;
	memcpy(&idcode, &rx_buffer[4], 4); // copy received IDCODE to variable
//	if (idcode != 0x00000000) {
//		perror("Error: Unexpected IDCODE");
//		goto cleanup;
//	}
	
	// send ISC_ENABLE command [class C command]
	tr.len = 4; // 8-bit command + 24-bit dummy-data
	memset(&tx_buffer[0], 0, 4); // reset tx-buffer to 0x00
	tx_buffer[0] = 0xC6; // command = ISC_ENABLE
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 0) {
		perror("Error: SPI-transmission failed");
		goto cleanup;
	}

	// send LSC_BITSTREAM_BURST command [class C command]
	tr.len = 4; // 8-bit command + 24-bit dummy-data
	memset(&tx_buffer[0], 0, 4); // reset tx-buffer to 0x00
	tx_buffer[0] = 0x7A; // command = LSC_BITSTREAM_BURST
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 0) {
		perror("Error: SPI-transmission failed");
		goto cleanup;
	}

    // send bitstream-data via SPI
    long total_bytes_sent = 0;
    int progress_bar_width = 50;
    while ((bytes_read = fread(tx_buffer, 1, sizeof(tx_buffer), bitstream_file)) > 0) {
        tr.len = bytes_read;
        ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 0) {
            perror("Error: SPI-transmission failed");
            goto cleanup;
        }

        // calculate progress-bar
        total_bytes_sent += bytes_read;
        int progress = (int)((double)total_bytes_sent / file_size * progress_bar_width);
        printf("\r[");
        for (int i = 0; i < progress_bar_width; ++i) {
            if (i < progress) {
                printf("█");
            }else{
                printf(" ");
            }
        }
        printf("] %ld/%ld Bytes (%.2f%%)", total_bytes_sent, file_size, (double)total_bytes_sent / file_size * 100);
        fflush(stdout);
    }
	
	// wait 10ms
	usleep(10000);
	
	// check status register
	tr.len = 8; // 8-bit command + 24-bit dummy-data + 32-bit read-data
	memset(&tx_buffer[0], 0, 8); // reset tx-buffer to 0x00
	tx_buffer[0] = 0x3C; // command = LSC_READ_STATUS
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 0) {
		perror("Error: SPI-transmission failed");
		goto cleanup;
	}
	uint32_t status;
	memcpy(&status, &rx_buffer[4], 4); // copy received status to variable
	// check if bit 8 (DONE) is set
	// check if bit 9 (ISC ENABLED) is set
	// check if bit 26 (EXECUTION ERROR) is not set
	if ( !((status & (1 << 8)) && (status & (1 << 9))) || (status & (1 << 26)) ) {
		perror("Error: Unexpected content in status-register");
		goto cleanup;
	}
	
	// send ISC_DISABLE command [class C command]
	tr.len = 4; // 8-bit command + 24-bit dummy-data
	memset(&tx_buffer[0], 0, 4); // reset tx-buffer to 0x00
	tx_buffer[0] = 0x26; // command = ISC_DISABLE
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 0) {
		perror("Error: SPI-transmission failed");
		goto cleanup;
	}

    fprintf(stdout, "\n\nBitstream transmitted.\n");

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
