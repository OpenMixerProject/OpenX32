/*
    ____                  __   ______ ___  
   / __ \                 \ \ / /___ \__ \ 
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
  | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |                               
         |_|                               
  
  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "uart.h"


Uart::Uart(X32BaseParameter* basepar): X32Base(basepar) {}



int Uart::Open(const char* ttydev, uint32_t baudrate, bool raw) {
    
    if (state->bodyless && !state->bodyless_with_surface_and_adda)
    {
        return 0;
    }

    struct termios tty;

    fd = open(ttydev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        perror("Error opening serial-port!");
        return 1;
    }

    if (tcgetattr(fd, &tty) != 0) {
        perror("Error reading serial-port-attributes!");
        close(fd);
        return 1;
    }

    if (raw){
        // https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_17.html
        // from Exmaple: int cfmakeraw (struct termios *termios-p)
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
        tty.c_oflag &= ~OPOST;
        tty.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
        tty.c_cflag &= ~(CSIZE|PARENB);
        tty.c_cflag |= CS8;
    } else {
        tty.c_iflag &= ~(IGNPAR | ICRNL);
        tty.c_iflag |= IGNPAR;
        tty.c_oflag &= ~(OPOST | ONLCR);
        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_lflag &= ~(ECHO | ECHOE | ECHOK | ICANON | ISIG);
    }

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

int Uart::Tx(MessageBase* message) {
    if (fd < 0) {
        fprintf(stderr, "Error: Problem on opening serial port\n");
        return -1;
    }

	if (helper->DEBUG_UART(DEBUGLEVEL_TRACE)) {
		printf("DEBUG_UART: fd=%d TransmitRaw: ", fd);
    	printf("hex: ");
        for (uint8_t i=0; i < message->current_length; i++){
        	printf("%.2X ", message->buffer[i]);
    	}
        printf("| char: ");
        for (uint8_t i=0; i < message->current_length; i++){
        	printf("%c", message->buffer[i]);
    	}
    	
        printf("\n");
	}

    // only write in bodyless mode when surface and adda are connected
    if (state->bodyless && !state->bodyless_with_surface_and_adda)
    {
        return 0;
    }

    int bytes_written = write(fd, message->buffer, message->current_length);

    return bytes_written;
}

int Uart::Rx(char* buf, uint16_t bufLen) {
    int bytesRead;
    int bytesAvailable;
    int bytesToRead;

	if (ioctl(fd, FIONREAD, &bytesAvailable) == -1) {
		perror("Error on ioctl FIONREAD");
		close(fd);
		return 1;
	}

	if (bytesAvailable > 0) {

        if (bytesAvailable < bufLen) {
                bytesToRead = bytesAvailable;
            } else { 
                bytesToRead = bufLen;
                helper->DEBUG_UART(DEBUGLEVEL_NORMAL, "buffer: %d bytes, bytes available to read: %d bytes - Buffer too small, we can not read all bytes!", bufLen, bytesAvailable);
            }

        bytesRead = read(fd, buf, bytesToRead);

		if (bytesRead < 0) {
			perror("Error reading from serial-port");
			close(fd);
			return -1;
		}

        // DEBUG: print received bytes as hex and string
        if (helper->DEBUG_UART(DEBUGLEVEL_TRACE)){
            printf("DEBUG_UART: fd=%d Receive: ", fd);
            printf("hex: ");
            for (uint8_t i=0; i < bytesRead; i++){
                printf("%.2X ", buf[i]);
            }
            printf("| char: ");
            for (uint8_t i=0; i < bytesRead; i++){
                printf("%c", buf[i]);
            }
            
            printf("\n");      
	    }

		return bytesRead;
	}
	return 0;
}

void Uart::MirrorBack() {
    int bytesRead;
    int bytesAvailable;
    char buf[100];
    int bytesToRead;

	if (ioctl(fd, FIONREAD, &bytesAvailable) == -1) {
		perror("Error on ioctl FIONREAD");
		close(fd);
		return;
	}
    bytesToRead = bytesAvailable;
    if (bytesToRead > 100) {
        bytesToRead = 100;
    }

	if (bytesToRead > 0) {
        bytesRead = read(fd, &buf[0], bytesToRead);

		if (bytesRead > 0) {
            // send via same UART
            write(fd, &buf[0], bytesRead);
		}
	}
}

void Uart::FlushRxBuffer() {
    char buf;
	while (Rx(&buf, 1) > 0);
}