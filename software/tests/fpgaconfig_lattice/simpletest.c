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

int test_lattice_spi() {
    int spi_fd = -1;
    struct spi_ioc_transfer tr = {0};
    uint8_t tx_buffer[4096];
    uint8_t rx_buffer[sizeof(tx_buffer)]; // not used here, but necessary
    int ret = 0;
    uint8_t buf[1024];

    uint8_t spiMode = SPI_MODE_0; // Lattice ECP5 uses MODE 0
    uint8_t spiBitsPerWord = 8;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    fprintf(stdout, "Simple connection-test to Lattice FPGA via SPI\n");
    fprintf(stdout, "  Connecting to SPI-interface...\n");
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

    // configuration-process
    tr.tx_buf = (unsigned long)tx_buffer;
    tr.rx_buf = (unsigned long)rx_buffer;
    tr.bits_per_word = spiBitsPerWord;
    tr.speed_hz = spiSpeed;

    fprintf(stdout, "  Setting PROGRAMN-Sequence HIGH -> LOW -> HIGH...\n");
    int fd = open("/sys/class/leds/reset_fpga/brightness", O_WRONLY);
    write(fd, "1", 1);
    usleep(500); // we have to keep at least 25ns. So 500us is more than enough
    write(fd, "0", 1);
    close(fd);
    usleep(50000); // we have to wait 50ms until we can send commands
	

	fprintf(stdout, "\n\nOK, we should have an open SPI-connection now. Lets see if we can communicate with the FPGA...\n\n");

	
	// send READ_ID command [class A command] -> read 32-bit IDCODE and check it
    fprintf(stdout, "Try to read the 32-bit IDCODE of the FPGA...\n");
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
	fprintf(stdout, "  Received IDCODE = 0x%8x\n", idcode);

	// wait 10ms
	usleep(10000);
	
	// check status register
	fprintf(stdout, "Try to read the 32-bit status-register of the FPGA...\n");
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
	fprintf(stdout, "  Received status-register = 0x%8x\n", status);

    fprintf(stdout, "Done.\n");

cleanup:
    if (spi_fd >= 0) close(spi_fd);

    return ret;
}


int main(int argc, char *argv[]) {
    return test_lattice_spi();
}
