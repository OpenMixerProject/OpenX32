// This software connectes to a specific SPI-interface as SPI-slave and tries to read individual bytes from a SPI-Master
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
#define SPI_SPEED_HZ 10000000 // 10 MHz

// ----------------------------------------------

int spi_read(const char *spidev) {
    int spi_fd = -1;
    struct spi_ioc_transfer tr = {0};
    uint8_t spiTxData[16];
    uint8_t spiRxData[sizeof(spiTxData)];
    int ret = 0;
    size_t bytesRead;

    uint8_t spiMode = SPI_MODE_0; // user-program uses SPI MODE 0
    uint8_t spiBitsPerWord = 32;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    fprintf(stdout, "SPI Reader v0.0.1\n");
    fprintf(stdout, "  Connecting to SPI %s...\n", spidev);
    spi_fd = open(spidev, O_RDWR);
    if (spi_fd < 0) {
        perror("Error: Could not open SPI-device");
        return -1;
    }

    // SPI-Modus 3 (3 = CPOL=1, CPHA=1)
    ioctl(spi_fd, SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    fprintf(stdout, "  SPI-Bus '%s' initialized. (Mode %d, Speed %d Hz).\n", spidev, spiMode, spiSpeed);

    tr.tx_buf = (unsigned long)spiTxData;
    tr.rx_buf = (unsigned long)spiRxData;
    tr.len = sizeof(spiTxData);
    tr.bits_per_word = spiBitsPerWord;
    tr.speed_hz = spiSpeed;


/*
    // now read data from SPI-slave
    spiTxData[0] = 0;
    while(1) {
        ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 0) {
            perror("Error: SPI-transmission failed\n");
        }else{
            uint32_t dataTmp;
            memcpy(&dataTmp, &spiRxData[0], 4);
            fprintf(stdout, "Requested data from register 0x%x. Received %u bytes: data = 0x%08x\n", spiTxData[0], ret, dataTmp);
        }

        // increase register-pointer. We have 20 registers to read
        spiTxData[0]++;
        if (spiTxData[0] > 19) {
            spiTxData[0] = 0;
        }

        sleep(1); // wait a second
    }
*/

    // send a command via SPI to DSP
    uint32_t spiData_TX[4];
    uint32_t spiData_RX[4];
    spiData_TX[0] = 0x0000002A; // StartMarker = '*'
    spiData_TX[1] = 0x00000042; // parameter (0x42 = LED)
    spiData_TX[2] = 0x00000002; // value (0x02 = toggle LED)
    spiData_TX[3] = 0x00000023; // EndMarker = '#'
    memcpy(&spiTxData[0], &spiData_TX[0], 16);

    while(1) {
        ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr); // send 4 values (16 bytes) via SPI
        memcpy(&spiData_RX[0], &spiRxData[0], 16); // copy received data to uint32_t-array

        fprintf(stdout, "Sent data = 0x%04x%04x%04x%04x. Received %u bytes. Received data = 0x%04x%04x%04x%04x\n", spiData_TX[0], spiData_TX[1], spiData_TX[2], spiData_TX[3], ret, spiData_RX[0], spiData_RX[1], spiData_RX[2], spiData_RX[3]);
        sleep(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s /dev/spidevX.Y\n", argv[0]);
        return 1;
    }

    return spi_read(argv[1]);
}
