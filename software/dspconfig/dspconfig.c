// DSP Configuration-tool for the Analog Devices SHARC 21371 DSP
// v0.0.1, 03.09.2025
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
#define SPI_DEVICE_A "/dev/spidev1.0"
#define SPI_DEVICE_B "/dev/spidev1.1"
#define SPI_SPEED_HZ 10000000 // 10 MHz

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
int configure_dsp_spi(const char *bitstream_path_a, const char *bitstream_path_b, uint8_t numStreams) {
    int spi_fd[2] = {-1};
	FILE *bitstream_file[2] = {NULL};
    struct spi_ioc_transfer tr = {0};
    uint8_t tx_buffer[4096];
    uint8_t rx_buffer[sizeof(tx_buffer)]; // not used here, but necessary
    int ret = 0;
    uint8_t buf[1024];
    uint16_t offset = 0;
    size_t bytes_read;
	long file_size[2];
	long total_bytes_sent = 0;
	int progress_bar_width = 50;

    uint8_t spiMode = SPI_MODE_3; // AnalogDevices uses MODE 2
    uint8_t spiBitsPerWord = 8;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    fprintf(stdout, "DSP Configuration Tool v0.0.1\n");
    fprintf(stdout, "  Connecting to SPI...\n");
    spi_fd[0] = open(SPI_DEVICE_A, O_RDWR);
    if (spi_fd[0] < 0) {
        perror("Error: Could not open SPI-device A");
        ret = -1; goto cleanup_gpio;
    }
	if (numStreams == 2) {
		spi_fd[1] = open(SPI_DEVICE_B, O_RDWR);
		if (spi_fd[1] < 0) {
			perror("Error: Could not open SPI-device A");
			ret = -1; goto cleanup_gpio;
		}
	}
    // SPI-Modus 3 (3 = CPOL=1, CPHA=1)
    ioctl(spi_fd[0], SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd[0], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd[0], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    fprintf(stdout, "  SPI-Bus '%s' initialized. (Mode %d, Speed %d Hz).\n", SPI_DEVICE_A, spiMode, spiSpeed);
	if (numStreams == 2) {
		ioctl(spi_fd[1], SPI_IOC_WR_MODE, &spiMode);
		ioctl(spi_fd[1], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
		ioctl(spi_fd[1], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
		fprintf(stdout, "  SPI-Bus '%s' initialized. (Mode %d, Speed %d Hz).\n", SPI_DEVICE_B, spiMode, spiSpeed);
	}

    // resetting DSPs
    fprintf(stdout, "  Resetting DSPs and start upload...\n");
    int fd = open("/sys/class/leds/reset_dsp/brightness", O_WRONLY);
    write(fd, "1", 1);
    usleep(5 * 1000);
    write(fd, "0", 1);
    close(fd);

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
		tr.tx_buf = (unsigned long)tx_buffer;
		tr.rx_buf = (unsigned long)rx_buffer;
		tr.bits_per_word = spiBitsPerWord;
		tr.speed_hz = spiSpeed;

		// send data
		total_bytes_sent = 0;
		progress_bar_width = 50;
		while ((bytes_read = fread(tx_buffer, 1, sizeof(tx_buffer), bitstream_file[i])) > 0) {
			tr.len = bytes_read;
			ret = ioctl(spi_fd[i], SPI_IOC_MESSAGE(1), &tr);
			if (ret < 0) {
				perror("Error: SPI-transmission failed\n");
				goto cleanup;
			}

			// calculate progress-bar
			total_bytes_sent += bytes_read;
			int progress = (int)((double)total_bytes_sent / file_size[i] * progress_bar_width);
			printf("\r[");
			for (int i = 0; i < progress_bar_width; ++i) {
				if (i < progress) {
					printf("█");
				}else{
					printf(" ");
				}
			}
			printf("] %ld/%ld Bytes (%.2f%%)", total_bytes_sent, file_size[i], (double)total_bytes_sent / file_size[i] * 100);
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
