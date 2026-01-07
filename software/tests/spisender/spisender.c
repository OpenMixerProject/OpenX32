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
#define SPI_FPGA_SPEED_HZ 1000000
#define SPI_DEVICE_FPGA "/dev/spidev2.0"

// ----------------------------------------------

int spiFpgaHandle;

void setFpgaChipSelectPin(bool state) {
    int fd = open("/sys/class/leds/cs_fpga/brightness", O_WRONLY);
    if (state) {
        write(fd, "1", 1); // assert ChipSelect (sets the real pin to LOW)
    }else{
        write(fd, "0", 1); // deassert ChipSelect (sets the real pin to HIGH)
    }
    close(fd);
}

bool SendFgpaData(uint8_t txData[], uint8_t rxData[], uint8_t len) {
    struct spi_ioc_transfer tr = {0};

    // configure SPI-system for this transmission
    tr.tx_buf = (unsigned long)txData;
    tr.rx_buf = (unsigned long)rxData;
    tr.bits_per_word = 8;
    tr.delay_usecs = 0; // microseconds to delay after this transfer before (optionally) changing the chipselect status
    tr.cs_change = 0; // disable CS between two messages
    tr.len = len;
    tr.speed_hz = SPI_FPGA_SPEED_HZ;

    setFpgaChipSelectPin(true); // assert ChipSelect
    int ret = ioctl(spiFpgaHandle, SPI_IOC_MESSAGE(1), &tr); // send via SPI
	setFpgaChipSelectPin(false); // deassert ChipSelect

    return (ret >= 0);
}

int main(int argc, char *argv[]) {
    uint8_t spiMode = SPI_MODE_0; // user-program uses SPI MODE 0
    uint8_t spiBitsPerWord = 8; // we are using standard 8-bit-mode here for communication
    uint32_t spiSpeed = SPI_FPGA_SPEED_HZ;

    spiFpgaHandle = open(SPI_DEVICE_FPGA, O_RDWR);
    if (spiFpgaHandle < 0) {
        return false;
    }

    ioctl(spiFpgaHandle, SPI_IOC_WR_MODE, &spiMode);
    ioctl(spiFpgaHandle, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spiFpgaHandle, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);

	uint8_t txData[2];
	uint8_t rxData[2];

	while(1) {
		txData[0] = 0b00001111;
		txData[1] = 0b01010101;
		SendFgpaData(&txData[0], &rxData[0], 2);
		
		usleep(10000); // 10ms
	}

}
