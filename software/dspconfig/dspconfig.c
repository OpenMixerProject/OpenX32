// DSP Configuration-tool for the Analog Devices SHARC 21371 DSP
// v0.1.0, 04.09.2025
//
// This software reads a *.ldr file from CrossCore EmbeddedStudio and sends it using
// the SPI-connection /dev/spidev1.0 and /dev/spidev1.1
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
#define SPI_DEVICE_A "/dev/spidev0.0"
#define SPI_DEVICE_B "/dev/spidev0.1"
#define SPI_SPEED_HZ 10000000 // 10 MHz, but seems to be ignored by Linux-driver

// ----------------------------------------------

long get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1; // Fehler
}

uint32_t reverseBitOrder_uint32(uint32_t n) {
    n = ((n >> 1) & 0x55555555) | ((n & 0x55555555) << 1);
    n = ((n >> 2) & 0x33333333) | ((n & 0x33333333) << 2);
    n = ((n >> 4) & 0x0F0F0F0F) | ((n & 0x0F0F0F0F) << 4);
    n = ((n >> 8) & 0x00FF00FF) | ((n & 0x00FF00FF) << 8);
    n = (n >> 16) | (n << 16);
    return n;
}

void reverseBitOrderArray(uint8_t* data, uint32_t len) {
    // reverse bits in array
    uint32_t* pData = (uint32_t*)data;
    for (uint32_t i = 0; i < (len/4); i++) {
        *pData = reverseBitOrder_uint32(*pData);
        pData++;
    }
}

// configures AnalogDevices 21371 SHARC DSP via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
int configure_dsp_spi(const char *bitstream_path_a, const char *bitstream_path_b, uint8_t numStreams) {
    int spi_fd[2] = {-1};
    FILE *bitstream_file[2] = {NULL};
    struct spi_ioc_transfer tr = {0};
    int ret = 0;
    size_t bytesRead;
    uint8_t spiTxData[64];
    uint8_t spiRxData[sizeof(spiTxData)];
    long file_size[2];
    long totalBytesSent = 0;
    int progress_bar_width = 50;

    uint8_t spiMode = SPI_MODE_3; // AnalogDevices uses MODE 3 (CPOL=1, CPHA=1)
    uint8_t spiBitsPerWord = 32;
    uint32_t spiSpeed = SPI_SPEED_HZ;
    uint8_t spiLsbFirst = 0; // Linux-driver for i.MX25 seems to have problems with this option

    fprintf(stdout, "DSP Configuration Tool v0.0.1\n");
    fprintf(stdout, "  Connecting to SPI %s...\n", SPI_DEVICE_A);
    spi_fd[0] = open(SPI_DEVICE_A, O_RDWR);
    if (spi_fd[0] < 0) {
        perror("Error: Could not open SPI-device A");
        ret = -1; goto cleanup_gpio;
    }
    if (numStreams == 2) {
        fprintf(stdout, "  Connecting to SPI %s...\n", SPI_DEVICE_B);
	spi_fd[1] = open(SPI_DEVICE_B, O_RDWR);
	if (spi_fd[1] < 0) {
            perror("Error: Could not open SPI-device B");
            ret = -1; goto cleanup_gpio;
        }
    }
    // SPI-Modus 3 (3 = CPOL=1, CPHA=1)
    ioctl(spi_fd[0], SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd[0], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd[0], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    ioctl(spi_fd[0], SPI_IOC_WR_LSB_FIRST, &spiLsbFirst);
    fprintf(stdout, "  SPI-Bus '%s' initialized. (Mode %d, Speed %d Hz).\n", SPI_DEVICE_A, spiMode, spiSpeed);
    if (numStreams == 2) {
        ioctl(spi_fd[1], SPI_IOC_WR_MODE, &spiMode);
        ioctl(spi_fd[1], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
        ioctl(spi_fd[1], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
        ioctl(spi_fd[1], SPI_IOC_WR_LSB_FIRST, &spiLsbFirst);
        fprintf(stdout, "  SPI-Bus '%s' initialized. (Mode %d, Speed %d Hz).\n", SPI_DEVICE_B, spiMode, spiSpeed);
    }

    // write dummy-data to setup SPI
    tr.tx_buf = (unsigned long)spiTxData;
    tr.rx_buf = (unsigned long)spiRxData;
    tr.bits_per_word = spiBitsPerWord;
    tr.speed_hz = spiSpeed;
    ioctl(spi_fd[0], SPI_IOC_MESSAGE(1), &tr);
    if (numStreams == 2) {
        ioctl(spi_fd[1], SPI_IOC_MESSAGE(1), &tr);
    }
    usleep(10 * 1000);

    // resetting DSPs
    fprintf(stdout, "  Resetting DSPs and start upload...\n");
    int fd = open("/sys/class/leds/reset_dsp/brightness", O_WRONLY);
    write(fd, "1", 1);
    usleep(10 * 1000);
    write(fd, "0", 1);
    close(fd);
    usleep(50 * 1000);

	// read bitstream-files
	fprintf(stdout, "  Checking input files...\n");
	file_size[0] = get_file_size(bitstream_path_a);
	if (numStreams == 2) {
		file_size[1] = get_file_size(bitstream_path_b);
	}

	for (uint8_t i = 0; i < numStreams; i++) {
		bitstream_file[i] = fopen(bitstream_path_a, "rb");
		if (!bitstream_file[i]) {
			perror("Error: Could not open bitstream-file\n");
			ret = -1; goto cleanup;
		}

		// now send the data
		fprintf(stdout, "Send bitstream to DSP #%u...\n", i+1);
		totalBytesSent = 0;
		progress_bar_width = 50;
		while ((bytesRead = fread(&spiTxData[0], 1, sizeof(spiTxData), bitstream_file[i])) > 0) {
                        tr.len = bytesRead;

                        // reverse bitorder as linux-driver seems to have no support for this
                        reverseBitOrderArray(&spiTxData[0], bytesRead);

			ret = ioctl(spi_fd[i], SPI_IOC_MESSAGE(1), &tr);
			if (ret < 0) {
				perror("Error: SPI-transmission failed\n");
				goto cleanup;
			}

/*
                        // check handshake
                        if ((spiRxData[0] != 0xFF) && (spiRxData[1] != 0xFF) && (spiRxData[2] != 0xFF) && (spiRxData[3] != 0xFF)) {
                            // wait for DSP
                            while(1) {
                                ioctl(fd, SPI_IOC_MESSAGE(1), &tr); // read data from DSP
                                if ((spiRxData[0] == 0xFF) && (spiRxData[1] == 0xFF) && (spiRxData[2] == 0xFF) && (spiRxData[3] == 0xFF)) {
                                    break; // DSP is ready for new data
                                }
                                usleep(100);
                            }
                        }
*/
			// calculate progress-bar
			totalBytesSent += bytesRead;
			int progress = (int)((double)totalBytesSent / file_size[i] * progress_bar_width);
			printf("\r[");
			for (int i = 0; i < progress_bar_width; ++i) {
				if (i < progress) {
					printf("█");
				}else{
					printf(" ");
				}
			}
			printf("] %ld/%ld Bytes (%.2f%%)", totalBytesSent, file_size[i], (double)totalBytesSent / file_size[i] * 100);
			fflush(stdout);
		}
		printf("\n");
	}
    fprintf(stdout, "\n\nBitstream transmitted.\n");

cleanup:
	for (uint8_t i = 0; i < numStreams; i++) {
		if (bitstream_file[i]) fclose(bitstream_file[i]);
		if (spi_fd[i] >= 0) close(spi_fd[i]);
	}

cleanup_gpio:
    return ret;
}


int main(int argc, char *argv[]) {
    if (!((argc == 2) || (argc == 3))) {
        fprintf(stderr, "Usage:\n%s <dsp1.ldr>\n%s <dsp1.ldr> <dsp2.ldr>\n", argv[0], argv[0]);
        return 1;
    }

    return configure_dsp_spi(argv[1], argv[2], (argc-1)); // paths to file 1 and 2, number of streams

	return 0;
}
