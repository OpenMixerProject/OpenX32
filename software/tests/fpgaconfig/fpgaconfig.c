// FPGA Configuration-tool for the Xilinx Spartan 3A X3CS1400A
// v0.1.0, 24.07.2025
//
// This software reads a *.bit or *.bin file from Xilinx ISE 14.7 and sends it using
// the SPI-connection /dev/spidev2.0 (CSPI3-connection of i.MX25)
// it waits for "DONE"-pin to get HIGH and quits
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
//#include <gpiod.h>

// SPI configuration for i.MX25
#define SPI_DEVICE "/dev/spidev2.0"
#define SPI_SPEED_HZ 10000000 // 10 MHz (Spartan 3A should be able up to 50 MHz)

// GPIO configuration for i.MX25
// nPROG_B is on GPIO 1:30
// nCS0 is on GPIO 1:31
// nCS1 is on GPIO 1:6
// DONE is on GPIO 1:2
// nINIT_B is on GPIO 2:29
#define PROG_B_GPIO_OFFSET 30
#define DONE_GPIO_OFFSET 2

// ----------------------------------------------

long get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1; // Fehler
}

// configures a Xilinx Spartan 3A via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
int configure_spartan3a_spi(const char *bitstream_path) {
    int spi_fd = -1;
    FILE *bitstream_file = NULL;
    struct spi_ioc_transfer tr = {0};
    uint8_t tx_buffer[4096];
    uint8_t rx_buffer[sizeof(tx_buffer)]; // not used here, but necessary
    int ret = 0;
    uint8_t buf[1024];
    uint16_t offset = 0;
    size_t bytes_read;

    uint8_t spiMode = SPI_MODE_0; // Xilinx Spartan-3A uses MODE 0
    uint8_t spiBitsPerWord = 8;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    // libgpiod variables
/*
    struct gpiod_chip *gpio0_chip = NULL;
    struct gpiod_line *prog_b_line = NULL;
    struct gpiod_line *done_line = NULL;
*/

    fprintf(stdout, "FPGA Configuration Tool v0.1.0\n");

/*
    fprintf(stdout, "  Connecting to GPIOs...\n");
    gpio0_chip = gpiod_chip_open_by_name("gpiochip0");
    prog_b_line = gpiod_chip_get_line(gpio0_chip, PROG_B_GPIO_OFFSET);
    done_line = gpiod_chip_get_line(gpio0_chip, DONE_GPIO_OFFSET);

    // check if we could connect to all GPIOs
    if ((!gpio0_chip) || (!prog_b_line) || (!done_line)) {
        perror("Error: Could not open GPIO-pins using libgpiod!");
        gpiod_line_release(prog_b_line);
		gpiod_line_release(done_line);
        gpiod_chip_close(gpio0_chip);
        return -1;
    }

    // connect as input/output
    if (gpiod_line_request_output(prog_b_line, "spartan3a_prog_b", 1) < 0) {
        perror("Error: Cannot configure PROG_B as output");
        ret = -1; goto cleanup_gpio;
    }
    if (gpiod_line_request_input(done_line, "spartan3a_done") < 0) {
        perror("Error: Cannot configure DONE as input");
        ret = -1; goto cleanup_gpio;
    }
*/

    fprintf(stdout, "  Connecting to SPI...\n");
    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0) {
        perror("Error: Could not open SPI-device");
        ret = -1; goto cleanup_gpio;
    }

    // SPI-Modus (0 = CPOL=0, CPHA=0; Xilinx FPGAs oft Modus 0 oder 2)
    ioctl(spi_fd, SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    fprintf(stdout, "  SPI-Bus '%s' initialized. (Mode %d, Speed %d Hz).\n", SPI_DEVICE, spiMode, spiSpeed);

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

    // now open the file again and jump of the header (if any)
    fprintf(stdout, "Configuring Xilinx Spartan-3A...\n");
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
    fprintf(stdout, "Send bitstream '%s' to FPGA...\n", bitstream_path);
    tr.tx_buf = (unsigned long)tx_buffer;
    tr.rx_buf = (unsigned long)rx_buffer;
    tr.bits_per_word = spiBitsPerWord;
    tr.speed_hz = spiSpeed;

    fprintf(stdout, "  Setting PROG_B-Sequence HIGH -> LOW -> HIGH and start upload...\n");
    int fd = open("/sys/class/leds/reset_fpga/brightness", O_WRONLY);
    write(fd, "1", 1);
    usleep(500);
    write(fd, "0", 1);
    close(fd);
    usleep(500);

/*
    gpiod_line_set_value(prog_b_line, 0);
    usleep(5); // wait at least 300ns-500ns (here 5us) to restart configuration-process (see Xilinx UG332 for more information)
    gpiod_line_set_value(prog_b_line, 1);
    usleep(1000); // wait at least 200ns (here 1ms)
*/

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
                printf("█");
            }else{
                printf(" ");
            }
        }
        printf("] %ld/%ld Bytes (%.2f%%)", total_bytes_sent, file_size, (double)total_bytes_sent / file_size * 100);
        fflush(stdout);
    }

    fprintf(stdout, "\n\nBitstream transmitted.\n");

/*
    // write additional dummy-data (ones = 0xFF = 0b11111111) to finalize configuration
    tr.len = 8; // eight additional bytes
    for (uint16_t i=0; i<100; i++) {
        tx_buffer[i] = 0xFF;
        rx_buffer[i] = 0;
    }
    ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
*/

/*
    // check DONE-pin
    fprintf(stdout, "Checking DONE-pin...\n");
    usleep(100 * 1000); // 100ms warten

    int done_value = gpiod_line_get_value(done_line);
    if (done_value == 1) {
        fprintf(stdout, "Success! FPGA configured successfully! DONE-Pin is HIGH.\n");
        ret = 0;
    } else if (done_value == 0) {
        fprintf(stderr, "Error: FPGA configuration failed! DONE-Pin is still LOW.\n");
        ret = -1;
    } else {
        perror("Error: Could not read DONE-Pin");
        ret = -1; goto cleanup;
    }
*/

cleanup:
    if (bitstream_file) fclose(bitstream_file);
    if (spi_fd >= 0) close(spi_fd);

cleanup_gpio:
/*
    if (prog_b_line) gpiod_line_release(prog_b_line);
    if (done_line) gpiod_line_release(done_line);
    if (gpio0_chip) gpiod_chip_close(gpio0_chip);
*/

    return ret;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <bitstream.bit>\n", argv[0]);
        return 1;
    }

    return configure_spartan3a_spi(argv[1]);
}
