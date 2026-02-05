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



// incoming message has the form: 0xFE 0x8i Class Index Data[] 0xFE
// Checksum is calculated using the following equation:
// chksum = ( 0xFE - i - class - index - sumof(data[]) - sizeof(data[]) ) and 0x7F
uint8_t Uart::calculateChecksum(const char* data, uint16_t len) {
  // a single message can contain up to max. 64 chars
  int32_t sum = 0xFE;
  for (uint8_t i = 0; i < (len-1); i++) {
    sum -= data[i];
  }
  sum -= (len - 3); // remove 2-byte HEADER (0xFE 0x8i) and 1-byte end (0xFE)

  // write the calculated sum to the last element of the array
  return (sum & 0x7F);
}


int Uart::Open(const char* ttydev, uint32_t baudrate, bool raw) {
    if (state->bodyless) {
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

int Uart::TxRaw(MessageBase* message) {
    if (state->bodyless) {
        return 0;
    }


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

    int bytes_written = write(fd, message->buffer, message->current_length);

    return bytes_written;
}

int Uart::Tx(MessageBase* message, bool addChecksum) {
    if (fd < 0) {
        fprintf(stderr, "Error: Problem on opening serial port\n");
        return -1;
    }

    message->AddRawByte(0xFE); // Endbyte

    if (addChecksum) {
        char checksum = 0;
        if (message->current_length >= 2) { // at least start- and end-byte
            checksum = calculateChecksum(message->buffer, message->current_length);
        }

        // add checksum to message and send data via serial-port
        message->AddRawByte(checksum);
    }

    return TxRaw(message);
}



int Uart::Rx(char* buf, uint16_t bufLen) {
    if (state->bodyless) {
        return 0;
    }


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

int Uart::TxToFPGA(uint16_t cmd, data_64b* data) {
  uint8_t serialData[14];
  uint16_t ErrorCheckWord;

  ErrorCheckWord = 0;
  for (uint8_t i=0; i<8; i++) {
    ErrorCheckWord += data->u8[i];
  }

  serialData[0] = '*';  // * = begin of command
  serialData[1] = (cmd >> 8);  // MSB of 16-bit cmd
  serialData[2] = cmd;         // LSB of 16-bit cmd
  serialData[3] = data->u8[7]; // MSB of 64-bit payload
  serialData[4] = data->u8[6];
  serialData[5] = data->u8[5];
  serialData[6] = data->u8[4];
  serialData[7] = data->u8[3];
  serialData[8] = data->u8[2];
  serialData[9] = data->u8[1];
  serialData[10] = data->u8[0]; // LSB of payload
  serialData[11] = (ErrorCheckWord >> 8); // MSB
  serialData[12] = ErrorCheckWord; // LSB
  serialData[13] = '#';  // # = end of command

  MessageBase message;

  for (uint8_t i=0; i<14; i++) {
      message.AddRawByte(serialData[i]);
  }

  return TxRaw(&message);
}