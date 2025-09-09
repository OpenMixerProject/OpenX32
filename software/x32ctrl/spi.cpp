#include "x32ctrl.h"

// two defines for FPGA-configuration
#define PROG_B_GPIO_OFFSET 30
#define DONE_GPIO_OFFSET 2
#define FPGA_BUFFER_SIZE 4096
#define FPGA_FILE_BUFFER_SIZE 1024

// configures a Xilinx Spartan 3A via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
int spiConfigureFpga(const char *bitstream_path) {
    int spi_fd = -1;
    FILE *bitstream_file = NULL;
    int ret = 0;
    struct spi_ioc_transfer tr = {0};
    uint8_t* tx_buffer = (uint8_t*)malloc(FPGA_BUFFER_SIZE); // use heap instead of stack
    uint8_t* rx_buffer = (uint8_t*)malloc(FPGA_BUFFER_SIZE); // use heap instead of stack
    uint8_t* file_buffer = (uint8_t*)malloc(FPGA_FILE_BUFFER_SIZE); // use heap instead of stack
    uint16_t offset = 0;
    size_t bytes_read;

    uint8_t spiMode = SPI_MODE_0; // Xilinx Spartan-3A uses MODE 0
    uint8_t spiBitsPerWord = 8;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    x32debug("Connecting to SPI for FPGA...\n");
    spi_fd = open(SPI_DEVICE_FPGA, O_RDWR);
    if (spi_fd < 0) {
        x32log("Error: Could not open SPI-device\n");
        free(tx_buffer);
        free(rx_buffer);
        free(file_buffer);
        return -1;
    }

    // SPI-Modus (0 = CPOL=0, CPHA=0; Xilinx FPGAs oft Modus 0 oder 2)
    ioctl(spi_fd, SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);

    // read bitstream-file and search for RAW-data
    long file_size = getFileSize(bitstream_path);
    if (file_size <= 0) {
        x32log("Error: Problem with bitstream-file\n");
        free(tx_buffer);
        free(rx_buffer);
        free(file_buffer);
        return -1;
    }

    bitstream_file = fopen(bitstream_path, "rb");
    if (!bitstream_file) {
        x32log("Error: Could not open bitstream-file\n");
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        free(tx_buffer);
        free(rx_buffer);
        free(file_buffer);
        return -1;
    }
    if ((bytes_read = fread(file_buffer, 1, FPGA_FILE_BUFFER_SIZE, bitstream_file)) > 0) {
        if (file_buffer[0] != 0xFF) {
            // skip header
            offset = ((file_buffer[0]<<8) | file_buffer[1]) + 4;

            // find 'e' record in header
            while (file_buffer[offset] != 0x65) {
                // skip the record
                offset += (file_buffer[offset+1]<<8 | file_buffer[offset+2]) + 3;

                // exit if the next record isn't within the buffer
                if (offset >= 1022) {
                    x32log("Error: Cannot find expected header in bit-file!\n");
                    if (bitstream_file) fclose(bitstream_file);
                    if (spi_fd >= 0) close(spi_fd);
                    free(tx_buffer);
                    free(rx_buffer);
                    free(file_buffer);
                    return -1;
		}
            }

            // skip the field name and bitstrem length
            offset += 5;

            x32debug("Detected bit-file with header...\n");
	}else{
            x32debug("Detected bin-file without header...\n");
            offset = 0; // start reading at byte 0
	}
    }
    fclose(bitstream_file);

    // now open the file again and jump of the header (if any)
    x32debug("Configuring Xilinx Spartan-3A...\n");
    bitstream_file = fopen(bitstream_path, "rb");
    if (!bitstream_file) {
        x32log("Error: Could not open bitstream-file\n");
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        free(tx_buffer);
        free(rx_buffer);
        free(file_buffer);
        return -1;
    }
	// read number of header-bytes to buffer (will remain unused and will be overwritten)
	if (offset > 0) {
	    fread(tx_buffer, 1, offset, bitstream_file);
	}

    // now send the data
    x32debug("Sending bitstream to FPGA...\n");
    tr.tx_buf = (unsigned long)tx_buffer;
    tr.rx_buf = (unsigned long)rx_buffer;
    tr.bits_per_word = spiBitsPerWord;
    tr.speed_hz = spiSpeed;

    x32debug("Setting PROG_B-Sequence HIGH -> LOW -> HIGH and start upload...\n");
    int fdResetFpga = open("/sys/class/leds/reset_fpga/brightness", O_WRONLY);
    write(fdResetFpga, "1", 1);
    usleep(5 * 1000);
    write(fdResetFpga, "0", 1);
    close(fdResetFpga);

    // send data
    long total_bytes_sent = 0;
    int progress_bar_width = 50;
    while ((bytes_read = fread(tx_buffer, 1, FPGA_BUFFER_SIZE, bitstream_file)) > 0) {
        tr.len = bytes_read;
        ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 0) {
            x32log("Error: SPI-transmission failed\n");
            if (bitstream_file) fclose(bitstream_file);
            if (spi_fd >= 0) close(spi_fd);
            free(tx_buffer);
            free(rx_buffer);
            free(file_buffer);
            return -1;
        }

        // calculate progress-bar
        total_bytes_sent += bytes_read;
        int progress = (int)((double)total_bytes_sent / file_size * progress_bar_width);
        printf("\rFPGA [");
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
    printf("\n");

/*
    // write additional dummy-data (ones = 0xFF = 0b11111111) to finalize configuration
    tr.len = 8; // eight additional bytes
    for (uint16_t i=0; i<100; i++) {
        tx_buffer[i] = 0xFF;
        rx_buffer[i] = 0;
    }
    ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
*/
    free(tx_buffer);
    free(rx_buffer);
    free(file_buffer);

    return 0;
}


// configures AnalogDevices 21371 SHARC DSP via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
// callit like: spiConfigureDsp(argv[1], argv[2], 2)
int spiConfigureDsp(const char *bitstream_path_a, const char *bitstream_path_b, uint8_t numStreams) {
    if ((numStreams < 1) || (numStreams > 2)) {
        return -1;
    }

    int spi_fd[2] = {-1};
    FILE *bitstream_file[2] = {NULL};
    int ret = 0;
    struct spi_ioc_transfer tr = {0};
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

    x32debug("Connecting to SPI for DSP1...\n");
    spi_fd[0] = open(SPI_DEVICE_DSP1, O_RDWR);
    if (spi_fd[0] < 0) {
        x32log("Error: Could not open SPI-device for DSP1\n");
        return -1;
    }
    if (numStreams == 2) {
        x32debug("Connecting to SPI for DSP2...\n");
	spi_fd[1] = open(SPI_DEVICE_DSP2, O_RDWR);
	if (spi_fd[1] < 0) {
            x32log("Error: Could not open SPI-device for DSP2\n");
            return -1;
        }
    }
    // SPI-Modus 3 (3 = CPOL=1, CPHA=1)
    ioctl(spi_fd[0], SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd[0], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd[0], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    ioctl(spi_fd[0], SPI_IOC_WR_LSB_FIRST, &spiLsbFirst);
    if (numStreams == 2) {
        ioctl(spi_fd[1], SPI_IOC_WR_MODE, &spiMode);
        ioctl(spi_fd[1], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
        ioctl(spi_fd[1], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
        ioctl(spi_fd[1], SPI_IOC_WR_LSB_FIRST, &spiLsbFirst);
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
    int fdResetDsp = open("/sys/class/leds/reset_dsp/brightness", O_WRONLY);
    write(fdResetDsp, "1", 1);
    usleep(10 * 1000);
    write(fdResetDsp, "0", 1);
    close(fdResetDsp);
    usleep(10 * 1000);

	// read bitstream-files
	file_size[0] = getFileSize(bitstream_path_a);
	if (file_size[0] <= 0) {
		x32log("Error: Problem with bitstream-file\n");
      		return -1;
 	}
	if (numStreams == 2) {
		file_size[1] = getFileSize(bitstream_path_b);
		if (file_size[1] <= 0) {
			x32log("Error: Problem with bitstream-file\n");
      			return -1;
	 	}
	}

	for (uint8_t i = 0; i < numStreams; i++) {
		bitstream_file[i] = fopen(bitstream_path_a, "rb");
		if (!bitstream_file[i]) {
			x32log("Error: Could not open bitstream-file\n");
		        for (uint8_t i_b = 0; i_b < numStreams; i_b++) {
		                if (bitstream_file[i_b]) fclose(bitstream_file[i_b]);
                		if (spi_fd[i_b] >= 0) close(spi_fd[i_b]);
		        }
			return -1;
		}

		// now send the data
		x32debug("Sending bitstream to DSP...\n");
		totalBytesSent = 0;
		progress_bar_width = 50;
		while ((bytesRead = fread(&spiTxData[0], 1, sizeof(spiTxData), bitstream_file[i])) > 0) {
                        tr.len = bytesRead;

                        // reverse bitorder as linux-driver seems to have no support for this
                        reverseBitOrderArray(&spiTxData[0], bytesRead);

			ret = ioctl(spi_fd[i], SPI_IOC_MESSAGE(1), &tr);
			if (ret < 0) {
				x32log("Error: SPI-transmission failed\n");
			        for (uint8_t i_b = 0; i_b < numStreams; i_b++) {
		        	        if (bitstream_file[i_b]) fclose(bitstream_file[i_b]);
                			if (spi_fd[i_b] >= 0) close(spi_fd[i_b]);
			        }
				return ret;
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
			printf("\rDSP1 [");
			for (int i_progress = 0; i_progress < progress_bar_width; ++i_progress) {
				if (i_progress < progress) {
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

	return 0;
}

