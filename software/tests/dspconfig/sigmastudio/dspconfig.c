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

struct spi_ioc_transfer tr = {0};
uint8_t tx_buffer[4096];
uint8_t rx_buffer[sizeof(tx_buffer)]; // not used here, but necessary
int ret = 0;
int spi_fd = -1;

// configures AnalogDevices 21371 SHARC DSP via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
int configure_dsp_spi(const char *bitstream_path) {
    uint8_t spiMode = SPI_MODE_3; // AnalogDevices uses MODE 2
    uint8_t spiBitsPerWord = 8;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    fprintf(stdout, "DSP Configuration Tool v0.0.1\n");
    fprintf(stdout, "  Connecting to SPI...\n");

    spi_fd = open(SPI_DEVICE_A, O_RDWR);
    if (spi_fd < 0) {
        perror("Error: Could not open SPI-device");
        ret = -1;
    }

    // SPI-Modus 3 (3 = CPOL=1, CPHA=1)
    ioctl(spi_fd, SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    fprintf(stdout, "  SPI-Bus '%s' initialized. (Mode %d, Speed %d Hz).\n", SPI_DEVICE_A, spiMode, spiSpeed);

    // prepare buffer
    tr.tx_buf = (unsigned long)tx_buffer;
    tr.rx_buf = (unsigned long)rx_buffer;
    tr.bits_per_word = spiBitsPerWord;
    tr.speed_hz = spiSpeed;

    // send data
    fprintf(stdout, "Sending bitstream to AnalogDevices 21371 SHARC DSP...\n");
    default_download_IC_1();

    return ret;
}

// ============== FUNCTIONS CALLED BY SIGMASTUDIO CODE ==============
int32_t SIGMA_WRITE_REGISTER_BLOCK(uint8_t devAddress, uint16_t address, uint16_t length, ADI_REG_TYPE *pData) {
    printf("SIGMA_WRITE_REGISTER_BLOCK address: %u length: %u pData: %u\n", address, length, pData);

    long total_bytes_sent = 0;
    int progress_bar_width = 50;
    int bytes_read = 0;

    // write addres
    tx_buffer[0] = 0x00;
    tx_buffer[1] = (address >> 8);
    tx_buffer[2] = (address & 0xFF);
    tr.len = 3;
    ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 0) {
            perror("Error: SPI-transmission failed");
        }

    // write data
    for (uint16_t i = 0; i < length; i+=sizeof(tx_buffer)) {
        // copy data to buffer
        if ((i + 1) * sizeof(tx_buffer) <= length) {
            // read and transmit full buffer-size
            bytes_read = sizeof(tx_buffer);
            memcpy(&tx_buffer[0], &pData[i], bytes_read); // dst, src, size
            tr.len = bytes_read;
        }else{
            // read remaining bytes
            bytes_read = length - (i * sizeof(tx_buffer));
            memcpy(&tx_buffer[0], &pData[i], bytes_read); // dst, src, size
            tr.len = bytes_read;
        }

        // send data via SPI
        ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 0) {
            perror("Error: SPI-transmission failed");
            break;
        }

        // calculate progress-bar
        total_bytes_sent += bytes_read;
        int progress = (int)((double)total_bytes_sent / length * progress_bar_width);
        printf("\r[");
        for (int i = 0; i < progress_bar_width; ++i) {
            if (i < progress) {
                printf("█");
            }else{
                printf(" ");
            }
        }
        printf("] %ld/%ld Bytes (%.2f%%)", total_bytes_sent, length, (double)total_bytes_sent / length * 100);
        fflush(stdout);
    }

    printf("\n\n");

    return 0;
}

int32_t SIGMA_WRITE_REGISTER_CONTROL(uint8_t devAddress, uint16_t address, uint16_t length, ADI_REG_U8 *pData) {
    printf("Function SIGMA_WRITE_REGISTER_CONTROL() is not implemented");
    return 0;
}

int32_t SIGMA_SAFELOAD_WRITE_ADDR(uint8_t devAddress, uint16_t addrAddress, uint16_t address) {
    printf("Function SIGMA_SAFELOAD_WRITE_ADDR() is not implemented");
    return 0;
}

int32_t SIGMA_SAFELOAD_WRITE_DATA(uint8_t devAddress, uint16_t dataAddress, uint16_t length, ADI_REG_U8 *pData) {
    printf("Function SIGMA_SAFELOAD_WRITE_DATA() is not implemented");
    return 0;
}

int32_t SIGMA_SAFELOAD_WRITE_TRANSFER_BIT(uint8_t devAddress) {
    printf("Function SIGMA_SAFELOAD_WRITE_TRANSFER_BIT() is not implemented");
    return 0;
}

// ============== MAIN FUNCTION ==============

int main(int argc, char *argv[]) {
    ret = configure_dsp_spi(argv[1]);
    close(spi_fd);
    fprintf(stdout, "\nDone.\n");

    return ret;
}
