// DSP Configuration-tool for the AnalogDevices 21371 SHARC DSP
// v0.0.1, 28.08.2025
//
// This software reads files from SigmaStudio 4.7 / CrossCore EmbeddedStudio
// and transmit them using SPI-connection /dev/spidev1.0 and /dev/spidev1.1
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

// include SigmaStudio files
#include "sigma_test_IC_1.h"

// SPI configuration for i.MX25
#define SPI_DEVICE_A "/dev/spidev1.0"
#define SPI_DEVICE_B "/dev/spidev1.1"
#define SPI_SPEED_HZ 10000000 // 10 MHz (AnalogDevices 21371 should be able up to 50 MHz)

// ----------------------------------------------

long get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1; // Fehler
}

// configures AnalogDevices 21371 SHARC DSP via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
int configure_dsp_spi(const char *bitstream_path) {
    int spi_fd = -1;
    FILE *bitstream_file = NULL;
    struct spi_ioc_transfer tr = {0};
    uint8_t tx_buffer[4096];
    uint8_t rx_buffer[sizeof(tx_buffer)]; // not used here, but necessary
    int ret = 0;
    uint8_t buf[1024];
    uint16_t offset = 0;
    size_t bytes_read;

    uint8_t spiMode = SPI_MODE_0; // AnalogDevices uses MODE 0
    uint8_t spiBitsPerWord = 8;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    fprintf(stdout, "DSP Configuration Tool v0.0.1\n");

    fprintf(stdout, "  Connecting to SPI...\n");
    spi_fd = open(SPI_DEVICE_A, O_RDWR);
    if (spi_fd < 0) {
        perror("Error: Could not open SPI-device");
        ret = -1; goto cleanup_gpio;
    }

    // SPI-Modus (0 = CPOL=0, CPHA=0)
    ioctl(spi_fd, SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    fprintf(stdout, "  SPI-Bus '%s' initialized. (Mode %d, Speed %d Hz).\n", SPI_DEVICE_A, spiMode, spiSpeed);

/*
    // read bitstream-file and search for RAW-data
    fprintf(stdout, "Checking input file...\n");
    long file_size = get_file_size(bitstream_path);
    bitstream_file = fopen(bitstream_path, "rb");
    if (!bitstream_file) {
        perror("Error: Could not open bitstream-file");
        ret = -1; goto cleanup;
    }
    if ((bytes_read = fread(buf, 1, sizeof(buf), bitstream_file)) > 0) {
        if (buf[0] != 0xFF) {
            // skip header
            offset = ((buf[0]<<8) | buf[1]) + 4;

            // find 'e' record in header
            while (buf[offset] != 0x65) {
                // skip the record
                offset += (buf[offset+1]<<8 | buf[offset+2]) + 3;

                // exit if the next record isn't within the buffer
                if (offset >= 1022) {
                    perror("Error: Cannot find expected header in bit-file!");
                    ret -1; goto cleanup;
		}
            }

            // skip the field name and bitstrem length
            offset += 5;

            fprintf(stdout, "Detected bit-file with %d bytes of header...\n", offset);
	}else{
            fprintf(stdout, "Detected bin-file without header...\n");
            offset = 0; // start reading at byte 0
	}
    }
    fclose(bitstream_file);
*/

/*
    // now open the file again and jump of the header (if any)
    fprintf(stdout, "Configuring AnalogDevices 21371 SHARC DSP...\n");
    bitstream_file = fopen(bitstream_path, "rb");
    if (!bitstream_file) {
        perror("Error: Could not open bitstream-file");
        ret = -1; goto cleanup;
    }
	// read number of header-bytes to buffer (will remain unused and will be overwritten)
	if (offset > 0) {
	    fread(tx_buffer, 1, offset, bitstream_file);
	}

    // now send the data
    fprintf(stdout, "Send bitstream '%s' to AnalogDevices 21371 SHARC DSP...\n", bitstream_path);
    tr.tx_buf = (unsigned long)tx_buffer;
    tr.rx_buf = (unsigned long)rx_buffer;
    tr.bits_per_word = spiBitsPerWord;
    tr.speed_hz = spiSpeed;

    fprintf(stdout, "  Setting PROG_B-Sequence HIGH -> LOW -> HIGH and start upload...\n");
    int fd = open("/sys/class/leds/reset_dsp/brightness", O_WRONLY);
    write(fd, "1", 1);
    usleep(5 * 1000);
    write(fd, "0", 1);
    close(fd);
*/

/*
    // send data
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
                printf("#");
            }else{
                printf(" ");
            }
        }
        printf("] %ld/%ld Bytes (%.2f%%)", total_bytes_sent, file_size, (double)total_bytes_sent / file_size * 100);
        fflush(stdout);
    }

    fprintf(stdout, "\n\nBitstream transmitted.\n");
*/

cleanup:
    if (bitstream_file) fclose(bitstream_file);
    if (spi_fd >= 0) close(spi_fd);

cleanup_gpio:

    return ret;
}


int main(int argc, char *argv[]) {
    return configure_dsp_spi(argv[1]);
}
