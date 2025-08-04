#include "uart.h"

messageBuilder message;
//int fdDebug
int fdSurface;
int fdAdda;
int fdFpga;
//int fdMidi;

void messageBuilderInit(messageBuilder *message) {
    message->current_length = 0;
}

// allows writing a single byte to buffer without using byte-stuffing
int messageBuilderAddRawByte(messageBuilder *message, char byte) {
    if (message->current_length >= MAX_MESSAGE_SIZE) {
        fprintf(stderr, "Error: Message buffer overflow when adding byte 0x%02X!\n", byte);
        return -1;
    }
    message->buffer[message->current_length++] = byte;
    return 0;
}

// add a single byte to buffer using byte-stuffing
int messageBuilderAddDataByte(messageBuilder *message, char byte) {
    // check if we have space left in message-buffer (max. 64 bytes for payload)
    if (message->current_length >= MAX_MESSAGE_SIZE) {
        fprintf(stderr, "Error: Message buffer overflow before adding data byte 0x%02X!\n", byte);
        return -1;
    }

    message->buffer[message->current_length++] = byte;

    // if added byte is 0xFE -> add 0xFF (byte-stuffing)
    // check if we have space left for byte-stuffing
    if (byte == 0xFE) {
        if (message->current_length >= MAX_MESSAGE_SIZE) {
            fprintf(stderr, "Error: Message buffer overflow during stuffing for 0xFE!\n");
            return -1;
        }
        message->buffer[message->current_length++] = 0xFF;
    }
    return 0;
}

// add a multiple bytes to buffer without byte-stuffing
int messageBuilderAddString(messageBuilder *message, const char* str) {
    // check if we have space left in message-buffer (max. 64 bytes for payload)
    if (message->current_length >= MAX_MESSAGE_SIZE) {
        fprintf(stderr, "Error: Message buffer overflow before adding data!\n");
        return -1;
    }

    // as we do not apply byte-stuffing for ASCII-characters, we can copy the data directly to the buffer
    memcpy(&message->buffer[message->current_length], str, strlen(str)); // dst, src, size
    message->current_length += strlen(str);
    return 0;
}

// add a multiple bytes to buffer using byte-stuffing
int messageBuilderAddDataArray(messageBuilder *message, const char *data, uint8_t len) {
    // check if we have space left in message-buffer (max. 64 bytes for payload)
    if (message->current_length >= MAX_MESSAGE_SIZE) {
        fprintf(stderr, "Error: Message buffer overflow before adding data!\n");
        return -1;
    }

    // now step through the data, add it to the message and check if we have to do byte-stuffing
    for (uint8_t i=0; i<len; i++) {
        message->buffer[message->current_length++] = data[i];

        // if added byte is 0xFE -> add 0xFF (byte-stuffing)
        // check if we have space left for byte-stuffing
        if (data[i] == 0xFE) {
            if (message->current_length >= MAX_MESSAGE_SIZE) {
                fprintf(stderr, "Error: Message buffer overflow during stuffing for 0xFE!\n");
                return -1;
            }
            message->buffer[message->current_length++] = 0xFF;
        }
    }
    return 0;
}

// incoming message has the form: 0xFE 0x8i Class Index Data[] 0xFE
// Checksum is calculated using the following equation:
// chksum = ( 0xFE - i - class - index - sumof(data[]) - sizeof(data[]) ) and 0x7F
uint8_t calculateChecksum(const char *data, uint16_t len) {
  // a single message can contain up to max. 64 chars
  int32_t sum = 0xFE;
  for (uint8_t i = 0; i < (len-1); i++) {
    sum -= data[i];
  }
  sum -= (len - 3); // remove 2-byte HEADER (0xFE 0x8i) and 1-byte end (0xFE)

  // write the calculated sum to the last element of the array
  return (sum & 0x7F);
}

int uartOpen(char *ttydev, uint32_t baudrate, int *fd) {
    struct termios tty;

    *fd = open(ttydev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (*fd < 0) {
        perror("Error opening serial-port!");
        return 1;
    }

    if (tcgetattr(*fd, &tty) != 0) {
        perror("Error reading serial-port-attributes!");
        close(*fd);
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

    if (baudrate == 115200) {
        cfsetispeed(&tty, B115200);
        cfsetospeed(&tty, B115200);
    } else if (baudrate == 38400) {
        cfsetispeed(&tty, B38400);
        cfsetospeed(&tty, B38400);
    } else {
        perror("Error: unsupported baudrate!");
        return 1;
    }

    if (tcsetattr(*fd, TCSANOW, &tty) != 0) {
        perror("Error setting serial-port-attributes!");
        close(*fd);
        return 1;
    }

    return 0;
}

int uartTx(int *fd, messageBuilder *message, bool addChecksum) {
    if (*fd < 0) {
        fprintf(stderr, "Error: Problem on opening serial port\n");
        return -1;
    }

    if (addChecksum) {
        char checksum = 0;
        if (message->current_length >= 2) { // at least start- and end-byte
            checksum = calculateChecksum(message->buffer, message->current_length);
        }

        // add checksum to message and send data via serial-port
        messageBuilderAddRawByte(message, checksum);
    }
    int bytes_written = write(*fd, message->buffer, message->current_length);

    return bytes_written;
}

int uartRx(int *fd, char *buf, uint16_t bufLen) {
    int bytesRead;
    int bytesAvailable;

	if (ioctl(*fd, FIONREAD, &bytesAvailable) == -1) {
		perror("Error on ioctl FIONREAD");
		close(*fd);
		return 1;
	}

	if (bytesAvailable > 0) {
		bytesRead = read(*fd, buf, (bytesAvailable < bufLen) ? bytesAvailable : bufLen);

		if (bytesRead < 0) {
			perror("Error reading from serial-port");
			close(*fd);
			return -1;
		}

		return bytesRead;
	}

	return 0;
}

int uartTxToFPGA(uint16_t cmd, data_64b *data) {
  uint8_t serialData[14];
  data_16b ErrorCheckWord;

  ErrorCheckWord.u16 = data->u8[0] + data->u8[1] + data->u8[2] + data->u8[3] + data->u8[4] + data->u8[5] + data->u8[6] + data->u8[7];

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
  serialData[11] = ErrorCheckWord.u8[1]; // MSB
  serialData[12] = ErrorCheckWord.u8[0]; // LSB
  serialData[13] = '#';  // # = end of command

  messageBuilderInit(&message);
  for (uint8_t i=0; i<14; i++) {
      messageBuilderAddRawByte(&message, serialData[i]);
  }
  return uartTx(&fdFpga, &message, false);
}
