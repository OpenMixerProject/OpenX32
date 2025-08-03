#ifndef UART_H_
#define UART_H_

#include "x32osc-reaper.h"

// Maximal mögliche Paketlänge inklusive Startbyte, Endbyte und Checksumme
// Langes Paket: 0xFE (1) + BoardID (1) + ID (1) + Value_MSB (1) + Value_LSB (1) + 0xFE (1) + CHECKSUM (1) = 7 Bytes
#define MAX_BUFFER_SIZE 32 // Etwas größer als die maximale Paketlänge, um Überläufe zu handhaben
#define MAX_MESSAGE_SIZE 64

typedef struct {
    unsigned char buffer[MAX_MESSAGE_SIZE];
    size_t current_length;
} messageBuilder;

void messageBuilderInit(messageBuilder *message);
int messageBuilderAddRawByte(messageBuilder *message, unsigned char byte);
int messageBuilderAddDataByte(messageBuilder *message, unsigned char byte);
int messageBuilderAddString(messageBuilder *message, const char* str);
int messageBuilderAddDataArray(messageBuilder *message, const char* data, uint8_t len);

uint8_t calculateChecksum(const unsigned char *data, uint16_t len);
int uartTxData(messageBuilder *builder);
int uartOpen();
int uartRead();

#endif
