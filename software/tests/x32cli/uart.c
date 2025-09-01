#include "uart.h"

// uart
int fd;
struct termios tty;
char buffer_uart[256]; // Puffer für UART-Lesevorgänge
ssize_t bytes_read;
int bytes_available;
uint8_t receivedBoardId;

void messageBuilderInit(messageBuilder *message) {
    message->current_length = 0;
}

// allows writing a single byte to buffer without using byte-stuffing
int messageBuilderAddRawByte(messageBuilder *message, unsigned char byte) {
    if (message->current_length >= MAX_MESSAGE_SIZE) {
        fprintf(stderr, "Error: Message buffer overflow when adding byte 0x%02X!\n", byte);
        return -1;
    }
    message->buffer[message->current_length++] = byte;
    return 0;
}

// add a single byte to buffer using byte-stuffing
int messageBuilderAddDataByte(messageBuilder *message, unsigned char byte) {
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
int messageBuilderAddDataArray(messageBuilder *message, const char* data, uint8_t len) {
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
uint8_t calculateChecksum(const unsigned char *data, uint16_t len) {
  // a single message can contain up to max. 64 chars
  int32_t sum = 0xFE;
  for (uint8_t i = 0; i < (len-1); i++) {
    sum -= data[i];
  }
  sum -= (len - 3); // remove 2-byte HEADER (0xFE 0x8i) and 1-byte end (0xFE)

  // write the calculated sum to the last element of the array
  return (sum & 0x7F);
}

int uartTxData(messageBuilder *message) {
    if (fd < 0) {
        fprintf(stderr, "Error: Problem on opening serial port\n");
        return -1;
    }

    unsigned char checksum = 0;
    if (message->current_length >= 2) { // at least start- and end-byte
        checksum = calculateChecksum(message->buffer, message->current_length);
    }

    // add checksum to message and send data via serial-port
    messageBuilderAddRawByte(message, checksum);
    int bytes_written = write(fd, message->buffer, message->current_length);

    return bytes_written;
}

int uartOpen() {
    fd = open("/dev/ttymxc1", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        perror("Error opening /dev/ttymxc1 !");
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

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Error setting serial-port-attributes!");
        close(fd);
        return 1;
    }

    return 0;
}

int uartRead() {
    uint8_t receivedClass = 0;
    uint8_t receivedIndex = 0;
    uint16_t receivedValue = 0;
    //uint8_t receivedChecksum = 0;
    uint8_t currentByte;
    uint8_t packet_buffer[MAX_BUFFER_SIZE];
    int packet_buffer_len = 0;

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
				//printf("%02X ", currentByte); // empfangene Bytes als HEX-Wert ausgeben

				// add received byte to buffer
				if (packet_buffer_len < MAX_BUFFER_SIZE) {
					packet_buffer[packet_buffer_len++] = currentByte;
				} else {
					// buffer full -> remove oldest byte
					memmove(packet_buffer, packet_buffer + 1, MAX_BUFFER_SIZE - 1);
					packet_buffer[MAX_BUFFER_SIZE - 1] = currentByte;
				}

				// we are expecting two types of messages: long (standard) and short (repeating message)
				// Short:                          Index + Value_LSB + 0xFE + CHECKSUM = 5 Bytes
				// Short:                          Index + Value_LSB + Value_MSB + 0xFE + CHECKSUM = 6 Bytes
				// Long:  0xFE + BoardID + Class + Index + Value_LSB             + 0xFE + CHECKSUM = 7 Bytes
				// Long:  0xFE + BoardID + Class + Index + Value_LSB + Value_MSB + 0xFE + CHECKSUM = 8 Bytes
				int receivedPacketLength = 0; // Länge des erkannten Pakets

				// check if we received enought data to process at least the shortest message
				if (packet_buffer_len >= 4) {
					// check for expected end-sequence (0xFE CHECKSUM)
					uint8_t offset = 0;
					if ((packet_buffer[packet_buffer_len-2] != 0xFE) && (packet_buffer[packet_buffer_len-1] == 0xFE)) {
						// this packet has no checksum!?
						offset = 1;
					}

					if ((packet_buffer[packet_buffer_len - 2] == 0xFE) || (packet_buffer[packet_buffer_len - 1] == 0xFE)) {
						// check for long 16-bit-packet (8 bytes)
						// 0xFE + BoardID + Class + Index + Value_LSB + Value_MSB + 0xFE + CHECKSUM = 8 Bytes
						if (packet_buffer_len >= (8 - offset) && packet_buffer[packet_buffer_len - 8 + offset] == 0xFE) {
							receivedBoardId = packet_buffer[packet_buffer_len - 7 + offset] & 0x7F;
							receivedClass = packet_buffer[packet_buffer_len - 6 + offset];
							receivedIndex = packet_buffer[packet_buffer_len - 5 + offset];
							receivedValue = ((uint16_t)packet_buffer[packet_buffer_len - 3 + offset] << 8) | (uint16_t)packet_buffer[packet_buffer_len - 4 + offset];
							//receivedChecksum = packet_buffer[packet_buffer_len - 1];
							receivedPacketLength = 8 - offset;
						}
						// check for long 8-bit-packet (7 bytes)
						// 0xFE + BoardID + Class + Index + Value_LSB             + 0xFE + CHECKSUM = 7 Bytes
						else if ((packet_buffer_len >= (7 - offset)) && (packet_buffer[packet_buffer_len - 7 + offset] == 0xFE)) {
							receivedBoardId = packet_buffer[packet_buffer_len - 6 + offset] & 0x7F;
							receivedClass = packet_buffer[packet_buffer_len - 5 + offset];
							receivedIndex = packet_buffer[packet_buffer_len - 4 + offset];
							receivedValue = packet_buffer[packet_buffer_len - 3 + offset];
							//receivedChecksum = packet_buffer[packet_buffer_len - 1];
							receivedPacketLength = 7 - offset;
						}
						// check for short 16-bit-packet (6 bytes)
						// Class + Index + Value_LSB + Value_MSB + 0xFE + CHECKSUM = 8 Bytes
						else if (packet_buffer_len >= (6 - offset)) {
							// 0xFE and BoardID is missing here
							receivedClass = packet_buffer[packet_buffer_len - 6 + offset];
							receivedIndex = packet_buffer[packet_buffer_len - 5 + offset];
							receivedValue = ((uint16_t)packet_buffer[packet_buffer_len - 3 + offset] << 8) | (uint16_t)packet_buffer[packet_buffer_len - 4 + offset];
							//receivedChecksum = packet_buffer[packet_buffer_len - 1];
							receivedPacketLength = 6 - offset;
						}
						// check for short 8-bit-packet (5 bytes)
						// ID Value_MSB Value_LSB 0xFE CHECKSUM
						else if (packet_buffer_len >= (5 - offset)) {
							// 0xFE and BoardID is missing here
							receivedClass = packet_buffer[packet_buffer_len - 5 + offset];
							receivedIndex = packet_buffer[packet_buffer_len - 4 + offset];
							receivedValue = packet_buffer[packet_buffer_len - 3 + offset];
							//receivedChecksum = packet_buffer[packet_buffer_len - 1];
							receivedPacketLength = 5 - offset;
						}

/*
						// now recalculate the checksum and check against received one
						unsigned char buf[receivedPacketLength];
						for (uint8_t i=0; i<(receivedPacketLength-1); i++) {
							buf[i] = packet_buffer[packet_buffer_len - receivedPacketLength - i];
						}
						calculateChecksum(buf, sizeof(buf));
						if ((receivedPacketLength > 0) && (receivedChecksum == buf[sizeof(buf)-1])) {
							surfaceCallback(receivedBoardId, receivedClass, receivedIndex, receivedValue);
						}
*/
						if (receivedPacketLength > 0) {
							surfaceCallback(receivedBoardId, receivedClass, receivedIndex, receivedValue);
						}
						/*
						if (packet_buffer_len > 0) {
						  printf("Warnung: verbleibende Bytes im Puffer: %d\n", packet_buffer_len);
						}
						*/

						// shift remaining bytes byte by processed amount of data
						memmove(packet_buffer, packet_buffer + receivedPacketLength, packet_buffer_len - receivedPacketLength);
						packet_buffer_len -= receivedPacketLength;
					}
				}
			}
			fflush(stdout);
		}
	}

	return 0;
}
