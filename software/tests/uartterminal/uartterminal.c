// simple uart-tester, that outputs data from UART4 with 115200 baud to the screen
// v0.0.1, 23.07.2025

#define MAX_BUFFER_SIZE 32 // Etwas größer als die maximale Paketlänge, um Überläufe zu handhaben
#define MAX_MESSAGE_SIZE 64

// standard includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <linux/input.h>

// special includes
#include <string.h>

// includes for timer
#include <time.h>
#include <signal.h>

// includes for uart
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h> // for FIONREAD

// uart
int fd;
struct termios tty;
char buffer_uart[256]; // Puffer für UART-Lesevorgänge
ssize_t bytes_read;
int bytes_available;

int uartOpen(const char* uartport, uint32_t baudrate) {
    fd = open(uartport, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        perror("Error opening uart port !");
        return 1;
    }

    if (tcgetattr(fd, &tty) != 0) {
        perror("Error reading serial-port-attributes!");
        close(fd);
        return 1;
    }

    tty.c_iflag &= ~(IGNPAR | ICRNL);
    tty.c_iflag |= IGNPAR;
    tty.c_oflag &= ~(OPOST | ONLCR);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_lflag &= ~(ECHO | ECHOE | ECHOK | ICANON | ISIG);

    tty.c_cc[VMIN] = 0; // Nicht blockierend lesen
    tty.c_cc[VTIME] = 0;

    if (baudrate == 9600) {
        cfsetispeed(&tty, B9600);
        cfsetospeed(&tty, B9600);
    } else if (baudrate == 19200) {
        cfsetispeed(&tty, B19200);
        cfsetospeed(&tty, B19200);
    } else if (baudrate == 38400) {
        cfsetispeed(&tty, B38400);
        cfsetospeed(&tty, B38400);
    } else if (baudrate == 57600) {
        cfsetispeed(&tty, B57600);
        cfsetospeed(&tty, B57600);
    } else if (baudrate == 115200) {
        cfsetispeed(&tty, B115200);
        cfsetospeed(&tty, B115200);
    } else if (baudrate == 500000) {
        cfsetispeed(&tty, B500000);
        cfsetospeed(&tty, B500000);
    } else if (baudrate == 921600) {
        cfsetispeed(&tty, B921600);
        cfsetospeed(&tty, B921600);
    } else if (baudrate == 1000000) {
        cfsetispeed(&tty, B1000000);
        cfsetospeed(&tty, B1000000);
    } else if (baudrate == 1500000) {
        cfsetispeed(&tty, B1500000);
        cfsetospeed(&tty, B1500000);
    } else if (baudrate == 2000000) {
        cfsetispeed(&tty, B2000000);
        cfsetospeed(&tty, B2000000);
    } else if (baudrate == 2500000) {
        cfsetispeed(&tty, B2500000);
        cfsetospeed(&tty, B2500000);
    } else if (baudrate == 3000000) {
        cfsetispeed(&tty, B3000000);
        cfsetospeed(&tty, B3000000);
    } else if (baudrate == 3500000) {
        cfsetispeed(&tty, B3500000);
        cfsetospeed(&tty, B3500000);
    } else if (baudrate == 4000000) {
        cfsetispeed(&tty, B4000000);
        cfsetospeed(&tty, B4000000);
    } else {
        perror("Error: unsupported baudrate!");
        return 1;
    }

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Error setting serial-port-attributes!");
        close(fd);
        return 1;
    }

    return 0;
}


int uartRead() {
    uint8_t currentByte;

        if (ioctl(fd, FIONREAD, &bytes_available) == -1) {
                perror("Error on ioctl FIONREAD");
                close(fd);
                return 1;
        }

        if (bytes_available > 0) {
                bytes_read = read(fd, buffer_uart, (bytes_available < sizeof(buffer_uart)) ? bytes_available : sizeof(buffer_uart));

                if (bytes_read < 0) {
                        perror("Error reading from serial-port");
                        close(fd);
                        return 1;
                } else if (bytes_read > 0) {
                        for (int i = 0; i < bytes_read; i++) {
                                currentByte = (uint8_t)buffer_uart[i];
                                printf("0x%02X", currentByte); // empfangene Bytes ausgeben
                        }
                        printf("\n");
                        for (int i = 0; i < bytes_read; i++) {
                                currentByte = (uint8_t)buffer_uart[i];
                                printf("%c", currentByte); // empfangene Bytes ausgeben
                        }
                        printf("\n");
                        fflush(stdout);
                }
        }

        return 0;
}

int main(int argc, char *argv[]) {                                                                                                                                                           srand(time(NULL));
    printf("UART Tester\n");
    printf("v0.0.2, 07.08.2025\n");
    printf("https://github.com/xn--nding-jua/OpenX32\n");

    if (argc != 3) {
        printf("Error: Wrong parameter!\n");
        printf("Start this software like this: ./uartterminal /dev/ttymxc3 115200\n");
    }else{
        char* uartport = argv[1];
        char* baudrate = argv[2];
        printf("Connecting to UART on port %s with %s baud...\n", uartport, baudrate);
        uartOpen(uartport, atoi(baudrate));

        printf("Wait for incoming data on %s...\n", uartport);
        printf("Press Ctrl+C to terminate program.\n");
        while (1) {
            uartRead();

            // sleep for 1ms to lower CPU-load
            usleep(1000);
        }
    }
    return 0;
}
